#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX
#define pll pair<ll, ll>


typedef struct Point{
    int id;
    ll l;
    ll r;
    ll x;
} Point;

struct Lcomp{
    bool operator()(const Point& lhs, const Point& rhs) const{
        if(lhs.l==rhs.l){
            return lhs.x > rhs.x;
        }
        return lhs.l < rhs.l;
    }
};


struct Rcomp{
    bool operator()(const Point& lhs, const Point& rhs) const{
        if(lhs.r==rhs.r){
            return lhs.x < rhs.x;
        }
        return lhs.r < rhs.r;
    }
};


int n;
ll l1, l2, r1, r2;
ll ans;
Point arr[1000005];

void solve(int l, int r){
    if(l==r) return;
    int mid = (l+r)/2;
    solve(l, mid);
    solve(mid+1, r);

    int temp[r-l+1];
    int i=l, j=mid+1, k=0;
    while(i<=mid && j<=r){
        if(arr[i].id < arr[j].id){
            temp[k] = arr[i].id;
            i++, k++;
        }
        else{
            temp[k] = arr[j].id;
            k++, j++, ans+=mid-i+1;
        }
    }
    while(i<=mid){
        temp[k] = arr[i].id;
        i++, k++;
    }
    while(j<=r){
        temp[k] = arr[j].id;
        j++, k++;
    }
    for(int i=0; i<k; i++){
        arr[i+l].id = temp[i];
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    scanf("%d", &n);
    scanf("%lld %lld %lld %lld", &l1, &l2, &r1, &r2);
    // cin >> n;
    // cin >> l1 >> l2 >> r1 >> r2;
    for(int i=0; i<n; i++){
        ll x, y; 
        scanf("%lld %lld", &x, &y);
        // cin >> x >> y;
        arr[i].x = x;
        arr[i].l = x*l1-y*l2;
        arr[i].r = x*r1-y*r2;
    }

    sort(arr, arr+n, Lcomp());
    for(int i=0; i<n; i++){
        arr[i].id = i+1;
    }
    sort(arr, arr+n, Rcomp());

    solve(0, n-1);
    printf("%lld\n", ans);
    // cout << ans << '\n';
    return 0;
}