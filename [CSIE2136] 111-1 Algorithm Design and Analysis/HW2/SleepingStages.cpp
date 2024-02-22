#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT64_MAX

ll arr[1005];
ll dp[1005][1005];

// I/O optimize
void getInt(ll &p){
	p=0;
	int flag=1;
	char c=getchar();
	while(c<'0' || c>'9'){
		if(c=='-') flag=-1;
		c=getchar();
	}
	while(c>='0' && c<='9')
		p=p*10+(c-'0'),c=getchar();
	p*=flag;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    ll N, K; 
    cin >> N >> K;
    // getInt(N);
    // getInt(K);
    for(int i=1; i<=N; i++){
        cin >> arr[i];
        //getInt(arr[i]);
    }

    ll smax = -INF;
    ll cur = 0;
    for(int i=1; i<=N; i++){
        cur += arr[i];
        if(cur > smax) smax = cur;
        dp[i][1] = i*smax;
        if(cur < 0) cur = 0;
    }

    for(int k=2; k<=K; k++){
        for(int n=k; n<=N; n++){
            ll dpmin = INF;
            ll smax = -INF;
            ll cur = 0;
            for(int i=1; i<=n-k+1; i++){
                cur += arr[n-i+1];
                if(cur > smax) smax = cur;
                if(i*smax+dp[n-i][k-1] < dpmin){
                    dpmin = i*smax+dp[n-i][k-1];
                }
                if(cur < 0) cur = 0;
            }
            dp[n][k] = dpmin;
        }
    }

    cout << dp[N][K] << '\n';
    return 0;
}