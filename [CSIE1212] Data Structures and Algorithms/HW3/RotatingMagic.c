#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#define ll unsigned long long
#define TOTAL 4

ll prime[TOTAL] = {10619863, 12207031, 29986577, 40960001};
//ll prime[TOTAL] = {20000027};
ll h[TOTAL];
ll p[TOTAL][1000000 + 5];
ll s[TOTAL];
bool table[TOTAL][29986577 + 5];
char S[1000000 + 5];
char P[1000000 + 5];
int n, m;
ll d = 27;
ll ans;
ll INF = 18446744073709551610ULL;

int main(){
    scanf("%d %d", &n, &m);
    scanf("%s", S);
    scanf("%s", P);

    for(int i=0; i<TOTAL; i++){
        h[i] = 1;
        for(int j=0; j<m-1; j++){
            h[i] = (h[i]*d)%prime[i];
        }
    }
    for(int i=0; i<TOTAL; i++){
        for(int j=0; j<m; j++){
            p[i][0] = (((d*p[i][0])%prime[i]) + (P[j]-'A'+1)) % prime[i];
            s[i] = (((d*s[i])%prime[i]) + (S[j]-'A'+1)) % prime[i];
            table[i][p[i][0]] = true;
        }
        
        for(int j=1; j<m; j++){
            p[i][j] = p[i][j-1] + prime[i];
            p[i][j] -= (h[i] * (P[j-1]-'A'+1)) % prime[i];
            p[i][j] %= prime[i];
            p[i][j] *= d;
            p[i][j] %= prime[i];
            p[i][j] += (P[j-1]-'A'+1);
            p[i][j] %= prime[i];
            table[i][p[i][j]] = true;
        }
        // for(int j=1; j<m; j++){
        //     for(int k=0; k<m; k++){
        //         p[i][j] = (((d*p[i][j])%prime[i]) + (P[(j+k)%m]-'A'+1)) % prime[i];
        //     }
        //     table[i][p[i][j]] = true;
        // }
    }

    // for(int j=0; j<m; j++){
    //     for(int i=0; i<TOTAL; i++){
    //         printf("p[%d][%d]: %lld\n", i, j, p[i][j]);
    //     }
    // }

    bool haveAns = false;
    for(int t=0; t<=n-m; t++){
        // printf("t: %d\n", t);
        // for(int i=0; i<TOTAL; i++){
        //     printf("\ts[%d]: %lld\n", i, s[i]);
        // }
        haveAns = true;
        for(int i=0; i<TOTAL; i++){
            if(table[i][s[i]] == false) {haveAns = false; break;}
        }
        if(haveAns == true) ans += 1;

        if(t<n-m){
            for(int i=0; i<TOTAL; i++){
                s[i] += prime[i];
                s[i] = (((d*(s[i] - ((S[t]-'A'+1)*h[i])%prime[i])%prime[i])) + (S[t+m]-'A'+1)) % prime[i];
            }
        }
    }
    printf("%lld\n", ans);
    return 0;
}