#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX

int n, m;
bool x[2000+5];
int c[100000+5][3];

void generate(){
    for(int i=0; i<n; i++){
        x[i] = rand()%2==1?true:false;
    }
    return;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    srand( time(NULL) );

    cin >> n;
    cin >> m;
    for(int i=0; i<m; i++){
        cin >> c[i][0] >> c[i][1] >> c[i][2];
    }
    int cnt;
    while(1){
        cnt = 0;
        generate();
        for(int i=0; i<m; i++){
            bool arg1, arg2, arg3;
            c[i][0]<0 ? arg1=!x[abs(c[i][0])-1] : arg1=x[abs(c[i][0])-1];
            c[i][1]<0 ? arg2=!x[abs(c[i][1])-1] : arg2=x[abs(c[i][1])-1];
            c[i][2]<0 ? arg3=!x[abs(c[i][2])-1] : arg3=x[abs(c[i][2])-1];
            if(arg1||arg2||arg3) cnt += 1;
        }
        if(8*cnt >= 7*m){
            break;
        }
    }
    for(int i=0; i<n; i++){
        if(i==n-1){
            cout << ((x[i]==true)?1:-1) << '\n';
        }
        else{
            cout << ((x[i]==true)?1:-1) << ' ';
        }
    }
    return 0;
}