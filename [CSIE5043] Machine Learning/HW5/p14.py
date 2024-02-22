from libsvm.svmutil import *
from libsvm import *
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
    Y, X = svm_read_problem('./letter.scale.tr')
    Test_Y, Test_X = svm_read_problem('./letter.scale.t')

    Y = transform(Y, 7)
    Test_Y = transform(Test_Y, 7)

    tgt_C = [0.01, 0.1, 1, 10, 100]
    for c in tgt_C:
        m = svm_train(Y, X, f'-s 0 -t 2 -g 1 -c {c} -q')
        p_labs, p_acc, p_vals = svm_predict(Test_Y, Test_X, m)
        print(f"c: {c} / Eout:{((100-p_acc[0]) / 100)}")