
/*   hyperparameter   */
double threshold = 0.7;      // fidelity threshold
double fiber_distance = 250; // fiber的極限傳輸距離
int node = 20;               // 節點數量
int times = 15;              // 跑幾次
int k = 10;                  // dp存的數量
double beta; // 算fidelity - distance的beta參數，會根據fiber極限距離計算
int memory_low = 10, memory_up = 30; // memory數量random，從[low-up]
int coor_low = 10, coor_up = 300;    // 座標位置random，從[low-up]
