#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX


// Sol2 Kadane learned from Github
int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n; cin >> n;
    ll x; cin >> x;
    ll max = x, cur = x;
    for(int i=1; i<n; i++){
        cin >> x;
        cur = std::max(cur+x, x);
        max = std::max(max, cur);
    }
    cout << max << '\n';
    return 0;
}