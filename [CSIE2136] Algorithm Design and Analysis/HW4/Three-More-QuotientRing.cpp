#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX
#define MAX(x,y) x>y?x:y

int N;
ll v[405][405*2+10];
ll dp[405][405][405]; // dp[n][l][k]: max value of subarray with start:n, size: l, k cuts

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cin >> N;
    for(int i=1; i<=N; i++){
        for(int j=0; j<N; j++){
            if(j < i) cin >> v[i][N+1+j];
            else cin >> v[i][j+1];
        }
    }

    for(int n=1; n<=N; n++){
        for(int l=1; l<=(N-n+1); l++){
            int offset = l==N-n+1?n+l+n-1:n+l;
            dp[n][l][1] = v[n][offset];
            for(int k=2; k<=l; k++){
                for(int i=k-1; i<=l-1; i++){
                    dp[n][l][k] = MAX(dp[n][l][k], dp[n][i][k-1]+v[n+i][offset]);
                }
            }
        }
    }

    for(int k=1; k<=N; k++){
        ll ans = 0;
        for(int n=1; n<=N; n++){
            ans = MAX(ans, dp[n][N-n+1][k]);
        }
        if(k!=N) cout << ans << ' ';
        else cout << ans << '\n';
    }
    
    return 0;
}