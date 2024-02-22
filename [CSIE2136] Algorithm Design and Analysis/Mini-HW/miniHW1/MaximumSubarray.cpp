#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n; cin >> n;
    ll max = -INF, cur = 0; 
    int x;

    for(int i=0; i<n; i++){
        cin >> x;
        cur += x;
        //max = cur>max?cur:max;
        if(cur>max) max = cur;
        if(cur < 0){
            cur = 0;
        }
    }
    cout << max << '\n';
    return 0;
}