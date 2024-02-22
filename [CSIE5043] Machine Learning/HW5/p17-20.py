from libsvm.svmutil import *
from libsvm import *
import numpy as np
import math

INF = 9999999

def sign(x):
    return 1 if x >= 0 else -1

def h(s, x, theta):
    return s*sign(x - theta)

def transform(X, Y, a, b):
    sz = len(Y)
    newY = []
    newX = []
    cnt = 0
    for i in range(sz):
        if Y[i] == a:
            newY.append(1)
            newX.append(X[i])
        elif Y[i] == b:
            cnt += 1
            newY.append(-1)
            newX.append(X[i])
    return newX, newY, cnt

def dict_to_list(data):
    keys = data[0].keys()
    newX = np.empty((len(data), len(keys)))

    for i, dict in enumerate(data):
        newX[i] = [dict[key] for key in keys]
    return newX

if __name__ == "__main__":
    # data preprocess
    Y, X = svm_read_problem('./letter.scale.tr')
    Test_Y, Test_X = svm_read_problem('./letter.scale.t')
    X, Y, cnt = transform(X, Y, 11, 26)
    Test_X, Test_Y, Test_cnt = transform(Test_X, Test_Y, 11, 26)
    X = dict_to_list(X)
    Test_X = dict_to_list(Test_X)
    data = np.hstack((X, np.reshape(Y, (-1, 1))))
    Test_data = np.hstack((Test_X, np.reshape(Test_Y, (-1, 1))))
    
    # data size
    N = len(data)
    Test_N = len(Test_data)
    Ein_small = []
    U = np.full(N, 1/N)
    Gin = np.full(N, 0.0)
    Gout = np.full(Test_N, 0.0)
    for t in range(1000):
        best_theta = -INF
        best_s = INF
        best_i = INF
        min_Ein_u = INF
        # decision stump
        for k in range(16):
            # sort by features i
            sorted_indices = np.argsort(data[:, k])
            U = U[sorted_indices]
            data = data[sorted_indices]
            Gin = Gin[sorted_indices]
            # get xi, y
            Xi = data[:,k].copy()
            y = data[:,-1].copy()

            # DP tables
            POS = [0.0 for i in range(N)]
            NEG = [0.0 for i in range(N)]

            # DP
            for i in range(N):
                if y[i] == 1:
                    NEG[0] += U[i]*1
                else:
                    POS[0] += U[i]*1
            for i in range(1, N):
                if(y[i-1] == -1):
                    POS[i] = POS[i-1] - U[i-1]*1
                    NEG[i] = NEG[i-1] + U[i-1]*1
                else:
                    POS[i] = POS[i-1] + U[i-1]*1
                    NEG[i] = NEG[i-1] - U[i-1]*1

            minPosIdx = np.argmin(POS)
            minNegIdx = np.argmin(NEG)
            if POS[minPosIdx] < min_Ein_u:
                min_Ein_u = POS[minPosIdx]
                best_s = 1
                best_i = k
                best_theta = (Xi[minPosIdx-1] + Xi[minPosIdx]) / 2 if minPosIdx!=0 else -INF
            if NEG[minNegIdx] < min_Ein_u:
                min_Ein_u = NEG[minNegIdx]
                best_s = -1
                best_i = k
                best_theta = (Xi[minNegIdx-1] + Xi[minNegIdx]) / 2 if minNegIdx!=0 else -INF

        # Calculate Ein_gt
        Ein = 0
        Xi = data[:,best_i].copy()
        y = data[:,-1].copy()
        Test_Xi = Test_data[:, best_i].copy()
        Test_y = Test_data[:, -1].copy()
        for i in range(N):
            pred_y = h(best_s, Xi[i], best_theta)
            if pred_y != y[i]:
                Ein += 1
        Ein_small.append(Ein/N)
        # Calculate epsilon_t
        U_sum = 0.0
        for i in range(N):
            U_sum += U[i]
        e = min_Ein_u / U_sum
        update_t = ((1-e)/e)**(1/2)
        # update U
        for i in range(N):
            pred_y = h(best_s, Xi[i], best_theta)
            if pred_y != y[i]:
                U[i] *= update_t
            else:
                U[i] /= update_t

        alpha = np.log(update_t)
        for i in range(N):
            pred_y = h(best_s, Xi[i], best_theta)
            Gin[i] += alpha*pred_y
        for i in range(Test_N):
            pred_Test_y = h(best_s, Test_Xi[i], best_theta)
            Gout[i] += alpha*pred_Test_y


    Ein_small.sort()
    print(Ein_small)
    print(f'min Ein_g: {Ein_small[np.argmin(Ein_small)]}')
    print(f'max Ein_g: {Ein_small[np.argmax(Ein_small)]}')

    # Err of G
    y = data[:,-1].copy()
    Test_y = Test_data[:, -1].copy()
    G_Ein = 0
    G_Eout = 0
    for i in range(N):
        if(sign(Gin[i]) != y[i]):
            G_Ein += 1
    for i in range(Test_N):
        if(sign(Gout[i]) != Test_y[i]):
            G_Eout += 1
    print(f'G_Ein: {G_Ein/N}')
    print(f'G_Eout: {G_Eout/N}')