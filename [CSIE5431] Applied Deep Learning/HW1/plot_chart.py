import matplotlib.pyplot as plt

###### Q3 ######
# Cross-Entropy
#data1 = [6655.3037109375, 3668.15625, 2636.31787109375, 2135.25927734375, 1917.15234375, 1769.3028564453125, 1651.6190185546875]

# Average Cross-Entropy
#data1 = [0.9519279028987209, 0.5361510551222178, 0.37472597252583467, 0.31054645157356553, 0.2738492116364359, 0.2554658354147095, 0.23970159868884594]

# EM
#data2 = [79.79395147889664, 80.9238949817215, 81.09006314390163, 80.35892323030907, 80.32568959787305, 80.59155865736125, 80.5250913924892]

###### Q5 ######
data1 = [1.1782059174451665, 1.0004927897001237, 0.9205471004225062, 0.8790917405193734, 0.8385458976441917]
data2 = [77.06879361914257, 77.3678963110668, 77.73346626786308, 77.30142904619476, 76.80292455965437]

x = list(range(1, len(data1) + 1))


plt.figure(1) 
plt.plot(x, data1, marker='s', linestyle='--', color='r', label='Loss', )
plt.xlabel('Epoch')
plt.ylabel('Loss')
plt.title('Learning curve : Loss')
plt.legend()
plt.xticks(range(min(x), max(x) + 1, 1))

plt.figure(2)
plt.plot(x, data2, marker='s', linestyle='--', color='r', label='EM')
plt.xlabel('Epoch')
plt.ylabel('EM')
plt.title('Learning curve : EM')
plt.legend()

plt.xticks(range(min(x), max(x) + 1, 1))
plt.show()