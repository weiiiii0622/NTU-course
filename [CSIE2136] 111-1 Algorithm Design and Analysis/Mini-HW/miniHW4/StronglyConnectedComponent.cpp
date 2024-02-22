#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX

class Graph{
public:
    Graph(int, int);
    ~Graph();
    void init();
    void DFS1(stack<int>&, int, int[]);
    void DFS2(int, int[], ll&);
    void solve();
private:
    int n;
    int m;
    int SCC;
    ll sol;
    vector<vector<int>> l;
    vector<vector<int>> lT;
};

Graph::Graph(int n, int m){
    this->n = n;
    this->m = m;
    this->SCC = 0;
    this->sol = 0;
    l = vector<vector<int>>(n+5);
    lT = vector<vector<int>>(n+5);
}

Graph::~Graph(){
}

void Graph::init(){
    int u, v;
    for(int i=0; i<m; i++){
        cin >> u >> v;
        l[u].push_back(v);
        lT[v].push_back(u);
    }
}

void Graph::DFS1(stack<int> &s, int u, int visited[]){
    visited[u] = 1;
    for(auto &x:l[u]){
        if(!visited[x]){
            DFS1(s, x, visited);
        }
    }
    s.push(u);
}

void Graph::DFS2(int u, int visited[], ll &cnt){
    //cout << u << ' ';
    cnt += 1;
    visited[u] = 1;
    for(auto &x:lT[u]){
        if(!visited[x]){
            DFS2(x, visited, cnt);
        }
    }
}

void Graph::solve(){
    stack<int> s;
    int visited[n+5];
    memset(visited, 0, sizeof(int)*(n+5));
    for(int i=0; i<n; i++){
        if(!visited[i]){
            //cout << "i: " << i << '\n';
            DFS1(s, i, visited);
        }
    }
    memset(visited, 0, sizeof(int)*(n+5));
    while(!s.empty()){
        if(!visited[s.top()]){
            ll cnt = 0;
            //cout << "s.top(): " << s.top() << '\n';
            DFS2(s.top(), visited, cnt);
            //cout << cnt << '\n';
            sol += (cnt)*(cnt-1)/2;
            SCC += 1;
        }
        s.pop();
    }
    cout << sol << '\n';
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m; cin >> n >> m;
    Graph G(n, m);
    G.init();
    G.solve();
    return 0;
}