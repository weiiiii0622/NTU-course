#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define ll long long
#define swap(x,y) {int temp=x; x=y; y=temp;}

typedef struct _ {
    char board[30][30];
} Board;

int n, m;
int row[30][15];
int col[30][15];

Board* constuctBoard(){
    Board* newBoard = (Board*) malloc(sizeof(Board));
    for(int i=0; i<30; i++){
        for(int j=0; j<30; j++){
            newBoard->board[i][j] = ' ';
        }
    }
    return newBoard;
}

bool check(Board *board){
    // row
    for(int i=0; i<n; i++){
        int s = 0;
        int cnt = 0;
        int tmp[25];
        int j=0;
        bool start = false;
        while(j<m){
            if(board->board[i][j] == '_'){
                if(start) tmp[s++] = cnt;
                start = false;
                cnt = 0;
            }
            else{
                start = true;
                cnt += 1;
            }
            j++;
        }
        if(start) tmp[s++] = cnt;
        if(row[i][0] != s) return false;
        for(int k=0; k<s; k++){
            if(row[i][1+k] != tmp[k]) return false;
        }
    }
    // col
    for(int i=0; i<m; i++){
        int s = 0;
        int cnt = 0;
        int tmp[25];
        int j=0;
        bool start = false;
        while(j<n){
            if(board->board[j][i] == '_'){
                if(start) tmp[s++] = cnt;
                start = false;
                cnt = 0;
            }
            else{
                start = true;
                cnt += 1;
            }
            j++;
        }
        if(start) tmp[s++] = cnt;
        if(col[i][0] != s) return false;
        for(int k=0; k<s; k++){
            if(col[i][1+k] != tmp[k]) return false;
        }
    }
    return true;
}

bool checkCol(Board* board, int i, int total){
    // i-th col
    int s = 0;
    int cnt = 0;
    int tmp[25];
    int j=0;
    bool start = false;
    while(j<total){
        if(board->board[j][i] == '_'){
            if(start) tmp[s++] = cnt;
            start = false;
            cnt = 0;
        }
        else{
            start = true;
            cnt += 1;
        }
        j++;
    }
    if(start) tmp[s++] = cnt;
    // printf("col=%d / s=%d\n", i, s);
    if(col[i][0] < s) return false;
    for(int k=0; k<s-1; k++){
        if(col[i][1+k] != tmp[k]) return false;
    }
    return true;
}

bool checkRow(Board* board, int i){
    // i-th row
    int s = 0;
    int cnt = 0;
    int tmp[25];
    int j=0;
    bool start = false;
    while(j<m){
        if(board->board[i][j] == '_'){
            if(start) tmp[s++] = cnt;
            start = false;
            cnt = 0;
        }
        else{
            start = true;
            cnt += 1;
        }
        j++;
    }
    if(start) tmp[s++] = cnt;
    if(row[i][0] != s) return false;
    for(int k=0; k<s; k++){
        if(row[i][1+k] != tmp[k]) return false;
    }
    return true;
}

void print(Board* board){
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            printf("%c", board->board[i][j]);
        }
        printf("\n");
    }
    return;
}

bool solve(Board* board, int i, int j){
    if(i==n && j==0){
        if(check(board)){
            print(board);
            return true;
        }
        else return false;
    }
    if(i!=0 && j==0){
        if(!checkRow(board, i-1)){
            return false;
        }
    }
    if(i!=0 && j!=0){
        
    }
    int x = (j+1)/m + i;
    int y = (j+1) % m;
    board->board[i][j] = 'o';
    if(checkCol(board, j, i+1)){
        if(solve(board, x, y)){
            return true;
        }
    }
    board->board[i][j] = '_';
    if(checkCol(board, j, i+1)){
        if(solve(board, x, y)){
            return true;
        }
    }
    board->board[i][j] = ' ';
    return false;
}

int main(){
    scanf("%d %d", &n, &m);

    for(int i=0; i<n; i++){
        scanf("%d", &row[i][0]);
        for(int j=0; j<row[i][0]; j++){
            scanf("%d", &row[i][1+j]);
        }
    }
    for(int i=0; i<m; i++){
        scanf("%d", &col[i][0]);
        for(int j=0; j<col[i][0]; j++){
            scanf("%d", &col[i][1+j]);
        }
    }

    Board* board = constuctBoard();

    solve(board, 0, 0);
    return 0;
}