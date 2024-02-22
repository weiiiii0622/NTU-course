#include "ypglpk.hpp"
#include <iostream>
#include <vector>

int n, m;

int main(){
    //ypglpk::set_output(true);
    std::cin >> n >> m;
    std::vector<std::vector<double> > A(n+2*n+m, std::vector<double>(m+n));
    std::vector<double> b(n+2*n+m), c(m+n);
    std::vector<int> vartype(m+n);
    for(int i=0; i<3*n+m; i++){
        for(int j=0; j<m+n; j++){
            A[i][j] = 0;
        }
    }  
    b[0] = 1; b[n-1] = -1;  
    for(int i=1; i<n-1; i++){
        b[i] = 0;
    }
    for(int i=n; i<3*n; i++){
        b[i] = 1;
    }
    b[2*n-1] = 0;
    for(int i=3*n; i<3*n+m; i++){
        b[i] = n-1;
    }
    for(int i=0; i<m; i++){
        int u, v; double w; 
        std::cin >> u >> v >> w;
        A[u-1][i] = 1;
        A[v-1][i] = -1;
        A[n+u-1][i] = 1;
        A[2*n+v-1][i] = 1;
        A[3*n+i][m+u-1] = 1;
        A[3*n+i][m+v-1] = -1;
        A[3*n+i][i] = n;
        c[i] = w;
    }
    for(int i=m; i<m+n; i++){
        c[i] = 0;
    }

    for(int i=0; i<m; i++){
        vartype[i] = GLP_BV;
    }
    for(int i=m; i<m+n; i++){
        vartype[i] = GLP_CV;
    }
    std::pair<double, std::vector<double>> res;
    res = ypglpk::mixed_integer_linear_programming(A, b, c, vartype);

    if(res.first == -ypglpk::INF){
        std::cout << -1 << '\n';
    }
    else{
        std::cout << (long long)res.first << '\n';
        for(int i=0; i<m; i++){
            std::cout << res.second[i];
        }
        std::cout << '\n';
    }
    return 0;
}
