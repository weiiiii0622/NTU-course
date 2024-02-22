#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define ll unsigned long long
#define TOTAL 3

typedef struct _node{
    int cnt;
    int *val;
} Node;

ll INF = 18446744073709551610ULL;
ll prime[TOTAL] = {50000017, 325491039, 1000000009};
ll D[TOTAL]; // d^(M-1) for rolling hash
Node table[50000017]; // Hash table
int N, Q, M;
int d = 27;
ll ans = 0;

void helper(char* str, int P){
    ll min_value_0 = INF;
    ll cur_0 = 0;
    ll min_value_1 = INF;
    ll cur_1 = 0;
    ll min_value_2 = INF;
    ll cur_2 = 0;
    // Normal hash
    for(int j=0; j<M; j++){
        cur_0 = (((d*cur_0)%prime[0]) + (str[j]-'a'+1)) % prime[0]; // 1st prime
        cur_1 = (((d*cur_1)%prime[1]) + (str[j]-'a'+1)) % prime[1]; // 2nd prime
        cur_2 = (((d*cur_2)%prime[2]) + (str[j]-'a'+1)) % prime[2]; // 3rd prime
    }
    if(cur_0 < min_value_0) min_value_0 = cur_0;
    if(cur_1 < min_value_1) min_value_1 = cur_1;
    if(cur_2 < min_value_2) min_value_2 = cur_2;
    // Rotate
    for(int j=1; j<M; j++){
        // 1st prime
        cur_0 = cur_0 + prime[0];
        cur_0 -= (D[0] * (str[j-1]-'a'+1)) % prime[0];
        cur_0 %= prime[0];
        cur_0 *= d;
        cur_0 %= prime[0];
        cur_0 += (str[j-1]-'a'+1);
        cur_0 %= prime[0];
        if(cur_0 < min_value_0) min_value_0 = cur_0;
        // 2nd prime
        cur_1 = cur_1 + prime[1];
        cur_1 -= (D[1] * (str[j-1]-'a'+1)) % prime[1];
        cur_1 %= prime[1];
        cur_1 *= d;
        cur_1 %= prime[1];
        cur_1 += (str[j-1]-'a'+1);
        cur_1 %= prime[1];
        if(cur_1 < min_value_1) min_value_1 = cur_1;
        // 3rd prime
        cur_2 = cur_2 + prime[2];
        cur_2 -= (D[2] * (str[j-1]-'a'+1)) % prime[2];
        cur_2 %= prime[2];
        cur_2 *= d;
        cur_2 %= prime[2];
        cur_2 += (str[j-1]-'a'+1);
        cur_2 %= prime[2];
        if(cur_2 < min_value_2) min_value_2 = cur_2;
    }
    // Linear Probing
    int loc = min_value_0;
    while(table[loc].cnt !=0 && table[loc].val[0]!=min_value_1 && table[loc].val[1]!=min_value_2){
        loc = (loc+1)%prime[0];
    }
    // Hash the min value and update answer
    if(table[loc].cnt == 0){
        if(P==2) return;
        table[loc].cnt = 1;
        table[loc].val = (int*) malloc(sizeof(int)*(TOTAL-1));
        table[loc].val[0] = min_value_1;
        table[loc].val[1] = min_value_2;
    }
    else{
        if(P==1){
            ans += table[loc].cnt;
            table[loc].cnt += 1;
        }
        else if(P==2){
            table[loc].cnt -= 1;
            ans -= table[loc].cnt;
        }
    }
}

int main(){
    scanf("%d %d", &N, &Q);
    char str[1000000+5];

    // Preprocess the first str to get D
    scanf("%s", str);
    M = strlen(str);
    for(int i=0; i<TOTAL; i++){
        D[i] = 1;
        for(int j=0; j<M-1; j++){
            D[i] = (D[i]*d)%prime[i];
        }
    }
    helper(str, 1);
    // Stage 1
    for(int i=1; i<N; i++){
        scanf("%s", str);
        helper(str, 1);
    }
    printf("%lld\n", ans);
    // Stage 2
    int P;
    for(int i=0; i<Q; i++){
        scanf("%d", &P);
        scanf("%s", str);
        helper(str, P);
        printf("%lld\n", ans);
    }
    return 0;
}