#include "choose.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <bitset>
#include <string>

namespace {
    const int MAXN = 500;
    int a, b, n;
    unsigned int task[MAXN + 5], endtime[3];
    int counter;
    bool assigned[MAXN + 5];
    void wa(const std::string &msg) {
        printf("Wrong Answer: %s\n", msg.c_str());
        exit(0);
    }
    double verify() {
        for (int i = 1; i <= n; ++i)
            if (!assigned[i])
                wa("the tasks are not fully scheduled, missing task " + std::to_string(i) + ".");
        int sum = 0;
        static std::bitset<250005> dp;
        dp[0] = 1;
        for (int i = 1; i <= n; ++i) {
            dp = dp | (dp << task[i]);
            sum += task[i];
        }
        int opt = 1e9, ans = std::max(endtime[1], endtime[2]);
        for (int i = 0; i <= sum; ++i)
            if (dp[i])
                opt = std::min(opt, std::max(i, sum - i));
        if (ans * b > opt * a)
            wa("approximation failed. opt = " + std::to_string(opt) + " vs your scheduling = " + std::to_string(ans) + ".");
        dp.reset();
        return (double)ans / opt;
    }
}

int compare_task(int x, int y) {
    ++counter;
    if (counter > n * n) wa("too many queries.");
    if (x <= 0 || x > n) wa("invalid task id, " + std::to_string(x) + ".");
    if (y <= 0 || y > n) wa("invalid task id, " + std::to_string(y) + ".");
    if (x == y) return 0;
    int res;
    if (task[x] < task[y]) res = -1;
    else if (task[x] == task[y]) res = 0;
    else res = 1;
    return res;
}

int assign_task(int m, int x) {
    if (m <= 0 || m > 2) wa("invalid machine id, " + std::to_string(m) + ".");
    if (x <= 0 || x > n) wa("invalid task id, " + std::to_string(x) + ".");
    if (assigned[x]) wa("task " + std::to_string(x) + " has been scheduled before.");
    assigned[x] = m;
    endtime[m] += task[x];
    return endtime[m];
}

int main() {
    assert(3 == scanf("%d%d%d", &a, &b, &n));
    for (int i = 1; i <= n; ++i)
        assert(1 == scanf("%u", &task[i]));
    counter = 0;
    schedule(n);
    double res = verify();
    printf("Accepted with approximation ratio %.4lf.\n", res);
}
