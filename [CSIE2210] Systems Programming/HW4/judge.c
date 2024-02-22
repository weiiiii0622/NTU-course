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
const char* ans[99] = {
    "./answers/0t.out", "./answers/134031t.out", "./answers/91140t.out", "./answers/96996t.out", "./answers/6247t.out", "./answers/49248t.out", "./answers/53904t.out", "./answers/131600t.out", "./answers/112814t.out", "./answers/140691t.out", "./answers/135704t.out", "./answers/57495t.out", "./answers/18887t.out", "./answers/5073t.out", "./answers/822t.out","./answers/47175t.out", "./answers/25244t.out", "./answers/94582t.out", "./answers/21792t.out", "./answers/16398t.out", "./answers/99062t.out", "./answers/24829t.out", "./answers/143750t.out", "./answers/14028t.out", "./answers/103804t.out", "./answers/78056t.out", "./answers/93458t.out", "./answers/117427t.out", "./answers/110763t.out", "./answers/135232t.out", "./answers/130800t.out", "./answers/127955t.out", "./answers/14263t.out", "./answers/109105t.out", "./answers/93982t.out", "./answers/4147t.out", "./answers/93012t.out", "./answers/97549t.out", "./answers/134365t.out", "./answers/26024t.out", "./answers/148907t.out", "./answers/98055t.out", "./answers/24416t.out", "./answers/159756t.out", "./answers/146575t.out", "./answers/107238t.out", "./answers/121625t.out"
};
const char* testcase[30] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30"
};
const char* logFile[30] = {"./0t.out", "./134031t.out", "./91140t.out", "./96996t.out", "./6247t.out", "./49248t.out", "./53904t.out", "./131600t.out", "./112814t.out", "./140691t.out", "./135704t.out", "./57495t.out", "./18887t.out", "./5073t.out", "./822t.out", "./47175t.out", "./25244t.out", "./94582t.out", "./21792t.out", "./16398t.out", "./99062t.out", "./24829t.out", "./143750t.out", "./14028t.out", "./103804t.out", "./78056t.out", "./93458t.out", "./117427t.out", "./110763t.out", "./135232t.out", "./130800t.out", "./127955t.out", "./14263t.out", "./109105t.out", "./93982t.out", "./4147t.out", "./93012t.out", "./97549t.out", "./134365t.out", "./26024t.out", "./148907t.out", "./98055t.out", "./24416t.out", "./159756t.out", "./146575t.out", "./107238t.out", "./121625t.out"};
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