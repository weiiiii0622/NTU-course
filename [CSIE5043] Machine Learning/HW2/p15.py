import numpy as np

# variables
INF = 9999999
N = 2
TOTAL_ITER = 10000
tau = 0.20
Data = []
Results = []

# DP tables
POS = [0 for i in range(N)]
NEG = [0 for i in range(N)]

# functions
def f(x):
    return 1 if x>0 else -1 # sign(0) = -1

def flip(x, p):
    return -x if p<tau else x

def Eout(s, theta):
    if(s==1):
        return ( min(abs(theta), 0.5)*(1-2*tau) + tau )
    else:
        return ( 1 - min(abs(theta), 0.5) - tau + 2*min(abs(theta), 0.5)*tau )

def experiment():
    for iter in range(TOTAL_ITER):
        rng = np.random.default_rng()

        # Construct Data
        Data.clear()
        cnt = 0
        for i in range(N):
            x = rng.uniform(low=-0.5, high=0.5)
            y = flip(f(x), rng.random())
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
                Results.append( Eout(1, minPosTheta)- minEin )
            else:
                minEin = NEG[minNegIdx] / N
                Results.append( Eout(-1, minNegTheta)- minEin )
        elif(EinPos < EinNeg):
            minEin = POS[minPosIdx] / N
            Results.append( Eout(1, minPosTheta)- minEin )
        else:
            minEin = NEG[minNegIdx] / N
            Results.append( Eout(-1, minNegTheta)- minEin )

experiment()
print(np.mean(Results))