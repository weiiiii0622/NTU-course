import numpy as np

# Init data
data = np.loadtxt('hw2_train.dat', dtype=np.float64)
X = data[:, 0:-1].copy()
Y = data[:, -1].copy()

# variables
INF = 9999999
N = X.shape[0]
d = X.shape[1]
TOTAL_ITER = 10000
tau = 0
Data = []
Results = []

# DP tables
POS = [0 for i in range(N)]
NEG = [0 for i in range(N)]

# functions
def f(x):
    return 1 if x>0 else -1 # sign(0) = -1

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

for i in range(d):
    experiment(i)

print(Results[np.argmax(Results)] - Results[np.argmin(Results)])