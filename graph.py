# %%
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt

import numpy as np
import collections
from mpl_toolkits.mplot3d import Axes3D

plt.rcParams['font.sans-serif'] = ['Microsoft JhengHei'] 
plt.rcParams['axes.unicode_minus'] = False

def graph_2d():
    with open('output.txt','r') as f:
        m = collections.defaultdict(list)
        for line in f:
            data = line.split(" ")
            path = '->'.join(data[2].split(','))
            m[path].append([float(data[0]),float(data[1])])

    fig, ax = plt.subplots()
    plt.xlabel("fidelity")
    plt.ylabel("success prob.")
    ax.set_title("fidelity,prob對同path的影響")
    cmap = matplotlib.colormaps.get_cmap('Set2')
    j = 0
    for k,v in m.items():
        for i,(x,y) in enumerate(v):
            if i == 0:
                ax.scatter(x,y,label = k, color = cmap.colors[j])
            else:
                ax.scatter(x,y ,color = cmap.colors[j])

        j+=1
    ax.legend(bbox_to_anchor = (0.85,.5))
    #plt.savefig('2d_graph.png')

def graph_3d():
    with open('output.txt','r') as f:
        x,y,m = [],[],[]
        for line in f:
            data = line.split(" ")
            memory = data[-1].split(',')
            x.append(float(data[0]))
            y.append(float(data[1]))
            m.append(sum([int(el) for el in memory]))
    
    fig = plt.figure()
    mx_memory_use = max(m)
    ax = fig.add_subplot(projection = '3d')
    ax.set_title("prob與使用memory量的影響")
    x = np.array(x)
    y = np.array(y)
    m = np.array(m)
    z = y + m/mx_memory_use
    plt.xlabel("fidelity")
    plt.ylabel("success prob.")
    ax.set_zlabel("value")
    surface = ax.plot_trisurf(x,y,z,linewidth=0.1,cmap = 'coolwarm_r')
    fig.colorbar(surface,shrink = 1.0,aspect = 10)
    #plt.savefig('3d_graph.png')

graph_2d()
graph_3d()
plt.show()
plt.close()
# %%
