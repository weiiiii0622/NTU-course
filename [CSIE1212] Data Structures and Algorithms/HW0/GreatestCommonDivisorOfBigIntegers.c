#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ll long long
#define swap(x,y) {int temp=x; x=y; y=temp;}


// n=min(a, b), m=max(a, b);
void comp(char a[], char b[], char **n, char **m){
    int lenA = strlen(a), lenB = strlen(b);
    if(lenA == lenB){
        *m = a;
        *n = b;
        for(int i=0; i<lenA; i++){
            if(a[i]==b[i]) continue;
            if(a[i]>b[i]){
                *m = a;
                *n = b;
            }
            else{
                *m = b;
                *n = a;
            }
            break;
        }
    }else{
        if(lenA > lenB){
            *m = a;
            *n = b;
        }
        else{
            *m = b;
            *n = a;
        }
    }
}

// m = m-n
void sub(char **m, char **n){
    int lenM = strlen(*m);
    int lenN = strlen(*n);
    int M[lenM+5]; for(int i=0; i<lenM; i++) M[i] = (*m)[lenM-i-1]-'0';
    int N[lenN+5]; for(int i=0; i<lenN; i++) N[i] = (*n)[lenN-i-1]-'0';

    int borrow = 0;
    for(int i=0; i<lenM; i++){
        if(i<lenN){
            if((M[i]-borrow) < N[i]){
                M[i] = 10 + M[i] - N[i] - borrow;
                borrow = 1;
            }
            else{
                M[i] = M[i] - N[i] - borrow;
                borrow = 0;
            }
        }
        else{
            if((M[i]-borrow) < 0){
                M[i] = 10 + M[i] - borrow;
                borrow = 1;
            }
            else{
                M[i] = M[i] - borrow;
                borrow = 0;
            }
        }
    }

    int i=lenM-1, j=0;
    for(; i>=0; i--) if(M[i] != 0) break;
    
    // i = -1 -> m = 0
    if(i == -1) i = 0;
    for(; i>=0; j++, i--){
        (*m)[j] = ('0' + M[i]);
    }
    
    (*m)[j] = '\0';
    return;
}

void div2(char **x){
    int len = strlen(*x);
    int num[len+5]; for(int i=0; i<len; i++) num[i] = (*x)[i]-'0';

    int carry = 0;
    for(int i=0; i<len; i++){
        int tgt = carry*10 + num[i];
        carry = tgt % 2;
        num[i] = tgt / 2;
    }

    int i = 0, j = 0;
    for(; i<len; i++) if(num[i] != 0) break;
    for(; i<len; j++, i++){
        (*x)[j] = ('0' + num[i]);
    }
    
    (*x)[j] = '\0';
    return;
}

void mul2k(char **x, int k){
    int len = strlen(*x);
    int num[len*50]; 
    int size = len;
    for(int i=0; i<len*50; i++) num[i] = 0;
    for(int i=0; i<len; i++) num[i] = (*x)[len-i-1]-'0';

    while(k--){
        int carry = 0;
        for(int i=0; ;i++){
            if(num[i]==0 && carry==0 && i >= size) {size = i; break;}
            int tgt = (num[i]*2+carry);
            carry =  tgt/10;
            num[i] = tgt%10;
        }
    }

    int i = size+1, j = 0;
    for(; i>=0; i--) if(num[i] != 0) break;
    for(; i>=0; j++, i--){
        (*x)[j] = ('0' + num[i]);
    }
    
    (*x)[j] = '\0';
    return;
}

int main(){

    char a[260];
    char b[260];

    char *n;
    char *m;
    int k = 0;

    scanf("%s %s", a, b);
    comp(a, b, &n, &m);

    while(n[0] != '0' && m[0] != '0'){
        if((n[strlen(n)-1]-'0')%2==0 && (m[strlen(m)-1]-'0')%2==0){
            k++;
            div2(&n);
            div2(&m);
        }
        else if((n[strlen(n)-1]-'0')%2==0){
            div2(&n);
        }
        else if((m[strlen(m)-1]-'0')%2==0){
            div2(&m);
        }
        comp(a, b, &n, &m);
        sub(&m, &n);
    }
    mul2k(&n, k);
    printf("%s\n", n);
    return 0;
}