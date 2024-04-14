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

    y = [[2, 3, 4], [1, 3, 5], [4, 8, 9]]
    """

    threshold = [0.7, 0.8, 0.9]
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

    plt.xlabel("Fidelity threshold")
    plt.title("Different_threshold")
    plt.legend(loc="upper right")
    plt.savefig("image/different_theta.png")


different_threshold([], [])
# RLBSP_point("allpoint.txt")
# RLBSP_point("RLBSPpoint.txt")
plt.show()
plt.close()
# %%
