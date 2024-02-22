#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define ll long long
#define INF 1000000009
#define DEBUG 1

typedef struct node{
    int val;
    int end;
} Node;

typedef struct Ret{
    int val;
    int cnt;
} Ret;

typedef struct company{
    int id;
    int size;
    struct node *cost;
    struct company *next;
    struct company *child;
} Company;

Company* newComp(int id, int m){
    Company* tmp = (Company*) malloc(sizeof(Company));
    tmp->cost = (Node*) malloc(sizeof(Node)*(m+1));
    tmp->child = NULL;
    tmp->next = NULL;
    tmp->id = id;
    tmp->size = 0;
    return tmp;
}

void swap(Node* a, Node* b){
    int tmp = a->val;
    a->val = b->val;
    b->val = tmp;
    tmp = a->end;
    a->end = b->end;
    b->end = tmp;
}

void heapify(Node* arr, int i, int size){
    if(size <= 1){
        return;
    }
    int minNode = i;
    int left = 2*i;
    int right = 2*i+1;
    if(left <= size && arr[left].val < arr[minNode].val){
        minNode = left;
    }
    if(right <= size && arr[right].val < arr[minNode].val){
        minNode = right;
    }
    if(minNode != i){
        swap(&arr[i], &arr[minNode]);
        heapify(arr, minNode, size);
    }
    return;
}

void insert(Node* arr, int size, int val, int end){
    int cur = size+1;
    int parent = cur/2; 
    arr[cur].val = val, arr[cur].end = end;
    while(parent>=1 && arr[cur].val < arr[parent].val){
        swap(&arr[cur], &arr[parent]);
        cur = parent;
        parent = cur/2;
    }
    return;
}

void pop(Node* arr, int size){
    swap(&arr[1], &arr[size]);
    heapify(arr, 1, size-1);
    return;
}

int max(int a, int b){
    return a>b?a:b;
}

Ret dfs(Company* root, int c, int today){
    // Pop expired melons
    while(root->cost[1].end < today){
        pop(root->cost, root->size);
        (root->size) -= 1;
    }
    // Base case
    if(root->child==NULL){
        Ret ret;
        if(root->cost[1].val <= c){
            ret.cnt = 1;
            ret.val = root->cost[1].val;
        }
        else{
            ret.cnt = 0;
            ret.val = INF;
        }
        return ret;
    }

    // Traverse the tree
    Ret ret;
    int maxCnt = 0;
    int totalCnt = 1;
    int totalVal = root->cost[1].val;
    Company* tmp = root->child;
    while(tmp != NULL){
        ret = dfs(tmp, c, today);
        maxCnt = max(maxCnt, ret.cnt);
        if(totalVal+ret.val > c || ret.val>=INF) totalVal = INF;
        else totalVal += ret.val;
        totalCnt += ret.cnt;
        tmp = tmp->next;
    }
    if(totalVal <= c){
        ret.cnt = totalCnt;
        ret.val = totalVal;
    }
    else{
        ret.cnt = maxCnt;
        ret.val = INF;
    }
    return ret;
}

Company* comp[1000000 + 5];

// DEBUG FUNCTION
void checkTree(int n){
    for(int i=1; i<=n; i++){
        Company *tmp = comp[i]->child;
        printf("%d:", comp[i]->id);
        while(tmp != NULL){
            printf(" %d", tmp->id);
            tmp = tmp->next;
        }
        printf("\n");
    }
}

void checkMin(int n){
    for(int i=1; i<=n; i++){
        printf("Comp %d:", i);
        for(int j=1; j<=3; j++){
            printf(" %d", comp[i]->cost[j].val);
        }
        printf("\n");
    }
}

int main(){

    int n, m, c; scanf("%d %d %d", &n, &m, &c);
    // Construct Tree
    Company* child[n+1];
    for(int i=1; i<=n; i++){
        comp[i] = newComp(i, m);
        child[i] = NULL;
    }
    for(int i=2; i<=n; i++){
        int par; scanf("%d", &par);
        if(child[par] == NULL) comp[par]->child = comp[i], child[par] = comp[i];
        else child[par]->next = comp[i], child[par] = comp[i];
    }

    // Start
    int cj, dj;
    for(int j=1; j<=m; j++){
        for(int i=1; i<=n; i++){
            scanf("%d %d", &cj, &dj);
            insert(comp[i]->cost, comp[i]->size, cj, j+dj);
            (comp[i]->size) += 1;
        }
        printf("%d\n", dfs(comp[1], c, j).cnt);
    }
    return 0;
}