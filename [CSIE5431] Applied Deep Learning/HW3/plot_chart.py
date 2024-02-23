import matplotlib.pyplot as plt


# Q1
data = [
    3.561242261886597,
	3.731644229412079,
	4.02179647838592,
	4.176944113254547
]

# Generate x-axis values (data point indices)
x = list(range(1, len(data)+1))

# Plotting the ROUGE scores
plt.figure(1)
plt.plot(x, data, marker='o', linestyle='--', label='Public Test')
#plt.plot(x, rougeLsum, marker='.', linestyle='--', label='rougeLsum')

plt.xticks(x, [str(i) for i in x])
plt.xlabel('Checkpoint')
plt.ylabel('PPL')
plt.title('PPL over Checkpoint')
plt.legend()
#plt.show()



# Q2

# Create bar plot
type_names = ['zero-shot-0', 'zero-shot-1', 'one-shot', 'four-shot', 'lora']
ppl_scores = [5.452863416671753, 5.412987493515015, 4.6702891345024105, 4.547110088825226, 3.561242261886597]

plt.figure(2)
plt.bar(type_names, ppl_scores, color='orange', bottom=0, align='center', width=0.5)
plt.xlabel('Method')
plt.ylabel('PPL')
plt.title('PPL with Different Method')


# Bonus
data = [
    3.561242261886597,
	3.731644229412079,
	4.02179647838592,
	4.176944113254547
]

data2 = [
    3.849330807685852,
    3.8958204884529115,
    4.129890115261078,
    4.292995451927185
]

# Generate x-axis values (data point indices)
x = list(range(1, len(data)+1))

# Plotting the ROUGE scores
plt.figure(3)
plt.plot(x, data, marker='o', linestyle='--', label='Taiwan-LLM-7B-v2.0-chat')
plt.plot(x, data2, marker='o', linestyle='--', label='FlagAlpha/Llama2-Chinese-7b-Chat')
#plt.plot(x, rougeLsum, marker='.', linestyle='--', label='rougeLsum')

plt.xticks(x, [str(i) for i in x])
plt.xlabel('Checkpoint')
plt.ylabel('PPL on Public Test')
plt.title('PPL over Checkpoint')
plt.legend()

# Display the plot
plt.show()