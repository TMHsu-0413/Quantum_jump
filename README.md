# 跳關的code

## code

### WeinerState

- SinglePath.cpp / parameter.cpp / formula.cpp 主要的跳關程式及設定
- 剩下的都是測試用的程式碼

- 給一個Linked List的圖，找開頭到尾巴滿足threshold的fidelity的所有path，包含經過的節點編號、memory消耗量、成功機率，並把東西輸出到output.txt
- 點與點之間的距離可以設定，在SinglePath.cpp內的init_path function內
- threshold也可以設定，在parameter.cpp內

### TwoState

- SinglePath.cpp / formula.cpp 主要的跳關程式與設定，使用RDBSP的方法來找到convex hull

- 先建一張圖，不同purify次數分別代表一條edge，在這張圖上找一個S到T的shortest path(separation oracle)，但複雜度應該很高

- main.py，用來產生隨機點座標、memory、swapping成功機率，並存在graph.txt內

### Algorithm

- [這篇](https://www.sciencedirect.com/science/article/pii/S0305054814003141)的方法實作，但有多了超過threshold的限制判斷

## plot

- graph.py 透過SinglePath內的write_to_txt，用allpoint.txt與RLBSPpoing.txt繪製散點圖
