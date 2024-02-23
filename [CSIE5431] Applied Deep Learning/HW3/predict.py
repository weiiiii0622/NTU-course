import torch
import numpy as np
from tqdm import tqdm
from transformers import AutoModelForCausalLM, AutoTokenizer
import json
from peft import PeftModel
from utils import get_prompt, get_bnb_config
import argparse


def predict(
    model, tokenizer, data, max_length=2048,
):
    data_size = len(data)
    instructions = [get_prompt(x["instruction"]) for x in data]

    # Tokenize data
    tokenized_instructions = tokenizer(instructions, add_special_tokens=False)

    # Format data
    for i in range(data_size):
        instruction_input_ids = [tokenizer.bos_token_id] + \
            tokenized_instructions["input_ids"][i]
        tokenized_instructions["input_ids"][i] = instruction_input_ids
        tokenized_instructions["attention_mask"][i] = [
            1] * len(tokenized_instructions["input_ids"][i])

        tokenized_instructions["input_ids"][i] = torch.tensor(
            tokenized_instructions["input_ids"][i][:max_length])
        tokenized_instructions["attention_mask"][i] = torch.tensor(
            tokenized_instructions["attention_mask"][i][:max_length])

    # Generate output
    generated_outputs = []
    for i in tqdm(range(data_size)):
        input_ids = tokenized_instructions["input_ids"][i].unsqueeze(0)
        attn_mask = tokenized_instructions["attention_mask"][i].unsqueeze(0)

        with torch.no_grad():
            generated_output = model.generate(
                input_ids=input_ids, attention_mask=attn_mask, max_length=max_length, 
                num_return_sequences=1
            )
        generated_text = tokenizer.decode(generated_output[0], skip_special_tokens=True)
        marker = "ASSISTANT:"
        index = generated_text.find(marker)
        
        if index != -1:
            generated_text = generated_text[index + len(marker):].strip()

        generated_outputs.append(generated_text.strip())
    return generated_outputs


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--base_model_path",
        type=str,
        default="",
        help="Path to the checkpoint of Taiwan-LLM-7B-v2.0-chat. If not set, this script will use "
        "the checkpoint from Huggingface (revision = 5073b2bbc1aa5519acdc865e99832857ef47f7c9)."
    )
    parser.add_argument(
        "--peft_path",
        type=str,
        required=True,
        help="Path to the saved PEFT checkpoint."
    )
    parser.add_argument(
        "--test_data_path",
        type=str,
        default="",
        required=True,
        help="Path to test data."
    )
    parser.add_argument(
        "--output_path",
        type=str,
        default="",
        required=True,
        help="Path to prediction.json."
    )
    args = parser.parse_args()

    # Load model
    bnb_config = get_bnb_config()

    if args.base_model_path:
        model = AutoModelForCausalLM.from_pretrained(
            args.base_model_path,
            torch_dtype=torch.bfloat16,
            quantization_config=bnb_config
        )
        tokenizer = AutoTokenizer.from_pretrained(args.base_model_path)
    else:
        model_name = "yentinglin/Taiwan-LLM-7B-v2.0-chat"
        revision = "5073b2bbc1aa5519acdc865e99832857ef47f7c9"
        model = AutoModelForCausalLM.from_pretrained(
            model_name,
            revision=revision,
            torch_dtype=torch.bfloat16,
            quantization_config=bnb_config
        )
        tokenizer = AutoTokenizer.from_pretrained(
            model_name,
            revision=revision,
        )

    if tokenizer.pad_token_id is None:
        tokenizer.pad_token_id = tokenizer.eos_token_id

    # Load LoRA
    model = PeftModel.from_pretrained(model, args.peft_path)

    with open(args.test_data_path, "r") as f:
        data = json.load(f)

    model.eval()
    outputs = predict(model, tokenizer, data)

    d = []
    l = len(data)
    for i in range(l):
        obj = {
            "id": data[i]['id'],
            "output": outputs[i]
        }
        d.append(obj)

    with open(args.output_path, "w", encoding="utf-8") as output_file:
        json.dump(d, output_file, indent=2, ensure_ascii=False)
