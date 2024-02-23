// 1
#include "kernel/types.h"
// 2
#include "user/user.h"
// 3
#include "user/threads.h"

#define NULL 0

void f2(void *arg) {
    printf("f2 - start......\n");
    thread_yield();
    printf("f2 - end!!\n");
}

void s1(int signo) {
    printf("s1 - start......\n");
    thread_yield();
    printf("s1 - end!!\n");
}

void f1(void *arg) {
    thread_register_handler(0, s1);
    struct thread *t2 = thread_create(f2, NULL);
    thread_add_runqueue(t2);

    printf("f1 - start......\n");
    thread_yield();

    thread_kill(t2, 0);
    thread_yield();

    printf("f1 - end!!\n");
    thread_exit();
}

int main() {
    printf("----main----\n\n");
    struct thread *t1 = thread_create(f1, NULL);
    thread_add_runqueue(t1);
    thread_start_threading();
    printf("\n----exit----\n");

    /**
     * Expected:
     * 
     * f1 start
     * f2 start
     * s1 start
     * f1 end
     * s1 end
     * f2 end
    */

    exit(0);
}