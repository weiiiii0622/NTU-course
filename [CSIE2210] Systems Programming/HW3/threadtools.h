#ifndef THREADTOOL
#define THREADTOOL
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
#define THREAD_MAX 16  // maximum number of threads created
#define BUF_SIZE 512
struct tcb {
    int id;  // the thread id
    jmp_buf environment;  // where the scheduler should jump to
    int arg;  // argument to the function
    int fd;  // file descriptor for the thread
    char fifoName[512];
    char buf[BUF_SIZE];  // buffer for the thread
    int i, x, y;  // declare the variables you wish to keep between switches
};

extern int timeslice;
extern jmp_buf sched_buf;
extern struct tcb *ready_queue[THREAD_MAX], *waiting_queue[THREAD_MAX];
/*
 * rq_size: size of the ready queue
 * rq_current: current thread in the ready queue
 * wq_size: size of the waiting queue
 */
extern int rq_size, rq_current, wq_size;
/*
* base_mask: blocks both SIGTSTP and SIGALRM
* tstp_mask: blocks only SIGTSTP
* alrm_mask: blocks only SIGALRM
*/
extern sigset_t base_mask, tstp_mask, alrm_mask;
/*
 * Use this to access the running thread.
 */
#define RUNNING (ready_queue[rq_current])

void sighandler(int signo);
void scheduler();
int getReadyThread();

#define thread_create(func, id, arg) {\
    func(id, arg);\
}

// Create FIFO [thread id]_[function name] & open with NONBLOCKING mode
// Call setjump
#define thread_setup(id, arg) {\
    struct tcb* newTCB = (struct tcb*)malloc(sizeof(struct tcb));\
    newTCB->id = id;\
    newTCB->arg = arg;\
    sprintf(newTCB->fifoName, "%d_%s", id, __func__);\
    mkfifo(newTCB->fifoName, 0777);\
    newTCB->fd = open(newTCB->fifoName, O_RDWR|O_NONBLOCK);\
    ready_queue[rq_size++] = newTCB;\
    if(setjmp(newTCB->environment) == 0){\
        return;\
    }\
}

// Remove FIFO & Jump to scheduler
#define thread_exit() {\
    close(RUNNING->fd);\
    unlink(RUNNING->fifoName);\
    longjmp(sched_buf, 3);\
}

#define thread_yield() {\
    if(setjmp(RUNNING->environment) == 0){\
        sigprocmask(SIG_SETMASK, &alrm_mask, NULL);\
        sigprocmask(SIG_SETMASK, &tstp_mask, NULL);\
        sigprocmask(SIG_SETMASK, &base_mask, NULL);\
    }\
    continue;\
}

#define async_read(count) ({\
    if(setjmp(RUNNING->environment)==0){\
        longjmp(sched_buf, 2);\
    }\
    read(RUNNING->fd, RUNNING->buf, count);\
})

#endif // THREADTOOL
