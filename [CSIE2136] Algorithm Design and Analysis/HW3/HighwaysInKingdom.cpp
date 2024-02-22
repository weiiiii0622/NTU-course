#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT64_MAX
#define pll pair<ll, ll>

typedef struct _{
    int id;
    ll l;
    ll c;
} Edge;

typedef struct node{
    int id;
    ll dis;
    ll cost;
} Node;

int n, m;
vector<Edge> G[1000000+5];

ll key[400000+5];
ll cost[4000000+5];
ll parent[400000+5];
bool visited[400000+5]; 

struct Cmp{
    bool operator()(const Node& lhs, const Node& rhs) const{
        if(lhs.dis == rhs.dis){
            return lhs.cost > rhs.cost;
        }
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
        cin >> a >> b >> e.l >> e.c;
        e.id = b;
        G[a].push_back(e);
        e.id = a;
        G[b].push_back(e);
    }
    for(int i=0; i<n; i++){
        cost[i] = INF;
        key[i] = INF;
        parent[i] = -1;
    }
    
    ll max_dist = 0;
    ll max_dist_cost = 0;
    key[0] = 0;
    cost[0] = 0;
    priority_queue<Node, vector<Node>, Cmp> pq;
    Node s; s.id=0, s.dis=0, s.cost=0;
    pq.push(s);

    while(!pq.empty()){
        int v = pq.top().id; pq.pop();

        if(visited[v]) continue;
        visited[v] = true;
        if(key[v]>max_dist){
            max_dist = key[v];
            max_dist_cost = cost[v];
        }
        else if(key[v]==max_dist){
            max_dist_cost = cost[v];
        }

        for(auto e : G[v]){
            int u = e.id;
            if(!visited[u] && e.l <= key[u]){
                
                ll temp_cost;
                if(e.l < max_dist){
                    temp_cost = max_dist_cost;
                }
                else if(e.l == max_dist){
                    temp_cost = max_dist_cost+e.c;
                }
                else{
                    temp_cost = e.c;
                }
                if(e.l<key[u] || (e.l==key[u])&&temp_cost < cost[u]){
                    parent[u] = v;
                    key[u] = e.l;
                    cost[u] = temp_cost;
                    Node x; x.id=u, x.dis=key[u], x.cost=cost[u];
                    pq.push(x);
                }

                // cout << "u: " << u << '\n';
                //cout << "max_dist: " << max_dist << '\n';
                // cout << "e.l: " << e.l << '\n';
                // cout << "key["<<u<<"]: " << key[u] << '\n';
                //cout << "cost["<<u<<"]: " << cost[u] << '\n';
            }
        }
    }
    ll sum = 0;
    for(int i=0; i<n; i++){
        // cout << "parent["<<i<<"]: " << parent[i] << '\n';
        // cout << "cost["<<i<<"]: " << cost[i] << '\n';
        // cout << "key["<<i<<"]: " << key[i] << "\n\n";
        sum += cost[i];
    }
    cout << sum << '\n';
    return 0;
}