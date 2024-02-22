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

    Y = transform(Y, 1.0)
    Test_Y = transform(Test_Y, 1.0)

    m = svm_train(Y, X, '-s 0 -t 0 -c 1')

    nr_sv = m.get_nr_sv()
    sv = m.get_SV()
    coef = m.get_sv_coef()
    W = [0 for i in range(16)]
    #print(sv[0][16])
    for i in range(nr_sv):
        c = coef[i][0]
        for j in range(1, 17):
            W[j-1] += c * sv[i][j]
    
    ans = 0
    for w in W:
        ans += w*w
    print(ans**(1/2))