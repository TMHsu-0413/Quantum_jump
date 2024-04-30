import subprocess
import random
import sys
from collections import defaultdict
from plot import graph
import time


def compile_and_run(input_file, threshold):
    try:
        subprocess.run(["./output/qPath", input_file, str(threshold)])
        subprocess.run(["./output/qLeap", input_file, str(threshold)])
        # subprocess.run(["./output/RLBSP", input_file, str(threshold)])
        # subprocess.run(["./output/RSP", input_file, str(threshold), str(0.50)])
        subprocess.run(["./output/RSP", input_file, str(threshold), str(0.70)])
        # subprocess.run(["./output/RSP", input_file, str(threshold), str(0.90)])

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
        print(len(y), len(arr))
        for i, line in enumerate(arr):
            if i == 0:
                f.write(str(line[0]))
            elif i == 1:
                f.write(str(0) + " " + str(prev_y) + " " + " ".join(line[2:]))
            else:
                f.write(str(0) + " " + str(prev_y + y[j]) + " " + " ".join(line[2:]))
                prev_y = prev_y + y[j]
                j += 1


def print_diffNode_prob(node, times):
    # 依照node array各生成一張圖，印出prob，執行時間
    qPath = defaultdict(lambda: defaultdict(list))
    qLeap = defaultdict(lambda: defaultdict(list))
    rsp_5 = defaultdict(lambda: defaultdict(list))
    rsp_7 = defaultdict(lambda: defaultdict(list))
    rsp_9 = defaultdict(lambda: defaultdict(list))

    for n in node:
        threshold = [0.7, 0.75, 0.8, 0.85, 0.9]
        rsp_5sum, rsp_7sum, rsp_9sum, qPathsum, qLeapsum = (
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0],
        )
        for _ in range(times):
            subprocess.run(
                [
                    "python3",
                    "main.py",
                    "graph.txt",
                    str(n),
                    "5",
                    "9",
                    "0.3",
                    "0.7",
                    "0.7",
                ]
            )
            number_of_nodes = int(n)

            for idx, th in enumerate(threshold):
                compile_and_run("graph.txt", th)

                readResult(qPath, number_of_nodes, "qPath")
                readResult(qLeap, number_of_nodes, "qLeap")
                # readResult(rLbsp, number_of_nodes, "RLBSP")
                # readResult(rsp_5, number_of_nodes, "RSP", "0.50")
                readResult(rsp_7, number_of_nodes, "RSP", "0.70")
                # readResult(rsp_9, number_of_nodes, "RSP", "0.90")

                # rsp_5sum[idx] += rsp_5["Probability"][number_of_nodes][-1]
                rsp_7sum[idx] += rsp_7["Probability"][number_of_nodes][-1]
                # rsp_9sum[idx] += rsp_9["Probability"][number_of_nodes][-1]
                qPathsum[idx] += qPath["Probability"][number_of_nodes][-1]
                qLeapsum[idx] += qLeap["Probability"][number_of_nodes][-1]

        for i in range(len(rsp_7sum)):
            rsp_7sum[i] /= times
            # rsp_9sum[i] /= times
            qPathsum[i] /= times
            qLeapsum[i] /= times

        graph.different_threshold(
            [rsp_7sum, qPathsum, qLeapsum],
            threshold,
            n,
        )

        f = open(f"outputTxt/different_threshold_{n}_nodes.ans", "w")
        for i in range(len(rsp_7sum)):
            f.write(f"{threshold[i]} {rsp_7sum[i]} {qPathsum[i]} {qLeapsum[i]} \n")
        # graph.execution_time_on_different_threshold( [rsp_7sum, rsp_9sum, qPathsum, qLeapsum], n, threshold,)
    # print_etodn(rsp_7,rsp_9,rsp_99, qPath, qLeap, node)


def print_etodn(rsp_5, rsp_7, rsp_9, qPath, qLeap, node_num):
    # rLbspTime = []
    qPathTime = []
    rsp_5Time = []
    rsp_7Time = []
    rsp_9Time = []
    qLeapTime = []
    for el in node_num:
        # rLbspTime.append(rLbsp["Time"][el][0])
        qPathTime.append(qPath["Time"][el][0])
        qLeapTime.append(qLeap["Time"][el][0])
        # rsp_5Time.append(rsp_5["Time"][el][0])
        rsp_7Time.append(rsp_7["Time"][el][0])
        # rsp_9Time.append(rsp_9["Time"][el][0])
    graph.execution_time_on_different_node([rsp_7Time, qPathTime, qLeapTime], node_num)


def print_average_in_different_nodes(node_num, runTime, threshold):
    ans = [[] for _ in range(3)]
    for node in node_num:
        rsp_5Time, rsp_7Time, rsp_9Time, qPathTime, qLeapTime = 0, 0, 0, 0, 0
        validTime = runTime
        for _ in range(runTime):
            subprocess.run(
                [
                    "python3",
                    "main.py",
                    "graph.txt",
                    str(node),
                    "10",
                    "14",
                    "0.4",
                    "0.7",
                    "0.7",
                ]
            )
            compile_and_run("graph.txt", threshold)

            # rsp_5Time += validAnswer("RSP0.50")
            if not validAnswer("RSP0.70"):
                validTime -= 1
                continue
            rsp_7Time += validAnswer("RSP0.70")
            # rsp_9Time += validAnswer("RSP0.90")
            qPathTime += validAnswer("qPath")
            qLeapTime += validAnswer("qLeap")

        # ans[0].append((rsp_5Time / runTime) * 100)
        ans[0].append((rsp_7Time / validTime) * 100)
        # ans[1].append((rsp_9Time / runTime) * 100)
        ans[1].append((qPathTime / validTime) * 100)
        ans[2].append((qLeapTime / validTime) * 100)

    graph.find_answer_rate(ans, node_num, threshold)
    f = open(f"outputTxt/find_answer_rate_on_{threshold}th.ans", "w")
    for i in range(len(node_num)):
        f.write(f"{node_num[i]} {ans[0][i]} {ans[1][i]} {ans[2][i]} \n")


def print_answer_in_different_memory(mem, runTime, node, th):
    ans = [[] for _ in range(3)]
    for [mn, mx] in mem:
        rsp_5Time, rsp_7Time, rsp_9Time, qPathTime, qLeapTime = 0, 0, 0, 0, 0
        for _ in range(runTime):
            subprocess.run(
                [
                    "python3",
                    "main.py",
                    "graph.txt",
                    str(node),
                    str(mn),
                    str(mx),
                    "0.3",
                    "0.7",
                    "0.7",
                ]
            )
            compile_and_run("graph.txt", th)

            # rsp_5Time += validAnswer("RSP0.50")
            rsp_7Time += validAnswer("RSP0.70")
            # rsp_9Time += validAnswer("RSP0.90")
            qPathTime += validAnswer("qPath")
            qLeapTime += validAnswer("qLeap")

        # ans[0].append((rsp_5Time / runTime) * 100)
        ans[0].append((rsp_7Time / runTime) * 100)
        # ans[1].append((rsp_9Time / runTime) * 100)
        ans[1].append((qPathTime / runTime) * 100)
        ans[2].append((qLeapTime / runTime) * 100)
    graph.find_diff_memory(ans, mem, node, th)
    f = open(f"outputTxt/find_diff_memory.ans", "w")
    for i in range(len(mem)):
        f.write(f"{mem[i]} {ans[0][i]} {ans[1][i]} {ans[2][i]} \n")


def print_diff_prob(swap_prob_list, th, nodes):
    ans = [[] for _ in range(3)]
    qPath = defaultdict(lambda: defaultdict(list))
    qLeap = defaultdict(lambda: defaultdict(list))
    rsp_5 = defaultdict(lambda: defaultdict(list))
    rsp_7 = defaultdict(lambda: defaultdict(list))
    rsp_9 = defaultdict(lambda: defaultdict(list))
    subprocess.run(
        ["python3", "main.py", "graph.txt", str(nodes), "5", "9", "0.3", "1", "1"]
    )

    for p in swap_prob_list:
        modify_swap_prob("graph.txt", str(p))

        compile_and_run("graph.txt", th)
        readResult(qPath, nodes, "qPath")
        readResult(qLeap, nodes, "qLeap")
        # readResult(rLbsp, number_of_nodes, "RLBSP")
        # readResult(rsp_5, nodes, "RSP", "0.50")
        readResult(rsp_7, nodes, "RSP", "0.70")
        # readResult(rsp_9, nodes, "RSP", "0.90")

    # ans[0] = rsp_5["Probability"][nodes]
    ans[0] = rsp_7["Probability"][nodes]
    # ans[1] = rsp_9["Probability"][nodes]
    ans[1] = qPath["Probability"][nodes]
    ans[2] = qLeap["Probability"][nodes]

    graph.find_diff_swapProb(ans, swap_prob_list, nodes, th)
    f = open(f"outputTxt/find_diff_swapProb.ans", "w")
    for i in range(len(swap_prob_list)):
        f.write(f"{swap_prob_list[i]} {ans[0][i]} {ans[1][i]} {ans[2][i]} \n")


def ans_point_Scatter(node, th):
    subprocess.run(
        [
            "python3",
            "main.py",
            "graph.txt",
            str(node),
            "5",
            "9",
            "0.3",
            "0.8",
            "0.8",
        ]
    )

    qPath = defaultdict(lambda: defaultdict(list))
    qLeap = defaultdict(lambda: defaultdict(list))
    rsp_5 = defaultdict(lambda: defaultdict(list))
    rsp_7 = defaultdict(lambda: defaultdict(list))
    rsp_9 = defaultdict(lambda: defaultdict(list))
    subprocess.run(["./output/RLBSP", "graph.txt", str(1)])
    compile_and_run("graph.txt", th)
    readResult(qPath, node, "qPath")
    readResult(qLeap, node, "qLeap")
    # readResult(rsp_5, node, "RSP", "0.50")
    readResult(rsp_7, node, "RSP", "0.70")
    # readResult(rsp_9, node, "RSP", "0.90")

    ans = [[] for _ in range(3)]
    # ans[0] = [rsp_5["Fidelity"][node][0], rsp_5["Probability"][node][0]]
    ans[0] = [rsp_7["Fidelity"][node][0], rsp_7["Probability"][node][0]]
    # ans[1] = [rsp_9["Fidelity"][node][0], rsp_9["Probability"][node][0]]
    ans[1] = [qPath["Fidelity"][node][0], qPath["Probability"][node][0]]
    ans[2] = [qLeap["Fidelity"][node][0], qLeap["Probability"][node][0]]

    # print(ans)

    graph.ans_point(ans, "output/allpoint.txt", th)


def avg_purify_time(node, th, swap_prob, times, mem):
    sum_count = [[times, times, times] for _ in range(3)]
    max_count = [[times, times, times] for _ in range(3)]

    def calsum(arr, i, j):
        nonlocal sum_count
        if arr[0] == -1:
            sum_count[i][j] -= 1
            return 0
        new_arr = [int(el) for el in arr.split(" ")]
        return sum(new_arr)

    def calmax(arr, i, j):
        nonlocal max_count
        if arr[0] == -1:
            max_count[i][j] -= 1
            return 0
        new_arr = [int(el) for el in arr.split(" ")]
        return max(new_arr)

    qPath = defaultdict(lambda: defaultdict(list))
    qLeap = defaultdict(lambda: defaultdict(list))
    rsp_5 = defaultdict(lambda: defaultdict(list))
    rsp_7 = defaultdict(lambda: defaultdict(list))
    rsp_9 = defaultdict(lambda: defaultdict(list))
    avgSum = [[0, 0, 0] for _ in range(3)]
    maxSum = [[0, 0, 0] for _ in range(3)]
    for _ in range(times):
        subprocess.run(
            [
                "python3",
                "main.py",
                "graph.txt",
                str(node),
                "1",
                "1",
                "0.3",
                str(swap_prob),
                str(swap_prob),
            ]
        )
        for i, m in enumerate(mem):
            modify_memory("graph.txt", m)
            compile_and_run("graph.txt", th)
            readResult(qPath, node, "qPath")
            readResult(qLeap, node, "qLeap")
            # readResult(rsp_5, node, "RSP", "0.50")
            readResult(rsp_7, node, "RSP", "0.70")
            # readResult(rsp_9, node, "RSP", "0.90")

            # avgSum[0][i] += calsum(rsp_5["PurTimes"][node][-1], 0, i)
            avgSum[0][i] += calsum(rsp_7["PurTimes"][node][-1], 0, i)
            # avgSum[1][i] += calsum(rsp_9["PurTimes"][node][-1], 1, i)
            avgSum[1][i] += calsum(qPath["PurTimes"][node][-1], 1, i)
            avgSum[2][i] += calsum(qLeap["PurTimes"][node][-1], 2, i)

            # maxSum[0][i] += calmax(rsp_5["PurTimes"][node][-1], 0, i)
            maxSum[0][i] += calmax(rsp_7["PurTimes"][node][-1], 0, i)
            # maxSum[1][i] += calmax(rsp_9["PurTimes"][node][-1], 1, i)
            maxSum[1][i] += calmax(qPath["PurTimes"][node][-1], 1, i)
            maxSum[2][i] += calmax(qLeap["PurTimes"][node][-1], 2, i)

        for i in range(len(avgSum)):
            for j in range(len(mem)):
                if sum_count[i][j] != 0:
                    avgSum[i][j] /= sum_count[i][j]
                if max_count[i][j] != 0:
                    maxSum[i][j] /= max_count[i][j]

    graph.avg_purify_time(avgSum, mem, node, th)
    f = open("outputTxt/avg_purify_time.ans", "w")
    for i in range(len(avgSum)):
        f.write(f"{mem[i]} {avgSum[0][i]} {avgSum[1][i]} {avgSum[2][i]} \n")
    # graph.max_purify_time(maxSum, mem, node, th)


def avg_entangle_dis(node, th, swap_prob, avg_dis, times):
    def random_number_with_avg(n, avg):
        total = n * avg
        numbers = []
        for _ in range(n):
            rand_num = random.uniform(avg - 5, avg + 5)
            numbers.append(rand_num)
            total -= rand_num
        return numbers

    ans = [[0, 0, 0, 0, 0] for _ in range(3)]
    for _ in range(times):
        qPath = defaultdict(lambda: defaultdict(list))
        qLeap = defaultdict(lambda: defaultdict(list))
        rsp_5 = defaultdict(lambda: defaultdict(list))
        rsp_7 = defaultdict(lambda: defaultdict(list))
        rsp_9 = defaultdict(lambda: defaultdict(list))
        subprocess.run(
            [
                "python3",
                "main.py",
                "graph.txt",
                str(node),
                "5",
                "9",
                "0.3",
                str(swap_prob),
                str(swap_prob),
            ]
        )

        d = random_number_with_avg(node, avg_dis[0])
        for j in range(5):
            modify_y_label("graph.txt", d)
            compile_and_run("graph.txt", th)

            readResult(qPath, node, "qPath")
            readResult(qLeap, node, "qLeap")
            # readResult(rsp_5, node, "RSP", "0.50")
            readResult(rsp_7, node, "RSP", "0.70")
            # readResult(rsp_9, node, "RSP", "0.90")

            # ans[0].append(rsp_5["Probability"][node][-1])
            ans[0][j] += rsp_7["Probability"][node][-1]
            # ans[1].append(rsp_9["Probability"][node][-1])
            ans[1][j] += qPath["Probability"][node][-1]
            ans[2][j] += qLeap["Probability"][node][-1]

            for i in range(len(d)):
                d[i] += 5

    for i in range(len(ans)):
        for j in range(len(ans[i])):
            ans[i][j] /= times
    graph.avg_entangle_dis(ans, avg_dis, node, th)
    f = open("outputTxt/avg_entangle_dis.ans", "w")
    for i in range(len(avg_dis)):
        f.write(f"{avg_dis[i]-5}-{avg_dis[i]+5} {ans[0][i]} {ans[1][i]} {ans[2][i]} \n")


def avg_purify_dis(node, th, swap_prob, avg_dis, times):
    sum_count = [[times, times, times, times, times] for _ in range(3)]

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
    for _ in range(times):
        qPath = defaultdict(lambda: defaultdict(list))
        qLeap = defaultdict(lambda: defaultdict(list))
        rsp_5 = defaultdict(lambda: defaultdict(list))
        rsp_7 = defaultdict(lambda: defaultdict(list))
        rsp_9 = defaultdict(lambda: defaultdict(list))
        subprocess.run(
            [
                "python3",
                "main.py",
                "graph.txt",
                str(node),
                "5",
                "9",
                "0.3",
                str(swap_prob),
                str(swap_prob),
            ]
        )

        d = random_number_with_avg(node, avg_dis[0])
        for i, dis in enumerate(avg_dis):
            modify_y_label("graph.txt", d)
            compile_and_run("graph.txt", th)

            readResult(qPath, node, "qPath")
            readResult(qLeap, node, "qLeap")
            # readResult(rsp_5, node, "RSP", "0.50")
            readResult(rsp_7, node, "RSP", "0.70")
            # readResult(rsp_9, node, "RSP", "0.90")

            # ans[0][i] += calsum(rsp_5["PurTimes"][node][-1], 0, i)
            ans[0][i] += calsum(rsp_7["PurTimes"][node][-1], 0, i)
            # ans[1][i] += calsum(rsp_9["PurTimes"][node][-1], 1, i)
            ans[1][i] += calsum(qPath["PurTimes"][node][-1], 1, i)
            ans[2][i] += calsum(qLeap["PurTimes"][node][-1], 2, i)
            for i in range(len(d)):
                d[i] += 5

    for i in range(len(ans)):
        for j in range(len(ans[0])):
            if sum_count[i][j] != 0:
                ans[i][j] /= sum_count[i][j]
    graph.avg_purifyTime_dis(ans, avg_dis, node, th)
    f = open("outputTxt/avg_purifyTime_dis.ans", "w")
    for i in range(len(avg_dis)):
        f.write(f"{avg_dis[i]-5}-{avg_dis[i]+5} {ans[0][i]} {ans[1][i]} {ans[2][i]} \n")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 script.py <number of node> <number of node> ...")
        sys.exit(1)
    subprocess.run(["g++", "--std=c++17", "qPath/qPath.cpp", "-o", "output/qPath"])
    subprocess.run(["g++", "--std=c++17", "qLeap/qLeap.cpp", "-o", "output/qLeap"])
    subprocess.run(["g++", "--std=c++17", "RSP/RSP.cpp", "-o", "output/RSP"])
    subprocess.run(["g++", "--std=c++17", "RLBSP/RLBSP.cpp", "-o", "output/RLBSP"])

    node_num = []
    for i in range(1, len(sys.argv)):
        node_num.append(sys.argv[i])
    # print_diffNode_prob(node_num, 1)

    average_time = 25
    swap_prob_list = [0.3, 0.4, 0.5, 0.6, 0.7]
    th_list = [0.7, 0.75, 0.8, 0.85, 0.9]
    th = 0.8
    swap_prob = 0.8
    average_node = 2
    for th in th_list:
        print_average_in_different_nodes([10, 12, 15, 17, 20], average_time, th)
    # print_answer_in_different_memory( [[5, 7], [8, 10], [11, 13]], average_time, average_node, th)

    # print_diff_prob(swap_prob_list, th, average_node)
    # ans_point_Scatter(5, 0.8)
    ans_point_Scatter(7, 0.7)

    """
    # 固定節點與swap prob，跑average_time次後取平均的purify次數
    # avg_purify_time( average_node, th, swap_prob, average_time, [[8, 10], [11, 13], [14, 16]])
    avg_entangle_dis(average_node, th, swap_prob, [10, 15, 20, 25, 30], average_time)

    avg_purify_dis(average_node, th, swap_prob, [10, 15, 20, 25, 30], average_time)
    """

