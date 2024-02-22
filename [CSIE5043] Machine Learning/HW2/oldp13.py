import numpy as np

# variables
INF = 99999999
N = 2
TOTAL_ITER = 10000
tau = 0
s = 1
Theta = []
Data = []
Results = []

# functions
def f(x):
    return 1 if x>0 else -1 # sign(0) = -1

def h(s, theta, x):
    return s*f(x-theta)

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
        for i in range(N):
            x = rng.uniform(low=-0.5, high=0.5)
            y = flip(f(x), rng.random())
            Data.append((x, y))
        Data.sort()

        # Construct Theta
        Theta.append(-INF)
        for i in range(N-1):
            Theta.append((Data[i][0]+Data[i+1][0])/2)

        # Get g
        minEin = INF
        minS = INF
        minTheta = INF
        for theta in Theta:
            for s in range(1, 3):
                # Get current Ein(g)
                cnt = 0
                for data in Data:
                    if(h(s, theta, data[0]) != data[1]):
                        cnt += 1
                Ein = cnt / N
                # Update if needed
                if(Ein < minEin or Ein == minEin and theta*s < minS*minTheta):
                    minEin = Ein
                    minS = s
                    minTheta = theta

        # Calculate Eout-Ein
        Results.append( Eout(minS, minTheta)-minEin )

experiment()
print(np.mean(Results))

