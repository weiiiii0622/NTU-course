from accelerate import Accelerator
from accelerate.logging import get_logger
from accelerate.utils import set_seed
import argparse
from datasets import load_dataset
from transformers import AutoTokenizer, AutoModelForSeq2SeqLM, AutoConfig, DataCollatorForSeq2Seq
import nltk
from torch.utils.data import DataLoader
import torch
from tqdm.auto import tqdm
import json
import jsonlines


## Arguments
parser = argparse.ArgumentParser()

parser.add_argument("--model_path", type=str, help="The path for your model")
parser.add_argument("--test_file", type=str, help="The path for data.jsonl")
parser.add_argument("--submission_file", type=str, help="The path for submission.jsonl")
parser.add_argument(
        "--source_prefix",
        type=str,
        default=None,
        help="A prefix to add before every source text (useful for T5 models).",
    )
parser.add_argument(
        "--text_column",
        type=str,
        default=None,
        help="The name of the column in the datasets containing the full texts (for summarization).",
    )
parser.add_argument(
        "--summary_column",
        type=str,
        default=None,
        help="The name of the column in the datasets containing the summaries (for summarization).",
    )
parser.add_argument(
        "--ignore_pad_token_for_loss",
        type=bool,
        default=True,
        help="Whether to ignore the tokens corresponding to padded labels in the loss computation or not.",
    )
parser.add_argument(
        "--max_source_length",
        type=int,
        default=1024,
        help=(
            "The maximum total input sequence length after "
            "tokenization.Sequences longer than this will be truncated, sequences shorter will be padded."
        ),
    )
parser.add_argument(
        "--max_target_length",
        type=int,
        default=128,
        help=(
            "The maximum total sequence length for target text after "
            "tokenization. Sequences longer than this will be truncated, sequences shorter will be padded. "
            "during ``evaluate`` and ``predict``."
        ),
    )
parser.add_argument(
        "--val_max_target_length",
        type=int,
        default=None,
        help=(
            "The maximum total sequence length for validation "
            "target text after tokenization.Sequences longer than this will be truncated, sequences shorter will be "
            "padded. Will default to `max_target_length`.This argument is also used to override the ``max_length`` "
            "param of ``model.generate``, which is used during ``evaluate`` and ``predict``."
        ),
    )
parser.add_argument(
        "--pad_to_max_length",
        action="store_true",
        help="If passed, pad all samples to `max_length`. Otherwise, dynamic padding is used.",
    )
parser.add_argument(
        "--max_test_samples",
        type=int,
        default=None,
        help=(
            "For debugging purposes or quicker training, truncate the number of test examples to this "
            "value if set."
        ),
    )
parser.add_argument(
        "--per_device_test_batch_size",
        type=int,
        default=8,
        help="Batch size (per device) for the evaluation dataloader.",
    )
parser.add_argument(
        "--use_fp16",
        action="store_true",
        help="Whether to enable fp16.",
    )
parser.add_argument(
        "--do_sample",
        action="store_true",
        help="Whether or not to use sampling ; use greedy decoding otherwise.",
    )
parser.add_argument(
        "--top_k",
        type=int,
        default=50,
        help="The number of highest probability vocabulary tokens to keep for top-k-filtering.",
    )
parser.add_argument(
        "--top_p",
        type=float,
        default=1.0,
        help="If set to float < 1, only the smallest set of most probable tokens with probabilities that add up to top_p or higher are kept for generation.",
    )
parser.add_argument(
        "--num_beams",
        type=int,
        default=None,
        help=(
            "Number of beams to use for evaluation. This argument will be "
            "passed to ``model.generate``, which is used during ``evaluate`` and ``predict``."
        ),
    )
parser.add_argument(
        "--temperature",
        type=float,
        default=1.0,
        help="The value used to modulate the next token probabilities",
    )
args = parser.parse_args()

accelerator = Accelerator()
config = AutoConfig.from_pretrained(args.model_path)
tokenizer = AutoTokenizer.from_pretrained(args.model_path)
model = AutoModelForSeq2SeqLM.from_pretrained(args.model_path, config=config)

data_files = {}
if args.test_file is not None:
    data_files["test"] = args.test_file
else:
    raise FileNotFoundError()

extension = args.test_file.split(".")[-1]
raw_datasets = load_dataset(extension, data_files=data_files)

# We resize the embeddings only when necessary to avoid index errors. If you are creating a model from scratch
# on a small vocab and want a smaller embedding size, remove this test.
embedding_size = model.get_input_embeddings().weight.shape[0]
if len(tokenizer) > embedding_size:
    model.resize_token_embeddings(len(tokenizer))
if model.config.decoder_start_token_id is None:
    raise ValueError("Make sure that `config.decoder_start_token_id` is correctly defined")

prefix = args.source_prefix if args.source_prefix is not None else ""

## Tokenize
column_names = raw_datasets["test"].column_names
text_column = args.text_column
summary_column = args.summary_column

if args.val_max_target_length is None:
    args.val_max_target_length = args.max_target_length

# Temporarily set max_target_length for training.
max_target_length = args.max_target_length
padding = "max_length" if args.pad_to_max_length else False

def preprocess_function(examples):
    inputs = examples[text_column]
    targets = examples[summary_column]
    inputs = [prefix + inp for inp in inputs]
    model_inputs = tokenizer(inputs, max_length=args.max_source_length, padding=padding, truncation=True)

    # Tokenize targets with the `text_target` keyword argument
    labels = tokenizer(text_target=targets, max_length=max_target_length, padding=padding, truncation=True)

    # If we are padding here, replace all tokenizer.pad_token_id in the labels by -100 when we want to ignore
    # padding in the loss.
    if padding == "max_length" and args.ignore_pad_token_for_loss:
        labels["input_ids"] = [
            [(l if l != tokenizer.pad_token_id else -100) for l in label] for label in labels["input_ids"]
        ]

    model_inputs["labels"] = labels["input_ids"]
    return model_inputs

with accelerator.main_process_first():
    test_dataset = raw_datasets["test"].map(
        preprocess_function,
        batched=True,
        remove_columns=column_names,
    )
    if args.max_test_samples is not None:
        # During Feature creation dataset samples might increase, we will select required samples again
        test_dataset = test_dataset.select(range(args.max_test_samples))

label_pad_token_id = -100
data_collator = DataCollatorForSeq2Seq(
    tokenizer,
    model=model,
    label_pad_token_id=label_pad_token_id,
    pad_to_multiple_of=8 if accelerator.use_fp16 else None,
)

def postprocess_text(preds):
    preds = [pred.strip() for pred in preds]

    # rougeLSum expects newline after each sentence
    # preds = ["\n".join(nltk.sent_tokenize(pred)) for pred in preds]

    return preds

test_dataloader = DataLoader(test_dataset, collate_fn=data_collator, batch_size=args.per_device_test_batch_size)

# Prepare everything with our `accelerator`.
device = accelerator.device
#model.to(device)
model, test_dataloader = accelerator.prepare(model, test_dataloader)
print(device)

# Predict
model.eval()

gen_kwargs = {
    "max_length": args.val_max_target_length,
    "num_beams": args.num_beams,
    "do_sample": args.do_sample,
    "top_k": args.top_k,
    "top_p": args.top_p,
    "temperature": args.temperature
}

# Read JSON lines and collect into a list
with open(args.test_file, 'r') as json_file:
    datas = json.load(json_file)


total = len(test_dataloader)
with jsonlines.open(args.submission_file, mode='a') as writer:
    for idx, batch in tqdm(iterable=enumerate(test_dataloader), total=total, colour="green"):
        with torch.no_grad():
            generated_tokens = accelerator.unwrap_model(model).generate(
                batch["input_ids"],
                attention_mask=batch["attention_mask"],
                **gen_kwargs,
            )

            # print(generated_tokens)

            generated_tokens = accelerator.pad_across_processes(
                generated_tokens, dim=1, pad_index=tokenizer.pad_token_id
            )

            generated_tokens = generated_tokens.cpu().numpy()

            if isinstance(generated_tokens, tuple):
                generated_tokens = generated_tokens[0]
            decoded_preds = tokenizer.batch_decode(generated_tokens, skip_special_tokens=True)

            decoded_preds= postprocess_text(decoded_preds)

            # print(decoded_preds)
            
            for offset in range(args.per_device_test_batch_size):
                try:
                    obj = {
                        "title": decoded_preds[offset],
                        "id": datas[idx*args.per_device_test_batch_size + offset]['id']
                    }
                    writer.write(obj)
                except:
                    break
    