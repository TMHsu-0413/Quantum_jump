# %%
import matplotlib
import os
import matplotlib.pyplot as plt

import numpy as np
import collections

marker = ["o","s","x"]
color = ['r','g','b']

def RLBSP_point(name, x=1.0, y=1.0):
    mx_x, mx_y = x, y
    with open(name, "r") as f:
        point = []
        for line in f:
            data = line.split(" ")
            point.append([float(data[0]), float(data[1])])
            mx_x = max(mx_x, float(data[0]))
            mx_y = max(mx_y, float(data[1]))
    fig, ax = plt.subplots()
    plt.xlabel("fidelity")
    plt.ylabel("probability")
    plt.xlim(0, mx_x)
    plt.ylim(0, mx_y)
    ax.set_title(str(name[:-4]))
    for x, y in point:
        ax.scatter(x, y)
    ax.legend(loc="upper right")
    img_dir = os.path.dirname(__file__) + "/image/"
    plt.savefig(img_dir + name[3:-4] + ".png")
    return mx_x, mx_y
    # plt.savefig('2d_graph.png')

RLBSP_point("allpoint.txt")
RLBSP_point("RLBSPpoint.txt")
plt.show()
plt.close()
# %%
