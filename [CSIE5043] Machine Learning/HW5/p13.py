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
    
    target = [2.0, 3.0, 4.0, 5.0, 6.0]
    for tgt in target:
        Y, X = svm_read_problem('./letter.scale.tr')
        Test_Y, Test_X = svm_read_problem('./letter.scale.t')

        Y = transform(Y, tgt)
        Test_Y = transform(Test_Y, tgt)

        m = svm_train(Y, X, '-s 0 -t 1 -d 2 -g 1 -r 1 -c 1')