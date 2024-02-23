# Step to train my model

## Step 1

- Create a folder named `./data` and place the training data `train.json` in it.
- First select 4000 data out for training from `train.json` by executing:

```bash
python3 preprocess.py
```

## Step 2

- Simply run the following script to train my model

```bash
accelerate launch -m axolotl.cli.train ./train.yml
```

- Simply run the following script to train my bonus model

```bash
accelerate launch -m axolotl.cli.train ./train_bonus.yml
```

- It then will train a model located at `"./trained_model`

## Step 3

- For predicting, pick the model from `checkpoint 77` and place it in the folder `adapter_checkpoint`
