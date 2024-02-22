#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define ll long long
#define swap(x,y) {int temp=x; x=y; y=temp;}


typedef struct _{
    int id;
    int atk;
    struct _* left;
    struct _* right;
} Player;

int total;

Player* createPlayer(int id, int atk){
    Player* newPlayer = (Player*) malloc(sizeof(Player));
    newPlayer->atk = atk;
    newPlayer->id = id;
    newPlayer->left = newPlayer->right = NULL;
    return newPlayer;
}

int cmp(const void* lhs, const void *rhs){
    return (*(int*)rhs - *(int*)lhs);
}

int main(){
    Player* dummy = createPlayer(-1, -1);
    dummy->left = dummy->right = dummy;
    Player* end = dummy;
    int n, m; scanf("%d %d", &n, &m);
    for(int i=1; i<=n; i++){
        int atk; scanf("%d", &atk);
        
        printf("Round %d:", i);
        int died[m+1];
        int cnt = 0;

        // GetIdx
        while(end!=dummy && end->atk < atk){
            //printf(" %d", end->id);
            died[cnt++] = end->id;
            Player *tmp = end;
            end = end->left;
            free(tmp);
            total -= 1;
        }
        
        // Revolution
        if(total==m){
            Player *head = dummy->right;
            //printf(" %d", head->id);
            died[cnt++] = head->id;
            head->right->left = dummy;
            dummy->right = head->right;
            if(dummy->right == dummy) end = dummy;
            free(head);
            total -= 1;
        }

        // Add New Player
        Player *newPlayer = createPlayer(i, atk);
        end->right = newPlayer;
        newPlayer->left = end;
        newPlayer->right = dummy;
        end = newPlayer;
        total += 1;

        qsort(died, cnt, sizeof(died[0]), cmp);
        for(int j=0; j<cnt; j++){
            printf(" %d", died[j]);
        }
        printf("\n");
    }
    
    // Final Alive
    int alive[m+1];
    int cnt = 0;

    Player *cur = dummy->right;
    while(cur != dummy){
        //printf(" %d", cur->id);
        alive[cnt++] = cur->id;
        Player* tmp = cur;
        cur = cur->right; 
        free(tmp);
    }
    
    qsort(alive, cnt, sizeof(alive[0]), cmp);
    printf("Final:");
    for(int j=0; j<cnt; j++){
        printf(" %d", alive[j]);
    }
    printf("\n");
    free(dummy);
    return 0;
}