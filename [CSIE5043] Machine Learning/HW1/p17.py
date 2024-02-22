import numpy as np

# Init data
data = np.loadtxt('hw1_train.dat', dtype=np.float64)

x = data[:, 0:-1].copy()
y = data[:, -1].copy()

# Init const.
N = x.shape[0]
M = 4*N
update = []
TOTAL_ITER = 1000

# Add x0 = 1
x = np.hstack((np.ones((x.shape[0], 1)), x))

# Scaling
x /= 2

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
            update_cnt += 1
            w += x[i] * y[i]
        else:
            cnt += 1
    update.append(update_cnt)
    return w

# Main function
for t in range(0, TOTAL_ITER):
    w = np.zeros(11)
    w = PLA(w, x, y, M)
update.sort()
print(update[TOTAL_ITER//2])
