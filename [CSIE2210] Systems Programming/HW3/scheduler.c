#include "threadtools.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#define DEBUG 0

/*
 * Print out the signal you received.
 * If SIGALRM is received, reset the alarm here.
 * This function should not return. Instead, call longjmp(sched_buf, 1).
 */
void sighandler(int signo) {
    // TODO
    if(signo == SIGALRM){
        printf("caught SIGALRM\n");
        alarm(timeslice);
    }
    else if(signo == SIGTSTP){
        printf("caught SIGTSTP\n");
    }
    sigprocmask(SIG_SETMASK, &base_mask, NULL);
    longjmp(sched_buf, 1);
}

/*
 * Prior to calling this function, both SIGTSTP and SIGALRM should be blocked.
 */

// 1: from sighandler triggered by thread_yeild
// 2: from async_read
// 3: from thread_exit
void scheduler() {
    // TODO
    int STATUS = setjmp(sched_buf);
    if(DEBUG) printf("rq_size: %d ", rq_size);
    if(DEBUG) printf("wq_size: %d ", wq_size);
    if(DEBUG) printf("rq_current: %d\n", rq_current);
    if(STATUS == 0){
        longjmp(RUNNING->environment, 1);
    }

    // Get Ready Thread
    int ret = getReadyThread();
    //if(DEBUG) printf("hole_cnt: %d\n", ret);

    if(STATUS == 1){
        if(DEBUG) printf("id: %d Yield.\n", RUNNING->id);
        rq_current = (rq_current+1)%rq_size;
    }
    else if(STATUS == 2){
        if(DEBUG) printf("id: %d AsyncRead.\n", RUNNING->id);
        waiting_queue[wq_size++] = RUNNING;
        ready_queue[rq_current] = ready_queue[rq_size-1];
        rq_size -= 1;
        if(rq_current >= rq_size) rq_current = 0;
        //if(rq_size <= 0) getReadyThread();
    }
    else if(STATUS == 3){
        if(DEBUG) printf("id: %d Exit.\n", RUNNING->id);
        free(RUNNING);
        ready_queue[rq_current] = ready_queue[rq_size-1];
        rq_size -= 1;
        // if(rq_size <= 0){
        //     getReadyThread();
        //     rq_current = 0;
        // }
        if(rq_current >= rq_size) rq_current = 0;
        // End of Scheduling
        if(rq_size == 0 && wq_size == 0){
            return;
        }
    }

    if(rq_size <= 0){
        getReadyThread();
        rq_current = 0;
    }

    // Jump Back to Thread
    if(DEBUG) printf("Jump to id: %d\n", RUNNING->id);
    longjmp(RUNNING->environment, 1);
}

int getReadyThread(){
    if(wq_size <= 0){
        if(DEBUG) printf("No waiting queue\n");
        return -1;
    }
    // Extract Ready
    int maxfd = -1;
    fd_set work_set, main_set;
    FD_ZERO(&main_set);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if(DEBUG) printf("ORIGINAL waiting_queue: ");
    for(int i=0; i<wq_size; i++){
        if(waiting_queue[i] != NULL){
            if(DEBUG) printf("%d ", waiting_queue[i]->id);
            FD_SET(waiting_queue[i]->fd, &main_set);
            maxfd = maxfd > waiting_queue[i]->fd ? maxfd : waiting_queue[i]->fd;
        }
    }
    if(DEBUG) printf("\n");

    int isReady[maxfd+1];
    int getThread[maxfd+1];
    for(int i=0; i<wq_size; i++){
        if(waiting_queue[i] != NULL){
            getThread[waiting_queue[i]->fd] = i;
        }
    }
    while(1){
        memcpy(&work_set, &main_set, sizeof(main_set));
        int n;
        if(rq_size <= 0) n = select(maxfd+1, &work_set, NULL, NULL, NULL);
        else n = select(maxfd+1, &work_set, NULL, NULL, &timeout);
        if(n <= 0){
            if(rq_size <= 0) continue;
            else {
                if(DEBUG) printf("Break from not finding ready thread.\n");
                return -1;
            }
        }
        for(int i=0; i<=maxfd; i++){
            isReady[i] = 0;
            if(FD_ISSET(i, &work_set)){
                isReady[getThread[i]] = 1;
            }
        }
        break;
    }
    if(DEBUG) printf("IsREADY: ");
    // Append to ready queue
    int hole_cnt = 0;
    for(int i=0; i<wq_size; i++){
        if(isReady[i] == 1){
            if(DEBUG) printf(" %d ", waiting_queue[i]->id);
            hole_cnt += 1;
            ready_queue[rq_size++] = waiting_queue[i];
            waiting_queue[i] = NULL;
        }
    }
    if(DEBUG) printf("\n");
    if(DEBUG) printf("Hole: %d\n", hole_cnt);

    // Fill hole in waiting queue
    for(int i=0; i<wq_size; i++){
        if(waiting_queue[i] != NULL){
            int j=i;
            while(j-1>=0 && waiting_queue[j-1]==NULL){
                struct tcb* tmp = waiting_queue[j];
                waiting_queue[j] = waiting_queue[j-1];
                waiting_queue[j-1] = tmp;
                j -= 1;
            }
        }
    }
    wq_size -= hole_cnt;
    if(DEBUG) printf("NEW waiting_queue: ");
    for(int i=0; i<wq_size; i++){
        if(DEBUG) printf(" %d", waiting_queue[i]->id);
    }
    if(DEBUG) printf("\n");

    return hole_cnt;
}
