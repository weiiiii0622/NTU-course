import numpy as np

# Init data
Traindata = np.loadtxt('hw3_train.dat', dtype=np.float64)
Testdata = np.loadtxt('hw3_test.dat', dtype=np.float64)
X = Traindata[:, 0:-1].copy()
Y = Traindata[:, -1].copy()
Test_X = Testdata[:, 0:-1].copy()
Test_Y = Testdata[:, -1].copy()

# Add x0,n = 1
X = np.hstack((np.ones((X.shape[0], 1)), X))
Test_X = np.hstack((np.ones((Test_X.shape[0], 1)), Test_X))

# variables
N = X.shape[0]
d = X.shape[1]
Test_N = Test_X.shape[0]
eta = 0.001
TOTAL_EXP = 1000
TOTAL_ITER = 800

# wlin
pinvX = np.linalg.pinv(X)

# logistic function
def logistic(x):
    return 1 / (1 + np.exp(-x))

# SGD with Linear Regression
def SGD_LIN():
    # wlin
    w = np.dot(pinvX, Y)
    rng = np.random.default_rng()
    for i in range(TOTAL_ITER):
        idx = rng.integers(0, N)
        x = X[idx]
        y = Y[idx]
        
        w = w + eta*logistic(-1*y*np.dot(w, x))*y*x
    Ein = 0
    for i in range(N):
        if np.sign(np.dot(w, X[i])) != Y[i]:
            Ein += 1
    Eout = 0
    for i in range(Test_N):
        if np.sign(np.dot(w, Test_X[i])) != Test_Y[i]:
            Eout += 1
    return abs(Ein/N - Eout/Test_N)

# Get Ein
Ein = 0
for i in range(TOTAL_EXP):
    Ein += SGD_LIN()
print(Ein / TOTAL_EXP)