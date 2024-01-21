import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import collections
from mpl_toolkits.mplot3d import Axes3D

def graph_2d():
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
    plt.show()
    plt.savefig('2d_graph.png')

def graph_3d():
    with open('output.txt','r') as f:
        x,y = [],[]
        for line in f:
            data = line.split(" ")
            x.append(float(data[0]))
            y.append(float(data[1]))
    # %%
    fig = plt.figure()
    ax = fig.gca(projection = '3d')
    x = np.array(x)
    y = np.array(y)
    z = y**1.5
    plt.xlabel("fidelity")
    plt.ylabel("success prob.")
    surface = ax.plot_trisurf(x,y,z,linewidth=0.1,antialiased = True,cmap = 'coolwarm_r')
    fig.colorbar(surface,shrink = 1.0,aspect = 10)
    plt.show()
    plt.savefig('3d_graph.png')

graph_2d()
graph_3d()
