#include "RSP.h"
// #include "../../parameter.cpp"
#include "../formula.h"
#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
using namespace std;
using t = tuple<double, double, Node *>;

class RSP_Algo {
public:
  RSP_Algo(string name, double threshold, double eps, double beta) {
    this->filename = name;
    this->threshold = threshold;
    this->beta = beta;
    this->eps = eps;
  }
  void run() {
    auto start = chrono::high_resolution_clock::now();
    buildGraph(filename);
    Node *src = getNode(0, 0), *dst = getNode(-1, -1);

    RSP(src, dst, 1, node_number, eps);
    auto end = chrono::high_resolution_clock::now();
    auto diff = end - start;
    double time = chrono::duration<double>(diff).count();
    write_path_info(time);
  }

protected:
  int n;               // 總節點個數
  int node_number = 0; // 轉圖後的節點個數
  double threshold;
  double beta;
  double eps;
  vector<double> x, y;          // 點的x,y座標
  vector<double> memory;        // 單點的memory
  vector<double> swapping_prob; // 每個點做swapping的機率
  vector<double> dis;
  string filename;

  vector<Node *> Cpath;
  double d1 = DBL_MAX, d2 = DBL_MAX;
  unordered_map<int, unordered_map<int, Node *>> nodeMap;

  Node *getNode(int ID, int mem) {
    if (nodeMap.find(ID) == nodeMap.end() ||
        nodeMap[ID].find(mem) == nodeMap[ID].end()) {
      nodeMap[ID][mem] = new Node(ID, mem);
      node_number++;
    }
    return nodeMap[ID][mem];
  }

  double distance(int i, int j) {
    return sqrt(powf(x[i] - x[j], 2) + powf(y[i] - y[j], 2));
  }

  void buildGraph(string name) {
    ifstream in;
    in.open(name);
    in >> n;
    x.resize(n);
    y.resize(n);
    dis.resize(n, 0);
    memory.resize(n);
    swapping_prob.resize(n);

    for (int i = 0; i < n; i++) {
      in >> x[i] >> y[i] >> memory[i] >> swapping_prob[i];
      // cout << "Node " << i << " memory : " << memory[i] << "
      // swappingSuccessProb : " << swapping_prob[i] << endl;
    }

    in.close();

    for (int i = 1; i < n; i++)
      dis[i] = dis[i - 1] + distance(i - 1, i);

    // from i to j
    for (int i = 0; i < n - 1; i++) {
      for (int j = i + 1; j < n; j++) {
        // 若 2 點距離超過 fiber distance，不建邊
        // cout << "i :" << i << " j : " << j << " dis : " << dis[j] - dis[i] <<
        // endl;
        // if ((dis[j] - dis[i]) > fiber_distance) break;

        // iMemory, iM: memory node i already used
        // jMemory: memory node j already used
        for (int iMemory = 1; iMemory <= memory[i]; iMemory++) {
          int iM = (i == 0) ? 0 : iMemory;
          // 代表點0,0已經建過了
          if (iM == 0 && iMemory != 1)
            break;
          Node *s = getNode(i, iM);

          // prob to create the edge
          double entangleProb = entangle_success_prob((dis[j] - dis[i]));
          double curProb = entangleProb;
          double entangleFidelity = entangle_fidelity((dis[j] - dis[i]), beta);
          double curFidelity = entangleFidelity;

          for (int jMemory = 1; jMemory <= memory[j]; jMemory++) {
            // iMemory + jMemory = the memory node i used after create the edge
            if (iM + jMemory > memory[i])
              break;

            Node *d = getNode(j, jMemory);
            // j 是終點
            if (j == (n - 1)) {
              s->neighbor.push_back(path(-ln(curFidelity), -ln(curProb), d));
              d->parent.push_back(path(-ln(curFidelity), -ln(curProb), s));
            } else {
              s->neighbor.push_back(
                  path(-ln(curFidelity), -ln(curProb * swapping_prob[j]), d));
              d->parent.push_back(
                  path(-ln(curFidelity), -ln(curProb * swapping_prob[j]), s));
              // s->neighbor.push_back(path(-ln(curFidelity), -ln(curProb *
              // swapping_prob[j]), d));
              // d->parent.push_back(path(-ln(curFidelity), -ln(curProb *
              // swapping_prob[j]), s));
            }

            curProb = curProb * entangleProb *
                      purify_success_prob(curFidelity, entangleFidelity);
            curFidelity = purify_fidelity(curFidelity, entangleFidelity);
          }
        }
      }
    }

    for (int mem = 1; mem <= memory[n - 1]; mem++) {
      Node *s = getNode(n - 1, mem);
      Node *virtual_dst = getNode(-1, -1);

      s->neighbor.push_back(path(-ln(1.0), -ln(1.0), virtual_dst));
      virtual_dst->parent.push_back(path(-ln(1.0), -ln(1.0), s));
    }
  }

  bool over_Threshold(double cost1) { return cost1 > -ln(threshold); }

  void getPath(
      unordered_map<
          Node *, unordered_map<double, tuple<Node *, double, double>>> &parent,
      Node *cur, double cost, Node *s, vector<Node *> &Cpath, double &res) {

    if (cur == s) {
      Cpath.push_back(cur);
      return;
    }

    getPath(parent, get<0>(parent[cur][cost]), get<1>(parent[cur][cost]), s,
            Cpath, res);
    res += get<2>(parent[cur][cost]);
    Cpath.push_back(cur);
    return;
  }

  void printPath(vector<Node *> &Cpath, double cost, double delay) {
    // cout << "fidelity: " << delay << " prob: " << cost << endl;
    cout << "fidelity: " << exp(-delay) << " prob: " << exp(-cost) << endl;
    vector<int> memoryUsed;
    cout << "path : ";
    for (int i = 0; i < Cpath.size() - 1; i++) {
      cout << Cpath[i]->ID << " ";
      if (i != 0)
        memoryUsed.push_back(Cpath[i]->memory);
    }
    cout << endl << "mem  :  ";
    for (auto &c : memoryUsed) {
      cout << c << " ";
    }
    cout << endl;
  }

  void write_path_info(double time) {
    ofstream ofs;
    ofs.open("output/RSP.txt");
    if (!ofs.is_open()) {
      cout << "error to open output.txt" << endl;
      return;
    }
    if (d1 > -ln(threshold)) {
      ofs << "error: no path" << endl;
      ofs << "Time: " << time << endl;
      return;
    }

    vector<int> memoryUsed;
    ofs << "Path: ";
    for (int i = 0; i < Cpath.size() - 1; i++) {
      ofs << Cpath[i]->ID << " ";
      if (i != 0)
        memoryUsed.push_back(Cpath[i]->memory);
    }
    ofs << endl << "PurTimes: ";
    for (auto c : memoryUsed)
      ofs << c - 1 << " ";
    ofs << endl;
    ofs << "Fidelity: " << exp(-d1) << endl;
    ofs << "Probability: " << exp(-d2) << endl;
    ofs << "Time: " << time << endl;
  }

  bool SPPP(Node *s, Node *d, double L, double U, double eps) {
    double S = (L * eps) / (n + 1);
    double U_bar = floor(U / S) + n + 1;
    unordered_map<Node *, unordered_map<int, double>> dp;
    unordered_map<Node *, unordered_map<double, tuple<Node *, double, double>>>
        parent;

    for (auto &[ID, memMap] : nodeMap) {
      for (auto &[mem, v] : memMap) {
        if (v != s)
          dp[v][0] = DBL_MAX;
      }
    }

    dp[s][0] = 0;

    for (int i = 1; i <= U_bar; i++) {
      for (auto &[ID, memMap] : nodeMap) {
        for (auto &[mem, v] : memMap) {
          dp[v][i] = dp[v][i - 1];
          for (auto &edge : v->parent) {
            double delay = edge.fidelity, cost = edge.prob;
            Node *p = edge.node;

            double rounded_cost = floor(cost / S) + 1;
            if (rounded_cost > i)
              continue;
            if ((dp[p][i - rounded_cost] + delay) < dp[v][i]) {
              dp[v][i] = min(dp[v][i], dp[p][i - rounded_cost] + delay);
              parent[v][i] = {p, i - rounded_cost, cost};
            }
          }
        }
      }

      if (dp[d][i] <= -ln(threshold)) {
        vector<Node *> path;
        double cost = 0;
        getPath(parent, d, i, s, path, cost);
        printPath(path, cost, dp[d][i]);
        Cpath = path;
        d1 = dp[d][i];
        d2 = cost;
        return true;
      }
    }
    return false;
  }

  void RSP(Node *s, Node *d, double LB, double UB, double eps) {
    double B_L = LB, B_U = ceil(UB / 2);
    while ((B_U / B_L) > 2) {
      double B = powf((B_L * B_U), 0.5);

      if (!SPPP(s, d, B, B, 1))
        B_L = B;
      else
        B_U = B;
    }
    SPPP(s, d, B_L, 2 * B_U, eps);
  }
};

int main(int argc, char *argv[]) {
  if (argc != 4) {
    cout << "./a.out <input.txt> <threshold> <eps> is need";
    return 1;
  }
  double beta = 0.00438471;
  RSP_Algo rsp(argv[1], atof(argv[2]), atof(argv[3]), beta);
  rsp.run();
}
