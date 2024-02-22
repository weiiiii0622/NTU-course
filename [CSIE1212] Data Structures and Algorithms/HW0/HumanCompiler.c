// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
// #define ll long long
// #define swap(x,y) {int temp=x; x=y; y=temp;}
// #define MINN -50 // inclusive
// #define MAXN 50 // inclusive

// int main(){

//     // 3-[a]
//     int fake_a [] = {1, 3};
//     int fake_b [] = {2, 4};
//     int *real_a = fake_a;
//     int *real_b = fake_b;

//     for (int i=0; i <2; i++)
//         printf("%d ", *( real_a + i));
//     for (int i=0; i <2; i++)
//         printf("%d ", *( real_b + i));

//     int *tmp = real_a;
//     // ___ (1) ___ = ___ (2) ___;
//     real_a = real_b;
//     // ___ (3) ___ = ___ (4) ___;
//     real_b = tmp;

//     for (int i=0; i <2; i++)
//         printf("%d ", *( real_a + i));
//     for (int i=0; i <2; i++)
//         printf("%d ", *( real_b + i));

//     // 3-[b]
//     int storage[MAXN - MINN + 1]={0};
//     // int *ary = ___ (1) ___;
//     int *ary = &storage[MAXN];

//     for (int i=MINN; i<= MAXN; i++)
//         ary[i] = i;
//     for (int i=MINN; i<= MAXN; i++)
//         printf("%d ", ary[i]);

//     return 0;
// }



// 3-[c]

// #include <stdio.h>
// #include <stdlib.h> // malloc / free
// #include <memory.h> // memset

// // Use typedef to define "struct node" as "node".
// typedef struct node{
//     int data;
//     struct node *nxt;
// } node;

// node *alloc(int data , node *nxt ){
//     node *tmp = (node *) malloc(sizeof(node ));
//     tmp ->data = data;
//     tmp ->nxt = nxt;
//     return tmp;
// }

// void destory(node *head ){
//     // if (___ (1) ___ ){
//     if (head != NULL){
//         destory(head ->nxt );
//         // clean sensitive data.
//         memset(head , 0, sizeof(node));
//         free(head );
//     }
// }

// int main (){
//     // create nodes [0, 1, 2, 4]
//     node *head = alloc (0, alloc (1, alloc (2, alloc (4, NULL ))));
//     node *tmp = head;
//     // print the nodes subsequently
//     while (tmp != NULL ){
//         //printf("%d -> ", ___ (2) ___ );
//         printf("%d -> ", tmp->data);
//         //tmp = ___ (3) ___;
//         tmp = tmp->nxt;
//     }
//     printf("NULL");
//     // free the nodes subsequently to avoid memory leak
//     destory(head );
//     return 0;
// }


#include <stdio.h>
#include <stdlib.h> // malloc / free
#include <memory.h> // memset

// Use typedef to substitute "struct node" with "node".
typedef struct node {
    int data;
    struct node *left , *right;
} node;

// creates a node filled with predefined values
node *alloc(int data , node *left , node *right ){
    node *tmp = (node *) malloc(sizeof(node ));
    tmp ->data = data;
    tmp ->left = left;
    tmp ->right = right;
    return tmp;
}
// traverses the nodes recursively
void traverse(node *root ){
    // if (___ (1) ___){
    if ( root!=NULL ){
        printf("%d ", root ->data );

        // traverse(___ (2) ___);
        traverse( root->left );

        // traverse(___ (3) ___);
        traverse( root->right );
    }
}
// frees the nodes recursively
void destory(node *root ){
    // if (___ (1) ___ ){
    if ( root!=NULL ){

        // destory(___ (2) ___ );
        destory( root->left );
        // destory(___ (3) ___ );
        destory( root->right );
        
        memset(root , 0, sizeof(node));

        // free(___ (4) ___ );
        free( root );
    }
}
int main (){
    // creates a hierarchical data structure
    node *root = \
        alloc (0,
            alloc (3,
                alloc (7, NULL , NULL),
                alloc (4, NULL , NULL)
            ),
            alloc (2,
                alloc (1, NULL , NULL),
                alloc (9, NULL , NULL)
            )
        );
    // traverses the nodes one by one
    traverse(root );
    // frees the nodes to avoid memory leak
    destory(root );
    return 0;
}