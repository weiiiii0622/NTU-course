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

void place2front(Node* prev, Node* cur, Node* head){
    prev->next = cur->next; // Pop cur out of the list
    cur->next = head;       // Point cur to head
    head = cur;             // Set head to cur
}

Node* solve(Node* ptr){
    Node* head = reverse(ptr); // Reverse the direction of the linked-list and get its new head
    Node* cur = head;          // Current node
    Node* prev = head;         // Prev Node
    Node* next;                // Next Node
    while(cur != NIL){
        next = cur->next; // Get next node
        if(cur->id < 0){  // Found negative ID -> place cur to the front
            place2front(prev, cur, head);
        }
        else{
            prev = cur;  // If cur's ID is not negative, move prev forward
        }
        cur = next;   // go forward
    }
    Node* head = reverse(head); // Reverse the direction of the linked-list again and return its new head
    return head;
}

int main(){

    

    return 0;
}