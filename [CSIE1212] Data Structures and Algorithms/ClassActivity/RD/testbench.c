//#include "../QA/testbench.h"
#include<stdio.h>
#include <time.h>

typedef struct student{
    char name[15];
    int grade;
}Std;


/* include all the headers you need */

/* Please try to figure out how to design the parameter interface of your functions by observing the given bozo sort function */
/* Otherwise, you might suffer from rewriting your code to meet the final requirements */

int N;

//待測
void bubble_sort(Std **stdptr,int n){

    for(int i = 0; i < n-1;i++){
        for(int j= 0 ; j <n-i-1 ;j++ ){
            if(stdptr[j]->grade>stdptr[j+1]->grade){
                //由小到大 前面比較小就交換
                Std *temp = stdptr[j];
                stdptr[j] = stdptr[j+1];
                stdptr[j+1]= temp;
            }
        }

    }

}

// l 是陣列最左端，r 是陣列最右端
void quick_sort(Std **stdptr, int l, int r){
    
    if(l < r){
        int key = stdptr[(l+r)/2]->grade;
        int i = l - 1;
        int j = r + 1;

        while(1){
            while(stdptr[++i]->grade > key);  // 向右找
            while(stdptr[--j]->grade < key);  // 向左找
            if (i >= j)
                break;
            // swap
            Std *tmp = stdptr[i];
            stdptr[i] = stdptr[j];
            stdptr[j] = tmp;                         
        }

        quick_sort(stdptr, l, i-1);  // 繼續處理較小部分的子循環
        quick_sort(stdptr, j+1, r);  // 繼續處理較大部分的子循環
    }
}

void insertion_sort(Std **stdptr, int len){
    for(int i = 1; i < len; i++){

        Std *key = stdptr[i];
        int j = i - 1;

        // 把 arr[0..i-1] 之中大於 key 的數字往後移動一格
        while(j >= 0 && stdptr[j]->grade > key->grade){
            stdptr[j+1] = stdptr[j];
            j--;
        }
        stdptr[j+1] = key;
    }
}


void bucket_sort(Std **stdptr,int len){
    Std *Bucket[10][N]; // buckets
    int BucketLen[10] = {0};//分10個buckets和對應的個數
    for(int i = 0; i < len; i++){
        //分7個區段Ｎ＊10000000
        int g = (stdptr[i]->grade)/10000000;
        Bucket[g][BucketLen[g]] = stdptr[i];
        BucketLen[g]++;
    }
    for(int i=0 ; i <10; i++){
        // 對每一個bucket做insertion sort
        insertion_sort(Bucket[i], BucketLen[i]);
    }
    int count = 0;
    //把每個bucket放到最終陣列
    for(int i=0 ; i<10; i++){
        for(int j = 0; j < BucketLen[i]; j++){
            stdptr[count+j] = Bucket[i][j];
            count ++;
        }
    }   
}

int main(int argc, char *argv[]){
    scanf("%d", &N);

    Std student[5*N];
    // stdptr[i] 指向 stduent[i]
    Std *stdptr[5*N];
    
    
    // read input
    for(int i = 0; i < 5*N; i++){
        scanf("%s %d", student[i].name, &(student[i].grade));
        stdptr[i] = &student[i];
    }
    // start clock
    int start = clock();
    // apply sorting algorithm

    //insertion_sort(stdptr, 5*N);
    //bubble_sort(stdptr, 5*N);
    //quick_sort(stdptr, 0, 5*N-1);
    bucket_sort(stdptr, 5*N);

    int end = clock();
    printf("time: %d\n", end-start);

    // print output
    for(int i = 0; i < N; i++){
        printf("Group:%d\n", i);
        for(int j = 0; j < 5; j++){
            printf("Student: %s %d\n", stdptr[i+j*N]->name, i+j*N+1);
        }
    }
        
        
    return 0;
}