#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define ll long long
#define swap(x,y) {int temp=x; x=y; y=temp;}
#define PRIME 1000000007
int readInt() {
	int x = 0, c = getchar(), neg = false;
	while(('0' > c || c > '9') && c!='-' && c!=EOF) c = getchar();
	if(c == '-') neg = true, c = getchar();
	while('0' <= c && c <= '9') x = x*10 + (c^'0'), c = getchar();
	if(neg) x = -x;
	return x; // returns 0 if EOF
}

void write(ll x) {
  static int sta[68];
  int top = 0;
  do {
    sta[top++] = x % 10, x /= 10;
  } while (x);
  while (top) putchar(sta[--top] + 48);  // 48 是 '0'
}

typedef struct _Node{
    ll dif;
    int type;  // 0:原本是偶 1:原本是奇
    char color;
    struct _Node *left;
    struct _Node *right;
    struct _Node *p;
    ll l_size, r_size;
    ll val[2]; // 0: left val sum / 1: right val sum
} Node;

Node* root;
Node* NIL;
Node* dump[100005];
ll cost[100005][2];
ll total = 0;
int N, M;

Node* newNode(int dif){
    Node* tmp = (Node*) malloc(sizeof(Node));
    tmp->type = -1;
    tmp->color = 'r';
    tmp->dif = dif;
    tmp->left = tmp->right = tmp->p = NIL;
    tmp->l_size = tmp->r_size = tmp->val[0] = tmp->val[1] = 0;
    return tmp;
}

void update_val(Node* cur, ll num, int type){ // !!!!!
    if(type == 0) cur->type = num%2; // init
    cur->val[0] = cur->val[1] = 0;
    ll l_num = num - 1 - cur->l_size + cur->left->l_size + 1;
    ll r_num = num + cur->right->l_size + 1;
    if(cur->type == num%2){ // 自己奇偶沒有reverse
        // self
        cur->val[1-(num%2)] += cur->dif;
        // left sub-tree
        if(cur->left != NIL){
            if(cur->left->type == l_num%2){ // 左邊 奇偶沒有reverse
                cur->val[0] += cur->left->val[0];
                cur->val[1] += cur->left->val[1];
            }
            else{ // 左邊 奇偶reverse
                cur->val[0] += cur->left->val[1];
                cur->val[1] += cur->left->val[0];
            }
        }
        // right sub-tree
        if(cur->right != NIL){
            if(cur->right->type == r_num%2){ // 右邊 奇偶沒有reverse
                cur->val[0] += cur->right->val[0];
                cur->val[1] += cur->right->val[1];
            }
            else{ // 右邊 奇偶reverse
                cur->val[0] += cur->right->val[1];
                cur->val[1] += cur->right->val[0];
            }
        }
    }
    else{ // 自己 奇偶reverse
        // self
        cur->val[num%2] += cur->dif;
        // left sub-tree
        if(cur->left != NIL){
            if(cur->left->type == l_num%2){ // 左邊 奇偶沒有reverse
                cur->val[1] += cur->left->val[0];
                cur->val[0] += cur->left->val[1];
            }
            else{ // 左邊 奇偶reverse
                cur->val[1] += cur->left->val[1];
                cur->val[0] += cur->left->val[0];
            }
        }
        // right sub-tree
        if(cur->right != NIL){
            if(cur->right->type == r_num%2){ // 右邊 奇偶沒有reverse
                cur->val[1] += cur->right->val[0];
                cur->val[0] += cur->right->val[1];
            }
            else{ // 右邊 奇偶reverse
                cur->val[1] += cur->right->val[1];
                cur->val[0] += cur->right->val[0];
            }
        }
    }
}

// rb-tree opertaion
void Left_Rotate(Node* x, ll num, int type){
    Node* y = x->right;
    ll x_num = num;
    ll y_num = num + y->l_size + 1;

    x->right = y->left;
    if(y->left != NIL){
        y->left->p = x;
    }
    y->p = x->p;
    if(x->p == NIL){
        root = y;
    }
    else if(x == x->p->left){
        x->p->left = y;
    }
    else{
        x->p->right = y;
    }
    y->left = x;
    x->p = y;

    // update l_size and r_size
    x->r_size = y->l_size;
    y->l_size = x->l_size + x->r_size + 1;
    // update val
    if(type == 1){
        update_val(x, x_num, 1);
        update_val(y, y_num, 1);
    }
}

void Right_Rotate(Node* x, ll num, int type){
    Node* y = x->left;
    ll x_num = num;
    ll y_num = num - 1 - y->r_size;
    x->left = y->right;
    if(y->right != NIL){
        y->right->p = x;
    }
    y->p = x->p;
    if(x->p == NIL){
        root = y;
    }
    else if(x == x->p->left){
        x->p->left = y;
    }
    else{
        x->p->right = y;
    }
    y->right = x;
    x->p = y;
    // update l_size and r_size
    x->l_size = y->r_size;
    y->r_size = x->l_size + x->r_size + 1;
    // update val
    if(type == 1){
        update_val(x, x_num, 1);
        update_val(y, y_num, 1);
    }
}

void insert_fixup(Node* z, ll num, int type){
    ll z_num = num;
    Node* y;
    while(z->p->color == 'r'){
        if(z->p == z->p->p->left){
            y = z->p->p->right;
            if(y->color == 'r'){
                z->p->color = 'b';
                y->color = 'b';
                z->p->p->color = 'r';
                // change z_num
                if(z == z->p->left) z_num = z_num + (z->r_size + 1 + z->p->r_size + 1);
                else z_num = z_num - z->l_size - 1 + z->p->r_size + 1;
                z = z->p->p;
            }
            else{
                if(z == z->p->right){
                    // change z_num
                    z_num = z_num - z->l_size - 1;
                    z = z->p;
                    Left_Rotate(z, z_num, type);
                }
                z->p->color = 'b';
                z->p->p->color = 'r';
                Right_Rotate(z->p->p, (z_num + (z->r_size + 1 + z->p->r_size + 1)), type);
            }
        }
        else{
            y = z->p->p->left;
            if(y->color == 'r'){
                z->p->color = 'b';
                y->color = 'b';
                z->p->p->color = 'r';
                // change z_num
                if(z == z->p->left) z_num = z_num + z->r_size + 1 - z->p->l_size - 1;
                else z_num = z_num - z->l_size - 1 - z->p->l_size - 1;
                z = z->p->p;
            }
            else{
                if(z == z->p->left){
                    // change z_num
                    z_num = z_num + z->r_size + 1;
                    z = z->p;
                    Right_Rotate(z, z_num, type);
                }
                z->p->color = 'b';
                z->p->p->color = 'r';
                Left_Rotate(z->p->p, z_num - z->l_size - 1 - z->p->l_size - 1, type);
            }
        }
    }
    root->color = 'b';
}

void insert(Node* z, int type){
    ll z_num = 0;
    Node* y = NIL;
    Node* x = root;
    while(x != NIL){
        y = x;
        if(z->dif > x->dif){ // go left
            x->l_size += 1;
            x = x->left;
        }
        else{ // go right
            z_num = z_num + x->l_size + 1;
            x->r_size += 1;
            x = x->right;
        }
    }
    z_num += 1;
    z->p = y;
    if(y == NIL){
        root = z;
    }
    else if(z->dif > y->dif){
        y->left = z;
    }
    else{
        y->right = z;
    }

    // update upward
    ll tmp_num = z_num;
    Node* tmp = z;
    int i=0;
    while(tmp != NIL && type == 1){
        update_val(tmp, tmp_num, i);
        if(tmp->p->left == tmp) tmp_num = tmp_num + tmp->r_size + 1;
        else tmp_num = tmp_num - tmp->l_size - 1;
        tmp = tmp->p;
        i++;
    }
    insert_fixup(z, z_num, type);
}

void delete_fixup(Node* x, ll num, int type){
    Node* w;
    ll x_num = num;
    while(x != root && x->color == 'b'){
        if(x == x->p->left){
            w = x->p->right;
            if(w->color == 'r'){
                w->color = 'b';
                x->p->color = 'r';
                Left_Rotate(x->p, (x_num + x->r_size + 1), type);
                w = x->p->right;
            }
            if(w->left->color == 'b' && w->right->color == 'b'){
                w->color = 'r';
                // update x_num
                x_num = x_num + x->r_size + 1;
                x = x->p;
            }
            else{
                if(w->right->color == 'b'){
                    w->left->color = 'b';
                    w->color = 'r';
                    Right_Rotate(w, (x_num + x->r_size + 1 + w->l_size + 1), type);
                    w = x->p->right;
                }
                w->color = x->p->color;
                x->p->color = 'b';
                w->right->color = 'b';
                Left_Rotate(x->p, (x_num + x->r_size + 1), type);
                x = root;
            }
        }
        else{
            w = x->p->left;
            if(w->color == 'r'){
                w->color = 'b';
                x->p->color = 'r';
                Right_Rotate(x->p, (x_num - x->l_size - 1), type);
                w = x->p->left;
            }
            if(w->right->color == 'b' && w->left->color == 'b'){
                w->color = 'r';
                // update x_num
                x_num = x_num - x->l_size - 1;
                x = x->p;
            }
            else{
                if(w->left->color == 'b'){
                    w->right->color = 'b';
                    w->color = 'r';
                    Left_Rotate(w, (x_num - x->l_size - 1 - 1 - w->r_size), type);
                    w = x->p->left;
                }
                w->color = x->p->color;
                x->p->color = 'b';
                w->left->color = 'b';
                Right_Rotate(x->p, (x_num - x->l_size - 1), type);
                x = root;
            }
        }
    }
    x->color = 'b';
}

void delete(int dif, int type){
    ll z_num = 0;
    ll y_num = 0, x_num = 0;
    Node* z = root;
    Node* x;
    Node* y;
    while(z != NIL){
        if(dif == z->dif) break;
        else if(dif > z->dif){ // go left
            z->l_size -= 1;
            z = z->left;
        }
        else{ // go right
            z_num = z_num + z->l_size + 1;
            z->r_size -= 1;
            z = z->right;
        }
    }
    z_num = z_num + z->l_size + 1;
    if(z->left == NIL || z->right == NIL){
        y = z;
        y_num = z_num;
    }
    else{
        y = z->right;
        if(y != NIL) z->r_size -= 1;
        while(y->left != NIL){
            y->l_size -= 1;
            y = y->left;
        }
        y_num = z_num + y->l_size + 1;
    }
    if(y->left != NIL){
        x_num = y_num - 1 - y->left->r_size;
        x = y->left;
    }
    else{
        x_num = y_num + y->right->l_size + 1;
        x = y->right;
    }
    x->p = y->p;
    if(y->p == NIL){
        root = x;
    }
    else if(y == y->p->left){
        y->p->left = x;
    }
    else{
        y->p->right = x;
    }
    if(y != z){
        z->dif = y->dif;
    }

    // update upward
    Node* tmp = x->p;
    ll tmp_num;
    if(tmp->left == x) tmp_num = x_num + x->r_size + 1;
    else tmp_num = x_num - 1 - 1 - x->l_size;
    while(tmp != NIL && type == 1){
        update_val(tmp, tmp_num, 1);
        if(tmp->p->left == tmp) tmp_num = tmp_num + tmp->r_size + 1;
        else tmp_num = tmp_num - tmp->l_size - 1;
        tmp = tmp->p;
    }

    if(y->color == 'b'){
        delete_fixup(x, x_num, type);
    }
}

void init(Node* root, ll num){
    if(root == NIL) return;
    init(root->left, num);
    init(root->right, num+root->l_size+1);   
    update_val(root, num+root->l_size+1, 0);
}

// DEBUG
void print(Node *cur, ll num){
    printf("Node #%lld\n", num);
    printf("\tdif:%lld\n", cur->dif);
    printf("\tcolor: %c, type: %d\n", cur->color, cur->type);
    printf("\tsize: (%lld,%lld) // val: (%lld,%lld)\n", cur->l_size, cur->r_size, cur->val[0], cur->val[1]);
}
void inorder(Node* root, ll num){
    if(root == NIL) return;
    inorder(root->left, num);
    print(root, num+root->l_size+1);
    inorder(root->right, num+root->l_size+1);
}

int main(){
    // init NIL and root
    NIL = (Node*) malloc(sizeof(Node));
    NIL->color = 'b';
    NIL->type = -1;
    NIL->left = NIL->right = NIL->p = NIL;
    NIL->l_size = NIL->r_size = NIL->val[0] = NIL->val[1] = NIL->dif = 0;
    root = NIL;

    N = readInt(); M = readInt();
    ll a, b;
    for(int i=0; i<N; i++){
        a = readInt(); b = readInt();
        cost[i][0] = a; cost[i][1] = b;
        total += a; total += b;
        dump[i] = newNode(llabs(b-a));
        insert(dump[i], 0);
    }
    init(root, 0);

    write((total-llabs(root->val[0]-root->val[1]))/2);
    putchar('\n');
    // printf("%lld\n", (total-llabs(root->val[0]-root->val[1]))/2);

    ll t,c,d,e,f;
    for(int i=0; i<M-1; i++){
        t = readInt(); c = readInt(), d = readInt(); e = readInt(); f = readInt();
        ll P = (total-llabs(root->val[0]-root->val[1]))/2;
        P = P%PRIME; c = c%PRIME; d = d%PRIME; e = e%PRIME; f = f%PRIME;
        
        total -= cost[t-1][0];
        total -= cost[t-1][1];
        delete(llabs(cost[t-1][1] - cost[t-1][0]), 1);


        cost[t-1][0] = (((c*P)%PRIME) + d) % PRIME;
        cost[t-1][1] = (((e*P)%PRIME) + f) % PRIME;
        total += cost[t-1][0];
        total += cost[t-1][1];
        Node *tmp = newNode(llabs(cost[t-1][1] - cost[t-1][0]));
        insert(tmp, 1);

        write((total-llabs(root->val[0]-root->val[1]))/2);
        putchar('\n');
    }
    return 0;
}