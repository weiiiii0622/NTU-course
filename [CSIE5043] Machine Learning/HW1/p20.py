import numpy as np

# Init data
data = np.loadtxt('hw1_train.dat', dtype=np.float64)

x = data[:, 0:-1].copy()
y = data[:, -1].copy()

# Init const.
N = x.shape[0]
M = 4*N
w0x0 = []
TOTAL_ITER = 1000

# Add x0 = 0.1126
x = np.hstack((np.full((x.shape[0], 1), 0.1126), x))

# PLA
def PLA(w, x, y, M):
    update_cnt = 0
    cnt = 0
    rng = np.random.default_rng()
    while(cnt < M):
        i = rng.integers(0, N)
        res = 1 if np.dot(w, x[i])>=0 else -1

        if(res != y[i]):
            cnt = 0
            w += x[i] * y[i]
        else:
            cnt += 1
    return w

# Main function
for t in range(0, TOTAL_ITER):
    w = np.zeros(11)
    w = PLA(w, x, y, M)
    for i in range(0, N):
        w0x0.append(w[0]*x[i, 0])
#w0x0.sort()
#print(w0x0[TOTAL_ITER//2])
print(np.median(w0x0))
