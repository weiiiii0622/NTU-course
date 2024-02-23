import torch
import json
import argparse
import csv
from tqdm import tqdm
from transformers import pipeline, AutoTokenizer, AutoModelForMultipleChoice, AutoConfig, PreTrainedTokenizerBase
from datasets import load_dataset 
from torch.utils.data import DataLoader
from dataclasses import dataclass
from itertools import chain
from typing import Optional, Union
from transformers.utils import PaddingStrategy
from accelerate import Accelerator


## Arguments
parser = argparse.ArgumentParser()

parser.add_argument("--do_mc", action="store_true", default=False, help="Whether predict MC or not")
parser.add_argument("--do_qa", action="store_true", default=False, help="Whether predict QA or not")
parser.add_argument("--do_qa_data", action="store_true", default=False, help="Whether predict QA from csv or not")
parser.add_argument("--model_mc_path", type=str, help="The path for your MC model")
parser.add_argument("--model_qa_num", type=int, help="The path for your QA model")
parser.add_argument("--model_qa1_path", type=str, help="The path for your QA model")
parser.add_argument("--model_qa2_path", type=str, help="The path for your QA model")
parser.add_argument("--model_qa3_path", type=str, help="The path for your QA model")
parser.add_argument("--model_qa4_path", type=str, help="The path for your QA model")
parser.add_argument("--context_path", type=str, help="The path for context.json")
parser.add_argument("--part1_data_path", type=str, help="The path for part1 data")
parser.add_argument("--part2_data_path", type=str, help="The path for part2 data")
parser.add_argument("--test_path", type=str, help="The path for test.json")
parser.add_argument("--submission_path", type=str, help="The path for submission.csv")

args = parser.parse_args()

DO_MC           = args.do_mc                # Predict MC if True
DO_QA           = args.do_qa                # Predict QA if True
DO_QA_DATA      = args.do_qa_data           # Predict QA from CSV if True
MODEL_MC        = args.model_mc_path
MODEL_QA_NUMBER = args.model_qa_num
MODEL_QA1       = args.model_qa1_path
MODEL_QA2       = args.model_qa2_path
MODEL_QA3       = args.model_qa3_path
MODEL_QA4       = args.model_qa4_path
CONTEXT_PATH    = args.context_path
TEST_PATH       = args.test_path
SUBMISSION_PATH = args.submission_path
PART1_DATA_PATH = args.part1_data_path
PART2_DATA_PATH = args.part2_data_path


# Change if qa model path or number have been changed
QA_MODELS = [MODEL_QA1, MODEL_QA2, MODEL_QA3, MODEL_QA4]
# DATA_CNT = 4
# DATA = ["./result/ver4/submission_ver11.csv", "./result/ver4/submission_ver12.csv", "./result/ver4/submission_ver29.csv", "./result/ver4/submission_ver32.csv"]

SPECIAL_CHARACTER = ["，", "。"]

with open(CONTEXT_PATH, 'r') as json_file:
    paragraph = json.load(json_file)
with open(TEST_PATH, 'r') as json_file:
    datas = json.load(json_file)

##### Step1 : Convert test.json to desired part1 data #####


# Preprocess Test
d = []
    
for data in datas:
    obj = {
        "id": data['id'], 
        "sent1": data['question'], 
        "sent2": "", 
        "ending0": paragraph[data['paragraphs'][0]], 
        "ending1": paragraph[data['paragraphs'][1]], 
        "ending2": paragraph[data['paragraphs'][2]],
        "ending3": paragraph[data['paragraphs'][3]],
    }
    d.append(obj)

with open(PART1_DATA_PATH, "w") as f:
    json.dump(d, f, ensure_ascii=False, indent=2)


##### Step2 : Predict MC for test_data #####

@dataclass
class DataCollatorForMultipleChoice:
    """
    Data collator that will dynamically pad the inputs for multiple choice received.

    Args:
        tokenizer ([`PreTrainedTokenizer`] or [`PreTrainedTokenizerFast`]):
            The tokenizer used for encoding the data.
        padding (`bool`, `str` or [`~utils.PaddingStrategy`], *optional*, defaults to `True`):
            Select a strategy to pad the returned sequences (according to the model's padding side and padding index)
            among:

            - `True` or `'longest'`: Pad to the longest sequence in the batch (or no padding if only a single sequence
              if provided).
            - `'max_length'`: Pad to a maximum length specified with the argument `max_length` or to the maximum
              acceptable input length for the model if that argument is not provided.
            - `False` or `'do_not_pad'` (default): No padding (i.e., can output a batch with sequences of different
              lengths).
        max_length (`int`, *optional*):
            Maximum length of the returned list and optionally padding length (see above).
        pad_to_multiple_of (`int`, *optional*):
            If set will pad the sequence to a multiple of the provided value.

            This is especially useful to enable the use of Tensor Cores on NVIDIA hardware with compute capability >=
            7.5 (Volta).
    """

    tokenizer: PreTrainedTokenizerBase
    padding: Union[bool, str, PaddingStrategy] = True
    max_length: Optional[int] = None
    pad_to_multiple_of: Optional[int] = None

    def __call__(self, features):
        batch_size = len(features)
        num_choices = len(features[0]["input_ids"])
        flattened_features = [
            [{k: v[i] for k, v in feature.items()} for i in range(num_choices)] for feature in features
        ]
        flattened_features = list(chain(*flattened_features))

        batch = self.tokenizer.pad(
            flattened_features,
            padding=self.padding,
            max_length=self.max_length,
            pad_to_multiple_of=self.pad_to_multiple_of,
            return_tensors="pt",
        )

        # Un-flatten
        batch = {k: v.view(batch_size, num_choices, -1) for k, v in batch.items()}
        return batch
    

if DO_MC:
    # Load Model
    accelerator = Accelerator()
    config = AutoConfig.from_pretrained(MODEL_MC)
    tokenizer = AutoTokenizer.from_pretrained(MODEL_MC)
    model = AutoModelForMultipleChoice.from_pretrained(MODEL_MC, config=config)

    embedding_size = model.get_input_embeddings().weight.shape[0]
    if len(tokenizer) > embedding_size:
        model.resize_token_embeddings(len(tokenizer))

    padding = False
    data_collator = DataCollatorForMultipleChoice(
                tokenizer, pad_to_multiple_of=(8 if accelerator.use_fp16 else None)
            )
    ending_names = [f"ending{i}" for i in range(4)]
    context_name = "sent1"
    question_header_name = "sent2"

    def preprocess_function(examples):
        first_sentences = [[context] * 4 for context in examples[context_name]]
        question_headers = examples[question_header_name]
        second_sentences = [
            [f"{header} {examples[end][i]}" for end in ending_names] for i, header in enumerate(question_headers)
        ]
        # Flatten out
        first_sentences = list(chain(*first_sentences))
        second_sentences = list(chain(*second_sentences))

        # Tokenize
        tokenized_examples = tokenizer(
            first_sentences,
            second_sentences,
            max_length= 512,
            padding=padding,
            truncation=True,
        )
        # Un-flatten
        tokenized_inputs = {k: [v[i : i + 4] for i in range(0, len(v), 4)] for k, v in tokenized_examples.items()}
        return tokenized_inputs

    data_files = {}
    data_files["test"] = PART1_DATA_PATH
    raw_datasets = load_dataset('json', data_files=data_files)
    with accelerator.main_process_first():
        processed_datasets = raw_datasets.map(
                preprocess_function, batched=True, remove_columns=raw_datasets["test"].column_names
            )
    data = processed_datasets["test"]
    test_dataloader = DataLoader(
            data, collate_fn=data_collator
        )

    # Predict and create test data for part2

    device = accelerator.device
    model.to(device)
    model, test_dataloader = accelerator.prepare(model, test_dataloader)

    d = []
    model.eval()
    total = len(test_dataloader)
    for idx, batch in tqdm(iterable=enumerate(test_dataloader), total=total, colour="green"):
        with torch.no_grad():
            outputs = model(**batch)
            predictions = outputs.logits.argmax(dim=-1)
            id = predictions[0].item()
            obj = {
                "id": datas[idx]['id'], 
                "title": "",
                "context": paragraph[datas[idx]['paragraphs'][id]],
                "question": datas[idx]['question'],
            }
            d.append(obj)

    with open(PART2_DATA_PATH, "w") as f:
        json.dump(d, f, ensure_ascii=False, indent=2)


##### Step 3 : Predict QA and output submission.csv

def check_characters(str):
    for ch in SPECIAL_CHARACTER:
        if ch in str[1:-1]:
            return 0
    else:
        return 1
def truncate(input_str):
    flag = True
    while(flag):
        if input_str.startswith('"') and input_str.endswith('"'):
            input_str = input_str[1:-1]
            flag = False
        if input_str.startswith('「') and input_str.endswith('」'):
            input_str = input_str[1:-1]
            flag = False
        if input_str.startswith('「') and '」' not in input_str:
            input_str = input_str[1:]
            flag = False
        if input_str.endswith('」') and '「' not in input_str:
            input_str = input_str[:-1]
            flag = False
        if flag == False:
            flag = True
        else:
            flag = False
    return input_str
    
# Perform majority voting
def majority_vote(predictions):
    all_votes = []
    for i in range(MODEL_QA_NUMBER):
        all_votes.append(predictions[i]['answer'])

    vote_counts = {}
    max_count = 0
    most_common_answer = None
    for answer in all_votes:
        answer = truncate(answer)
        score = check_characters(answer)
        if answer in vote_counts:
            vote_counts[answer] += score
        else:
            vote_counts[answer] = score

        if vote_counts[answer] >= max_count:
            max_count = vote_counts[answer]
            most_common_answer = answer
    return most_common_answer

if DO_QA:
    COLUMN_NAME = ["id", "answer"]
    pipe = [None for i in range(MODEL_QA_NUMBER)]
    for i, model in enumerate(QA_MODELS):
        pipe[i] = pipeline("question-answering", model=model, config=model, tokenizer=model, device="cuda")

    with open(PART2_DATA_PATH, 'r') as json_file:
        datas = json.load(json_file)

    with open(SUBMISSION_PATH, 'w') as file:
        writer = csv.DictWriter(file, fieldnames=COLUMN_NAME)
        writer.writeheader()

        total = len(datas)
        for idx, d in tqdm(iterable=enumerate(datas), total=total, colour="green"):
            id = d['id']
            question = d['question']
            context = d['context']
            res = []
            for p in pipe:
                res.append(p(question=question, context=context))
            ans = majority_vote(res)

            obj = {
                "id": id,
                "answer": ans
            }
            writer.writerow(obj)

# if DO_QA_DATA:
#     reader = [None for i in range(DATA_CNT)]
#     COLUMN_NAME = ["id", "answer"]
#     predictions = []
#     for i, csv_file in enumerate(DATA):
#         with open(csv_file, 'r', newline='') as file:
#             reader[i] = csv.DictReader(file)
#             prediction = [row for row in reader[i]]
#             predictions.append(prediction)

#     with open(SUBMISSION_PATH, 'w') as file:
#         writer = csv.DictWriter(file, fieldnames=COLUMN_NAME)
#         writer.writeheader()

#         for i in range(2213):

#             id = predictions[0][i]['id']
#             majority_ans = majority_vote(predictions, i)
#             obj = {
#                 "id": id,
#                 "answer": truncate(majority_ans)
#             }
#             writer.writerow(obj)