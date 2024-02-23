CONTEXT_PATH="$1"
TEST_PATH="$2"
SUBMISSION_PATH="$3"
PART1_DATA_PATH="./test_part1.json"
PART2_DATA_PATH="./test_part2.json"
MODEL_MC_PATH="./MC_MODEL"
MODEL_QA_NUM=4
MODEL_QA1_PATH="./QA_MODEL1"
MODEL_QA2_PATH="./QA_MODEL2"
MODEL_QA3_PATH="./QA_MODEL3"
MODEL_QA4_PATH="./QA_MODEL4"

export CUDA_VISIBLE_DEVICES=0

python3 predict.py \
  --do_mc \
  --do_qa \
  --model_mc_path "$MODEL_MC_PATH" \
  --model_qa_num "$MODEL_QA_NUM" \
  --model_qa1_path "$MODEL_QA1_PATH" \
  --model_qa2_path "$MODEL_QA2_PATH" \
  --model_qa3_path "$MODEL_QA3_PATH" \
  --model_qa4_path "$MODEL_QA4_PATH" \
  --context_path "$CONTEXT_PATH" \
  --part1_data_path "$PART1_DATA_PATH" \
  --part2_data_path "$PART2_DATA_PATH" \
  --test_path "$TEST_PATH" \
  --submission_path "$SUBMISSION_PATH"