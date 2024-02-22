import numpy as np

# Init data
data = np.loadtxt('hw1_train.dat', dtype=np.float64)

x = data[:, 0:-1].copy()
y = data[:, -1].copy()

# Init const.
N = x.shape[0]
M = N/2
TOTAL_ITER = 1000

# Add x0 = 1
x = np.hstack((np.ones((x.shape[0], 1)), x))

# PLA
def PLA(w, x, y, M):
    cnt = 0
    rng = np.random.default_rng()
    while(cnt < M):
        i = rng.integers(0, N)
        #i = random.randint(0, N-1)
        res = 1 if np.dot(w, x[i])>=0 else -1

        if(res != y[i]):
            cnt = 0
            w += x[i] * y[i]
        else:
            cnt += 1
    return w

# Main function
Ein = 0
for t in range(0, TOTAL_ITER):
    cnt = 0
    w = np.zeros(11)
    w = PLA(w, x, y, M)
    for i in range(0, N):
        res = 1 if np.dot(w, x[i])>=0 else -1
        if res != y[i]:
            cnt += 1
    Ein += cnt/(N)
print(Ein/TOTAL_ITER)