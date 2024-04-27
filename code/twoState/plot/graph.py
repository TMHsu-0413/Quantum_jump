# %%
import matplotlib
import os
import matplotlib.pyplot as plt

import numpy as np
import collections

marker = ["o", "*", "s", "x", "+"]
color = ["r", "g", "b", "c", "m"]
name = ["Ours_0.5", "Ours_0.7", "Ours_0.99", "Q-Path", "Q-Leap"]


def RLBSP_point(name, x=1.0, y=3.5):
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
    plt.savefig(img_dir + name[7:-4] + ".png")
    plt.close()
    return mx_x, mx_y
    # plt.savefig('2d_graph.png')


def different_threshold(y, threshold, n):
    """

    y : 2d list
    y : 1d list

    """

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
    plt.ylabel("Probability")
    plt.title("Different threshold")
    plt.legend(loc="upper right")
    plt.savefig(f"plot/image/different_threshold_on_{n}_nodes")
    plt.close()


def execution_time_on_different_threshold(time, n, threshold):
    for i in range(len(time)):
        plt.plot(threshold, time[i], color="black")
        plt.plot(
            threshold,
            time[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
        )

    plt.xticks(threshold)
    plt.xlabel("threshold")
    plt.ylabel("second")
    plt.title("Different threshold")
    plt.legend(loc="upper right")
    plt.savefig(f"plot/image/time_on_{n}_nodes_with_different_threshold.png")
    plt.close()


def execution_time_on_different_node(time, n):
    for i in range(len(time)):
        plt.plot(n, time[i], color="black")
        plt.plot(
            n,
            time[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
        )

    plt.xticks(n)
    plt.xlabel("the number of nodes")
    plt.ylabel("second")
    plt.title("Different nodes")
    plt.legend(loc="upper right")
    plt.savefig(f"plot/image/time_on_different_nodes.png")
    plt.close()


def find_answer_rate(time, n, th):
    for i in range(len(time)):
        plt.plot(n, time[i], color="black")
        plt.plot(
            n,
            time[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
        )

    plt.xticks(n)
    plt.xlabel("the number of nodes")
    plt.ylabel("%")
    plt.title("Different nodes")
    plt.legend(loc="upper right")
    plt.savefig(f"plot/image/find_rate_{th}threshold.png")
    plt.close()


def find_diff_memory(ans, mem, node, th):
    xsticks = []
    for i in range(len(mem)):
        new_arr = []
        for j in range(2):
            new_arr.append(str(mem[i][j]))
        xsticks.append("-".join(new_arr))

    for i in range(len(ans)):
        plt.plot(xsticks, ans[i], color="black")
        plt.plot(
            xsticks,
            ans[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
        )

    plt.xticks(xsticks)
    plt.xlabel("the number of memorys")
    plt.ylabel("%")
    plt.title("Different memory")
    plt.legend(loc="upper right")
    plt.savefig(f"plot/image/find_rate_{th}threshold_{node}nodes.png")
    plt.close()


# RLBSP_point("output/RLBSPpoint.txt")
# RLBSP_point("output/allpoint.txt")

# 某個8個節點 memory 10-14的case，0代表找不到`
# different_threshold([[0.0288548,0.0131193,0.00488779],[0.00748051,0.00095429,0.000147434],[0.000206319,0,0]], [0.7.0.8,0.9])
# different_nodes([[0.0658073,],[0.0201253],[]],[10,50,70])
# RLBSP_point("allpoint.txt")
# RLBSP_point("RLBSPpoint.txt")
# plt.show()
# plt.close()
# %%
# %%


