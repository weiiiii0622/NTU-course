#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define UYLW  "\033[4;33m"
#define KYLW  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"

const char* judge = "/home/student/10/b10902138/spjudge/judge_execution.sh";
const char* ans[30] = {
    "/home/student/10/b10902138/spjudge/answer/answer_0.txt", "/home/student/10/b10902138/spjudge/answer/answer_1.txt", "/home/student/10/b10902138/spjudge/answer/answer_2.txt", "/home/student/10/b10902138/spjudge/answer/answer_3.txt", "/home/student/10/b10902138/spjudge/answer/answer_4.txt", "/home/student/10/b10902138/spjudge/answer/answer_5.txt", "/home/student/10/b10902138/spjudge/answer/answer_6.txt", "/home/student/10/b10902138/spjudge/answer/answer_7.txt", "/home/student/10/b10902138/spjudge/answer/answer_8.txt", "/home/student/10/b10902138/spjudge/answer/answer_9.txt", "/home/student/10/b10902138/spjudge/answer/answer_10.txt", "/home/student/10/b10902138/spjudge/answer/answer_11.txt", "/home/student/10/b10902138/spjudge/answer/answer_12.txt", "/home/student/10/b10902138/spjudge/answer/answer_13.txt", "/home/student/10/b10902138/spjudge/answer/answer_14.txt", "/home/student/10/b10902138/spjudge/answer/answer_15.txt", "/home/student/10/b10902138/spjudge/answer/answer_16.txt", "/home/student/10/b10902138/spjudge/answer/answer_17.txt", "/home/student/10/b10902138/spjudge/answer/answer_18.txt", "/home/student/10/b10902138/spjudge/answer/answer_19.txt", "/home/student/10/b10902138/spjudge/answer/answer_20.txt", "/home/student/10/b10902138/spjudge/answer/answer_21.txt", "/home/student/10/b10902138/spjudge/answer/answer_22.txt", "/home/student/10/b10902138/spjudge/answer/answer_23.txt", "/home/student/10/b10902138/spjudge/answer/answer_24.txt", "/home/student/10/b10902138/spjudge/answer/answer_25.txt", "/home/student/10/b10902138/spjudge/answer/answer_26.txt", "/home/student/10/b10902138/spjudge/answer/answer_27.txt", "/home/student/10/b10902138/spjudge/answer/answer_28.txt", "/home/student/10/b10902138/spjudge/answer/answer_29.txt"
};
const char* testcase[30] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30"
};
const char* logFile[30] = {"./logs/log_0.txt", "./logs/log_1.txt", "./logs/log_2.txt", "./logs/log_3.txt", "./logs/log_4.txt", "./logs/log_5.txt", "./logs/log_6.txt", "./logs/log_7.txt", "./logs/log_8.txt", "./logs/log_9.txt", "./logs/log_10.txt", "./logs/log_11.txt", "./logs/log_12.txt", "./logs/log_13.txt", "./logs/log_14.txt", "./logs/log_15.txt", "./logs/log_16.txt", "./logs/log_17.txt", "./logs/log_18.txt", "./logs/log_19.txt", "./logs/log_20.txt", "./logs/log_21.txt", "./logs/log_22.txt", "./logs/log_23.txt", "./logs/log_24.txt", "./logs/log_25.txt", "./logs/log_26.txt", "./logs/log_27.txt", "./logs/log_28.txt", "./logs/log_29.txt"};
//const char* logFile[30] = {"./logs/answer_0.txt", "./logs/answer_1.txt", "./logs/answer_2.txt", "./logs/answer_3.txt", "./logs/answer_4.txt", "./logs/answer_5.txt", "./logs/answer_6.txt", "./logs/answer_7.txt", "./logs/answer_8.txt", "./logs/answer_9.txt", "./logs/answer_10.txt", "./logs/answer_11.txt", "./logs/answer_12.txt", "./logs/answer_13.txt", "./logs/answer_14.txt", "./logs/answer_15.txt", "./logs/answer_16.txt", "./logs/answer_17.txt", "./logs/answer_18.txt", "./logs/answer_19.txt", "./logs/answer_20.txt", "./logs/answer_21.txt", "./logs/answer_22.txt", "./logs/answer_23.txt", "./logs/answer_24.txt", "./logs/answer_25.txt", "./logs/answer_26.txt", "./logs/answer_27.txt", "./logs/answer_28.txt", "./logs/answer_29.txt"};
const int testNum = 13;

int compare(FILE* log, FILE* answer){
    if(log == NULL || answer == NULL){
       printf(KRED "ERRFOPEN : JUDGE ERROR\n");
       return -1;
    }
    char log_line[1024];
    char ans_line[1024];

    while(1) {
        if(!fgets(log_line, sizeof(log_line), log) || !fgets(ans_line, sizeof(ans_line), answer)) return 1;
        if(strcmp(log_line, ans_line) != 0){
            // printf("log: %s\n", log_line); printf("answer: %s\n", ans_line);
            return 0;
        }
    }
    return 0;
};


int main(int argc, char *argv[]){
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    int pid;
    int selected[testNum];
    int target = -1;
    if(argc==2){
        target = atoi(argv[1]);
        if(target >= testNum){
            printf(KRED "INPUT Error. Testcase specified is not valid.\n");
            printf(KNRM "");
            return 0;
        }
    }
    if(target != -1){
        for(int i=0; i<testNum; i++){
            if(i==target){
                selected[i] = 1;
            }
            else{
                selected[i] = 0;
            }
        }
    }
    else{
        for(int i=0; i<testNum; i++){
            selected[i] = 1;
        }
    }

    printf(KCYN "Starting test judge ....\n");
    printf(KCYN "To specify which testcase to run, type ~b10902138/spjudge/judge [testcaseNum]\n");
    printf(UYLW "Testcase 0~2");
    printf(KYLW ": SAMPLE TESTCASE\n");
    printf(UYLW "Testcase 3~9");
    printf(KYLW ": 3 threads [fib] [col] [sub]\n");
    printf(UYLW "Testcase 10~12");
    printf(KYLW ": 7 threads [fib] [col] [sub] [fib] [col] [sub] [sub]\n");
    printf(KBLU "Your outputs can be found at -> ./logs\n");
    printf(KBLU "Answers can be found at -> ~b10902138/spjudge/answer\n");
    mkdir("logs", 0755);


    for(int i=0; i<testNum; i++){
        if(!selected[i]) continue;
        int fd = open(logFile[i], O_WRONLY|O_CREAT|O_TRUNC|O_APPEND, 0704);
        //int fd = open(ans[i], O_WRONLY|O_CREAT|O_TRUNC|O_APPEND, 0744);
        printf(KNRM "Testcase %d: ", i);

        if((pid = fork()) == 0){
            dup2(fd, STDOUT_FILENO);
            close(fd);
            
            execl(judge, judge, testcase[i], NULL);
            
            _exit(0);
        }
        else{
            int status;
            waitpid(pid, &status, 0);
            if(WEXITSTATUS(status) == 1){
                printf(KRED "Compile Error. Missing \"scheduler.c\" or \"threads.c\" or \"threadtools.h\".\n");
            }
            else{
                FILE* log = fopen(logFile[i], "r");
                FILE* answer = fopen(ans[i], "r");

                int ret = compare(log, answer);
                
                if(ret==1) printf(KGRN "Passed.\n");
                else if (ret==0) printf(KRED "Failed.\n");
            }
            //unlink(logFile);
        }
    }
    printf(KNRM"");
    return 0;
}