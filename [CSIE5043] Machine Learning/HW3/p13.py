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

# pseudo Inverse
pinvX = np.linalg.pinv(X)

# wlin
w = np.dot(pinvX, Y)

# get Ein
error = 0
for i in range(N):
    error += (np.dot(w, X[i]) - Y[i])**2
print(error/N)