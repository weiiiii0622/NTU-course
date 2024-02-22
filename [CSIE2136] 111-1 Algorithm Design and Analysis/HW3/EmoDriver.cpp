#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define pii pair<int,int>
#define INF INT_MAX
#define INF64 INT64_MAX

struct piiCmp{
    bool operator()(const pii& lhs, const pii& rhs) const{
        return lhs.second > rhs.second;
    }
};

int N, M;
vector<vector<pii>> E(2005);

ll dikjstra(){
    int s=1;
    ll sum = 0;
    vector<int> dist(2000+5, INF/2);
    vector<bool> vis(2000+5, false);
    priority_queue<pii, vector<pii>, piiCmp> pq;

    dist[1] = 0;
    pq.push(make_pair(1, dist[1]));

    while(!pq.empty()){
        tie(s, std::ignore) = pq.top();
        pq.pop();
        if(vis[s]) continue;
        vis[s] = true;
        //cout << "s: " << s << '\n';

        for(auto [v, w]: E[s]){
            //cout << "v: " << v  << " w: " << w <<'\n';
            if(dist[v] > dist[s]+w){
                //cout << "relaxed v: " << v << '\n';
                dist[v] = dist[s]+w;
                pq.push(make_pair(v, dist[v]));
            }
        }
    }
    for(int i=1; i<=N; i++){
        //cout << "dist[" << i << "]: " << dist[i] << '\n';
        sum += dist[i];
    }
    return sum;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    ll ans = INF64;
    cin >> N >> M;
    for(int i=0; i<M; i++){
        int a, b, c; cin >> a >> b >> c;
        E[a].push_back(make_pair(b, c));
    }
    for(int i=2; i<=N; i++){
        E[1].push_back(make_pair(i,0));
        ll ret = dikjstra();
        ans = min(ans, ret);
        E[1].pop_back();
    }
    cout << ans << '\n';
    return 0;
}