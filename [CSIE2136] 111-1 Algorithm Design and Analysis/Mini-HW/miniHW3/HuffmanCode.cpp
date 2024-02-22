#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define INF INT_MAX

typedef struct Node{
    int id;
    int fq;
    struct Node *l, *r;
    Node(int id, int fq){
        this->id = id;
        this->fq = fq;
        this->l = this->r = NULL;
    }
} Node;

struct cmp{
    bool operator()(const Node* lhs, const Node* rhs) const{
        return lhs->fq>rhs->fq;
    }
};

int n;
string encode[51];
priority_queue<Node*, vector<Node*>, cmp> pq;

void traverse(Node* cur, string code){
    if(cur==NULL) return;
    if(cur->id == -1){
        traverse(cur->l, code+"0");
        traverse(cur->r, code+"1");
    }
    else{
        encode[cur->id] = code;
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cin >> n;
    for(int i=0; i<n; i++){
        int fq; cin >> fq;
        pq.push(new Node(i, fq));
    }
    if(n==1){
        cout << "0\n";
    }
    else{
        while(pq.size() != 1){
            Node* a = pq.top(); pq.pop();
            Node* b = pq.top(); pq.pop();

            Node* z = new Node(-1, a->fq+b->fq);
            z->l = a;
            z->r = b;
            pq.push(z);
        }
        traverse(pq.top(), "");
        for(int i=0; i<n; i++){
            cout << encode[i] << '\n';
        }
    }
    return 0;
}