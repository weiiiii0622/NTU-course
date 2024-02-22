import numpy as np

# Init data
Traindata = np.loadtxt('hw2_train.dat', dtype=np.float64)
Testdata = np.loadtxt('hw2_test.dat', dtype=np.float64)
X = Traindata[:, 0:-1].copy()
Y = Traindata[:, -1].copy()
Test_X = Testdata[:, 0:-1].copy()
Test_Y = Testdata[:, -1].copy()

# variables
INF = 9999999
N = X.shape[0]
d = X.shape[1]
Test_N = Test_X.shape[0]
TOTAL_ITER = 10000
tau = 0
Data = []
Results = []
S = []
Theta = []


# DP tables
POS = [0 for i in range(N)]
NEG = [0 for i in range(N)]

# functions
def f(x):
    return 1 if x>0 else -1 # sign(0) = -1

def h(s, theta, x):
    return s*f(x-theta)

def experiment(d):
    tmpResults = []
    tmpS = []
    tmpTheta = []
    for iter in range(TOTAL_ITER):
        rng = np.random.default_rng()

        # Construct Data
        Data.clear()
        cnt = 0
        for i in range(N):
            x = X[i][d]
            y = Y[i]
            if(y==-1):
                cnt += 1
            Data.append((x, y))
        Data.sort()

        # INIT DP table
        POS[0] = cnt
        NEG[0] = N-cnt

        # Get G by DP
        for i in range(1, N):
            if(Data[i-1][1] == -1):
                POS[i] = POS[i-1] - 1
                NEG[i] = NEG[i-1] + 1
            else:
                POS[i] = POS[i-1] + 1
                NEG[i] = NEG[i-1] - 1

        minPosIdx = np.argmin(POS)
        minNegIdx = np.argmin(NEG)
        EinPos = POS[minPosIdx] / N
        EinNeg = NEG[minNegIdx] / N
        minPosTheta =  (Data[minPosIdx-1][0] + Data[minPosIdx][0]) / 2 if minPosIdx!=0 else -INF
        minNegTheta =  (Data[minNegIdx-1][0] + Data[minNegIdx][0]) / 2 if minNegIdx!=0 else -INF
        
        if(EinPos == EinNeg):
            if(1*minPosTheta < (-1)*minNegTheta):
                minEin = POS[minPosIdx] / N
                tmpResults.append( minEin )
                tmpS.append(1)
                tmpTheta.append(minPosTheta)
            else:
                minEin = NEG[minNegIdx] / N
                Results.append( minEin )
                tmpS.append(-1)
                tmpTheta.append(minNegTheta)
        elif(EinPos < EinNeg):
            minEin = POS[minPosIdx] / N
            tmpResults.append( minEin )
            tmpS.append(1)
            tmpTheta.append(minPosTheta)
        else:
            minEin = NEG[minNegIdx] / N
            tmpResults.append( minEin )
            tmpS.append(-1)
            tmpTheta.append(minNegTheta)
    Results.append(tmpResults[np.argmin(tmpResults)])
    S.append(tmpS[np.argmin(tmpResults)])
    Theta.append(tmpTheta[np.argmin(tmpResults)])


for i in range(d):
    experiment(i)

minEinIdx = np.argmin(Results)
maxEinIdx = np.argmax(Results)
minS = S[minEinIdx]
minTheta = Theta[minEinIdx]
maxS = S[maxEinIdx]
maxTheta = Theta[maxEinIdx]

minCnt = 0
maxCnt = 0
for j in range(Test_N):
    if(h(minS, minTheta, Test_X[j][minEinIdx]) != Test_Y[j]):
        minCnt += 1
    if(h(maxS, maxTheta, Test_X[j][maxEinIdx]) != Test_Y[j]):
        maxCnt += 1
print(maxCnt/Test_N - minCnt/Test_N)