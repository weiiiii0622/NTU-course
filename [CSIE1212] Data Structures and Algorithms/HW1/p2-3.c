#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define and &&
#define notEqual !=
#define NIL NULL
#define ll long long
#define swap(x,y) {int temp=x; x=y; y=temp;}


typedef struct _{
    int id;
    struct _ *next;
} Node;

// 1. Have ptr point to cur
// 2. Move cur to ptr
// 3. Move ptr to its next
void connect(Node* ptr, Node* cur){
    Node *tmp = ptr->next;
    ptr->next = cur;
    cur = ptr;
    ptr = tmp;
}

Node* solve(Node* ptr1, Node* ptr2){
    Node* cur = NIL; // current node
    while(ptr1 != NIL and ptr2 != NIL){
        if(ptr1->id <= ptr2->id){ // ptr1 is the next node
            connect(ptr1, cur);
        }
        else{ // ptr2 is the next node
            connect(ptr2, cur);
        }
    }
    // Only one of the "if statement" below will be executed
    while(ptr1 != NIL){ // ptr1 have nodes left
        connect(ptr1, cur);
    }
    while(ptr2 != NIL){ // ptr2 have nodes left
        connect(ptr2, cur);
    }
    return cur;
}

int main(){

    

    return 0;
}