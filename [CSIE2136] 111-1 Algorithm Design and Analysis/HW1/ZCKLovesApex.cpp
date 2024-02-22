#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX

// I/O optimize
int getint(){
    int w=0,q=0;
    char c=getchar();
    while(c<'0'||c>'9'&&c!='-')c=getchar();
    if(c=='-')q=1,c=getchar();
    while(c>='0'&&c<='9')w=w*10+c-'0',c=getchar();
    return q?-w:w;
}

int lc[100001], rc[100001];
int nums[500001];
ll ans;

void solve(int l, int r){
    if(l==r) return;
    int mid = (l+r)/2;
    solve(l, mid);
    solve(mid+1, r);

    int i = mid, j = mid+1;
    int lmn = nums[i], rmn = nums[j]; 
    int mn = min(lmn, rmn);
    int flag;  // 0: moveleft 1: moveright
    if(lmn > rmn){
        flag = 0;
    }
    else{
        flag = 1;
    }

    while(i>=l && j<=r){
        if(flag==0){
            if(nums[i]<rmn){
                lmn = nums[i];
                flag = 1;
                continue;
            }
            mn = min(mn, nums[i]);
            lc[nums[i]] += 1;
            if(nums[i]+mn <= 100000) ans += rc[nums[i]+mn];
            if(nums[i]-mn >= 0) ans += rc[nums[i]-mn];
            i--;
        }
        else{
            if(nums[j]<lmn){
                rmn = nums[j];
                flag = 0;
                continue;
            }
            mn = min(mn, nums[j]);
            rc[nums[j]] += 1;
            if(nums[j]+mn <= 100000) ans += lc[nums[j]+mn];
            if(nums[j]-mn >= 0) ans += lc[nums[j]-mn];
            j++; 
        }
    }
    while(i>=l){
        lc[nums[i]] += 1;
        mn = min(nums[i], mn);
        if(nums[i]+mn <= 100000) ans += rc[nums[i]+mn];
        if(nums[i]-mn >= 0) ans += rc[nums[i]-mn];
        i--;
    }
    while(j<=r){
        rc[nums[j]] += 1;
        mn = min(nums[j], mn);       
        if(nums[j]+mn <= 100000) ans += lc[nums[j]+mn];
        if(nums[j]-mn >= 0) ans += lc[nums[j]-mn];
        j++; 
    }
    for(int i=l; i<=mid; i++){
        lc[nums[i]] = 0;
    }
    for(int i=mid+1; i<=r; i++){
        rc[nums[i]] = 0;
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n = getint();
    for(int i=0; i<n; i++){
        nums[i] = getint();
    }
    solve(0, n-1);
    cout << ans << '\n';
    return 0;
}