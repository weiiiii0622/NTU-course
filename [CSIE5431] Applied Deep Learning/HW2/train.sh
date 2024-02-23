python jsonl_to_json.py \
    --jsonl_path ./HW2_data/train.jsonl \
    --json_path ./HW2_data/train.json

python jsonl_to_json.py \
    --jsonl_path ./HW2_data/public.jsonl \
    --json_path ./HW2_data/public.json

python trainer.py \
    --seed 1006 \
    --model_name_or_path google/mt5-small \
    --train_file ./HW2_data/train.json \
    --validation_file ./HW2_data/public.json \
    --source_prefix "summarize: " \
    --text_column maintext \
    --summary_column title \
    --max_source_length 256 \
    --max_target_length 64 \
    --pad_to_max_length \
    --num_train_epochs 30 \
    --per_device_train_batch_size 4 \
    --gradient_accumulation_steps 2 \
    --num_beams 3 \
    --output_dir ./HW2_model_train