#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX

// Credit: 王彥傑's hint from ADA dc community

int N, M, K;
int ans;
int dp[2][302][302];
int psum1[2][302][302];
int psum2[2][302][302];

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    scanf("%d %d %d", &N, &K, &M);
    for(int i=1; i<=K+1; i++){
        for(int j=1; j<=i; j++){
            dp[0][i][j] = 1;
            psum1[0][i][j] = psum1[0][i][j-1]+dp[0][i][j]>M ? psum1[0][i][j-1]+dp[0][i][j]-M : psum1[0][i][j-1]+dp[0][i][j];
            psum2[0][i][j] = psum2[0][i-1][j]+dp[0][i][j]>M ? psum2[0][i-1][j]+dp[0][i][j]-M : psum2[0][i-1][j]+dp[0][i][j];
        }
    }
    for(int n=1; n<N; n++){
        for(int i=1; i<=K+1; i++){
            for(int j=1; j<=i; j++){
                // int val = 0;
                // for(int s=0; s<=j; s++){
                //     val = val+dp[(n-1)%2][i][s] >= M ? val+dp[(n-1)%2][i][s]-M : val+dp[(n-1)%2][i][s];
                // }
                // for(int t=j; t<=i-1; t++){
                //     val = val+dp[(n-1)%2][t][j] >= M ? val+dp[(n-1)%2][t][j]-M : val+dp[(n-1)%2][t][j];
                // }
                // dp[n%2][i][j] = val;
                if(i>j){
                    dp[n%2][i][j] = psum1[(n-1)%2][i][j] - psum2[(n-1)%2][j-1][j];
                    dp[n%2][i][j] = dp[n%2][i][j]+psum2[(n-1)%2][i-1][j] > M ? dp[n%2][i][j]+psum2[(n-1)%2][i-1][j]-M : dp[n%2][i][j]+psum2[(n-1)%2][i-1][j];
                }
                else{
                    dp[n%2][i][j] = psum1[(n-1)%2][i][j];
                }
                psum1[n%2][i][j] = psum1[n%2][i][j-1]+dp[n%2][i][j]>M ? psum1[n%2][i][j-1]+dp[n%2][i][j]-M : psum1[n%2][i][j-1]+dp[n%2][i][j];
                psum2[n%2][i][j] = psum2[n%2][i-1][j]+dp[n%2][i][j]>M ? psum2[n%2][i-1][j]+dp[n%2][i][j]-M : psum2[n%2][i-1][j]+dp[n%2][i][j];
            }
        }
    }
    // for(int k=1; k<=K+1; k++){
    //     ans = ans+dp[(N-1)%2][K+1][k]>=M ? ans+dp[(N-1)%2][K+1][k]-M : ans+dp[(N-1)%2][K+1][k];
    // }
    printf("%d\n", psum1[(N-1)%2][K+1][K+1]);
    return 0;
}