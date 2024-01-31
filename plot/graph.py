# %%
import matplotlib
import matplotlib.pyplot as plt

import numpy as np
import collections
from mpl_toolkits.mplot3d import Axes3D

marker = [".", "o", "+", "x", "1", "^", "h", "*", "s"]


def graph_2d():
    with open("output.txt", "r") as f:
        m = collections.defaultdict(list)
        for line in f:
            data = line.split(" ")
            path = "->".join(data[2].split(","))
            m[path].append([float(data[0]), float(data[1])])

    fig, ax = plt.subplots()
    plt.xlabel("fidelity")
    plt.ylabel("success prob.")
    ax.set_title("fidelity and prob according to path")
    cmap = matplotlib.colormaps.get_cmap("Set2")
    j = 0
    for k, v in m.items():
        if j >= len(cmap.colors):
            break
        for i, (x, y) in enumerate(v):
            if i == 0:
                ax.scatter(x, y, label=k, color=cmap.colors[j], marker=marker[j])
            else:
                ax.scatter(x, y, color=cmap.colors[j], marker=marker[j])
        j += 1
    ax.legend(loc="upper right")
    # plt.savefig('2d_graph.png')


def graph_3d():
    with open("output.txt", "r") as f:
        x, y = [], []
        for line in f:
            data = line.split(" ")
            x.append(float(data[0]))
            y.append(float(data[1]))

    fig = plt.figure()
    ax = fig.add_subplot(projection="3d")
    # ax.set_title("z = x + y")
    mx_x, mx_y = max(x), max(y)
    x = np.array(x)
    y = np.array(y)

    z = x / mx_x + y / mx_y

    plt.xlabel("fidelity")
    plt.ylabel("success prob.")
    ax.set_zlabel("value")
    surface = ax.plot_trisurf(x, y, z, linewidth=0.1, cmap="coolwarm_r")
    fig.colorbar(surface, shrink=1.0, aspect=10)
    # plt.savefig('3d_graph.png')


graph_2d()
# graph_3d()
plt.show()
plt.close()
# %%
