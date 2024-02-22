#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX

typedef struct _{
    int a;
    int b;
    ll w;
} Edge;

struct eCmp{
    bool operator()(const Edge& lhs, const Edge& rhs) const{
        return lhs.w < rhs.w;
    }
};

int N, M;
ll ans;
vector<Edge> E;
int p[1000000+5];
int rk[1000000+5];

void Init(int N){
    for(int i=0; i<N; i++){
        p[i] = i;
        rk[i] = 1;
    }
}

int Find(int a){
    if(a == p[a]) return a;
    else{
        return p[a] = Find(p[a]);
    }
}

void Union(int a, int b){
    int ap = Find(a);
    int bp = Find(b);
    if(rk[ap] > rk[bp]){
        p[bp] = ap;
    }
    else if(rk[ap] < rk[bp]){
        p[ap] = bp;
    }
    else{
        p[bp] = ap;
        rk[ap] += 1;
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    cin >> N >> M;
    Init(N);
    for(int i=0; i<M; i++){
        Edge e;
        cin >> e.a >> e.b >> e.w;
        E.push_back(e);
    }
    sort(E.begin(), E.end(), eCmp());
    for(int i=0, j=0; i<N-1 && j<M; i++, j++){
        while(Find(E[j].a) == Find(E[j].b)) j++;
        ans += E[j].w;
        Union(E[j].a, E[j].b);
    }
    cout << ans << '\n';
    return 0;
}
