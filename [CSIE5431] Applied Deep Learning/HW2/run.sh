INPUT_PATH="$1"
OUTPUT_PATH="$2"
MODEL_PATH="./HW2_model"

python jsonl_to_json.py \
    --jsonl_path "$INPUT_PATH" \
    --json_path ./predict.json

python predict.py \
    --model_path "$MODEL_PATH" \
    --test_file ./predict.json \
    --submission_file "$OUTPUT_PATH" \
    --source_prefix "summarize: " \
    --text_column maintext \
    --summary_column title \
    --pad_to_max_length \
    --max_source_length 256 \
    --max_target_length 64 \
    --num_beams 3 \
    --per_device_test_batch_size 16