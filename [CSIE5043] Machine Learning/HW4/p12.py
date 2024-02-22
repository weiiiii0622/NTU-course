from liblinear.liblinearutil import *
import numpy as np

# Init data
Traindata = np.loadtxt('hw4_train.dat', dtype=np.float64)
Testdata = np.loadtxt('hw4_test.dat', dtype=np.float64)
X = Traindata[:, 0:-1].copy()
Y = Traindata[:, -1].copy()
Test_X = Testdata[:, 0:-1].copy()
Test_Y = Testdata[:, -1].copy()

# variables 
lamb = [-6, -3, 0, 3, 6]
optLamb = -1
optEout = 100
N = X.shape[0]
d = X.shape[1]
Test_N = Test_X.shape[0]

# fourth-order transform
N = X.shape[0]
d = X.shape[1]
Q = 4
cntTable = [[] for i in range(Q)]
for i in range(d):
    cntTable[0].append(1)
for q in range(1, Q):
    curSum = 0
    for i in range(d-1, -1, -1):
        curSum += cntTable[q-1][i]
        cntTable[q].insert(0, curSum)
idxTable = [[] for i in range(Q)]
for q in range(Q):
    idxTable[q].append(0)
for q in range(Q):
    for i in range(1, d):
        idxTable[q].append(cntTable[q][i-1] + idxTable[q][i-1])
TOTAL = [idxTable[i][-1]+1 for i in range(Q)]

def transfrom(X, Q, N):
    curX = X.copy()
    resX = X.copy()
    for q in range(1, Q):
        NewX = np.empty((N, 0))
        for i in range(d):
            for j in range(idxTable[q-1][i], TOTAL[q-1]):
                newCol = np.multiply(X[:, i], curX[:, j])
                NewX = np.hstack((NewX,  np.atleast_2d(newCol).T))
        curX = NewX.copy()
        resX = np.hstack((resX, NewX))
    return resX

X = transfrom(X, Q, N)
Test_X = transfrom(Test_X, Q, Test_N)

# Add x0,n = 1
X = np.hstack((np.ones((X.shape[0], 1)), X))
Test_X = np.hstack((np.ones((Test_X.shape[0], 1)), Test_X))

# main
for x in lamb:
    C = 1 / (2*pow(10, x))
    m = train(Y, X, '-s 0 -c '+str(C)+' -e 0.000001')
    p_label, p_acc, p_val = predict(Test_Y, Test_X, m)
    if 100 - p_acc[0] < optEout or (100 - p_acc[0] == optEout and x > optLamb):
        optLamb = x
        optEout = 100 - p_acc[0]

print("opt lambda: ", optLamb, " Eout: ",  optEout/100)