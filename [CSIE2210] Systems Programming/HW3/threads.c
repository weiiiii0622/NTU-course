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

void fibonacci(int id, int arg) {
    thread_setup(id, arg);

    for (RUNNING->i = 1; ; RUNNING->i++) {
        if (RUNNING->i <= 2)
            RUNNING->x = RUNNING->y = 1;
        else {
            /* We don't need to save tmp, so it's safe to declare it here. */
            int tmp = RUNNING->y;  
            RUNNING->y = RUNNING->x + RUNNING->y;
            RUNNING->x = tmp;
        }
        printf("%d %d\n", RUNNING->id, RUNNING->y);
        sleep(1);

        if (RUNNING->i == RUNNING->arg) {
            thread_exit();
        }
        else {
            thread_yield();
        }
    }
}

void collatz(int id, int arg) {
    // TODO
    thread_setup(id, arg);

    for(RUNNING->i = 1; ; RUNNING->i++){
        if(RUNNING->arg != 1){
            if(RUNNING->arg % 2){
                RUNNING->arg = RUNNING->arg*3 + 1;
            }
            else{
                RUNNING->arg = RUNNING->arg/2;
            }
        }
        printf("%d %d\n", RUNNING->id, RUNNING->arg);
        sleep(1);

        if(RUNNING->arg == 1){
            thread_exit();
        }
        else{
            thread_yield();
        }
    }
}

void max_subarray(int id, int arg) {
    // TODO
    thread_setup(id, arg);
    RUNNING->x =0 ; RUNNING->y = -9999999;
    for (RUNNING->i = 1; ; RUNNING->i++) {
        // x:cur_sum / y:max_sum
        async_read(5);
        int cur = atoi(RUNNING->buf);
        RUNNING->x += cur;
        if(RUNNING->x < 0) RUNNING->x = 0;
        if(RUNNING->y < RUNNING->x) RUNNING->y = RUNNING->x;

        printf("%d %d\n", RUNNING->id, RUNNING->y);
        sleep(1);

        if (RUNNING->i == RUNNING->arg) {
            thread_exit();
        }
        else {
            thread_yield();
        }
    }
}
