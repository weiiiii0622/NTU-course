import numpy as np

# Init data
Traindata = np.loadtxt('hw3_train.dat', dtype=np.float64)
Testdata = np.loadtxt('hw3_test.dat', dtype=np.float64)
X = Traindata[:, 0:-1].copy()
Y = Traindata[:, -1].copy()
Test_X = Testdata[:, 0:-1].copy()
Test_Y = Testdata[:, -1].copy()


# Transfrom Function
def transform(X, Q):           
    new_X = X
    for q in range(2, Q+1):
        new_X = np.hstack((new_X, np.power(X, q)))
    return new_X

Q = 8
X = transform(X, Q)
Test_X = transform(Test_X, Q)

# Add x0,n = 1
X = np.hstack((np.ones((X.shape[0], 1)), X))
Test_X = np.hstack((np.ones((Test_X.shape[0], 1)), Test_X))

# variables
N = X.shape[0]
d = X.shape[1]
Test_N = Test_X.shape[0]

# wlin
pinvX = np.linalg.pinv(X)
w = np.dot(pinvX, Y)


def sign(x):
    return 1 if x>=0 else -1

Ein = 0
for i in range(N):
    if np.sign(np.dot(w, X[i])) != Y[i]:
        Ein += 1
Eout = 0
for i in range(Test_N):
    if np.sign(np.dot(w, Test_X[i])) != Test_Y[i]:
        Eout += 1

print(abs(Ein/N - Eout/Test_N))