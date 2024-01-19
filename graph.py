import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import collections


with open('output.txt','r') as f:
    m = collections.defaultdict(list)
    for line in f:
        data = line.split(" ")
        m[data[2]].append([float(data[0]),float(data[1])])

fig, ax = plt.subplots()
plt.xlabel("fidelity")
plt.ylabel("success prob.")

cmap = matplotlib.cm.get_cmap('tab20_r')
j = 0
for k,v in m.items():
    for i,(x,y) in enumerate(v):
        if i == 0:
            ax.scatter(x,y,label = k, color = cmap.colors[j])
        else:
            ax.scatter(x,y ,color = cmap.colors[j])

    j+=1
ax.legend(bbox_to_anchor = (0.85,.5))
plt.savefig('graph.png')