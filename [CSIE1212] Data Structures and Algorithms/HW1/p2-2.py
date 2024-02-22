arr = [int(x) for x in input().split()]

def solve(arr, n):
    i=0
    j=1
    while i<n and j<n:
        if arr[j] == arr[i]*2:
            # mark as paired
            arr[i] = -1
            arr[j] = -1
            # Get next valid spot
            while(arr[i] == -1):
                i += 1
                if(i >= n): # ended
                    break
        j += 1

    if(i==n and j==n):
        return True
    else:
        return False

if solve(arr, len(arr)):
    print("TRUE")
else:
    print("FALSE")