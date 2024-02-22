from libsvm.svmutil import *
from libsvm import *
from sklearn.model_selection import train_test_split
import numpy as np

def transform(Y, k):
    sz = len(Y)
    for i in range(sz):
        if Y[i] != k:
            Y[i] = -1
        else:
            Y[i] = 1
    return Y

if __name__ == "__main__":
    cnt = [0, 0, 0, 0, 0]
    tgt_gamma = [0.1, 1, 10, 100, 1000]
    rng = np.random.default_rng()
    for iter in range(500):
        seed = rng.integers(0, 2**32)
        Y, X = svm_read_problem('./letter.scale.tr')
        X, Val_X, Y, Val_Y = train_test_split(X, Y, test_size=200, random_state=seed)
        Y = transform(Y, 7)
        Val_Y = transform(Val_Y, 7)

        opt_gamma = -1
        opt_Eval = 999
        for i in range(5):
            m = svm_train(Y, X, f'-s 0 -t 2 -g {tgt_gamma[i]} -c 0.1 -q')
            p_labs, p_acc, p_vals = svm_predict(Val_Y, Val_X, m)
            
            Eval = (100-p_acc[0])/100
            if(Eval < opt_Eval):
                opt_gamma = i
                opt_Eval = Eval
        cnt[opt_gamma] += 1

    for i in range(5):
        print(f"gamma: {tgt_gamma[i]} / cnt:{cnt[i]}")