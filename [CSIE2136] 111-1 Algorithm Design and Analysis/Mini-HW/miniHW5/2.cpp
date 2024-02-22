#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT64_MAX
#define pll pair<ll, ll>

typedef struct _{
    int id;
    ll l;
} Edge;

typedef struct node{
    int id;
    ll dis;
} Node;

int n, m;
vector<Edge> G[1000000+5];

ll key[1000000+5];
ll parent[1000000+5];
bool visited[1000000+5]; 

struct Cmp{
    bool operator()(const Node& lhs, const Node& rhs) const{

        return lhs.dis > rhs.dis;
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    cin >> n >> m;
    
    int heap[n];
    for(int i=0; i<m; i++){
        int a, b; Edge e;
        //cin >> a >> b >> e.l >> e.c;
        cin >> a >> b >> e.l;
        a = a-1; b = b-1;
        e.id = b;
        G[a].push_back(e);
        e.id = a;
        G[b].push_back(e);
    }
    for(int i=0; i<n; i++){
        key[i] = INF;
        parent[i] = -1;
        visited[i] = false;
    }
    Node s; s.id = 0, s.dis = 0;
    key[0] = 0;
    priority_queue<Node, vector<Node>, Cmp> pq;
    pq.push(s);
    while(!pq.empty()){
        int v = pq.top().id; pq.pop();
        //cout << "v: " << v << '\n';
        if(visited[v]) continue;
        visited[v] = true;

        for(auto e : G[v]){
            int u = e.id;
            if(!visited[u] && e.l < key[u]){
                parent[u] = v;
                key[u] = e.l;
                Node x; x.id=u, x.dis=key[u];
                pq.push(x);
            }
        }
        
    }
    ll sum = 0;
    for(int i=0; i<n; i++){
        sum += key[i];
    }
    cout << sum << '\n';
    return 0;
}