import random
import string

# Generate a random English name within 15 alphabets
def generate_name():
    name_length = random.randint(1, 15)
    return ''.join(random.choice(string.ascii_lowercase) for i in range(name_length))

# Generate a random score between 0 and 10^8
def generate_score():
    return random.randint(0, 10**2)

# Generate N random name-score pairs
def generate_data(N):
    data = []
    for i in range(5 * N):
        name = generate_name()
        score = generate_score()
        data.append((name, score))
    return data

def write_data_to_file(data, i):
    with open('./QA/testdata/{}.in'.format(i), 'w') as f:
        f.write(str(len(data) // 5) + '\n')
        for i in range(len(data)):
            name, score = data[i]
            f.write(name + ' ' + str(score) + '\n')



if __name__ == "__main__":
    for i in range(6, 7):     
        data = generate_data(100000)
        write_data_to_file(data, i)