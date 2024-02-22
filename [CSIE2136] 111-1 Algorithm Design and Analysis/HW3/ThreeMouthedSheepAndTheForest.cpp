#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX

typedef struct _{
    int id;
    int s;
    int f;
    int sum;
    vector<int> child;
} Node;

struct Cmp{
    bool operator()(const Node& lhs, const Node& rhs) const{
        return lhs.f < rhs.f;
    }
};


int N, M;
int dis;
int ci[3005], di[3005];
int dp[3005][3005];
vector<Node> v(3000+5);
vector<int> isRoot;

void DFS(int i){
    v[i].s = dis; dis++;
    for(auto x:v[i].child){
        DFS(x);
        v[i].sum += v[x].sum;
    }
    v[i].f = dis; dis++;
    v[i].sum += ci[i];
    return;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    cin >> N >> M;
    // 1-based
    for(int i=1; i<=N; i++){
        v[i].id = i;
        int f; cin >> f;
        if(f==-1){
            isRoot.push_back(i);
        }
        else{
            v[f+1].child.push_back(i);
        }
    }
    for(int i=1; i<=N; i++){
        cin >> ci[i] >> di[i];
    }
    for(auto x: isRoot){
        DFS(x);
        //cout << '\n';
    }
    sort(v.begin()+1, v.begin()+N+1, Cmp());
    // for(int i=1; i<=N; i++){
    //     cout << v[i].id << " sum: " << v[i].sum << '\n';
    // }
    // cout << '\n';
    // for(int c=0; c<=M; c++){
    //     int C0 = ci[v[0].id];
    //     int D0 = di[v[0].id];
    //     if(c-C0 >= 0 || c-D0 >= 0){
    //         dp[0][c] = max(dp[0][c], C0);
    //     }
    // }

    for(int i=1; i<=N; i++){
        // cout << "id: " << v[i].id << '\n';
        int Ci = ci[v[i].id];
        int Di = di[v[i].id];
        // cout << "Ci: " << Ci << " Di: " << Di << '\n';
        // cout << "off: " << (v[i].f-v[i].s+1)/2 << '\n';
        // cout << "Ci-sum: " << v[i].sum << '\n';
        for(int c=0; c<=M; c++){
            dp[i][c] = dp[i-1][c];
            if(c-Ci >= 0){
                dp[i][c] = max(dp[i][c], dp[i-1][c-Ci] + Ci);
            }
            if(c-Di >= 0){
                int off = (v[i].f-v[i].s+1)/2;
                dp[i][c] = max(dp[i][c], dp[i-off][c-Di] + v[i].sum);
            }
        }
        //cout << "dp: " << dp[i][M] << "\n\n";
    }
    cout << dp[N][M] << '\n';
    return 0;
}