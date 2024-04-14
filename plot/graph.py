# %%
import matplotlib
import os
import matplotlib.pyplot as plt

import numpy as np
import collections

marker = ["o", "s", "x"]
color = ["r", "g", "b"]


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


def different_threshold(y, threshold):
    """

    y : 2d list
    y : 1d list

    """
    name = ["Ours", "Q-Path", "Q-Leap"]

    for i in range(len(y)):
        plt.plot(threshold, y[i], color="black")
        plt.plot(
            threshold,
            y[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
        )

    plt.xticks(threshold)
    plt.xlabel("Fidelity threshold")
    plt.title("Different threshold")
    plt.legend(loc="upper right")
    plt.savefig("image/different_theta.png")

def execution_time_on_different_node(time,nodes):
    name = ["Ours", "Q-Path", "Q-Leap"]

    for i in range(len(time)):
        plt.plot(nodes, time[i], color="black")
        plt.plot(
            nodes,
            time[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
        )

    plt.xticks(nodes)
    plt.xlabel("the number of node")
    plt.title("Different nodes")
    plt.legend(loc="upper right")
    plt.savefig("image/different_node.png")


# 某個8個節點 memory 10-14的case，0代表找不到`
different_threshold([[0.0288548,0.0131193,0.00488779],[0.00748051,0.00095429,0.000147434],[0.000206319,0,0]], [0.7.0.8,0.9])
#different_nodes([[0.0658073,],[0.0201253],[]],[10,50,70])
# RLBSP_point("allpoint.txt")
# RLBSP_point("RLBSPpoint.txt")
plt.show()
plt.close()
# %%
