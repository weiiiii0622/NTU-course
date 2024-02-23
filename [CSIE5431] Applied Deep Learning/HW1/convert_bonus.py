import csv
import json

column_names = ["id", "title", "context", "question", "answers"]

input_file_name = ["./DATA/train.json", "./DATA/valid.json"]
output_file_name = ["./DATA/train_bonus.json", "./DATA/valid_bonus.json"]

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
            
            context = ""
            for j in range(4):
                context += paragraph[data['paragraphs'][j]]

            start = 0
            for j in range(label):
                start += len(paragraph[data['paragraphs'][j]])
            start += data['answer']['start']

            ans = {
                'text' : [data['answer']['text']],
                'answer_start': [start]
            }
            
            obj = {
                "id": data['id'], 
                "title": "",
                "context": context,
                "question": data['question'],
                "answers": ans
            }
            d.append(obj)
        
    with open(output_file_name[i], "w") as f:
        json.dump(d, f, ensure_ascii=False, indent=2)