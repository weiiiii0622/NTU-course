#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX

vector< queue<int> > v(63); 
vector< queue<int> > dis(63);
int cnt[63];
char str[500005];
ll K, len;

int getIdx(char c){
    if(c>='0' && c<='9'){
        return c-'0'+1;
    }
    else if(c>='A' && c<='Z'){
        return c-'A'+10+1;
    }
    else{
        return c-'a'+36+1;
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    scanf("%s", str);
    scanf("%lld", &K);
    len = strlen(str);
    for(int i=0; i<len; i++){
        int idx = getIdx(str[i]);
        v[idx].push(i);
        dis[idx].push(cnt[idx]);

        for(int j=1; j<idx; j++){
            cnt[j] += 1;
        }
    }

    int cur;
    for(cur=0; cur<len; cur++){
        if(K==0) break;
        int idx;
        for(idx=1; idx<=62; idx++){
            if(dis[idx].empty()) continue;
            if(dis[idx].front() > K){
                dis[idx].front() -= 1;
                continue;
            }

            K -= dis[idx].front();

            printf("%c", str[v[idx].front()]);
            str[v[idx].front()] = '\0';
            
            dis[idx].pop();
            v[idx].pop();
            break;
        }
    }

    for(int i=0; i<len; i++){
        if(str[i]!='\0'){
            printf("%c", str[i]);
        }
    }
    printf("\n");

    return 0;
}