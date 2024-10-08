from cmath import log10
import numpy as np
import math
import os
import matplotlib.pyplot as plt
import matplotlib.transforms

# import latex
import matplotlib
from matplotlib.offsetbox import AnchoredOffsetbox, TextArea, HPacker, VPacker

import sys

directory_path = "plot/image/"
directory_path2 = "plot/image/"


# matplotlib.rcParams['text.usetex'] = True
class ChartGenerator:
    # data檔名 Y軸名稱 X軸名稱 Y軸要除多少(10的多少次方) Y軸起始座標 Y軸終止座標 Y軸座標間的間隔
    def __init__(self, dataName, Ylabel, Xlabel):
        filename = dataName
        print("start generate", filename)

        if not os.path.exists(filename):
            print("file doesn't exist")
            return

        with open(filename, "r", encoding="utf-8") as f:
            lines = f.readlines()

        # Ydiv, Ystart, Yend, Yinterval
        Ypow = 0
        Xpow = 0

        # color = [
        #     "#000000",
        #     "#006400",
        #     "#FF1493",
        #     "#7FFF00",
        #     "#900321",
        # ]
        color = [
            "#FF0000",
            "#00FF00",
            "#0000FF",
            "#000000",
            "#7E2F8E",
        ]
        # matplotlib.rcParams['text.usetex'] = True

        fontsize = 32
        Xlabel_fontsize = fontsize
        Ylabel_fontsize = fontsize
        Xticks_fontsize = fontsize
        Yticks_fontsize = fontsize

        # matplotlib.rcParams['text.usetex'] = True
        # fig, ax = plt.subplots(figsize=(8, 6), dpi=600)

        andy_theme = {
            # "axes.grid": True,
            # "grid.linestyle": "--",
            # "legend.framealpha": 1,
            # "legend.facecolor": "white",
            # "legend.shadow": True,
            # "legend.fontsize": 14,
            # "legend.title_fontsize": 16,
            "xtick.labelsize": 20,
            "ytick.labelsize": 20,
            "axes.labelsize": 20,
            "axes.titlesize": 20,
            "mathtext.fontset": "custom",
            "font.family": "Times New Roman",
            "mathtext.default": "default",
            "mathtext.it": "Times New Roman:italic",
            "mathtext.cal": "Times New Roman:italic",
            # "mathtext.default": "regular",
            # "mathtext.fontset": "custom"
            # "figure.autolayout": True
            # "text.usetex": True,
            # "figure.dpi": 100,
        }

        matplotlib.rcParams.update(andy_theme)
        matplotlib.rcParams["pdf.fonttype"] = 42
        matplotlib.rcParams["ps.fonttype"] = 42
        fig, ax1 = plt.subplots(figsize=(7, 6), dpi=600)
        # ax1.spines['top'].set_linewidth(1.5)
        # ax1.spines['right'].set_linewidth(1.5)
        # ax1.spines['bottom'].set_linewidth(1.5)
        # ax1.spines['left'].set_linewidth(1.5)
        ax1.tick_params(direction="in")
        ax1.tick_params(bottom=True, top=True, left=True, right=True)
        ax1.tick_params(pad=20)

        ##data start##
        x = []
        x_data = []
        _y = []
        numOfData = 0
        counter = 0
        for line in lines:
            line = line.replace("\n", "")
            data = line.split(" ")
            numOfline = len(data)
            numOfData += 1
            for i in range(numOfline):
                if i == 0:
                    x.append(data[i])
                    x_data.append(counter)
                elif i != numOfline - 1:
                    _y.append(data[i])
            # print(_y)
            counter += 1
        numOfAlgo = len(_y) // numOfData

        y = [[] for _ in range(numOfAlgo)]
        for i in range(numOfData * numOfAlgo):
            y[i % numOfAlgo].append(_y[i])

        # print(x)
        # print(y)

        maxData = 0
        minData = math.inf

        for i in range(-10, -1, 1):
            if float(x[numOfData - 1]) <= 10**i:
                Xpow = i - 3

        Ydiv = float(10**Ypow)
        Xdiv = float(10**Xpow)

        for i in range(numOfData):
            x[i] = float(x[i]) / Xdiv

        for i in range(numOfAlgo):
            for j in range(numOfData):
                y[i][j] = float(y[i][j]) / Ydiv
                maxData = max(maxData, y[i][j])
                minData = min(minData, y[i][j])

        Yend = math.ceil(maxData) + 2
        Ystart = math.ceil(minData) - 2
        Yinterval = (Yend - Ystart) / 5

        if maxData > 1.1:
            Yinterval = Yinterval
            Yend = Yend
        else:
            Yend = 0.15
            Ystart = 0
            Yinterval = 0.03

        marker = ["o", "s", "v", "x"]
        linestyle = ["-", "--", ":"]
        for i in range(numOfAlgo):
            ax1.plot(
                x_data,
                y[i],
                color=color[i],
                lw=2.5,
                linestyle=linestyle[i],
                marker=marker[i],
                markersize=15,
                markerfacecolor="none",
                markeredgewidth=2.5,
                zorder=i,
            )
        # plt.show()

        plt.xticks(fontsize=Xticks_fontsize)
        plt.yticks(fontsize=Yticks_fontsize)

        AlgoName = ["NSPS", "Q-PATH", "Q-LEAP"]

        # AlgoName.reverse()
        leg = plt.legend(
            AlgoName,
            loc=10,
            bbox_to_anchor=(0.4, 1.25),
            prop={"size": fontsize, "family": "Times New Roman"},
            frameon="False",
            labelspacing=0.2,
            handletextpad=0.2,
            handlelength=1,
            columnspacing=0.2,
            ncol=3,
            facecolor="None",
        )

        leg.get_frame().set_linewidth(0.0)
        Ylabel += self.genMultiName(Ypow)
        Xlabel += self.genMultiName(Xpow)
        plt.subplots_adjust(top=0.75)
        plt.subplots_adjust(left=0.3)
        plt.subplots_adjust(right=0.95)
        plt.subplots_adjust(bottom=0.25)

        plt.yticks(
            np.arange(Ystart, Yend + Yinterval, step=Yinterval),
            fontsize=Yticks_fontsize,
        )
        plt.xticks(x_data, x)
        plt.ylabel(Ylabel, fontsize=Ylabel_fontsize, labelpad=35)
        plt.xlabel(Xlabel, fontsize=Xlabel_fontsize, labelpad=10)
        ax1.yaxis.set_label_coords(-0.3, 0.5)
        ax1.xaxis.set_label_coords(0.45, -0.27)
        # plt.show()
        # plt.tight_layout()
        pdfName = dataName.split("/")[1][0:-4]
        plt.savefig(directory_path + f"{pdfName}.eps", format="eps", dpi=1000)
        plt.savefig(directory_path2 + f"{pdfName}.jpg", dpi=1000)
        # Xlabel = Xlabel.replace(' (%)','')
        # Xlabel = Xlabel.replace('# ','')
        # Ylabel = Ylabel.replace('# ','')
        plt.close()

    def genMultiName(self, multiple):
        if multiple == 0:
            return str()
        else:
            return "($" + "10" + "^{" + str(multiple) + "}" + "$)"


if __name__ == "__main__":
    # data檔名 Y軸名稱 X軸名稱 Y軸要除多少(10的多少次方) Y軸起始座標 Y軸終止座標 Y軸座標間的間隔
    # ChartGenerator("numOfnodes_waitingTime.txt", "need ,#round", "#Request of a round", 0, 0, 25, 5)
    Xlabels = ["Swap. Probability"]
    # $\gamma \ (10^{-4})$
    # Average $Q(v)$
    # Resource Ratio
    # Average Demand Limit
    # \# SD Pairs
    # \# Nodes
    Ylabels = ["Avg. Probability"]
    # Throughput
    # Max Supply-Demand Ratio
    # Resource Efficiency of Channel
    # Resource Efficiency of Memory
    # print(latex.__path__)
    for Xlabel in Xlabels:
        for Ylabel in Ylabels:
            dataFileName = "prob_threshold.ans"
            ChartGenerator(dataFileName, Ylabel, Xlabel)

    # Xlabel = "Timeslot"
    # Ylabel = "#remainRequest"
    # dataFileName = Xlabel + "_" + Ylabel + ".ans"
    # ChartGenerator(dataFileName, Ylabel, Xlabel)
