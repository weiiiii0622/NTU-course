# Step to train my model

## Step 1

- Simply run the following bash script to train my model

```bash
./train.sh
```

- It then will train a model located at `"./HW2_model_train"`
- It basically converts datas in jsonl format to json format, then feed it into the model to start training.
- To predict with it, simply change the `"MODEL_PATH"` variable in `"./run.sh"` to `"./HW2_model_train"`
