#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define ll long long
#define swap(x,y) {int temp=x; x=y; y=temp;}

typedef struct _catnode{
    int apt;
    int idx;
    struct _catnode *prev;
    struct _catnode *next;
} CatNode;

typedef struct _colornode{
    int apt;
    int idx;
} ColorNode;

typedef struct _color{
    ColorNode* arr;
    int oldColor;
    int cnt;
    int head;
} Color;

typedef struct _cat{
    int apt;
    int idx;
    struct _catnode *node;
    int oldColor;
    int newColor;
} Cat;

int readInt() {
	int x = 0, c = getchar(), neg = false;
	while(('0' > c || c > '9') && c!='-' && c!=EOF) c = getchar();
	if(c == '-') neg = true, c = getchar();
	while('0' <= c && c <= '9') x = x*10 + (c^'0'), c = getchar();
	if(neg) x = -x;
	return x; // returns 0 if EOF
}

int aptCmp(const void* lhs, const void* rhs){
    return ((ColorNode*)lhs)->apt - ((ColorNode*)rhs)->apt;
}

int colorCmp(const void* lhs, const void* rhs){
    Cat* l = ((Cat*)lhs);
    Cat* r = ((Cat*)rhs);
    if(l->oldColor == r->oldColor){
        return l->apt - r->apt;
    }
    else return l->oldColor - r->oldColor;
}

CatNode* newNode(int idx, int apt){
    CatNode* tmp = (CatNode*) malloc(sizeof(CatNode));
    tmp->apt = apt;
    tmp->idx = idx;
    tmp->prev = tmp->next = NULL;
    return tmp;
}

int n, m;
int colorCnt = -1;
Cat IDX[3 * 100000 + 5];
Color COLOR[3 * 100000 + 5];
CatNode* APThead;
CatNode* APTtail;

// Magic
int bsearch_color(int c, int l, int r){
    int m;
    while(l<=r){
        m = (l+r)/2;
        if(COLOR[m].oldColor == c) break;
        else if(COLOR[m].oldColor < c){
            l = m+1;
        }
        else if(COLOR[m].oldColor > c){
            r = m-1;
        }
    }
    return m;
}

int bsearch_colorNode(int c, int head, int size, int apt, int l, int r){
    int m, realM;
    while(l<=r){
        m = (l+r)/2;
        realM = (m+head)%size;
        if(COLOR[c].arr[realM].apt == apt) return realM;
        else if(COLOR[c].arr[realM].apt < apt){
            l = m+1;
        }
        else if(COLOR[c].arr[realM].apt > apt){
            r = m-1;
        }
    }
    return realM;
}

int lowerbound(int c, int head, int size, int apt, int l, int r){
    int m, realM;
    int res = -1;
    while(l<=r){
        m = (l+r)/2;
        realM = (m+head)%size;
        if(COLOR[c].arr[realM].apt < apt){
            l = m+1;
        }
        else if(COLOR[c].arr[realM].apt >= apt){
            res = m;
            r = m-1;
        }
    }
    return res;    
}

int upperbound(int c, int head, int size, int apt, int l, int r){
    int m, realM;
    int res = -1;
    while(l<=r){
        m = (l+r)/2;
        realM = (m+head)%size;
        if(COLOR[c].arr[realM].apt <= apt){
            res = m;
            l = m+1;
        }
        else if(COLOR[c].arr[realM].apt > apt){
            r = m-1;
        }
    }
    return res;    
}

void swapNode(CatNode* a, CatNode* b){
    IDX[a->idx].node = b;
    IDX[b->idx].node = a;
    IDX[a->idx].apt = b->apt;
    IDX[b->idx].apt = a->apt;
    int tmp = a->idx;
    a->idx = b->idx;
    b->idx = tmp;
}

// DEBUG FUNCTION
void printIDX(){
    for(int i=0; i<n; i++){
        printf("(idx:%d, apt:%d, oldColor:%d, newColor:%d)\n",i,IDX[i].apt,IDX[i].oldColor,IDX[i].newColor);
    }
}
void printCOLOR(){
    int idx;
    for(int i=0; i<colorCnt; i++){
        printf("Newcolor %d (=OldColor %d)\n", i,COLOR[i].oldColor);
        for(int k=0; k<COLOR[i].cnt; k++){
            idx = (k+COLOR[i].head)%COLOR[i].cnt;
            printf(" (%d %d),", COLOR[i].arr[idx].idx, COLOR[i].arr[idx].apt);
        }
        printf("\n");
    }
}
void printAPT(){
    printf("APT TABLE:\n");
    CatNode* tmp = APThead;
    while(tmp!=NULL){
        printf(" (%d %d),", tmp->idx, tmp->apt);
        tmp = tmp->next;
    }
    printf("\n");
}

void print(){
    printIDX();printCOLOR();printAPT();
}


int main(){
    scanf("%d %d", &n, &m);
    int a, b;
    Cat tmpColor[n]; ColorNode tmpApt[n];
    for(int i=0; i<n; i++){
        //scanf("%d", &a); 
        a = readInt();
        IDX[i].apt = a, IDX[i].idx = i;
        tmpColor[i].apt = a, tmpColor[i].idx = i;
        tmpApt[i].apt = a, tmpApt[i].idx = i;
    }
    for(int i=0; i<n; i++){
        //scanf("%d", &b);
        b = readInt();
        IDX[i].oldColor = b;
        tmpColor[i].oldColor = b;
    }
    qsort(tmpColor, n, sizeof(tmpColor[0]), colorCmp);
    qsort(tmpApt, n, sizeof(tmpApt[0]), aptCmp);
    // Get new color
    int curColor = -1;
    
    for(int i=0; i<n; i++){
        if(curColor != tmpColor[i].oldColor){
            curColor = tmpColor[i].oldColor;
            colorCnt++;
            COLOR[colorCnt].cnt = 0;
            COLOR[colorCnt].oldColor = tmpColor[i].oldColor;
        }
        IDX[tmpColor[i].idx].newColor = colorCnt;
        COLOR[colorCnt].cnt += 1;
    }
    colorCnt++;
    for(int i=0, k=0; i<colorCnt; i++){
        int size = COLOR[i].cnt;
        COLOR[i].head = 0;
        COLOR[i].arr = (ColorNode*)malloc(sizeof(ColorNode)*(size));
        for(int s=0; s<size; s++,k++){
            (COLOR[i].arr)[s].apt = tmpColor[k].apt;
            (COLOR[i].arr)[s].idx = tmpColor[k].idx;
        }
    }

    // Construct Apt table
    int prevIdx, idx, apt;
    for(int i=0; i<n; i++){
        idx = tmpApt[i].idx;
        apt = tmpApt[i].apt;
        IDX[idx].node = newNode(idx, apt);
        if(i!=0){
            prevIdx = tmpApt[i-1].idx;
            IDX[idx].node->prev = IDX[prevIdx].node;
            IDX[prevIdx].node->next = IDX[idx].node;
        }
    }
    APThead = IDX[tmpApt[0].idx].node;
    APTtail = IDX[tmpApt[n-1].idx].node;

    //print();

    int cmd;
    while(m--){
        //scanf("%d", &cmd);
        cmd = readInt();
        if(cmd==1){
            int c, l, r; //scanf("%d %d %d", &c, &l, &r);
            c = readInt();
            l = readInt();
            r = readInt();
            int tmp = c;
            c = bsearch_color(c, 0, colorCnt-1);
            if(COLOR[c].oldColor != tmp){
                printf("0\n");
                continue;
            }
            int idx1 = lowerbound(c, COLOR[c].head, COLOR[c].cnt, l, 0, COLOR[c].cnt-1);
            int idx2 = upperbound(c, COLOR[c].head, COLOR[c].cnt, r, 0, COLOR[c].cnt-1);
            //printf("idx1: %d / idx2: %d\n", idx1, idx2);
            if(idx1 != -1) printf("%d\n", idx2-idx1+1);
            else printf("0\n");
        }
        else if(cmd == 2){
            int k; //scanf("%d", &k);
            k = readInt();
            int victimIdx;
            // Swap APT apt and get victim IDX
            // Swap IDX's apt and node pointer
            CatNode* cur = IDX[k].node;
            //printf("cur idx:%d / apt:%d\n", cur->idx, cur->apt);
            if(cur->next == NULL) continue; // cur is the biggest apt
            //if(cur->prev==NULL) printf("prev: NULL / next: (%d %d)\n", cur->next->idx, cur->next->apt);
            //else printf("prev: (%d %d) / next: (%d %d)\n", cur->prev->idx, cur->prev->apt, cur->next->idx, cur->next->apt);
            CatNode* victim = cur->next;
            victimIdx = victim->idx;
            swapNode(cur, victim);
            
            // Update COLOR apt, swap order if needed
            int idx1 = bsearch_colorNode(IDX[k].newColor, COLOR[IDX[k].newColor].head, COLOR[IDX[k].newColor].cnt, IDX[victimIdx].apt, 0, COLOR[IDX[k].newColor].cnt-1);
            int idx2 = bsearch_colorNode(IDX[victimIdx].newColor, COLOR[IDX[victimIdx].newColor].head, COLOR[IDX[victimIdx].newColor].cnt, IDX[k].apt, 0, COLOR[IDX[victimIdx].newColor].cnt-1);

            COLOR[IDX[k].newColor].arr[idx1].apt = IDX[k].apt;
            COLOR[IDX[victimIdx].newColor].arr[idx2].apt = IDX[victimIdx].apt;
            if(IDX[k].newColor == IDX[victimIdx].newColor){
                COLOR[IDX[k].newColor].arr[idx1].apt = IDX[victimIdx].apt;
                COLOR[IDX[victimIdx].newColor].arr[idx2].apt = IDX[k].apt;
                int tmp = COLOR[IDX[k].newColor].arr[idx1].idx;
                COLOR[IDX[k].newColor].arr[idx1].idx = COLOR[IDX[victimIdx].newColor].arr[idx2].idx;
                COLOR[IDX[victimIdx].newColor].arr[idx2].idx = tmp;
            }
        }
        else if(cmd == 3){
            int c, s, t; //scanf("%d %d %d", &c, &s, &t);
            c = readInt();
            s = readInt();
            t = readInt();
            c = bsearch_color(c, 0, colorCnt-1);
            int idx;
            // Change COLOR apt, head
            int newAPT;
            if(t==0) newAPT = APThead->apt - 1;
            else if(t==1) newAPT = APTtail->apt + 1;
            if(s==0){
                idx = COLOR[c].arr[COLOR[c].head].idx;
                if(t==0){
                    COLOR[c].arr[COLOR[c].head].apt = newAPT;
                }else if(t==1){
                    COLOR[c].arr[COLOR[c].head].apt = newAPT;
                    COLOR[c].head = (COLOR[c].head+1)%COLOR[c].cnt;
                }
            }else if(s==1){
                idx = COLOR[c].head>0?COLOR[c].head-1:COLOR[c].cnt-1;
                if(t==0){
                    COLOR[c].arr[idx].apt = newAPT;
                    COLOR[c].head = idx;
                }else if(t==1){
                    COLOR[c].arr[idx].apt = newAPT;
                }               
                idx = COLOR[c].arr[idx].idx;
            }
            // Change IDX apt
            IDX[idx].apt = newAPT;
            // Change APT apt, order
            CatNode* cur = IDX[idx].node;
            cur->apt = newAPT;
            // If cur==head==tail, change only apt
            if(cur->prev == NULL && cur->next == NULL) cur->apt = newAPT;
            else if(cur->prev == NULL){ // cur is head and t is 1
                if(t==1){
                    APThead = APThead->next;
                    APThead->prev = NULL;
                    cur->prev = APTtail;
                    cur->next = NULL;
                    APTtail->next = cur;
                    APTtail = cur;
                }
            }
            else if(cur->next == NULL){ // cur is tail and t is 0
                if(t==0){
                    APTtail = APTtail->prev;
                    APTtail->next = NULL;
                    cur->next = APThead;
                    cur->prev = NULL;
                    APThead->prev = cur;
                    APThead = cur;
                }
            }
            else{ // cur is in the middle
                if(t==0){ // move to head
                    cur->prev->next = cur->next;
                    cur->next->prev = cur->prev;
                    cur->next = APThead;
                    cur->prev = NULL;
                    APThead->prev = cur;
                    APThead = cur;
                }
                else if(t == 1){ // move to tail
                    cur->prev->next = cur->next;
                    cur->next->prev = cur->prev;
                    cur->prev = APTtail;
                    cur->next = NULL;
                    APTtail->next = cur;
                    APTtail = cur;
                }
            }
        }
        //print();
    }

    return 0;
}