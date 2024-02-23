#include "kernel/types.h"
#include "user/setjmp.h"
#include "user/threads.h"
#include "user/user.h"
#define NULL 0
#define DEBUG 0


static struct thread* current_thread = NULL;
static int id = 1;
static jmp_buf env_st;
static jmp_buf env_tmp;

struct thread *thread_create(void (*f)(void *), void *arg){
    struct thread *t = (struct thread*) malloc(sizeof(struct thread));
    unsigned long new_stack_p;
    unsigned long new_stack;
    new_stack = (unsigned long) malloc(sizeof(unsigned long)*0x100);
    new_stack_p = new_stack +0x100*8-0x2*8;
    t->fp = f;
    t->arg = arg;
    t->ID  = id;
    t->buf_set = 0;
    t->stack = (void*) new_stack;
    t->stack_p = (void*) new_stack_p;
    if(DEBUG) printf("Created thread %d\n", t->ID);
    id++;

    // part 2
    t->sig_handler[0] = NULL_FUNC;
    t->sig_handler[1] = NULL_FUNC;
    t->signo = -1;
    t->handler_buf_set = 0;
    return t;
}

void thread_add_runqueue(struct thread *t){
    if(current_thread == NULL){
        // TODO
        current_thread = t;
        t->previous = t;
        t->next = t;
    }
    else{
        // TODO
        // Part 1
        t->previous = current_thread->previous;
        t->next = current_thread;
        current_thread->previous->next = t; 
        current_thread->previous = t;
        // Part 2
        t->sig_handler[0] = current_thread->sig_handler[0];
        t->sig_handler[1] = current_thread->sig_handler[1];
    }
    if(DEBUG) printf("Added thread %d\n", t->ID);
}
// Part 1 thread_yield
// void thread_yield(void){
//     // TODO
//     if(setjmp(current_thread->env) == 0){
//         schedule();
//         dispatch();
//     }
//     else{
//         // return
//     }
// }

// Part2 thread_yield
void thread_yield(void){
    // TODO
    if(current_thread->signo == -1){
        // No signal
        if(setjmp(current_thread->env) == 0){
            current_thread->stack_p = (void*) current_thread->env->sp;
            schedule();
            dispatch();
        }
        else{
            // return
        }
    }
    else{
        // Signal interrupted
        if(setjmp(current_thread->handler_env) == 0){
            current_thread->stack_p = (void*) current_thread->handler_env->sp;
            schedule();
            dispatch();
        }
        else{
            // return
        }
    }
}

// Part1 dispatch
// void dispatch(void){
//     // TODO
//     if(current_thread->buf_set == 0){
//         if(setjmp(current_thread->env) == 0){
//             current_thread->buf_set = 1;
//             current_thread->env->sp = (unsigned long) current_thread->stack_p;
//             longjmp(current_thread->env, 1);
//         }
//         else{
//             current_thread->fp(current_thread->arg);
//             thread_exit();
//         }
//     }
//     else{
//         longjmp(current_thread->env, 1);
//     }
// }

// Part2 dispatch
void dispatch(void){
    // TODO
    // Have signal
    if(current_thread->signo != -1){
        if(DEBUG) printf("Thread %d recieved sig %d\n", current_thread->ID, current_thread->signo);
        // Signal interrupt
        if(current_thread->sig_handler[current_thread->signo] == NULL_FUNC){
            // No handler
            thread_exit();
        }

        if(current_thread->handler_buf_set == 0){
            if(setjmp(env_tmp) == 0){
                current_thread->handler_buf_set = 1;
                env_tmp->sp = (unsigned long) current_thread->stack_p;
                longjmp(env_tmp, 1);
            }
            else{
                current_thread->sig_handler[current_thread->signo](current_thread->signo);
                // Signal handler returned
                if(DEBUG) printf("Thread %d return from sig handler\n", current_thread->ID);
                current_thread->signo = -1;
                //dispatch();
            }
        }
        else{
            longjmp(current_thread->handler_env, 1);
        }
    }

    // No signal
    if(current_thread->buf_set == 0){
        if(setjmp(env_tmp) == 0){
            current_thread->buf_set = 1;
            env_tmp->sp = (unsigned long) current_thread->stack_p;
            longjmp(env_tmp, 1);
        }
        else{
            if(DEBUG) printf("Thread %d init exec\n", current_thread->ID);
            current_thread->fp(current_thread->arg);
            // returned
            thread_exit();
        }
    }
    else{
        if(DEBUG) printf("Thread %d LongJmp to continue\n", current_thread->ID);
        longjmp(current_thread->env, 1);
    }
}

void schedule(void){
    // TODO
    current_thread = current_thread->next;
    if(DEBUG) printf("Switch to thread %d\n", current_thread->ID);
}
void thread_exit(void){
    if(DEBUG) printf("Thread %d exit\n", current_thread->ID);
    if(current_thread->next != current_thread){
        // TODO
        struct thread* tmp = current_thread;
        current_thread->next->previous = current_thread->previous;
        current_thread->previous->next = current_thread->next;
        schedule();
        free(tmp->stack);
        free(tmp);
        dispatch();
    }
    else{
        // TODO
        // Hint: No more thread to execute
        longjmp(env_st, 1);
    }
}
void thread_start_threading(void){
    // TODO
    if(setjmp(env_st) == 0){
        dispatch();
    }
    else{
        free(current_thread->stack);
        free(current_thread);
        return;
    }
}
// part 2
void thread_register_handler(int signo, void (*handler)(int)){
    // TODO
    current_thread->sig_handler[signo] = handler;
}
void thread_kill(struct thread *t, int signo){
    // TODO
    t->signo = signo;
}