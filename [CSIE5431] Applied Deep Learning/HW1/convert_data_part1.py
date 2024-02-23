import json

column_names = ["id", "sent1", "sent2", "ending0", "ending1", "ending2", "ending3", "label"]

input_file_name = ["./DATA/train.json", "./DATA/valid.json"]
output_file_name = ["./DATA/train_part1.json", "./DATA/valid_part1.json"]

with open('./DATA/context.json', 'r') as json_file:
    paragraph = json.load(json_file)

# Preprocess train / valid dataset
for i in range(2):
    d = []
    with open(input_file_name[i], 'r') as json_file:
        datas = json.load(json_file)
        
    for data in datas:
        # get label from relevant
        label = -1
        for idx, p in enumerate(data['paragraphs']):
            if data['relevant'] == p:
                label = idx
                break

        obj = {
            "id": data['id'], 
            "sent1": data['question'], 
            "sent2": "", 
            "ending0": paragraph[data['paragraphs'][0]], 
            "ending1": paragraph[data['paragraphs'][1]], 
            "ending2": paragraph[data['paragraphs'][2]],
            "ending3": paragraph[data['paragraphs'][3]],
            "label": label,
        }
        d.append(obj)

    with open(output_file_name[i], "w") as f:
        json.dump(d, f, ensure_ascii=False, indent=2)