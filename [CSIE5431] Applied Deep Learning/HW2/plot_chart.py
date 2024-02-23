import matplotlib.pyplot as plt


# Q2
data = [
    {'rouge1': 9.5045, 'rouge2': 4.0379, 'rougeL': 9.4823, 'rougeLsum': 9.4722},
    {'rouge1': 11.6643, 'rouge2': 4.6873, 'rougeL': 11.5422, 'rougeLsum': 11.5426},
    {'rouge1': 13.4786, 'rouge2': 5.1542, 'rougeL': 13.3016, 'rougeLsum': 13.3438},
    {'rouge1': 14.7673, 'rouge2': 5.5008, 'rougeL': 14.5832, 'rougeLsum': 14.5909},
    {'rouge1': 15.464, 'rouge2': 5.6556, 'rougeL': 15.2879, 'rougeLsum': 15.2821},
    {'rouge1': 16.4945, 'rouge2': 5.9022, 'rougeL': 16.2441, 'rougeLsum': 16.2563},
    {'rouge1': 17.1868, 'rouge2': 6.1409, 'rougeL': 16.9648, 'rougeLsum': 16.9488},
    {'rouge1': 18.094, 'rouge2': 6.4234, 'rougeL': 17.8238, 'rougeLsum': 17.7983},
    {'rouge1': 17.889, 'rouge2': 6.2746, 'rougeL': 17.6199, 'rougeLsum': 17.6108},
    {'rouge1': 18.7207, 'rouge2': 6.3153, 'rougeL': 18.4391, 'rougeLsum': 18.4321},
    {'rouge1': 18.6641, 'rouge2': 6.6442, 'rougeL': 18.3946, 'rougeLsum': 18.3551},
    {'rouge1': 18.9344, 'rouge2': 6.6854, 'rougeL': 18.6566, 'rougeLsum': 18.6237},
    {'rouge1': 18.9714, 'rouge2': 6.3029, 'rougeL': 18.6536, 'rougeLsum': 18.6278},
    {'rouge1': 19.0373, 'rouge2': 6.6148, 'rougeL': 18.7316, 'rougeLsum': 18.7046},
    {'rouge1': 19.6139, 'rouge2': 6.9704, 'rougeL': 19.2818, 'rougeLsum': 19.2517},
    {'rouge1': 19.7571, 'rouge2': 6.8883, 'rougeL': 19.5103, 'rougeLsum': 19.4882},
    {'rouge1': 19.9511, 'rouge2': 6.8495, 'rougeL': 19.6648, 'rougeLsum': 19.6121},
    {'rouge1': 19.842, 'rouge2': 6.8793, 'rougeL': 19.4958, 'rougeLsum': 19.5102},
    {'rouge1': 20.2066, 'rouge2': 7.1858, 'rougeL': 19.9084, 'rougeLsum': 19.8926},
    {'rouge1': 20.5221, 'rouge2': 7.2156, 'rougeL': 20.218, 'rougeLsum': 20.2073},
    {'rouge1': 21.0032, 'rouge2': 7.2447, 'rougeL': 20.6949, 'rougeLsum': 20.6928},
    {'rouge1': 20.8688, 'rouge2': 7.2921, 'rougeL': 20.5696, 'rougeLsum': 20.5351},
    {'rouge1': 20.7931, 'rouge2': 7.0298, 'rougeL': 20.4835, 'rougeLsum': 20.4419},
    {'rouge1': 21.0616, 'rouge2': 7.1692, 'rougeL': 20.7995, 'rougeLsum': 20.7727},
    {'rouge1': 20.9757, 'rouge2': 7.2296, 'rougeL': 20.6961, 'rougeLsum': 20.6677},
    {'rouge1': 21.1148, 'rouge2': 6.9473, 'rougeL': 20.7864, 'rougeLsum': 20.7693},
    {'rouge1': 21.0842, 'rouge2': 7.1251, 'rougeL': 20.7885, 'rougeLsum': 20.7564},
    {'rouge1': 21.1995, 'rouge2': 7.1088, 'rougeL': 20.8703, 'rougeLsum': 20.8554},
    {'rouge1': 21.4055, 'rouge2': 7.2766, 'rougeL': 21.0571, 'rougeLsum': 21.037},
    {'rouge1': 21.4052, 'rouge2': 7.1478, 'rougeL': 21.0904, 'rougeLsum': 21.0505}
]

# Extract each ROUGE metric into separate lists
rouge1 = [d['rouge1'] for d in data]
rouge2 = [d['rouge2'] for d in data]
rougeL = [d['rougeL'] for d in data]
rougeLsum = [d['rougeLsum'] for d in data]

# Generate x-axis values (data point indices)
x = list(range(0, len(data)))

# Plotting the ROUGE scores
plt.figure(1)
plt.plot(x, rouge1, marker='.', linestyle='--', label='rouge1')
plt.plot(x, rouge2, marker='.', linestyle='--', label='rouge2')
plt.plot(x, rougeL, marker='.', linestyle='--', label='rougeL')
#plt.plot(x, rougeLsum, marker='.', linestyle='--', label='rougeLsum')

plt.xlabel('Epoch')
plt.ylabel('ROUGE Score')
plt.title('ROUGE Scores over Epoch')
plt.legend()
#plt.show()


# Q3
# - Beam Search



data = [
    {"rouge-1": 25.023210145212016, "rouge-2": 9.409571998321532, "rouge-l": 22.3900892934053},
    {"rouge-1": 26.243396505367633, "rouge-2": 10.502798507636231, "rouge-l": 23.485434720915918},
    {"rouge-1": 25.97040680259727, "rouge-2": 10.459695369062331, "rouge-l": 23.237683138272694}
]

# Extract each ROUGE metric into separate lists
rouge1 = [d['rouge-1'] for d in data]
rouge2 = [d['rouge-2'] for d in data]
rougeL = [d['rouge-l'] for d in data]

# Generate x-axis values (data point indices)
#x = list(range(0, len(data)))
x = [1,3,5]

# Plotting the ROUGE scores
plt.figure(2)
plt.plot(x, rouge1, marker='.', linestyle='--', label='rouge1')
#plt.plot(x, rouge2, marker='.', linestyle='--', label='rouge2')
plt.plot(x, rougeL, marker='.', linestyle='--', label='rougeL')
#plt.plot(x, rougeLsum, marker='.', linestyle='--', label='rougeLsum')

plt.xticks(x, [str(i) for i in x])
plt.xlabel('Beam Number')
plt.ylabel('ROUGE Score')
plt.title('ROUGE Scores over Beam Number')
plt.legend()
#plt.show()



# - Top-k Sample


data = [
    {"rouge-1": 25.810834735583672, "rouge-2": 10.131189974874824, "rouge-l": 23.129143201185553},
    {"rouge-1": 25.803207749160688, "rouge-2": 10.145129606249661, "rouge-l": 23.097810687093833},
    {"rouge-1": 25.691518968050183, "rouge-2": 10.000808087204126, "rouge-l": 22.88922565049978},
    {"rouge-1": 25.71556389615391, "rouge-2": 10.164527205635933, "rouge-l": 22.94106571094801},
    {"rouge-1": 25.848126335026034, "rouge-2": 10.182116944673767, "rouge-l": 23.099687450121884},
    {"rouge-1": 25.50465577322006, "rouge-2": 10.003358580651666, "rouge-l": 22.7675642320746},
    {"rouge-1": 25.547577317067756, "rouge-2": 9.976650208815688, "rouge-l": 22.73339165904095},
    {"rouge-1": 25.335353740891055, "rouge-2": 9.820889181962116, "rouge-l": 22.634564222102927},
    {"rouge-1": 25.488003813772536, "rouge-2": 10.033591409115246, "rouge-l": 22.717668256176644},
    {"rouge-1": 25.56271294643664, "rouge-2": 9.9859919053637, "rouge-l": 22.854821679876707}
]

# Extract each ROUGE metric into separate lists
rouge1 = [d['rouge-1'] for d in data]
rouge2 = [d['rouge-2'] for d in data]
rougeL = [d['rouge-l'] for d in data]

# Generate x-axis values (data point indices)
#x = list(range(0, len(data)))
x = [10,20,30,40,50,60,70,80,90,100]

# Plotting the ROUGE scores
plt.figure(3)
plt.plot(x, rouge1, marker='.', linestyle='--', label='rouge1')
#plt.plot(x, rouge2, marker='.', linestyle='--', label='rouge2')
plt.plot(x, rougeL, marker='.', linestyle='--', label='rougeL')
#plt.plot(x, rougeLsum, marker='.', linestyle='--', label='rougeLsum')

plt.xticks(x, [str(i) for i in x])
plt.xlabel('K')
plt.ylabel('ROUGE Score')
plt.title('ROUGE Scores over Top K')
plt.legend()
#plt.show()

# - Top-P Sample


data = [
    {"rouge-1": 25.64058386033518, "rouge-2": 9.951699946868612, "rouge-l": 22.952634759313103},
    {"rouge-1": 25.724795312919564, "rouge-2": 10.037950969102132, "rouge-l": 22.988950157740966},
    {"rouge-1": 25.805641846531863, "rouge-2": 10.09631675166291, "rouge-l": 23.063791149482785},
    {"rouge-1": 25.750055219617657, "rouge-2": 10.116143826965558, "rouge-l": 23.039495082891822},
    {"rouge-1": 25.775856165151882, "rouge-2": 10.118151704674164, "rouge-l": 22.951460128080743},
    {"rouge-1": 25.848126335026034, "rouge-2": 10.182116944673767, "rouge-l": 23.099687450121884}
]

# Extract each ROUGE metric into separate lists
rouge1 = [d['rouge-1'] for d in data]
rouge2 = [d['rouge-2'] for d in data]
rougeL = [d['rouge-l'] for d in data]

# Generate x-axis values (data point indices)
#x = list(range(0, len(data)))
x = [0.5, 0.6, 0.7, 0.8, 0.9, 1.0]

# Plotting the ROUGE scores
plt.figure(4)
plt.plot(x, rouge1, marker='.', linestyle='--', label='rouge1')
#plt.plot(x, rouge2, marker='.', linestyle='--', label='rouge2')
plt.plot(x, rougeL, marker='.', linestyle='--', label='rougeL')
#plt.plot(x, rougeLsum, marker='.', linestyle='--', label='rougeLsum')

plt.xticks(x, [str(i) for i in x])
plt.xlabel('P')
plt.ylabel('ROUGE Score')
plt.title('ROUGE Scores over Top P')
plt.legend()
#plt.show()

# - Temperature


data = [
    {"rouge-1": 24.88914162890027, "rouge-2": 9.550457424257882, "rouge-l": 22.363428156578816},
    {"rouge-1": 24.935094084727165, "rouge-2": 9.673700474310177, "rouge-l": 22.365453134088156},
    {"rouge-1": 25.759052530759654, "rouge-2": 10.201863280548263, "rouge-l": 23.01884107674031},
    {"rouge-1": 25.14390633663046, "rouge-2": 9.504375043363734, "rouge-l": 22.269738920677327},
    {"rouge-1": 24.460186191243473, "rouge-2": 8.969869027698502, "rouge-l": 21.66578079860883}
]

# Extract each ROUGE metric into separate lists
rouge1 = [d['rouge-1'] for d in data]
rouge2 = [d['rouge-2'] for d in data]
rougeL = [d['rouge-l'] for d in data]

# Generate x-axis values (data point indices)
#x = list(range(0, len(data)))
x = [0.6, 0.8, 1.0, 1.2, 1.4]

# Plotting the ROUGE scores
plt.figure(5)
plt.plot(x, rouge1, marker='.', linestyle='--', label='rouge1')
plt.plot(x, rouge2, marker='.', linestyle='--', label='rouge2')
plt.plot(x, rougeL, marker='.', linestyle='--', label='rougeL')
#plt.plot(x, rougeLsum, marker='.', linestyle='--', label='rougeLsum')

plt.xticks(x, [str(i) for i in x])
plt.xlabel('Temperature')
plt.ylabel('ROUGE Score')
plt.title('ROUGE Scores over Temperature')
plt.legend()
plt.show()