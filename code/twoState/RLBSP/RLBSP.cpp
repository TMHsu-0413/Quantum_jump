#include "RLBSP.h"
// #include "../../parameter.cpp"
#include "../formula.h"
#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
using namespace std;
using t = tuple<double, double, Node *>;

class RLBSP_Algo {
public:
  RLBSP_Algo(string name, double threshold, double beta) {
    this->filename = name;
    this->threshold = threshold;
    this->beta = beta;
  }
  void run() {
    auto start = chrono::high_resolution_clock::now();
    buildGraph(filename);
    // buildExampleGraph();
    // printGraph();
    Node *src = getNode(0, 0), *dst = getNode(-1, -1);
    Initial(dist, dist2, parent, used);
    bool havePath = dijkstra(src, dst, dist, parent);
    // array<double, 2> cost2_sp = dijkstra_on_cost2(dist2, src, dst);
    // 這張圖起點到終點沒連通
    if (!havePath) {
      cout << "can't arrive dst" << endl;
      return;
    }
    // cout << "Initial shortest path cost : " << dist[dst][0] << " " <<
    // dist[dst][1] << endl;
    vector<Node *> path;
    path.push_back(src);
    // dfs(used, src, dst, 0, 0, path);
    // write_to_txt(dfsAns, "output/allpoint.txt");
    RLBSP(dist, parent, src, dst);
    auto end = chrono::high_resolution_clock::now();
    auto diff = end - start;
    double time = chrono::duration<double>(diff).count();
    // cout << time << endl;
    write_path_info(time);
    write_to_txt(RLBSPAns, "output/RLBSPpoint.txt");
    //  cout << "\n\ncost2 shortest path cost : " << cost2_sp[1] << " " <<
    //  cost2_sp[0] << endl;
  }

protected:
  int n; // 總節點個數
  double threshold;
  double beta;
  double d1, d2;
  vector<Node *> Cpath;
  unordered_map<Node *, array<double, 2>> dist; // Node : {d1,d2}
  unordered_map<Node *, array<double, 2>> dist2;
  unordered_map<Node *, array<Node *, 2>> parent; // Node : {Cpred,pred}
  unordered_map<Node *, bool> used;               // Node : false
  vector<double> x, y;                            // 點的x,y座標
  vector<double> memory;                          // 單點的memory
  vector<double> swapping_prob; // 每個點做swapping的機率
  vector<double> dis;
  string filename;

  unordered_map<int, unordered_map<int, Node *>> nodeMap;

  Node *getNode(int ID, int mem) {
    if (nodeMap.find(ID) == nodeMap.end() ||
        nodeMap[ID].find(mem) == nodeMap[ID].end()) {
      nodeMap[ID][mem] = new Node(ID, mem);
    }
    return nodeMap[ID][mem];
  }

  double distance(int i, int j) {
    return sqrt(powf(x[i] - x[j], 2) + powf(y[i] - y[j], 2));
  }

  void printGraph() {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j <= memory[i]; j++) {
        if (nodeMap.find(i) == nodeMap.end() ||
            nodeMap[i].find(j) == nodeMap[i].end())
          continue;

        Node *cur = getNode(i, j);
        cout << endl;

        cout << "Node Info : ";
        cout << cur->ID << " " << cur->memory << endl;

        cout << "Neighbor : " << endl;
        for (auto &nxt : cur->neighbor) {
          // cout << nxt.fidelity << " " << nxt.prob << " " << nxt.node->ID << "
          // "
          cout << exp(-nxt.fidelity) << " " << exp(-nxt.prob) << " "
               << nxt.node->ID << " " << nxt.node->ID << nxt.node->memory
               << endl;
        }
        cout << "Parent : " << endl;
        for (auto &nxt : cur->parent) {
          cout << nxt.fidelity << " " << nxt.prob << " " << nxt.node->ID << " "
               << nxt.node->memory << endl;
        }
      }
    }

    Node *cur = getNode(-1, -1);
    cout << endl;

    cout << "Node Info : ";
    cout << cur->ID << " " << cur->memory << endl;

    cout << "Neighbor : " << endl;
    for (auto &nxt : cur->neighbor) {
      cout << nxt.fidelity << " " << nxt.prob << " " << nxt.node->ID << " "
           << " " << nxt.node->memory << endl;
    }
    cout << "Parent : " << endl;
    for (auto &nxt : cur->parent) {
      cout << nxt.fidelity << " " << nxt.prob << " " << nxt.node->ID << " "
           << nxt.node->memory << endl;
    }
  }

  void buildExampleGraph() {
    nodeMap[0][0] = new Node(0, 0);
    nodeMap[2][2] = new Node(2, 2);
    nodeMap[3][3] = new Node(3, 3);
    nodeMap[-1][-1] = new Node(-1, -1);

    nodeMap[0][0]->neighbor.push_back({0, 2, nodeMap[2][2]});
    nodeMap[2][2]->parent.push_back({0, 2, nodeMap[0][0]});

    nodeMap[2][2]->neighbor.push_back({1, 5, nodeMap[3][3]});
    nodeMap[3][3]->parent.push_back({1, 5, nodeMap[2][2]});

    nodeMap[3][3]->neighbor.push_back({1, 0, nodeMap[2][2]});
    nodeMap[2][2]->parent.push_back({1, 0, nodeMap[3][3]});

    nodeMap[3][3]->neighbor.push_back({0, 2, nodeMap[-1][-1]});
    nodeMap[-1][-1]->parent.push_back({0, 2, nodeMap[3][3]});

    nodeMap[0][0]->neighbor.push_back({4, 0, nodeMap[3][3]});
    nodeMap[3][3]->parent.push_back({4, 0, nodeMap[0][0]});

    nodeMap[2][2]->neighbor.push_back({3, 1, nodeMap[-1][-1]});
    nodeMap[-1][-1]->parent.push_back({3, 1, nodeMap[2][2]});
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
      cout << "Node " << i << " memory : " << memory[i]
           << " swappingSuccessProb : " << swapping_prob[i] << endl;
    }

    in.close();

    for (int i = 1; i < n; i++)
      dis[i] = dis[i - 1] + distance(i - 1, i);

    // from i to j
    for (int i = 0; i < n - 1; i++) {
      for (int j = i + 1; j < n; j++) {
        // 若 2 點距離超過 fiber distance，不建邊
        cout << "i :" << i << " j : " << j << " dis : " << dis[j] - dis[i]
             << endl;
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

  void Initial(unordered_map<Node *, array<double, 2>> &dist,
               unordered_map<Node *, array<double, 2>> &dist2,
               unordered_map<Node *, array<Node *, 2>> &parent,
               unordered_map<Node *, bool> &used) {
    for (auto &[k, v] : nodeMap) {
      for (auto &[k2, v2] : v) {
        dist[v2] = {DBL_MAX, DBL_MAX};
        dist2[v2] = {DBL_MAX, DBL_MAX};
        parent[v2] = {nullptr, nullptr};
        used[v2] = false;
      }
    }
  }

  void printPath(vector<Node *> &Cpath) {
    vector<int> memoryUsed;
    cout << "path : ";
    for (int i = 0; i < Cpath.size() - 1; i++) {
      cout << Cpath[i]->ID << " ";
      if (i != 0)
        memoryUsed.push_back(Cpath[i]->memory);
    }
    cout << endl << "mem  :  ";
    for (auto &c : memoryUsed)
      cout << c << " ";
    cout << endl;
  }

  vector<array<double, 2>> dfsAns, RLBSPAns;
  void dfs(unordered_map<Node *, bool> &used, Node *cur, Node *d, double c1,
           double c2, vector<Node *> &v) {
    if (cur == d) {
      // cout << "Fidelity : " << exp(-c1) << " " << "Probability : " <<
      // exp(-c2) << endl; printPath(v);
      //  dfsAns.push_back({exp(-c1), exp(-c2)});
      dfsAns.push_back({c1, c2});
      return;
    }
    for (auto &nxt : cur->neighbor) {
      Node *nxtNode = nxt.node;
      if (used[nxtNode])
        continue;
      used[nxtNode] = true;
      v.push_back(nxtNode);
      dfs(used, nxtNode, d, c1 + nxt.fidelity, c2 + nxt.prob, v);
      used[nxtNode] = false;
      v.pop_back();
    }
  }

  bool over_Threshold(double cost1) { return cost1 > -ln(threshold); }

  // 求Dijkstra找cost1的解
  bool dijkstra(Node *s, Node *d, unordered_map<Node *, array<double, 2>> &dist,
                unordered_map<Node *, array<Node *, 2>> &parent) {
    dist[s] = {0, 0};

    priority_queue<t, vector<t>, greater<t>> pq;
    pq.push({0, 0, s});
    while (!pq.empty()) {
      t temp = pq.top();
      pq.pop();
      double c1 = get<0>(temp), c2 = get<1>(temp);
      Node *cur = get<2>(temp);

      if (c1 != dist[cur][0] && c2 != dist[cur][1])
        continue;

      for (auto &nxt : cur->neighbor) {
        Node *nxtNode = nxt.node;
        if ((c1 + nxt.fidelity) <= dist[nxtNode][0]) {
          if (c1 + nxt.fidelity < dist[nxtNode][0]) {
            dist[nxtNode] = {c1 + nxt.fidelity, c2 + nxt.prob};
            parent[nxtNode] = {nullptr, cur};
            pq.push({dist[nxtNode][0], dist[nxtNode][1], nxtNode});
          } else if (c2 + nxt.prob < dist[nxtNode][1]) {
            dist[nxtNode] = {c1 + nxt.fidelity, c2 + nxt.prob};
            parent[nxtNode] = {nullptr, cur};
            pq.push({dist[nxtNode][0], dist[nxtNode][1], nxtNode});
          }
        }
      }
    }
    if (dist[d][0] != DBL_MAX)
      return true;
    return false;
  }

  // 求Dijkstra找cost2的解
  array<double, 2>
  dijkstra_on_cost2(unordered_map<Node *, array<double, 2>> &dist2, Node *s,
                    Node *d) {

    dist2[s] = {0, 0};

    priority_queue<t, vector<t>, greater<t>> pq;
    pq.push({0, 0, s});
    while (!pq.empty()) {
      t temp = pq.top();
      pq.pop();
      double c2 = get<0>(temp), c1 = get<1>(temp);
      Node *cur = get<2>(temp);

      if (cur == d) {
        return dist2[d];
      }

      if (c2 > dist2[cur][0])
        continue;

      for (auto &nxt : cur->neighbor) {
        Node *nxtNode = nxt.node;
        if ((c2 + nxt.prob) < dist2[nxtNode][0]) {
          dist2[nxtNode] = {c2 + nxt.prob, c1 + nxt.fidelity};
          // parent[nxtID] = {0, cur};
          pq.push({dist2[nxtNode][0], dist2[nxtNode][1], nxtNode});
        }
      }
    }
    return dist2[d];
  }

  void getPath(unordered_map<Node *, array<Node *, 2>> &parent,
               vector<Node *> &Cpath, Node *cur, Node *s) {
    if (cur == s) {
      Cpath.push_back(cur);
      return;
    }
    getPath(parent, Cpath, parent[cur][1], s);
    Cpath.push_back(cur);
    return;
  }

  void RLBSP(unordered_map<Node *, array<double, 2>> &dist,
             unordered_map<Node *, array<Node *, 2>> &parent, Node *s,
             Node *d) {
    unordered_map<Node *, double> theta;
    unordered_map<Node *, double> minDiff;
    double lastratio = 0;
    d1 = dist[d][0];
    d2 = dist[d][1];
    Cpath.clear();
    getPath(parent, Cpath, d, s);
    priority_queue<Label> pq;
    // Algo1中的4,5行
    for (auto &[id, m2] : nodeMap) {
      for (auto &[memory, cur] : m2) {
        theta[cur] = DBL_MAX;
        minDiff[cur] = DBL_MAX;
        double minTheta = DBL_MAX, minCost2 = DBL_MAX;
        for (auto &prev : cur->parent) {
          Node *prevNode = prev.node;
          double newCost1 = dist[prevNode][0] + prev.fidelity,
                 newCost2 = dist[prevNode][1] + prev.prob;

          if (over_Threshold(newCost1))
            continue;

          double diffCost1 = newCost1 - dist[cur][0],
                 diffCost2 = newCost2 - dist[cur][1];
          if (diffCost2 < 0) {
            double curTheta = -diffCost1 / diffCost2;
            if (curTheta == minTheta) {
              if (diffCost2 <= minCost2) {
                minCost2 = diffCost2;
                parent[cur][0] = prevNode;
              }
            } else if (curTheta < minTheta) {
              minTheta = curTheta;
              minCost2 = diffCost2;
              parent[cur][0] = prevNode;
            }
          }
        }
        if (parent[cur][0] != nullptr) {
          vector<Node *> tempPath;
          getPath(parent, tempPath, parent[cur][0], s);
          tempPath.push_back(cur);
          pq.push({minTheta, minCost2, cur, tempPath});
          // cout << "before pq push [" << minTheta << ", " << minCost2 << ", "
          // << cur->ID << ", " << cur->memory << endl; //
          // cout<<parent[cur][0]->ID<<"
          //  "<<parent[cur][0]->memory<<" "<<parent[cur][1]->ID<<"
          //  "<<parent[cur][1]->memory<<endl;
          theta[cur] = minTheta;
          minDiff[cur] = minCost2;
        }
      }
    }
    while (!pq.empty()) {
      // Label內改成紀錄目前這條path經過的點的資訊，不然會因為threshold超過導致更新有問題
      Label temp = pq.top();
      double slope = temp.slope, diffCost = temp.diffCost;
      Node *cur = temp.node;

      vector<Node *> curPath = temp.path;
      pq.pop();

      // lazy delete
      if (minDiff[cur] != diffCost) {
        continue;
      }

      // output part
      // cout << slope << " " << diffCost << " " << cur->ID << " " <<
      // cur->memory
      // << endl;

      // cout << "Cost d: " << dist[d][0] << " " << dist[d][1] << endl;
      /*
      cout << endl;
      cout << "pop : " << slope << " " << diffCost << " " << cur->ID << endl;
      for (auto &d : dist) {
        cout << "Dist: (" << d[0] << " " << d[1] << ") \t";
      }
      cout << endl;
      for (auto &d : parent) {
        cout << "Cpred, pred: (" << d[0] << "," << d[1] << ") \t";
      }
      cout << endl;
      for (auto &d : theta) {
        if (d == DBL_MAX)
          cout << "theta: inf \t";
        else
          cout << "theta: " << d << "\t";
      }
      cout << endl;
      */
      // cout << slope << " " << diffCost << " " << cur->ID << " " <<
      // cur->memory
      // << endl;

      minDiff[cur] = DBL_MAX;
      theta[cur] = DBL_MAX;
      parent[cur][1] = parent[cur][0];
      parent[cur][0] = nullptr;
      // cout << "pq pop : " << slope << " " << diffCost << " " << cur->ID << "
      // " << cur->memory << endl; // cout<<parent[cur][1]->ID<<"
      //  "<<parent[cur][1]->memory<<endl;
      dist[cur][0] = dist[cur][0] - (slope * diffCost);
      dist[cur][1] = dist[cur][1] + diffCost;

      if (cur == d) {
        if (slope >= lastratio) {
          cout << "\nfidelity : " << exp(-d1) << " prob : " << exp(-d2) << endl;
          RLBSPAns.push_back({d1, d2});
          // RLBSPAns.push_back({exp(-d1), exp(-d2)});
          printPath(Cpath);
          lastratio = slope;
        }
        Cpath.clear();
        Cpath = curPath;
        // getPath(parent, Cpath, d, s);
        d1 = dist[d][0];
        d2 = dist[d][1];
      }

      // 13行
      double minTheta = DBL_MAX, minCost2 = DBL_MAX;
      for (auto &prev : cur->parent) {
        Node *prevNode = prev.node;
        double newCost1 = dist[prevNode][0] + prev.fidelity,
               newCost2 = dist[prevNode][1] + prev.prob;

        if (over_Threshold(newCost1))
          continue;

        double diffCost1 = newCost1 - dist[cur][0],
               diffCost2 = newCost2 - dist[cur][1];

        if (diffCost2 < 0) {
          if (diffCost1 < 0)
            continue;
          double curTheta = -diffCost1 / diffCost2;
          if (curTheta == minTheta) {
            if (diffCost2 < minCost2) {
              minCost2 = diffCost2;
              parent[cur][0] = prevNode;
            }
          } else if (curTheta < minTheta) {
            minTheta = curTheta;
            minCost2 = diffCost2;
            parent[cur][0] = prevNode;
          }
        }
      }
      if (parent[cur][0] != nullptr) {
        vector<Node *> tempPath;
        getPath(parent, tempPath, parent[cur][0], s);
        tempPath.push_back(cur);
        pq.push({minTheta, minCost2, cur, tempPath});
        // cout << "pq push [" << minTheta << ", " << minCost2 << ", " <<
        // cur->ID
        // << ", " << cur->memory << endl; cout << "pq push parent: " <<
        // minTheta
        // << " " << minCost2 << " " << cur->ID << " " << cur->memory << endl;
        // cout << "Push 13: [" << minTheta
        // << " ," << minCost2 << " ," << cur
        // << endl;
        theta[cur] = minTheta;
        minDiff[cur] = minCost2;
      }

      // 15行
      vector<Node *> tempPath = curPath;
      for (auto &nxt : cur->neighbor) {
        Node *nxtNode = nxt.node;
        double newCost1 = dist[cur][0] + nxt.fidelity,
               newCost2 = dist[cur][1] + nxt.prob;
        if (over_Threshold(newCost1))
          continue;

        double diffCost1 = newCost1 - dist[nxtNode][0],
               diffCost2 = newCost2 - dist[nxtNode][1];

        double curTheta = -diffCost1 / diffCost2;
        if (diffCost2 < 0) {
          if ((curTheta < theta[nxtNode]) || ((curTheta == theta[nxtNode]) &&
                                              (diffCost2 < minDiff[nxtNode]))) {
            tempPath.push_back(nxtNode);
            pq.push({curTheta, diffCost2, nxtNode, tempPath});
            tempPath.pop_back();
            // cout << "pq push [" << curTheta << ", " << diffCost2 << ", " <<
            // nxtNode->ID << ", " << nxtNode->memory << endl;
            minDiff[nxtNode] = diffCost2;
            theta[nxtNode] = curTheta;
            parent[nxtNode][0] = cur;
          }
        }
      }
    }
    cout << "\nfidelity : " << exp(-d1) << " prob : " << exp(-d2) << endl;
    // RLBSPAns.push_back({exp(-d1), exp(-d2)});
    RLBSPAns.push_back({d1, d2});
    printPath(Cpath);
  }

  void write_path_info(double time) {
    ofstream ofs;
    ofs.open("output/RLBSP.txt");
    if (!ofs.is_open()) {
      cout << "error to open output.txt" << endl;
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
    for (auto &c : memoryUsed)
      ofs << c - 1 << " ";
    ofs << endl;
    ofs << "Fidelity: " << exp(-d1) << endl;
    ofs << "Probability: " << exp(-d2) << endl;
    ofs << "Time: " << time << endl;
  }

  // 寫入txt，印圖用
  void write_to_txt(vector<array<double, 2>> &v, string name) {
    ofstream ofs;
    ofs.open(name);
    if (!ofs.is_open()) {
      cout << "error to open output.txt" << endl;
      return;
    }
    for (auto &e : v)
      ofs << e[0] << " " << e[1] << endl;
  }
};

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "g++ <input.txt> is need";
    return 1;
  }
  double beta = 0.00438471;
  RLBSP_Algo rlbsp(argv[1], atof(argv[2]), beta);
  rlbsp.run();
}
