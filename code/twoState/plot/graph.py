# %%
import matplotlib
import os
import matplotlib.pyplot as plt

import numpy as np
import collections

marker = ["o", "*", "s", "x", "+"]
color = ["r", "g", "b", "c", "m"]
name = ["Ours_0.7", "Ours_0.9", "Q-Path", "Q-Leap"]


def ans_point(ans, filename, th, x=0.5, y=1.0):
    mx_x, mx_y = x, y
    st = set()
    print(ans)
    for i, solve in enumerate(ans):
        x, y = solve[0], solve[1]
        st.add((x, y))
    with open(filename, "r") as f:
        point_x, point_y = [], []
        for line in f:
            data = line.split(" ")
            if (float(data[0]), float(data[1])) in st:
                continue
            point_x.append(float(data[0]))
            point_y.append(float(data[1]))
            mx_x = max(mx_x, float(data[0]))
            mx_y = max(mx_y, float(data[1]))
    fig, ax = plt.subplots()
    plt.xlabel("fidelity")
    plt.ylabel("probability")
    plt.xlim(0, mx_x)
    plt.ylim(0, mx_y)
    plt.scatter(point_x, point_y, color="grey", label="_nolegend_")
    for i, solve in enumerate(ans):
        x, y = solve[0], solve[1]
        plt.scatter(x, y, color=color[i], marker=marker[i], label=name[i])
    plt.vlines(x=th, ymin=0, ymax=1, linestyles="dashed", colors="red")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )
    img_dir = os.path.dirname(__file__) + "/image/"
    plt.savefig(img_dir + "ans_scatter.png")
    plt.close()
    return mx_x, mx_y
    # plt.savefig('2d_graph.png')


def different_threshold(y, threshold, n):
    """

    y : 2d list
    y : 1d list

    """

    for i in range(len(y)):
        plt.plot(
            threshold,
            y[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            linestyle="-",
            label=name[i],
        )

    plt.xticks(threshold)
    plt.xlabel("Fidelity threshold")
    plt.ylabel("Probability")
    # plt.title("Different threshold")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )

    plt.savefig(f"plot/image/different_threshold_on_{n}_nodes")
    plt.close()


def execution_time_on_different_threshold(time, n, threshold):
    for i in range(len(time)):
        plt.plot(
            threshold,
            time[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
            linestyle="-",
        )

    plt.xticks(threshold)
    plt.xlabel("threshold")
    plt.ylabel("second")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )
    plt.savefig(f"plot/image/time_on_{n}_nodes_with_different_threshold.png")
    plt.close()


def execution_time_on_different_node(time, n):
    for i in range(len(time)):
        plt.plot(
            n,
            time[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
            linestyle="-",
        )

    plt.xticks(n)
    plt.xlabel("the number of nodes")
    plt.ylabel("second")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )
    plt.savefig(f"plot/image/time_on_different_nodes.png")
    plt.close()


def find_answer_rate(time, n, th):
    for i in range(len(time)):
        plt.plot(
            n,
            time[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
            linestyle="-",
        )

    plt.xticks(n)
    plt.xlabel("the number of nodes")
    plt.ylabel("%")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )
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
        plt.plot(
            xsticks,
            ans[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
            linestyle="-",
        )

    plt.xticks(xsticks)
    plt.xlabel("the number of memorys")
    plt.ylabel("%")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )
    plt.savefig(f"plot/image/find_rate_{th}threshold_{node}nodes.png")
    plt.close()


def find_diff_swapProb(ans, prob, node, th):
    for i in range(len(ans)):
        plt.plot(
            prob,
            ans[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
            linestyle="-",
        )

    plt.xticks(prob)
    plt.xlabel("swapping prob.")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )
    plt.savefig(f"plot/image/diff_prob.png")
    plt.close()


def avg_purify_time(ans, memory, node, th):
    xsticks = []
    for i in range(len(memory)):
        new_arr = []
        for j in range(2):
            new_arr.append(str(memory[i][j]))
        xsticks.append("-".join(new_arr))

    for i in range(len(ans)):
        plt.plot(
            xsticks,
            ans[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
            linestyle="-",
        )

    plt.xticks(xsticks)
    plt.xlabel("memory")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )
    plt.savefig(f"plot/image/avg_purify_time.png")
    plt.close()


def max_purify_time(ans, memory, node, th):
    xsticks = []
    for i in range(len(memory)):
        new_arr = []
        for j in range(2):
            new_arr.append(str(memory[i][j]))
        xsticks.append("-".join(new_arr))

    for i in range(len(ans)):
        plt.plot(
            xsticks,
            ans[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
            linestyle="-",
        )

    plt.xticks(xsticks)
    plt.xlabel("memory")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )
    plt.savefig(f"plot/image/max_purify_time.png")
    plt.close()


def avg_entangle_dis(ans, dis, node, th):
    xsticks = []
    for i in range(len(dis)):
        new_arr = []
        new_arr.append(str(dis[i] - 10))
        new_arr.append(str(dis[i] + 10))
        xsticks.append("-".join(new_arr))

    for i in range(len(ans)):
        plt.plot(
            xsticks,
            ans[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
            linestyle="-",
        )

    plt.xticks(xsticks)
    plt.xlabel("distance")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )
    plt.savefig(f"plot/image/avg_entangle_dis.png")
    plt.close()


def avg_purifyTime_dis(ans, dis, node, th):
    xsticks = []
    for i in range(len(dis)):
        new_arr = []
        new_arr.append(str(dis[i] - 10))
        new_arr.append(str(dis[i] + 10))
        xsticks.append("-".join(new_arr))

    for i in range(len(ans)):
        plt.plot(
            xsticks,
            ans[i],
            marker=marker[i],
            color=color[i],
            markerfacecolor="none",
            label=name[i],
            linestyle="-",
        )

    plt.xticks(xsticks)
    plt.xlabel("distance")
    plt.subplots_adjust(top=0.75)
    plt.subplots_adjust(left=0.2)
    plt.subplots_adjust(right=0.8)
    plt.subplots_adjust(bottom=0.25)
    plt.legend(
        name,
        loc=10,
        bbox_to_anchor=(0.5, 1.25),
        prop={"size": 16, "family": "Times New Roman"},
        frameon=False,
        labelspacing=0.2,
        handletextpad=0.2,
        columnspacing=0.2,
        ncol=3,
        facecolor=None,
    )
    plt.savefig(f"plot/image/avg_purifyTime_dis.png")
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
