import subprocess
import sys
from collections import defaultdict
from plot import graph


def compile_and_run(input_file, threshold):
    try:
        subprocess.run(["g++", "--std=c++17", "qPath/qPath.cpp", "-o", "output/qPath"])
        subprocess.run(["g++", "--std=c++17", "qLeap/qLeap.cpp", "-o", "output/qLeap"])
        subprocess.run(["g++", "--std=c++17", "RLBSP/RLBSP.cpp", "-o", "output/RLBSP"])
        subprocess.run(["./output/qPath", input_file, str(threshold)])
        subprocess.run(["./output/qLeap", input_file, str(threshold)])
        subprocess.run(["./output/RLBSP", input_file, str(threshold)])

    except Exception as e:
        print(f"Error occurred: {e}")


def readResult(arr, node, name):
    with open("output/" + name + ".txt", "r") as file:
        for line in file:
            if line[:5] == "error":
                arr["Fidelity"][node].append(0)
                arr["Probability"][node].append(0)
                arr["Path"][node].append([-1])
                arr["PurTimes"][node].append([-1])
            else:
                key = line.split(":")[0].strip()
                val = line.split(":")[1].strip()
                if key != "Path" and key != "PurTimes":
                    val = float(val)
                arr[key][node].append(val)


if __name__ == "__main__":
    import sys

    if len(sys.argv) < 2:
        print("Usage: python3 script.py <number of node> <number of node> ...")
        sys.exit(1)

    node_num = []
    qPath = defaultdict(lambda: defaultdict(list))
    qLeap = defaultdict(lambda: defaultdict(list))
    rLbsp = defaultdict(lambda: defaultdict(list))
    for i in range(1, len(sys.argv)):
        number_of_nodes = sys.argv[i]

        subprocess.run(
            [
                "python3",
                "main.py",
                "graph.txt",
                number_of_nodes,
                "10",
                "14",
                "0.4",
                "0.85",
                "1",
            ]
        )
        number_of_nodes = int(number_of_nodes)
        node_num.append(number_of_nodes)

        threshold = [0.7, 0.75, 0.8, 0.85, 0.9]
        for th in threshold:
            compile_and_run("graph.txt", th)

            readResult(qPath, number_of_nodes, "qPath")
            readResult(qLeap, number_of_nodes, "qLeap")
            readResult(rLbsp, number_of_nodes, "RLBSP")

        graph.different_threshold(
            [
                list(rLbsp["Probability"][number_of_nodes]),
                list(qPath["Probability"][number_of_nodes]),
                list(qLeap["Probability"][number_of_nodes]),
            ],
            threshold,
            number_of_nodes,
        )
        graph.execution_time_on_different_threshold(
            [
                rLbsp["Time"][number_of_nodes],
                qPath["Time"][number_of_nodes],
                qLeap["Time"][number_of_nodes],
            ],
            number_of_nodes,
            threshold,
        )

    rLbspTime = []
    qPathTime = []
    qLeapTime = []
    for el in node_num:
        rLbspTime.append(rLbsp["Time"][el][0])
        qPathTime.append(qPath["Time"][el][0])
        qLeapTime.append(qLeap["Time"][el][0])
    graph.execution_time_on_different_node([rLbspTime, qPathTime, qLeapTime], node_num)
