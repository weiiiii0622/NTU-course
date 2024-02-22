#include "choose.h"
#include <algorithm>

/*
Feel free to use global variables.
However, if you want to write your code more "correctly",
you can search for the keyword "anonymous namespace", a helpful tool in C++.
*/

int jobs[505];

struct Cmp{
    bool operator()(const int &lhs, const int &rhs) const{
        if(compare_task(lhs, rhs)>0){
            return 1;
        }
        return 0;
    }
};

// this is an example usage
void schedule(int N) {
    for(int i=1; i<=N; i++){
        jobs[i] = i;
    }
    std::sort(jobs+1, jobs+N+1, Cmp());

    unsigned int endtime[3];
    if(N==1){
        endtime[1] = assign_task(1,jobs[1]);
        return;
    }
    endtime[1] = assign_task(1, jobs[1]);
    endtime[2] = assign_task(2, jobs[2]);
    for(int i=3; i<=N; i++){
        if(endtime[1] <= endtime[2]){
            endtime[1] = assign_task(1, jobs[i]);
        }
        else{
            endtime[2] = assign_task(2, jobs[i]);
        }
    }
    return;
}

