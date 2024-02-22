import numpy as np

# Init data
Traindata = np.loadtxt('hw3_train.dat', dtype=np.float64)
X = Traindata[:, 0:-1].copy()
Y = Traindata[:, -1].copy()

# Add x0,n = 1
X = np.hstack((np.ones((X.shape[0], 1)), X))

# variables
N = X.shape[0]
d = X.shape[1]
eta = 0.001
TOTAL_EXP = 1000
TOTAL_ITER = 800

# SGD with Linear Regression
def SGD_LIN():
    w = np.zeros(d)
    rng = np.random.default_rng()
    for i in range(TOTAL_ITER):
        idx = rng.integers(0, N)
        x = X[idx]
        y = Y[idx]
        error = y - np.dot(w, x)
        w = w + eta*2*error*x
    Ein = 0
    for i in range(N):
        Ein += (np.dot(w, X[i]) - Y[i])**2
    return Ein/N

# Get Ein
Ein = 0
for i in range(TOTAL_EXP):
    Ein += SGD_LIN()
print(Ein / TOTAL_EXP)