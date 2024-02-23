import json
import argparse

parser = argparse.ArgumentParser()

parser.add_argument("--jsonl_path", type=str, help="The path for data.jsonl")
parser.add_argument("--json_path", type=str, help="The path for data.json")

args = parser.parse_args()

# Path to your input JSONL file and output JSON file
input_jsonl_file = args.jsonl_path
output_json_file = args.json_path

# Read JSON lines and collect into a list
json_lines = []
with open(input_jsonl_file, 'r') as input_file:
    for line in input_file:
        json_lines.append(json.loads(line))

# Write the collected data into a single JSON file
with open(output_json_file, 'w') as output_file:
    json.dump(json_lines, output_file, indent=4)  # Adjust indent for formatting, if desired