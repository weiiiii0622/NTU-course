#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT64_MAX
#define pll pair<ll, ll>
#define pill pair<ll, pair<ll, ll> >

vector< vector<pll> > v(100001);
ll s[100001];
pill res[100001];
ll ans;

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

struct SortGroup{
    bool operator()( const pll& lhs, const pll& rhs)const {
        return lhs.second > rhs.second;
    }
};

struct SortRes{
    bool operator()( const pill& lhs, const pill& rhs)const {
        
        return lhs.first*rhs.second.second > rhs.first*lhs.second.second;
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    ll N; 
    //cin >> N;
    getInt(N);
    for(int i=0; i<N; i++){
        //cin >> res[i].first;
        getInt(res[i].first);
        for(int j=0; j<res[i].first; j++){
            ll a, b; 
            //cin >> a >> b;
            getInt(a); getInt(b);
            res[i].second.second += a;
            v[i].push_back(make_pair(a,b));
        }
        
        sort(v[i].begin(), v[i].end(), SortGroup());
        // for(auto x: v[i]){
        //     cout << x.first << ' ';
        // }
        // cout << '\n';
        ll cnt = 0;
        ll longest = 0;
        for(int j=0; j<res[i].first; j++){
            cnt += v[i][j].first;
            longest = max(longest, cnt+v[i][j].second);
        }
        res[i].second.first = longest;
    }

    sort(res, res+N, SortRes());
    ll wt = 0;
    for(int i=0; i<N; i++){
        ans += res[i].first*(res[i].second.first+wt);
        wt += res[i].second.second;
    }
    //cout << ans << '\n';
    printf("%lld\n", ans);
    

    return 0;
}