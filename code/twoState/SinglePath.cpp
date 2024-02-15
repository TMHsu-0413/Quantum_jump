#include "../parameter.cpp"
#include "formula.cpp"
#include <bits/stdc++.h>
using namespace std;

vector<int> memory;           // 單點的memory
vector<double> swapping_prob; // 每個點做swapping的機率
vector<double> dis;           // 距離直接隨機給，用presum來取

unordered_map<int, vector<array<double, 3>>> g;
void buildGraph() {
  g.clear();
  memory.clear();
  swapping_prob.clear();
  dis.clear();
  dis.push_back(0);
}
int main() {
  srand(time(NULL));
  double time = 0;
  for (int i = 0; i < times; i++) {
    buildGraph();
  }
}
