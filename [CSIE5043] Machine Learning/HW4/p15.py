from liblinear.liblinearutil import *
import numpy as np

# Init data
Traindata = np.loadtxt('hw4_train.dat', dtype=np.float64)
Testdata = np.loadtxt('hw4_test.dat', dtype=np.float64)

rng = np.random.default_rng()
X = Traindata[:120, 0:-1].copy()
Y = Traindata[:120, -1].copy()
Val_X = Traindata[120:, 0:-1].copy()
Val_Y = Traindata[120:, -1].copy()
Test_X = Testdata[:, 0:-1].copy()
Test_Y = Testdata[:, -1].copy()

# variables 
lamb = [-6, -3, 0, 3, 6]
N = X.shape[0]
d = X.shape[1]
Val_N = Val_X.shape[0]
Test_N = Test_X.shape[0]
TOTAL_ITER = 256

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

def experiment():
    rng.shuffle(Traindata)
    X = Traindata[:120, 0:-1].copy()
    Y = Traindata[:120, -1].copy()
    Val_X = Traindata[120:, 0:-1].copy()
    Val_Y = Traindata[120:, -1].copy()
    Test_X = Testdata[:, 0:-1].copy()
    Test_Y = Testdata[:, -1].copy()
    
    X = transfrom(X, Q, N)
    Val_X = transfrom(Val_X, Q, Val_N)
    Test_X = transfrom(Test_X, Q, Test_N)
    # Add x0,n = 1
    X = np.hstack((np.ones((X.shape[0], 1)), X))
    Val_X = np.hstack((np.ones((Val_X.shape[0], 1)), Val_X))
    Test_X = np.hstack((np.ones((Test_X.shape[0], 1)), Test_X))

    optLamb = -1
    optEout = 100
    for i in range(5):
        x = lamb[i]
        C = 1 / (2*pow(10, x))
        m = train(Y, X, '-s 0 -c '+str(C)+' -e 0.000001')
        p_label, p_acc, p_val = predict(Val_Y, Val_X, m)
        if 100 - p_acc[0] < optEout or (100 - p_acc[0] == optEout and i > optLamb):
            optLamb = i
            optEout = 100 - p_acc[0]
    
    # Caculate Eout
    x = lamb[optLamb]
    C = 1 / (2*pow(10, x))
    m = train(Y, X, '-s 0 -c '+str(C)+' -e 0.000001')
    p_label, p_acc, p_val = predict(Test_Y, Test_X, m)
    return (100-p_acc[0])/100

Eout = 0
for t in range(TOTAL_ITER):
    Eout += experiment()
print(Eout / TOTAL_ITER)