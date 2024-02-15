# 跳關的code

## code

### WeinerState

- SinglePath.cpp / parameter.cpp / formula.cpp 主要的跳關程式及設定
- 剩下的都是測試用的程式碼

- 給一個Linked List的圖，找開頭到尾巴滿足threshold的fidelity的所有path，包含經過的節點編號、memory消耗量、成功機率，並把東西輸出到output.txt
- 點與點之間的距離可以設定，在SinglePath.cpp內的init_path function內
- threshold也可以設定，在parameter.cpp內

### TwoState

- SinglePath.cpp / formula.cpp 主要的跳關程式與設定

- 先建一張圖，不同purify次數分別代表一條edge，在這張圖上找一個S到T的shortest path(separation oracle)，但複雜度應該很高

## plot

- graph.py 透過SinglePath內的draw_to_txt，用output.txt繪製散點圖
