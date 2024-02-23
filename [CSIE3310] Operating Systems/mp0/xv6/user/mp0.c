#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/fs.h"

struct Ans{
    int dirNum;
    int fileNum;
};

int cntOccurence(char *path, char *key){
    char *p;
    int cnt = 0;
    for(p=path; p<path+strlen(path); p++){
        if(*p == *key){
            cnt += 1;
        }
    }
    return cnt;
}

void dfs(char *path, char *key, int *dirNum, int *fileNum){
    char buf[65], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        //fprintf(2, "mp0 dfs: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        //fprintf(2, "mp0 dfs: cannot stat %s\n", path);
        close(fd);
        return;
    }
    printf("%s %d\n\0", path, cntOccurence(path, key));
    switch(st.type){
        case T_FILE:
            *fileNum += 1;
            break;

        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                //printf("mp0 dfs: path too long\n");
                break;
            }
            *dirNum += 1;
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                if(strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0) 
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                dfs(buf, key, dirNum, fileNum);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]){

    if(argc != 3){
        //fprintf(2, "mp0 usage: \"mp0 <root_directory> <key>\"");
        exit(1);
    }

    int fd[2];
    if(pipe(fd) == -1){
        //fprintf(2, "mp0: pipe error\n");
        exit(1);
    }

    int pid = fork();
    if(pid < 0){
        //fprintf(2, "mp0: fork error\n");
        exit(1);
            
    } else if(pid == 0){
        int dirNum = 0, fileNum = 0;
        int fdcheck;
        struct stat st;
        if((fdcheck = open(argv[1], 0)) < 0){
            printf("%s [error opening dir]\n\0", argv[1]);
        }
        else{
            if(fstat(fdcheck, &st) < 0){
                printf("%s [error opening dir]\n\0", argv[1]);
                //close(fdcheck);
                //return;
            }
            else{
                if(st.type == T_DIR){
                    dfs(argv[1], argv[2], &dirNum, &fileNum);
                }
                else{
                    printf("%s [error opening dir]\n\0", argv[1]);
                }
            }
        }
        close(fdcheck);
        struct Ans ans;
        ans.dirNum = dirNum;
        ans.fileNum = fileNum;
        write(fd[1], &ans, sizeof(struct Ans));

    } else{
        // parent
        wait(0);
        struct Ans sol;
        read(fd[0], &sol, sizeof(struct Ans));
        if(sol.dirNum == 0 && sol.fileNum == 0){
            printf("\n0 directories, 0 files\n\0");
        }
        else{
            printf("\n%d directories, %d files\n\0", sol.dirNum-1, sol.fileNum);
        }
    }

    close(fd[0]);
    close(fd[1]);
    exit(0);
}