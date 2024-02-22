#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define ll long long
#define DEBUG 0

typedef struct _knight{
    int id;
    ll atk;
    ll hp;
    ll jmp;
} Knight;

typedef struct _set{
    Knight* heap;
    int id;
    ll total_atk;
    ll total_jmp;
    ll total_dmg;
    int size;     // # of knights
    int len;      // length of the heap 
} Set;

int root[200000 + 5];
Knight knight[200000 + 5];
Set* DSU[200000 + 5];

void swap(Knight* a, Knight* b){
    int tmp = a->id; a->id = b->id; b->id = tmp;
    tmp = a->atk; a->atk = b->atk; b->atk = tmp;
    tmp = a->hp; a->hp = b->hp; b->hp = tmp;
    tmp = a->jmp; a->jmp = b->jmp; b->jmp = tmp;
}

void heapify(Knight* arr, int i, int size){
    if(size <= 1){
        return;
    }
    int minNode = i;
    int left = 2*i;
    int right = 2*i+1;
    if(left <= size && arr[left].hp < arr[minNode].hp){
        minNode = left;
    }
    if(right <= size && arr[right].hp < arr[minNode].hp){
        minNode = right;
    }
    if(minNode != i){
        swap(&arr[i], &arr[minNode]);
        heapify(arr, minNode, size);
    }
    return;
}

void insert(Knight* arr, int size, int id, int atk, int hp, int jmp){
    int cur = size+1;
    int parent = cur/2; 
    arr[cur].id = id, arr[cur].atk = atk; arr[cur].hp = hp; arr[cur].jmp = jmp;
    while(parent>=1 && arr[cur].hp < arr[parent].hp){
        swap(&arr[cur], &arr[parent]);
        cur = parent;
        parent = cur/2;
    }
    return;
}

void pop(Knight* arr, int size){
    swap(&arr[1], &arr[size]);
    heapify(arr, 1, size-1);
    return;
}

void resizeHeap(Set* set){
    set->len = 2*(set->len);
    set->heap = realloc(set->heap, sizeof(Knight)*(set->len+1));
    return;
}


int find(int x){
    return x == root[x] ? x : (root[x]=find(root[x]));
}

void unite(int x, int y){
    int root_x = find(x);
    int root_y = find(y);
    if(root_x == root_y) return;
    if(knight[x].hp - DSU[root_x]->total_dmg <= 0 || knight[y].hp - DSU[root_y]->total_dmg <= 0) return;

    int size_x = DSU[root_x]->size;
    int size_y = DSU[root_y]->size;

    if(size_x <= size_y){
        // Check A's death and update their hp,jmp+=A.total_jmp and remove their atk from A
        while(DSU[root_x]->heap[1].hp - DSU[root_x]->total_dmg <= 0 && DSU[root_x]->size > 0){
            int id = DSU[root_x]->heap[1].id;
            knight[id].hp = DSU[root_x]->heap[1].hp - DSU[root_x]->total_dmg;    // update hp
            knight[id].jmp += DSU[root_x]->total_jmp;   // update jmp
            DSU[root_x]->total_atk -= knight[id].atk;   // remove atk from A
            pop(DSU[root_x]->heap, DSU[root_x]->size);  
            DSU[root_x]->size -= 1;
        }

        // Update B.total_dmg += A.total_atk, B.total_atk += A.total_atk
        DSU[root_y]->total_dmg += DSU[root_x]->total_atk;
        DSU[root_y]->total_atk += DSU[root_x]->total_atk;

        // Update A's alive jmp += A.total_jmp - B.total_jmp + 1
        // Update A's alive hp = hp - A.total_dmg + B.total_dmg
        // Push A's alive to B, Resize B if needed
        if(DSU[root_x]->size + DSU[root_y]->size >= DSU[root_y]->len){
            resizeHeap(DSU[root_y]);
        }
        while(DSU[root_x]->size > 0){
            int id = DSU[root_x]->heap[1].id;
            knight[id].jmp += (DSU[root_x]->total_jmp - DSU[root_y]->total_jmp + 1);            // update jmp
            knight[id].hp = knight[id].hp - DSU[root_x]->total_dmg + DSU[root_y]->total_dmg;    // update hp
            pop(DSU[root_x]->heap, DSU[root_x]->size);  
            DSU[root_x]->size -= 1;
            insert(DSU[root_y]->heap, DSU[root_y]->size, id, knight[id].atk, knight[id].hp, knight[id].jmp);
            DSU[root_y]->size += 1;
        }
        free(DSU[root_x]->heap);
        DSU[root_x]->total_atk = 0;
        DSU[root_x]->len = 0;
        // Update DSU
        root[root_x] = root_y;
    }
    else{
        // Check A's death and update their hp,jmp+=A.total_jmp and remove their atk from A
        while(DSU[root_x]->heap[1].hp - DSU[root_x]->total_dmg <= 0 && DSU[root_x]->size > 0){
            int id = DSU[root_x]->heap[1].id;
            knight[id].hp = DSU[root_x]->heap[1].hp - DSU[root_x]->total_dmg;    // update hp
            knight[id].jmp += DSU[root_x]->total_jmp;   // update jmp
            DSU[root_x]->total_atk -= knight[id].atk;   // remove atk from A
            pop(DSU[root_x]->heap, DSU[root_x]->size);  
            DSU[root_x]->size -= 1;
        }
        // Update B.total_dmg += A.total_atk, A.total_jmp += 1
        DSU[root_y]->total_dmg += DSU[root_x]->total_atk;
        DSU[root_x]->total_jmp += 1; 
        // Check B's death and update their hp/jmp+=B.total_jmp and remove their atk from B
        while(DSU[root_y]->heap[1].hp - DSU[root_y]->total_dmg <= 0 && DSU[root_y]->size > 0){
            int id = DSU[root_y]->heap[1].id;
            knight[id].hp = DSU[root_y]->heap[1].hp - DSU[root_y]->total_dmg;    // update hp
            knight[id].jmp += DSU[root_y]->total_jmp;                            // update jmp
            DSU[root_y]->total_atk -= knight[id].atk;                            // remove atk from B
            pop(DSU[root_y]->heap, DSU[root_y]->size);  
            DSU[root_y]->size -= 1;
        }       
        // Update A.total_atk += B.total_atk
        DSU[root_x]->total_atk += DSU[root_y]->total_atk;

        // Update B's alive jmp += B.total_jmp - A.total_jmp
        // Update B's alive hp = hp - B.total_dmg + A.total_dmg
        // Push B's alive to A / Resize A if needed
        if(DSU[root_x]->size + DSU[root_y]->size >= DSU[root_x]->len){
            resizeHeap(DSU[root_x]);
        }
        while(DSU[root_y]->size > 0){
            int id = DSU[root_y]->heap[1].id;
            knight[id].jmp += (DSU[root_y]->total_jmp - DSU[root_x]->total_jmp);                // update jmp
            knight[id].hp = knight[id].hp - DSU[root_y]->total_dmg + DSU[root_x]->total_dmg;    // update hp
            pop(DSU[root_y]->heap, DSU[root_y]->size);  
            DSU[root_y]->size -= 1;
            insert(DSU[root_x]->heap, DSU[root_x]->size, id, knight[id].atk, knight[id].hp, knight[id].jmp);
            DSU[root_x]->size += 1;
        }
        free(DSU[root_y]->heap);
        DSU[root_y]->total_atk = 0;
        DSU[root_y]->len = 0;
        // Update DSU
        root[root_y] = root_x;
    }
}

Knight* newKnight(int id, int atk, int hp){
    Knight* temp = (Knight*) malloc(sizeof(Knight));
    temp->id = id;
    temp->atk = atk;
    temp->hp = hp;
    temp->jmp = 0;
    return temp;
}

Set* newSet(int id){
    Set* temp = (Set*) malloc(sizeof(Set));
    temp->id = id;
    temp->heap = (Knight*) malloc(sizeof(Knight) * 2);
    temp->size = 0;
    temp->len = 1;
    temp->total_atk = temp->total_dmg = temp->total_jmp = 0;
    return temp;
}

// DEBUG
void printKnight(int n){
    printf("Current Knights:\n");
    for(int i=1; i<=n; i++){
        printf("id: %d / atk: %lld / hp: %lld / jmp: %lld\n", knight[i].id, knight[i].atk, knight[i].hp, knight[i].jmp);
    }
    printf("\n");
}
void printRoot(int n){
    printf("DSU ROOTS:\n");
    for(int i=1; i<=n; i++){
        printf("\tSet id: %d / root: %d\n", i, find(i));
    }
    printf("\n");
}
void printSet(int n){
    printf("DSU Heap:\n");
    for(int i=1; i<=n; i++){
        printf("Set id: %d\n", i);
        printf("\tsize: %d / len: %d\n", DSU[i]->size, DSU[i]->len);
        printf("\ttotal atk: %lld / total dmg: %lld / total jmp: %lld\n", DSU[i]->total_atk, DSU[i]->total_dmg, DSU[i]->total_jmp);
        printf("\tHeap:\n");
        for(int j=1; j<=DSU[i]->size; j++){
            printf("\t\t(id: %d / atk: %lld / hp: %lld / jmp: %lld)\n", DSU[i]->heap[j].id, DSU[i]->heap[j].atk, DSU[i]->heap[j].hp, DSU[i]->heap[j].jmp);
        }
    }
}

int readInt() {
	int x = 0, c = getchar(), neg = false;
	while(('0' > c || c > '9') && c!='-' && c!=EOF) c = getchar();
	if(c == '-') neg = true, c = getchar();
	while('0' <= c && c <= '9') x = x*10 + (c^'0'), c = getchar();
	if(neg) x = -x;
	return x; // returns 0 if EOF
}

void write(int x) {
  static int sta[35];
  int top = 0;
  do {
    sta[top++] = x % 10, x /= 10;
  } while (x);
  while (top) putchar(sta[--top] + 48);  // 48 是 '0'
}

int main(){
    int n, m; // scanf("%d %d", &n, &m);
    n = readInt();
    m = readInt();
    for(int i=1; i<=n; i++){
        root[i] = i;
        knight[i].id = i;
    }
    int h; 
    for(int i=1; i<=n; i++){
        h = readInt();
        knight[i].hp = h;
    }
    int a; //scanf("%d", &a);
    for(int i=1; i<=n; i++){
        a = readInt();
        knight[i].atk = a;
    }
    for(int i=1; i<=n; i++){
        DSU[i] = newSet(i);
        //printf("Set %d : Insert (id: %d / %d, %d, %d)\n", i,knight[i].id, knight[i].atk, knight[i].hp, knight[i].jmp);
        insert(DSU[i]->heap, DSU[i]->size, knight[i].id, knight[i].atk, knight[i].hp, knight[i].jmp);
        DSU[i]->size += 1;
        DSU[i]->total_atk += knight[i].atk;
    }
    
    if(DEBUG){
        printKnight(n);
        printRoot(n);
        printSet(n);
    }

    int x, y;
    while(m--){
        //scanf("%d %d", &x, &y);
        x = readInt(); y = readInt();
        if(knight[x].hp <= 0 || knight[y].hp <= 0) continue;
        unite(x, y);

        if(DEBUG){
            printKnight(n);
            printRoot(n);
            printSet(n);
        }
    }

    // Finalize
    for(int i=1; i<=n; i++){
        if(DSU[i]->size != 0){
            for(int j=1; j<=DSU[i]->size; j++){
                int id = DSU[i]->heap[j].id;
                knight[id].hp += DSU[i]->total_dmg;
                knight[id].jmp += DSU[i]->total_jmp; 
            }
        }
    }
    for(int i=1; i<=n; i++){
        if(i!=1) putchar(' ');
        write(knight[i].jmp);
    }
    putchar('\n');
    return 0;
}