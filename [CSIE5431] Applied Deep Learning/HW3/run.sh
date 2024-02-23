TW_LLaMa_PATH="$1"
ADAPTER_CHECKPOINT_PATH="$2"
INPUT_PATH="$3"
OUTPUT_PATH="$4"

python predict.py \
    --base_model_path "$TW_LLaMa_PATH" \
    --peft_path "$ADAPTER_CHECKPOINT_PATH" \
    --test_data_path "$INPUT_PATH" \
    --output_path "$OUTPUT_PATH"