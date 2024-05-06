# 跳關的code

- [題目及做法詳細說明，以2 state的部分為主](https://hackmd.io/w4bodB2OSc-Kn9Ftaamljw)
- [24 Globecom version](https://drive.google.com/file/d/1FfgEusEe0hHawQXyZ6NdW-Y25-6X_98x/view?usp=sharing)

## File Structure
```bash
Quantum_jump
├── code
│   ├── weinerState # 不同quantum state的版本，沒用到
│   ├── Algorithm   # 2015 dijkstra跳關的版本code
│   └── twoState
│       ├── RLBSP     # 改成paper版本的2015 dijkstra跳關版本，但有bug
│       ├── RSP       # Restricted shortest path code
│       ├── qLeap     # 比較方法1
│       ├── qPath     # 比較方法2
│       └── plot      # 改過後的繪製實驗圖code
└── AOS
    ├── ans  # 放在paper內的實驗圖
    └── plot # 沒改前的繪製實驗圖code
```

## 執行步驟

- 進入資料夾

```bash=
cd quantum_jump/code/twoState
```

- 一次產出輸入數字節點的資料，可帶多個，固定memory 5-9，swapping prob 0.7，threshold 0.8
- 只有第一個參數會執行所有的function，後續的都只會執行print_diffNode_prob function

```bash=
python script.py {number of nodes} {number of nodes} ...
```

- 會輸出所有的答案在outputTxt/內，並將\*.ans檔移到目錄的AOS/ans內
- 執行AOS/plot/\*.py，目前只有放在paper上的圖在裡面
- 散點圖在graph.py內的multiColor，需先執行script.py內的ans_plot_scatter function才可執行
