import json
import random

input_file = './data/train.json'
output_file = './data/random_train.json'
n = 5000
seed = 1006

with open(input_file, 'r') as file:
    data = json.load(file)

n = min(n, len(data))

random.seed(seed)
random_entries = random.sample(data, n)

with open(output_file, 'w', encoding="utf-8") as file:
    json.dump(random_entries, file, indent=2, ensure_ascii=False)
