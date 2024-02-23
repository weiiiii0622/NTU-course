import string

alphabet = string.ascii_lowercase * 40

with open('200KB.txt', 'w') as file:
    while file.tell() < 1024 * 200:
        file.write(alphabet)
    
with open('1MB.txt', 'w') as file:
    while file.tell() < 1024 * 1024:
        file.write(alphabet)

with open('2MB.txt', 'w') as file:
    while file.tell() < 1024 * 1024 * 2:
        file.write(alphabet)