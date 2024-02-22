#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#define ll long long
#define DEBUG 0

#define TOP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3


typedef struct _{
    int rank;
    int bread;
    int dist[4]; // 0:top 1:right 2:down 3:left
    struct _ *left;
    struct _ *right;
    struct _ *up;
    struct _ *down;
} Node;

int n, m;
int t1, r, t2;
int tmpRank[1001][1001]; 
int tmpBread[1001][1001]; 
Node* head;
Node* Dummy[4][1001];
Node* rank[1000000 + 5];
Node* helperRank[1000000 + 5];

Node* next(Node* prev, Node* cur, int mode){
    // mode = 0  -> go front
    if(mode == 0){
        if(prev == cur->left) return cur->right;
        if(prev == cur->right) return cur->left;
        if(prev == cur->up) return cur->down;
        if(prev == cur->down) return cur->up;
    }
    else if(mode == 1){ // turn right
        if(prev == cur->left) return cur->down;
        if(prev == cur->right) return cur->up;
        if(prev == cur->up) return cur->left;
        if(prev == cur->down) return cur->right;        
    }
    return NULL;
}

Node* newNode(int rank, int bread){
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->bread = bread;
    newNode->rank = rank;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->up = NULL;
    newNode->down = NULL;
    for(int i=0; i<4; i++) newNode->dist[i] = 1;
    return newNode;
}

void readBoard(){
    Node* tmp;
    Node* cur = head->right;
    Node* prev = head;
    for(int i=0; i<n; i++){
        int j=0;
        prev = Dummy[LEFT][i];
        cur = prev->right;
        while(cur!=NULL){
            rank[cur->rank] = cur;
            tmpRank[i][j] = cur->rank;
            tmpBread[i][j] = cur->bread;
            tmp = cur;
            cur = next(prev, cur, 0);
            prev = tmp;
            j++;
        }
    }
}

void constructBoard(){
    Node* prevRow[m];
    for(int i=0; i<m; i++) prevRow[i] = Dummy[TOP][i];
    for(int i=0; i<n; i++){
        Node* prevNode = Dummy[LEFT][i];
        for(int j=0; j<m; j++){
            Node* cur = newNode(tmpRank[i][j], tmpBread[i][j]);
            //if(i==0 && j==0) head = cur;
            if(prevNode != NULL){
                prevNode->right = cur;
                cur->left = prevNode;
            }
            if(prevRow[j] != NULL){
                prevRow[j]->down = cur;
                cur->up = prevRow[j];
            }
            rank[tmpRank[i][j]] = cur;
            prevRow[j] = prevNode = cur;
        }
        prevNode->right = Dummy[RIGHT][i];
        Dummy[RIGHT][i]->left = prevNode;
    }
    for(int i=0; i<m; i++) {prevRow[i]->down = Dummy[DOWN][i]; Dummy[DOWN][i]->up = prevRow[i];}
}

void constructHelperBoard(){
    int dead[n*m+1];
    Node* prevRow[m];
    int ptr_d = 0;
    for(int i=0; i<m; i++) prevRow[i] = NULL;
    for(int i=0; i<n; i++){
        Node* prevNode = NULL;
        for(int j=0; j<m; j++){
            Node* cur = newNode(tmpRank[i][j], tmpBread[i][j]);
            if(tmpBread[i][j] <= 0) dead[ptr_d++] = cur->rank;
            if(prevNode != NULL){
                prevNode->right = cur;
                cur->left = prevNode;
            }
            if(prevRow[j] != NULL){
                prevRow[j]->down = cur;
                cur->up = prevRow[j];
            }
            helperRank[tmpRank[i][j]] = cur;
            prevRow[j] = prevNode = cur;
        }
    }
    ptr_d--;
    while(ptr_d>=0){
        int rk = dead[ptr_d--];
        Node *tgt = helperRank[rk];
        if(tgt->up != NULL){
            tgt->up->down = tgt->down;
            tgt->up->dist[2] += tgt->dist[2];
        }
        if(tgt->down != NULL){
            tgt->down->up = tgt->up;
            tgt->down->dist[0] += tgt->dist[0];
        }
        if(tgt->right != NULL){
            tgt->right->left = tgt->left;
            tgt->right->dist[3] += tgt->dist[3];
        }
        if(tgt->left != NULL){
            tgt->left->right = tgt->right;
            tgt->left->dist[1] += tgt->dist[1];
        }
    }
    return;
}

void printBoard(Node* head){
    // Node* tmp1 = head;
    // for(int i=0; i<n; i++){
    //     Node* tmp2 = tmp1;
    //     while(tmp2!=NULL){
    //         printf("%d", tmp2->bread>0?tmp2->bread:0);
    //         tmp2 = tmp2->right;
    //         if(tmp2!=NULL) printf(" ");
    //     }
    //     printf("\n");
    //     tmp1 = tmp1->down;
    // }
    Node* tmp;
    Node* cur = head->right;
    Node* prev = head;
    for(int i=0; i<n; i++){
        int j=0;
        prev = Dummy[LEFT][i];
        cur = prev->right;
        while(cur!=NULL){
            printf("%d", cur->bread>0?cur->bread:0);
            tmp = next(prev, cur, 0);
            prev = cur;
            cur = tmp;
            if(cur!=NULL) printf(" ");
            if(cur->rank == -1) break;
            j++;
        }
        printf("\n");
    }
    if(DEBUG) printf("\n***********************************\n");
}

void printDEBUGBoard(Node* head){
    // Node* tmp1 = head;
    // for(int i=0; i<m; i++) printf("+---");
    // printf("+\n");
    // for(int i=0; i<n; i++){
    //     Node* tmp2 = tmp1;
    //     while(tmp2!=NULL){
    //         printf("|%3d", tmp2->rank);
    //         tmp2 = tmp2->right;
    //     }
    //     printf("|\n");
    //     tmp2 = tmp1;
    //     while(tmp2!=NULL){
    //         printf("|%3d", tmp2->bread>0?tmp2->bread:tmp2->bread);
    //         tmp2 = tmp2->right;
    //     }
    //     printf("|\n");
    //     for(int i=0; i<m; i++) printf("+---");
    //     printf("+\n");
    //     tmp1 = tmp1->down;
    // }
    Node* tmp;
    Node* cur = head->right;
    Node* prev = head;
    for(int i=0; i<m; i++) printf("+----");
    printf("+\n");
    for(int i=0; i<n; i++){
        int j=0;
        prev = Dummy[LEFT][i];
        cur = prev->right;
        while(cur!=NULL){
            printf("|%4d", cur->rank);
            tmp = next(prev, cur, 0);
            prev = cur;
            cur = tmp;
        }
        printf("|\n");
        prev = Dummy[LEFT][i];
        cur = prev->right;
        while(cur!=NULL){
            printf("|%4d", cur->bread>0?cur->bread:cur->bread);
            tmp = next(prev, cur, 0);
            prev = cur;
            cur = tmp;
        }
        printf("|\n");
        for(int i=0; i<m; i++) printf("+----");
        printf("+\n");
    }

    if(DEBUG) printf("\n***********************************\n");    
}

Node* getnextDir(Node* cur, int dir){
    if(dir==0) return cur->up;
    if(dir==1) return cur->right;
    if(dir==2) return cur->down;
    if(dir==3) return cur->left;
    return NULL;
}

Node* getNext(Node* cur){
    Node* tgt = NULL;
    int dist = 1126;
    int rank = 1000005;
    if(DEBUG){
        printf("cur: rank=%3d bread=%3d\n", cur->rank, cur->bread);
    }
    for(int i=0; i<4; i++){
        Node* tmp = getnextDir(cur, i);
        if(tmp == NULL) continue;
        if(DEBUG){
            printf("dir:%d dist=%3d rank = %3d\n", i, cur->dist[i], tmp->rank);
        }
        if(cur->dist[i] <= dist){
            if(cur->dist[i] == dist){
                if(tmp->rank < rank){
                    tgt = tmp;
                    rank = tmp->rank;
                }
            }
            else{
                tgt = tmp;
                dist = cur->dist[i];
                rank = tmp->rank;
            }
        }
    }
    return tgt;
}

void training(int rk, int lk, int sk){
    Node* cur = helperRank[rk];
    int stack[m*n+5];
    int ptr = 0;
    // remove gathering bakery
    Node* tgt = cur;
    stack[ptr++] = tgt->rank;
    if(tgt->up != NULL){
        tgt->up->down = tgt->down;
        tgt->up->dist[2] += tgt->dist[2];
    }
    if(tgt->down != NULL){
        tgt->down->up = tgt->up;
        tgt->down->dist[0] += tgt->dist[0];
    }
    if(tgt->right != NULL){
        tgt->right->left = tgt->left;
        tgt->right->dist[3] += tgt->dist[3];
    }
    if(tgt->left != NULL){
        tgt->left->right = tgt->right;
        tgt->left->dist[1] += tgt->dist[1];
    }
    while(lk--){
        tgt = getNext(cur);
        if(tgt == NULL) break;
        // Eat bread
        tgt->bread -= sk;
        rank[tgt->rank]->bread -= sk;
        // remove from map
        stack[ptr++] = tgt->rank;

        if(tgt->up != NULL){
            tgt->up->down = tgt->down;
            tgt->up->dist[2] += tgt->dist[2];
        }
        if(tgt->down != NULL){
            tgt->down->up = tgt->up;
            tgt->down->dist[0] += tgt->dist[0];
        }
        if(tgt->right != NULL){
            tgt->right->left = tgt->left;
            tgt->right->dist[3] += tgt->dist[3];
        }
        if(tgt->left != NULL){
            tgt->left->right = tgt->right;
            tgt->left->dist[1] += tgt->dist[1];
        }
        cur = tgt;
        if(DEBUG) printDEBUGBoard(Dummy[LEFT][0]);
    }
    // put them back
    ptr--;
    int dead[m*n+5];
    int ptr_d = 0;
    for(int p=ptr; p>=0; p--){
        int r = stack[p];
        Node* tgt = helperRank[r];
        if(tgt->bread <= 0) dead[ptr_d++] = tgt->rank;
        if(tgt->up != NULL){
            tgt->up->down = tgt;
            tgt->up->dist[2] -= tgt->dist[2];
        }
        if(tgt->down != NULL){
            tgt->down->up = tgt;
            tgt->down->dist[0] -= tgt->dist[0];
        }
        if(tgt->right != NULL){
            tgt->right->left = tgt;
            tgt->right->dist[3] -= tgt->dist[3];
        }
        if(tgt->left != NULL){
            tgt->left->right = tgt;
            tgt->left->dist[1] -= tgt->dist[1];
        }
    }
    // remove closed bakery
    ptr_d--;
    while(ptr_d>=0){
        int r = dead[ptr_d--];
        Node *tgt = helperRank[r];
        if(tgt->up != NULL){
            tgt->up->down = tgt->down;
            tgt->up->dist[2] += tgt->dist[2];
        }
        if(tgt->down != NULL){
            tgt->down->up = tgt->up;
            tgt->down->dist[0] += tgt->dist[0];
        }
        if(tgt->right != NULL){
            tgt->right->left = tgt->left;
            tgt->right->dist[3] += tgt->dist[3];
        }
        if(tgt->left != NULL){
            tgt->left->right = tgt->right;
            tgt->left->dist[1] += tgt->dist[1];
        }
        //free(tgt);
    }
    return;
}

int getDir(Node* from, Node* to){
    if(to->up == from) return 0;
    if(to->right == from) return 1;
    if(to->down == from) return 2;
    if(to->left == from) return 3;
    return -1;
}

void swap(Node* prev1, Node* ptr1, Node* prev2, Node* ptr2){
    Node* tmp1; Node* tmp2;
    int dirM_1 = getDir(prev1, ptr1);
    int dirM_2 = getDir(prev2, ptr2);
    
    if(dirM_1==0) tmp1 = ptr1->left;
    else if(dirM_1==1) tmp1 = ptr1->up;
    else if(dirM_1==2) tmp1 = ptr1->right;
    else if(dirM_1==3) tmp1 = ptr1->down;

    if(dirM_2==0) tmp2 = ptr2->left;
    else if(dirM_2==1) tmp2 = ptr2->up;
    else if(dirM_2==2) tmp2 = ptr2->right;
    else if(dirM_2==3) tmp2 = ptr2->down;

    if(DEBUG)printf("SWAPPING ptr1:%2d and ptr2:%2d\n", tmp1->rank, tmp2->rank);
    assert(tmp1->rank!=-1 && tmp2->rank!=-1);

    if(dirM_1==0) ptr1->left = tmp2;
    else if(dirM_1==1) ptr1->up = tmp2;
    else if(dirM_1==2) ptr1->right = tmp2;
    else if(dirM_1==3) ptr1->down = tmp2;

    if(dirM_2==0) ptr2->left = tmp1;
    else if(dirM_2==1) ptr2->up = tmp1;
    else if(dirM_2==2) ptr2->right = tmp1;
    else if(dirM_2==3) ptr2->down = tmp1;

    int dir1 = getDir(ptr1, tmp1);
    if(dir1==0) tmp1->up = ptr2;
    else if(dir1==1) tmp1->right = ptr2;
    else if(dir1==2) tmp1->down = ptr2;
    else if(dir1==3) tmp1->left = ptr2;

    int dir2 = getDir(ptr2, tmp2);
    if(dir2==0) tmp2->up = ptr1;
    else if(dir2==1) tmp2->right = ptr1;
    else if(dir2==2) tmp2->down = ptr1;
    else if(dir2==3) tmp2->left = ptr1;

}

void racing(int rk, int lk){
    if(lk==1) return;
    int mode =0;
    Node* cur = rank[rk];
    Node* tmp;
    Node* prev1 = cur;
    Node* prev2 = cur;
    Node* ptr1 = cur->left;
    Node* ptr2 = cur->right;
    // locate ptr1 & ptr2
    for(int i=0; i<(lk-1)/2; i++){
        tmp = ptr1;
        ptr1 = next(prev1, ptr1, 0);
        prev1 = tmp;
        tmp = ptr2;
        ptr2 = next(prev2, ptr2, 0);
        prev2 = tmp;
    }
    // turn right
    tmp = ptr1;
    ptr1 = next(prev1, ptr1, 1);
    prev1 = tmp;
    assert(ptr1!=NULL);
    tmp = ptr2;
    ptr2 = next(prev2, ptr2, 1);
    prev2 = tmp;
    assert(ptr2!=NULL);
    for(int i=0; i<(lk-1)/2; i++){
        tmp = ptr1;
        ptr1 = next(prev1, ptr1, 0);
        prev1 = tmp;
        tmp = ptr2;
        ptr2 = next(prev2, ptr2, 0);
        prev2 = tmp;
    }
    if(DEBUG){
        printf("cur:  rank:%3d bread:%3d\n", cur->rank, cur->bread);
        //printf("ptr1: rank:%3d bread:%3d\n", ptr1->rank, ptr1->bread);
        //printf("ptr2: rank:%3d bread:%3d\n", ptr2->rank, ptr2->bread);

    }
    //assert(1!=1);
    // swap 1st side
    Node* tmp1;
    Node* tmp2;
    mode = 1;
    for(int i=0; i<=lk; i++){
        if(DEBUG){
            printf("prev1: rank%3d\n", prev1->rank);
            //printf("ptr1: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr1->rank, ptr1->bread, ptr1->left->rank, ptr1->right->rank, ptr1->up->rank, ptr1->down->rank);
            printf("prev2: rank%3d\n", prev2->rank);
            //printf("ptr2: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr2->rank, ptr2->bread, ptr2->left->rank, ptr2->right->rank, ptr2->up->rank, ptr2->down->rank);
        }
        // rank[ptr1->rank] = ptr2;
        // rank[ptr2->rank] = ptr1;
        // int tmpR = ptr1->rank;
        // int tmpB = ptr1->bread;
        // ptr1->bread = ptr2->bread;
        // ptr1->rank = ptr2->rank;
        // ptr2->bread = tmpB;
        // ptr2->rank = tmpR;
        if(i==0){
            if(DEBUG){
                printf("\nAFTER\n");
                printf("prev1: rank%3d\n", prev1->rank);
                //printf("ptr1: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr1->rank, ptr1->bread, ptr1->left->rank, ptr1->right->rank, ptr1->up->rank, ptr1->down->rank);
                printf("prev2: rank%3d\n", prev2->rank);
                //printf("ptr2: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr2->rank, ptr2->bread, ptr2->left->rank, ptr2->right->rank, ptr2->up->rank, ptr2->down->rank);
            }
            tmp = ptr1;
            ptr1 = next(prev1, ptr1, 1);
            prev1 = tmp;
            tmp = ptr2;
            ptr2 = next(prev2, ptr2, 1);
            prev2 = tmp; 
            
        }
        else{

            swap(prev1, ptr1, prev2, ptr2);
            if(DEBUG){
                printf("\nAFTER\n");
                printf("prev1: rank%3d\n", prev1->rank);
                //printf("ptr1: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr1->rank, ptr1->bread, ptr1->left->rank, ptr1->right->rank, ptr1->up->rank, ptr1->down->rank);
                printf("prev2: rank%3d\n", prev2->rank);
                //printf("ptr2: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr2->rank, ptr2->bread, ptr2->left->rank, ptr2->right->rank, ptr2->up->rank, ptr2->down->rank);
            }
            tmp = ptr1;
            ptr1 = next(prev1, ptr1, 0);
            prev1 = tmp;
            tmp = ptr2;
            ptr2 = next(prev2, ptr2, 0);
            prev2 = tmp;
        }
        if(DEBUG) printDEBUGBoard(Dummy[LEFT][0]);

    }

    // swap 2nd side
    mode = 2;
    for(int i=0; i<=lk; i++){
        if(DEBUG){
            printf("prev1: rank%3d\n", prev1->rank);
            //printf("ptr1: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr1->rank, ptr1->bread, ptr1->left->rank, ptr1->right->rank, ptr1->up->rank, ptr1->down->rank);
            printf("prev2: rank%3d\n", prev2->rank);
            //printf("ptr2: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr2->rank, ptr2->bread, ptr2->left->rank, ptr2->right->rank, ptr2->up->rank, ptr2->down->rank);
        }
        // rank[ptr1->rank] = ptr2;
        // rank[ptr2->rank] = ptr1;
        // int tmpR = ptr1->rank;
        // int tmpB = ptr1->bread;
        // ptr1->bread = ptr2->bread;
        // ptr1->rank = ptr2->rank;
        // ptr2->bread = tmpB;
        // ptr2->rank = tmpR;
        if(i==0){
            if(DEBUG){
                printf("\nAFTER\n");
                printf("prev1: rank%3d\n", prev1->rank);
                //printf("ptr1: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr1->rank, ptr1->bread, ptr1->left->rank, ptr1->right->rank, ptr1->up->rank, ptr1->down->rank);
                printf("prev2: rank%3d\n", prev2->rank);
                //printf("ptr2: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr2->rank, ptr2->bread, ptr2->left->rank, ptr2->right->rank, ptr2->up->rank, ptr2->down->rank);
            }
            tmp = ptr1;
            ptr1 = next(prev1, ptr1, 1);
            prev1 = tmp;
            tmp = ptr2;
            ptr2 = next(prev2, ptr2, 1);
            prev2 = tmp; 
        }
        else{
            swap(prev1, ptr1, prev2, ptr2);
            if(DEBUG){
                printf("\nAFTER\n");
                printf("prev1: rank%3d\n", prev1->rank);
                //printf("ptr1: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr1->rank, ptr1->bread, ptr1->left->rank, ptr1->right->rank, ptr1->up->rank, ptr1->down->rank);
                printf("prev2: rank%3d\n", prev2->rank);
                //printf("ptr2: rank:%3d bread:%3d left:%3d right:%3d top:%3d down:%3d\n", ptr2->rank, ptr2->bread, ptr2->left->rank, ptr2->right->rank, ptr2->up->rank, ptr2->down->rank);
            }
            tmp = ptr1;
            ptr1 = next(prev1, ptr1, 0);
            prev1 = tmp;
            tmp = ptr2;
            ptr2 = next(prev2, ptr2, 0);
            prev2 = tmp;
        }
            if(DEBUG) printDEBUGBoard(Dummy[LEFT][0]);
    }
    return;
}

int main(){
    scanf("%d %d", &n, &m);
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            scanf("%d", &(tmpRank[i][j]));
        }
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            scanf("%d", &(tmpBread[i][j]));
        }
    }
    scanf("%d %d %d", &t1, &r, &t2);
    // create dummy wall
    for(int i=0; i<n; i++){
        Dummy[RIGHT][i] = newNode(-1, -1);
        Dummy[LEFT][i] = newNode(-1, -1);
        if(i!=0){
            Dummy[RIGHT][i-1]->down = Dummy[RIGHT][i];
            Dummy[RIGHT][i]->up = Dummy[RIGHT][i-1];
            Dummy[LEFT][i-1]->down = Dummy[LEFT][i];
            Dummy[LEFT][i]->up = Dummy[LEFT][i-1];
        }
    }
    for(int i=0; i<m; i++){
        Dummy[TOP][i] = newNode(-1, -1);
        Dummy[DOWN][i] = newNode(-1, -1);
        if(i!=0){
            Dummy[TOP][i-1]->right = Dummy[TOP][i];
            Dummy[TOP][i]->left = Dummy[TOP][i-1];
            Dummy[DOWN][i-1]->right = Dummy[DOWN][i];
            Dummy[DOWN][i]->left = Dummy[DOWN][i-1];
        }
    }
    Node* corner[4];
    for(int i=0; i<4; i++) corner[i] = newNode(-1, -1);
    // up-left corner
    Dummy[LEFT][0]->up = corner[0]; corner[0]->down = Dummy[LEFT][0];
    Dummy[TOP][0]->left = corner[0]; corner[0]->right = Dummy[TOP][0];
    // up-right corner
    Dummy[TOP][m-1]->right = corner[1]; corner[1]->left = Dummy[TOP][m-1];
    Dummy[RIGHT][0]->up = corner[1]; corner[1]->down = Dummy[RIGHT][0];
    // down-left corner
    Dummy[LEFT][n-1]->down = corner[2]; corner[2]->up = Dummy[LEFT][n-1];
    Dummy[DOWN][0]->left = corner[2]; corner[2]->right = Dummy[DOWN][0];
    // down-right corner
    Dummy[DOWN][m-1]->right = corner[3]; corner[3]->left = Dummy[DOWN][m-1];
    Dummy[RIGHT][n-1]->down = corner[3]; corner[3]->up = Dummy[RIGHT][n-1];

    head = Dummy[LEFT][0];


    constructBoard();
    
    constructHelperBoard();
    int rk, lk, sk;

    // training 1
    if(DEBUG) printf("\nSTART TRAINING 1\n");
    if(DEBUG) printDEBUGBoard(Dummy[LEFT][0]);
    for(int i=0; i<t1; i++){
        scanf("%d %d %d", &rk, &lk, &sk);
        training(rk, lk, sk);
    }
    for(int i=1; i<=n*m; i++){
        if(helperRank[i] != NULL){
            free(helperRank[i]);
        }
        helperRank[i] = NULL;
    }
    // racing

    if(DEBUG) printf("\nEND TRAINING 1\n");
    if(DEBUG) printDEBUGBoard(Dummy[LEFT][0]);
    if(DEBUG) printf("\nSTART RACING\n");

    for(int i=0; i<r; i++){
        scanf("%d %d", &rk, &lk);
        racing(rk, lk);
        if(DEBUG) printDEBUGBoard(Dummy[LEFT][0]);
    }
    if(DEBUG) printf("\nEND RACING \n");
    if(DEBUG) printDEBUGBoard(Dummy[LEFT][0]);
    readBoard();
    constructHelperBoard();
    // if(DEBUG){
    //     for(int i=1; i<=n*m; i++){
    //         printf("%d\n", helperRank[i]->rank);
    //     }
    // }
    //training 2
    if(DEBUG) printBoard(Dummy[LEFT][0]);
    if(DEBUG) printf("\nSTART TRAINING 2\n");
    for(int i=0; i<t2; i++){
        scanf("%d %d %d", &rk, &lk, &sk);
        training(rk, lk, sk);
    }
    for(int i=1; i<=n*m; i++){
        if(helperRank[i] != NULL){
            free(helperRank[i]);
        }
    }
    if(DEBUG) printf("\nEND TRAINING 1\n");
    if(DEBUG) printDEBUGBoard(Dummy[LEFT][0]);

    printBoard(Dummy[LEFT][0]);

    for(int i=1; i<=n*m; i++){
        if(rank[i] != NULL){
            free(rank[i]);
        }
    }
    return 0;
}