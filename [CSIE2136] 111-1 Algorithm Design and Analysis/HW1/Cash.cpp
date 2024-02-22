#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX

int dp[5001], p[5001];
int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n, w; cin >> n >> w;
    memset(dp, -1, sizeof(int)*5001);
    for(int i=0; i<n; i++){
        cin >> p[i];
        dp[p[i]] = 0;
    }
    sort(p, p+n);
    int cnt = 0;
    dp[0] = 0;
    for(int i=1; i<=w; i++){
        if(dp[i]==-1) dp[i] = ++cnt;
        else if(dp[i]==0){
            cnt = 0;
            for(int j=0; j<n; j++){
                
                if(i+p[j] > w) continue;
                dp[i+p[j]] = 0;
            }
        }
    }

    for(int i=1; i<=w; i++){
        int cur = i;
        for(int i=n-1; i>=0; i--){
            cur %= p[i];
        }
        // cout << "i= " << i << " left: " << dp[i] << '\n';
        cout << abs(cur-dp[i]) << '\n';
    }

    return 0;
}