// Define (1-based)
DP[n][l][k] := 開頭為n號 長度為l的subarray 切k次的max值 //1<=n<=N, 1<=k<=N, 1<=l<=(N-n+1)
v[n][2*n] := input table
offset := 現在要從input table取值的index

// Sample TestCase
Round1: [1  2  3  4]  // n=1, k=1~4
    k=1 -> [1234]
    k=2 -> [1,234], [12,34], [123,4]
    k=3 -> [1,2,34], [1,23,4], [12,3,4]
    k=4 -> [1,2,3,4]

Round2: [2  3  41]    // n=2, k=1~3
    k=1 -> [2341]
    k=2 -> [2,341], [23,41]
    k=3 -> [2,3,41]

Round3: [3  412]      // n=3, k=1~2
    k=1 -> [3412]
    k=2 -> [3,412]
    
Round4: [ 4123 ]      // n=4, k=1
    k=1 -> [4123]

-> //bottom-up
for n = 1 ~ N:
    for l = 1 ~ (N-n+1):
        if(l != N-n+1)
            offset = n+l // 找[2]＝> v[2][3] = v[1][2+1] 
        else
            offset = n+l+(n-1); // 找[2341]＝> v[2][6] = v[1][2+3+(2-1)] 
        
        DP[n][l][1] = V[n][offset] // base case k=1
        for k = 2 ~ l:
            for i = (k-1) ~ (l-1):
                dp[n][l][k] = max(self, dp[n][i][k-1] + v[l+n-1][offset])

// ans
ans = max(dp[n][N-n+1][k]); 1<=n<=N && 1<=k<=N && (N-n+1)>=k