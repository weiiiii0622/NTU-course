# Step to train my model

## Prerequisite

- To be able to use `evaluate-metric/accuracy` in the given MC model, we should install the following dependencies

```bash
pip install scikit-learn
```

## Step 1

- Run the following script to convert our input text to the desired input form for the MC and QA models.

```bash
python convert_data_part1.py
python convert_data_part2.py
python convert_bonus.py
```

## Step2

- Train the model with the data from above by executing modifying the desired hyper-parameters:

    1. For MC:

        ```bash
        python3 MC.py \
            --model_name_or_path hfl/chinese-bert-wwm-ext \
            --train_file ./DATA/train_part1.json \
            --validation_file ./DATA/valid_part1.json \
            --max_seq_length 512 \
            --per_device_train_batch_size 4 \
            --gradient_accumulation_steps 1 \
            --learning_rate 2e-5 \
            --num_train_epochs 2 \
            --output_dir ./tmp/Part1
        ```

    2. For QA (I use 4 models):

        ```bash
        python QA.py \
            --model_name_or_path hfl/chinese-roberta-wwm-ext \
            --train_file ./DATA/train_part2.json \
            --validation_file ./DATA/valid_part2.json \
            --max_seq_length 512 \
            --per_device_train_batch_size 4 \
            --gradient_accumulation_steps 2 \
            --learning_rate 3e-5 \
            --num_train_epochs 3 \
            --output_dir ./tmp/Part2_model1
        ```

        ```bash
        python QA.py \
            --model_name_or_path hfl/chinese-roberta-wwm-ext \
            --train_file ./DATA/train_part2.json \
            --validation_file ./DATA/valid_part2.json \
            --max_seq_length 512 \
            --per_device_train_batch_size 4 \
            --gradient_accumulation_steps 2 \
            --learning_rate 3e-5 \
            --num_train_epochs 5 \
            --output_dir ./tmp/Part2_model2
        ```

        ```bash
        python QA.py \
            --model_name_or_path hfl/chinese-bert-wwm-ext \
            --train_file ./DATA/train_part2.json \
            --validation_file ./DATA/valid_part2.json \
            --max_seq_length 512 \
            --per_device_train_batch_size 4 \
            --gradient_accumulation_steps 2 \
            --learning_rate 2e-5 \
            --num_train_epochs 6 \
            --output_dir ./tmp/Part2_model3
        ```

        ```bash
        python QA.py \
            --model_name_or_path hfl/chinese-bert-wwm-ext-large \
            --train_file ./DATA/train_part2.json \
            --validation_file ./DATA/valid_part2.json \
            --max_seq_length 512 \
            --per_device_train_batch_size 2 \
            --gradient_accumulation_steps 1 \
            --learning_rate 2e-5 \
            --num_train_epochs 4 \
            --output_dir ./tmp/Part2_model4
        ```

- For Bonus:

    ```bash
    python QA.py \
        --model_name_or_path hfl/chinese-roberta-wwm-ext \
        --train_file ./DATA/train_bonus.json \
        --validation_file ./DATA/valid_bonus.json \
        --max_seq_length 512 \
        --per_device_train_batch_size 4 \
        --gradient_accumulation_steps 2 \
        --learning_rate 3e-5 \
        --num_train_epochs 5 \
    ```

## Step 3

- Execute the following to get the predicted result

```bash
./run.sh ./DATA/context.json ./DATA/test.json /path/to/pred/prediction.csv
```

> Might have to go into `./run.sh` to change the path of the MC/QA models
