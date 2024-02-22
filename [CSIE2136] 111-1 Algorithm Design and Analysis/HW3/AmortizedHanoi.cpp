#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX
#define plli pair<ll, int>

struct wCmp{
    bool operator()(const plli &lhs, const plli &rhs) const{
        return lhs.first < rhs.first;
    }
};

struct oCmp{
    bool operator()(const plli &lhs, const plli &rhs) const{
        return lhs.second < rhs.second;
    }
};

ll arr[1000005];
int ptr;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    ptr = 1;
    int m; cin >> m;
    string cmd;
    
    while(m--){
        cin >> cmd;
        if(cmd=="PUT"){
            int w; cin >> w;
            arr[ptr] = arr[ptr-1]+w;
            ptr++;
        }
        else if(cmd=="TAKE"){
            ptr--;
            cout << arr[ptr]-arr[ptr-1] << '\n';
        }
        else if(cmd=="CC"){
            ll sum = 0;
            int x, k; cin >> x >> k;
            int p = (int)ceil((float)x/k);
            vector<plli> v;
            for(int i=x; i>0; i--){
                ptr--;
                v.push_back(make_pair(arr[ptr]-arr[ptr-1], i));
            }
            sort(v.begin(), v.end(), wCmp());
            for(int i=x-1; i>=x-p; i--){
                sum += v[i].first;
                v.pop_back();
            }
            cout << sum << '\n';
            sort(v.begin(), v.end(), oCmp());
            for(int i=0; i<x-p; i++){
                arr[ptr] = arr[ptr-1]+v[i].first;
                ptr++;
            }
        }
        else if(cmd=="DD"){
            int x; cin >> x;
            cout << arr[ptr-1] - arr[ptr-x-1] << '\n';
        }
    }

    return 0;
}