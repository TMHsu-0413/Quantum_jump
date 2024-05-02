import subprocess
import random
import sys
import os
import shutil
from collections import defaultdict
from plot import graph
from plot.node_threshold import ChartGenerator as node_threshold
from plot.dist_purify import ChartGenerator as dist_purify
from plot.node_percent import ChartGenerator as node_percent
from plot.dist_threshold import ChartGenerator as dist_threshold
from plot.prob_threshold import ChartGenerator as prob_threshold
import time

average_time = 50
swap_prob_list = [0.3, 0.4, 0.5, 0.6, 0.7]
th_list = [0.7, 0.75, 0.8, 0.85, 0.9]
th = 0.85
swap_prob = 0.7
average_node = 5

delta = "0.70"


def generate_test_case(n):
    for i in range(average_time):
        subprocess.run(
            [
                "python3",
                "main.py",
                f"testcase/graph{i}_{n}nodes.txt",
                str(n),
                "5",
                "9",
                "0.3",
                str(swap_prob),
                str(swap_prob),
            ]
        )


def compile_and_run(input_file, threshold):
    try:
        subprocess.run(["./output/qPath", input_file, str(threshold)])
        subprocess.run(["./output/qLeap", input_file, str(threshold)])
        subprocess.run(["./output/RSP", input_file, str(threshold), delta])
    except Exception as e:
        print(f"Error occurred: {e}")


def validAnswer(name):
    with open("output/" + name + ".txt", "r") as file:
        for line in file:
            if line[:5] == "error":
                return 0
        return 1


def readResult(arr, node, name, delta=""):
    with open("output/" + name + delta + ".txt", "r") as file:
        for line in file:
            if line[:5] == "error":
                arr["Fidelity"][node].append(0)
                arr["Probability"][node].append(0)
                arr["Path"][node].append([-1])
                arr["PurTimes"][node].append([-1])
            elif len(line) == 0:
                break
            else:
                key = line.split(":")[0].strip()
                val = line.split(":")[1].strip()
                if key != "Path" and key != "PurTimes":
                    val = float(val)
                arr[key][node].append(val)


def modify_swap_prob(filename, prob):
    arr = []
    with open(filename, "r") as f:
        for line in f:
            arr.append(line.split(" "))

    with open(filename, "w+") as f:
        for i, line in enumerate(arr):
            if i == 0:
                f.write(str(line[0]))
            else:
                f.write(" ".join(line[:-1]) + " " + str(prob) + "\n")


def modify_memory(filename, mem):
    arr = []
    with open(filename, "r") as f:
        for line in f:
            arr.append(line.split(" "))

    with open(filename, "w+") as f:
        for i, line in enumerate(arr):
            if i == 0:
                f.write(str(line[0]))
            else:
                f.write(
                    " ".join(line[:-2])
                    + " "
                    + str(random.randint(mem[0], mem[1]))
                    + " "
                    + line[-1]
                )


def modify_y_label(filename, y):
    arr = []
    with open(filename, "r") as f:
        for line in f:
            arr.append(line.split(" "))
    with open(filename, "w+") as f:
        prev_y = 0
        j = 0
        for i, line in enumerate(arr):
            if i == 0:
                f.write(str(line[0]))
            elif i == 1:
                f.write(str(0) + " " + str(prev_y) + " " + " ".join(line[2:]))
            else:
                f.write(str(0) + " " + str(prev_y + y[j]) + " " + " ".join(line[2:]))
                prev_y = prev_y + y[j]
                j += 1


def print_diffNode_prob(n):
    qPath = defaultdict(lambda: defaultdict(list))
    qLeap = defaultdict(lambda: defaultdict(list))
    rsp_7 = defaultdict(lambda: defaultdict(list))
    threshold = [0.7, 0.75, 0.8, 0.85, 0.9]

    rsp_7sum, qPathsum, qLeapsum = (
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
    )
    for i in range(average_time):
        for idx, th in enumerate(threshold):
            compile_and_run(f"testcase/graph{i}_{n}nodes.txt", th)

            readResult(qPath, n, "qPath")
            readResult(qLeap, n, "qLeap")
            readResult(rsp_7, n, "RSP", delta)

            rsp_7sum[idx] += rsp_7["Probability"][n][-1]
            qPathsum[idx] += qPath["Probability"][n][-1]
            qLeapsum[idx] += qLeap["Probability"][n][-1]

    for i in range(len(rsp_7sum)):
        rsp_7sum[i] /= average_time
        qPathsum[i] /= average_time
        qLeapsum[i] /= average_time

    print(rsp_7sum)
    f = open(f"outputTxt/different_threshold_{n}_nodes.ans", "w")
    for i in range(len(threshold)):
        f.write(f"{threshold[i]} {rsp_7sum[i]} {qPathsum[i]} {qLeapsum[i]} \n")


def print_average_in_different_nodes(node_num):
    ans = [[] for _ in range(3)]
    for n in node_num:
        validTime = average_time
        rsp_7Time, qPathTime, qLeapTime = 0, 0, 0
        for i in range(average_time):
            subprocess.run(
                [
                    "python3",
                    "main.py",
                    f"testcase/diffNode{n}_{i}.txt",
                    str(n),
                    "5",
                    "9",
                    "0.3",
                    str(swap_prob),
                    str(swap_prob),
                ]
            )
            compile_and_run(f"testcase/diffNode{n}_{i}.txt", th)

            if not validAnswer(f"RSP{delta}"):
                validTime -= 1
                continue
            rsp_7Time += validAnswer(f"RSP{delta}")
            qPathTime += validAnswer("qPath")
            qLeapTime += validAnswer("qLeap")
        if validTime == 0:
            ans[0].append(0)
            ans[1].append(0)
            ans[2].append(0)
        else:
            ans[0].append((rsp_7Time / validTime) * 100)
            ans[1].append((qPathTime / validTime) * 100)
            ans[2].append((qLeapTime / validTime) * 100)

    # graph.find_answer_rate(ans, node_num, threshold)
    f = open(f"outputTxt/find_answer_rate_on_{th}th.ans", "w")
    for i in range(len(node_num)):
        f.write(f"{node_num[i]} {ans[0][i]} {ans[1][i]} {ans[2][i]} \n")


def print_diff_prob(swap_prob_list, n):
    ans = [[] for _ in range(3)]

    for p in swap_prob_list:
        rsp_7sum, qPathsum, qLeapsum = 0, 0, 0
        qPath = defaultdict(lambda: defaultdict(list))
        qLeap = defaultdict(lambda: defaultdict(list))
        rsp_7 = defaultdict(lambda: defaultdict(list))
        for i in range(average_time):
            shutil.copyfile(
                f"testcase/graph{i}_{n}nodes.txt", f"testcase/graph{i}cp_{n}nodes.txt"
            )
            modify_swap_prob(f"testcase/graph{i}cp_{n}nodes.txt", str(p))
            compile_and_run(f"testcase/graph{i}cp_{n}nodes.txt", th)
            readResult(qPath, n, "qPath")
            readResult(qLeap, n, "qLeap")
            readResult(rsp_7, n, "RSP", delta)

            rsp_7sum += rsp_7["Probability"][n][-1]
            qPathsum += qPath["Probability"][n][-1]
            qLeapsum += qLeap["Probability"][n][-1]

        ans[0].append(rsp_7sum / average_time)
        ans[1].append(qPathsum / average_time)
        ans[2].append(qLeapsum / average_time)

    print(ans[0])
    # graph.find_diff_swapProb(ans, swap_prob_list, nodes, th)
    f = open("outputTxt/find_diff_swapProb.ans", "w")
    for i in range(len(swap_prob_list)):
        f.write(f"{swap_prob_list[i]} {ans[0][i]} {ans[1][i]} {ans[2][i]} \n")


def ans_point_Scatter(node, th):
    f = open(f"outputTxt/scatterPoint_{node}nodes.ans", "w")
    memSet = [[8, 9], [6, 7], [4, 5], [2, 3]]
    qPath = defaultdict(lambda: defaultdict(list))
    qLeap = defaultdict(lambda: defaultdict(list))
    rsp_7 = defaultdict(lambda: defaultdict(list))
    ans = []
    shutil.copyfile(
        f"testcase/graph0_{node}nodes.txt", f"testcase/graph0cp_{node}nodes.txt"
    )
    for i in range(4):
        modify_memory(f"testcase/graph0cp_{node}nodes.txt", memSet[i])
        subprocess.run(
            ["./output/RLBSP", f"testcase/graph0cp_{node}nodes.txt", str(th)]
        )
        compile_and_run(f"testcase/graph0cp_{node}nodes.txt", th)
        readResult(qPath, node, "qPath")
        readResult(qLeap, node, "qLeap")
        readResult(rsp_7, node, "RSP", delta)

        with open("output/allpoint.txt", "r") as f2:
            line = f2.readlines()
            for l in line:
                cur = l.split(" ")
                f.write(f"{i} 1 {cur[0]} {cur[1]}")

    for i, (fi, p) in enumerate(
        zip(rsp_7["Fidelity"][node], rsp_7["Probability"][node])
    ):
        f.write(f"4 2 {fi} {p}\n")
    for i, (fi, p) in enumerate(
        zip(qPath["Fidelity"][node], qPath["Probability"][node])
    ):
        f.write(f"5 3 {fi} {p}\n")
    for i, (fi, p) in enumerate(
        zip(qLeap["Fidelity"][node], qLeap["Probability"][node])
    ):
        f.write(f"6 4 {fi} {p}\n")

    # 這有時候會bug，印不出全部，可直接執行完再另外call一次
    # graph.multiColor("outputTxt/scatterPoint_5nodes.ans")


def avg_entangle_dis(avg_dis, n):
    def random_number_with_avg(n, avg):
        total = n * avg
        numbers = []
        for _ in range(n):
            rand_num = random.uniform(avg - 5, avg + 5)
            numbers.append(rand_num)
            total -= rand_num
        return numbers

    ans = [[0, 0, 0, 0, 0] for _ in range(3)]
    for t in range(average_time):
        qPath = defaultdict(lambda: defaultdict(list))
        qLeap = defaultdict(lambda: defaultdict(list))
        rsp_7 = defaultdict(lambda: defaultdict(list))

        shutil.copyfile(
            f"testcase/graph{t}_{n}nodes.txt", f"testcase/graph{t}cp_{n}nodes.txt"
        )
        d = random_number_with_avg(average_node, avg_dis[0])
        for j in range(len(avg_dis)):
            modify_y_label(f"testcase/graph{t}cp_{n}nodes.txt", d)
            compile_and_run(f"testcase/graph{t}cp_{n}nodes.txt", th)

            readResult(qPath, n, "qPath")
            readResult(qLeap, n, "qLeap")
            readResult(rsp_7, n, "RSP", delta)

            ans[0][j] += rsp_7["Probability"][n][-1]
            ans[1][j] += qPath["Probability"][n][-1]
            ans[2][j] += qLeap["Probability"][n][-1]

            for i in range(len(d)):
                d[i] += 5

    for i in range(len(ans)):
        for j in range(len(ans[i])):
            ans[i][j] /= average_time
    # graph.avg_entangle_dis(ans, avg_dis, node, th)
    f = open("outputTxt/avg_entangle_dis.ans", "w")
    for i in range(len(avg_dis)):
        f.write(f"{avg_dis[i]-5}-{avg_dis[i]+5} {ans[0][i]} {ans[1][i]} {ans[2][i]} \n")


def avg_purify_dis(avg_dis, n):
    sum_count = [
        [average_time, average_time, average_time, average_time, average_time]
        for _ in range(3)
    ]

    def calsum(arr, i, j):
        nonlocal sum_count
        if arr[0] == -1:
            sum_count[i][j] -= 1
            return 0
        new_arr = [int(el) for el in arr.split(" ")]
        return sum(new_arr)

    def random_number_with_avg(n, avg):
        total = n * avg
        numbers = []
        for _ in range(n):
            rand_num = random.uniform(avg - 5, avg + 5)
            numbers.append(rand_num)
            total -= rand_num
        return numbers

    ans = [[0, 0, 0, 0, 0] for _ in range(3)]
    for t in range(average_time):
        qPath = defaultdict(lambda: defaultdict(list))
        qLeap = defaultdict(lambda: defaultdict(list))
        rsp_7 = defaultdict(lambda: defaultdict(list))
        d = random_number_with_avg(average_node, avg_dis[0])

        shutil.copyfile(
            f"testcase/graph{t}_{n}nodes.txt", f"testcase/grahp{t}cp_{n}nodes.txt"
        )
        for i, dis in enumerate(avg_dis):
            modify_y_label(f"testcase/graph{t}cp_{n}nodes.txt", d)
            compile_and_run(f"testcase/graph{t}cp_{n}nodes.txt", th)

            readResult(qPath, n, "qPath")
            readResult(qLeap, n, "qLeap")
            readResult(rsp_7, n, "RSP", delta)

            ans[0][i] += calsum(rsp_7["PurTimes"][n][-1], 0, i)
            ans[1][i] += calsum(qPath["PurTimes"][n][-1], 1, i)
            ans[2][i] += calsum(qLeap["PurTimes"][n][-1], 2, i)
            for i in range(len(d)):
                d[i] += 5

    for i in range(len(ans)):
        for j in range(len(ans[0])):
            if sum_count[i][j] != 0:
                ans[i][j] /= sum_count[i][j]
    # graph.avg_purifyTime_dis(ans, avg_dis, node, th)
    f = open("outputTxt/avg_purifyTime_dis.ans", "w")
    for i in range(len(avg_dis)):
        f.write(f"{avg_dis[i]-5}-{avg_dis[i]+5} {ans[0][i]} {ans[1][i]} {ans[2][i]} \n")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 script.py <number of node> <number of node> ...")
        sys.exit(1)

    def create_dir():
        try:
            os.makedirs("output")
        except FileExistsError:
            print("output exists")

        try:
            os.makedirs("outputTxt")
        except FileExistsError:
            print("outputTxt exists")

        try:
            os.makedirs("testcase")
        except FileExistsError:
            print("testcase exists")

        try:
            os.makedirs("plot/image")
        except FileExistsError:
            print("plot/image exists")

    create_dir()
    subprocess.run(["g++", "--std=c++17", "qPath/qPath.cpp", "-o", "output/qPath"])
    subprocess.run(["g++", "--std=c++17", "qLeap/qLeap.cpp", "-o", "output/qLeap"])
    subprocess.run(["g++", "--std=c++17", "RSP/RSP.cpp", "-o", "output/RSP"])
    subprocess.run(["g++", "--std=c++17", "RLBSP/RLBSP.cpp", "-o", "output/RLBSP"])
    print_average_in_different_nodes([10, 12, 15, 17, 20])
    node_percent( "outputTxt/find_answer_rate_on_0.85th.ans", "% of Feasible Sol.", "# Nodes")
    for i in range(1, len(sys.argv)):
        generate_test_case(sys.argv[i])
        print_diffNode_prob(sys.argv[i])
    print_diff_prob(swap_prob_list, sys.argv[1])

    ans_point_Scatter(5, 0.8)
    # ans_point_Scatter(7, 0.7)

    avg_entangle_dis([10, 15, 20, 25, 30], sys.argv[1])
    avg_purify_dis([10, 15, 20, 25, 30], sys.argv[1])

    # plot
    #graph.multiColor("outputTxt/scatterPoint_5nodes.ans")
    for i in range(1, len(sys.argv)):
        node_threshold(
            f"outputTxt/different_threshold_{sys.argv[i]}_nodes.ans",
            "Avg. Probability",
            "Fidelity Threshold",
        )
    dist_purify("outputTxt/avg_purifyTime_dis.ans", "Avg. Purification", "Distance (km)")
    dist_threshold("outputTxt/avg_entangle_dis.ans", "Avg. Probability", "Distance (km)")
    prob_threshold(
        "outputTxt/find_diff_swapProb.ans", "Avg. Probability", "Swap. Probability"
    )