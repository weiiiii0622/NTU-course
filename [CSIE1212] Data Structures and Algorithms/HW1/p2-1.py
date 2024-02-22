missing_sub = []
def find_missing_sub(arr, l, r):
    m = (l+r)//2
    if(l<=r):
        if(l==r):
            # Found one missing sub
            if(arr[m]!=m):
                missing_sub.append(m)
            # m is at right boundary -> missing m+1
            elif(arr[m]==m):
                missing_sub.append(m+1)
        # left side is perfect -> look right side
        elif(arr[m] == m):
            find_missing_sub(arr, m+1, r)
        # look left side
        else:
            find_missing_sub(arr, l, m)

def find_missing_sub_2(arr, l, r):
    m = (l+r)//2
    if(l<=r):
        if(l==r):
            # Found one missing sub
            if(arr[m]!=m+1):
                missing_sub.append(m+1)
            # m+1 is at right boundary -> missing m+2
            elif(arr[m]==m+1):
                missing_sub.append(m+2)
        # left side is perfect -> look right side
        elif(arr[m] == m+1):
            find_missing_sub_2(arr, m+1, r)
        # look left side
        else:
            find_missing_sub_2(arr, l, m)

arr = [int(x) for x in input().split()]

n = len(arr)-1

find_missing_sub(arr, 0, n)
m = missing_sub[0]
print(missing_sub)

if(m==n+1):
    missing_sub.append(m+1)
else:
    find_missing_sub_2(arr, m, n)


print(missing_sub)






    