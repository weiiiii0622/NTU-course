#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define ll long long
#define swap(x,y) {int temp=x; x=y; y=temp;}


typedef struct _{
    int id;
    int atk;
    int rank;
    int rev;
    int rev_ind;
    struct _* left;
    struct _* right;
} Player;

typedef struct helper{
    int id;
    int rank;
    int rev;
    int rev_ind;
} Helper;


int total;

Player* createPlayer(int id, int atk, int rank, int rev, int rev_ind){
    Player* newPlayer = (Player*) malloc(sizeof(Player));
    newPlayer->atk = atk;
    newPlayer->id = id;
    newPlayer->rank = rank;
    newPlayer->rev = rev;
    newPlayer->rev_ind = rev_ind;
    newPlayer->left = newPlayer->right = NULL;
    return newPlayer;
}

int cmp(const void* lhs, const void *rhs){
    Helper *r = (Helper*)rhs;
    Helper *l = (Helper*)lhs;
    return ((r->id)-(l->id));
}

int main(){
    Player* dummy = createPlayer(-1, -1, -1, -1, -1);
    dummy->left = dummy->right = dummy;
    Player* end = dummy;
    int n, m; scanf("%d %d", &n, &m);
    int cur_rev = 0;
    int pre_rev = 0;
    int rev_ind = 0;
    for(int i=1; i<=n; i++){
        int atk; scanf("%d", &atk);
        
        printf("Round %d:", i);
        Helper died[m+1];
        int cnt = 0;

        // GetIdx & Rank
        while(end!=dummy && end->atk < atk){
            //printf(" %d", end->id);
            died[cnt].id = end->id;
            died[cnt].rank = end->rank;
            died[cnt].rev = end->rev;
            died[cnt].rev_ind = end->rev_ind;
            cnt++;
            Player *tmp = end;
            end = end->left;
            free(tmp);
            total -= 1;
        }
        
        // Revolution
        if(total==m){
            Player *head = dummy->right;
            //printf(" %d", head->id);
            died[cnt].id = head->id;
            died[cnt].rank = head->rank;
            died[cnt].rev = head->rev;
            died[cnt].rev_ind = head->rev_ind;
            cnt++;
            head->right->left = dummy;
            dummy->right = head->right;
            if(dummy->right == dummy) end = dummy;
            free(head);
            total -= 1;
            rev_ind += 1;
        }
        pre_rev = cur_rev;
        cur_rev += rev_ind;

        // Add New Player
        Player *newPlayer = createPlayer(i, atk, m-total, cur_rev, rev_ind);
        end->right = newPlayer;
        newPlayer->left = end;
        newPlayer->right = dummy;
        end = newPlayer;
        total += 1;

        qsort(died, cnt, sizeof(died[0]), cmp);
        for(int j=0; j<cnt; j++){
            //printf(" %d,%d", died[j].id, died[j].rank*(i-(died[j].id))+rev[i-1]-rev[died[j].id]-rev_ind[died[j].id]*(i-(died[j].id)-1));
            printf(" %d,%d", died[j].id, died[j].rank*(i-(died[j].id))+pre_rev-died[j].rev-died[j].rev_ind*(i-(died[j].id)-1));
        }
        printf("\n");
    }
    
    // Final Alive
    Helper alive[m+1];
    int cnt = 0;

    Player *cur = dummy->right;
    while(cur != dummy){
        //printf(" %d", cur->id);
        alive[cnt].id = cur->id;
        alive[cnt].rank = cur->rank;
        alive[cnt].rev = cur->rev;
        alive[cnt].rev_ind = cur->rev_ind;
        cnt++;
        Player* tmp = cur;
        cur = cur->right; 
        free(tmp);
    }
    
    qsort(alive, cnt, sizeof(alive[0]), cmp);
    printf("Final:");
    for(int j=0; j<cnt; j++){
        //printf(" %d,%d", alive[j].id, alive[j].rank*(n-alive[j].id+1)+rev[n]-rev[alive[j].id]-rev_ind[alive[j].id]*(n-(alive[j].id)));
        printf(" %d,%d", alive[j].id, alive[j].rank*(n-alive[j].id+1)+cur_rev-alive[j].rev-alive[j].rev_ind*(n-(alive[j].id)));
    }
    printf("\n");
    free(dummy);
    return 0;
}