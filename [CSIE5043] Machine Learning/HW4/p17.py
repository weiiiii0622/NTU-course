from liblinear.liblinearutil import *
import numpy as np

# Init data
Traindata = np.loadtxt('hw4_train.dat', dtype=np.float64)

rng = np.random.default_rng()
X = Traindata[:, 0:-1].copy()
Y = Traindata[:, -1].copy()

# variables 
lamb = [-6, -3, 0, 3, 6]
N = X.shape[0]
d = X.shape[1]
TOTAL_ITER = 256
FOLD = 40

# fourth-order transform
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
    X = Traindata[:, 0:-1].copy()
    Y = Traindata[:, -1].copy()
    
    X = transfrom(X, Q, N)
    # Add x0,n = 1
    X = np.hstack((np.ones((X.shape[0], 1)), X))

    optLamb = -1
    optEcv = 100
    for x in lamb:
        curEcv = 0
        C = 1 / (2*pow(10, x))
        for i in range(5):
            startIDX = FOLD*i
            endIDX   = FOLD*(i+1)
            cur_X = np.concatenate((X[0:startIDX, :], X[endIDX:, :]), axis=0)
            cur_Y = np.concatenate((Y[0:startIDX], Y[endIDX:]), axis=0)
            val_X = X[startIDX:endIDX, :]
            val_Y = Y[startIDX:endIDX]
            
            m = train(cur_Y, cur_X, '-s 0 -c '+str(C)+' -e 0.000001')
            p_label, p_acc, p_val = predict(val_Y, val_X, m)
            curEcv += (100-p_acc[0])
        
        curEcv /= 5
        if curEcv < optEcv or (curEcv == optEcv and x > optLamb):
            optLamb = x
            optEcv = curEcv

    return optEcv/100

Ecv = 0
for t in range(TOTAL_ITER):
    Ecv += experiment()
print(Ecv / TOTAL_ITER)