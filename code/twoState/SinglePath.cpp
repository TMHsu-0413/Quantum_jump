#include "../parameter.cpp"
#include "formula.cpp"
#include <bits/stdc++.h>
using namespace std;
using tNi = tuple<Node *, int>;

vector<int> memory;           // 單點的memory
vector<double> swapping_prob; // 每個點做swapping的機率
vector<double> dis;           // 距離直接隨機給，用presum來取
unordered_map<int, unordered_map<int, unordered_map<int, Node *>>> nodeMap;
int idx = -1;
int virtual_src = -200;
int virtual_dst = -201;

void init_path(int node) {
  memory.clear();
  swapping_prob.clear();
  dis.push_back(0);
  for (int i = 0; i < node; i++) {
    // int mem = rand() % (memory_up - memory_low + 1) + memory_low;
    // memory.push_back(mem);
    memory.push_back(5);

    double swapping_probaility = swapping_success_prob();
    swapping_prob.push_back(swapping_probaility);
    // nodeMap[i] = new Node(i);

    // double distance = (dist_up - dist_low) * rand() / RAND_MAX + dist_low;
    double distance = 30;
    dis.push_back(dis.back() + distance);
  }
}

Node *getNode(int from, int to, int mem) {
  if (nodeMap.find(from) == nodeMap.end() ||
      nodeMap[from].find(to) == nodeMap[from].end() ||
      nodeMap[from][to].find(mem) == nodeMap[from][to].end()) {
    nodeMap[from][to][mem] = new Node(from, to, mem);
  }
  return nodeMap[from][to][mem];
}

void printPath(unordered_map<Node *, Node *> &prev, Node *curNode, int &src) {
  map<int, int> m;
  while (curNode->from != src) {
    if (curNode->from != -1 && curNode->to != -1) {
      m[curNode->from] += curNode->memory;
      m[curNode->to] += curNode->memory;
    }
    curNode = prev[curNode];
  }
  cout << "Path : ";
  for (auto &[k, v] : m)
    cout << k << " ";
  cout << endl << "Memory : ";
  for (auto &[k, v] : m)
    cout << v << " ";
  cout << endl;
}

void printGraph() {
  for (auto &[from, d1] : nodeMap) {
    for (auto &[to, d2] : d1) {
      for (auto &[mem, node] : d2) {
        cout << "Current Node " << endl;
        cout << "from : " << node->from << " to : " << node->to << " "
             << " memory : " << node->memory << endl;
        if (node->neighbors.size() > 0) {
          for (auto &neig : node->neighbors) {
            cout << "Neighbors ";
            cout << "from : " << neig.node->from << " to : " << neig.node->to
                 << " "
                 << " memory : " << neig.node->memory
                 << " fidelity : " << neig.fidelity << " prob : " << neig.prob
                 << endl;
          }
        } else {
          cout << "No neighbors " << endl;
        }
        cout << endl;
      }
    }
  }
}

void buildGraph(int node, int src, int dst) {
  for (int i = 1; i <= memory[src]; i++) {
    Node *s = getNode(virtual_src, -1, 0);
    Node *d = getNode(src, -1, i);
    s->neighbors.push_back(pathInfo(1, 1, d));
  }
  for (int i = 0; i < node - 1; i++) {
    for (int j = i + 1; j < node; j++) {
      // 若2點距離超過fiber distance，不建邊
      if ((dis[j] - dis[i]) > fiber_distance)
        break;
      // 建一條的Fid與Prob
      double entangleFidelity = entangle_fidelity(dis[j] - dis[i], beta);
      double entangleProb = entangle_success_prob(dis[j] - dis[i]);

      double curFidelity = entangleFidelity, curProb = entangleProb;
      for (int iMemory = 1; iMemory <= memory[i]; iMemory++) {
        Node *s = getNode(i, -1, iMemory);
        Node *middleNode = getNode(i, j, iMemory);
        s->neighbors.push_back(pathInfo(curFidelity, curProb, middleNode));

        // 終點只有一個，沒分多個點
        if (j == dst) {
          if (iMemory <= memory[dst]) {
            Node *d = getNode(j, -1, 0);
            middleNode->neighbors.push_back(pathInfo(1, 1, d));
          }
        }
        // 非終點會分多個點，逐一拜訪
        else {
          for (int jMemory = 1; jMemory <= memory[j]; jMemory++) {
            // 若左邊的路用的memory +
            // 下一條路用的memory超過節點i的memory，則跳出
            if (iMemory + jMemory > memory[i])
              break;
            Node *d = getNode(j, -1, jMemory);

            middleNode->neighbors.push_back(pathInfo(1, 1, d));
          }
        }
        curProb = curProb * entangleProb *
                  purify_success_prob(curFidelity, entangleFidelity);
        curFidelity = purify_fidelity(curFidelity, entangleFidelity);
      }
    }
  }
}
void dijkstra(int src, int dst) {
  priority_queue<pathInfo> pq; // {prob,fid,cur,used_memory}
  unordered_map<Node *, bool> used;
  unordered_map<Node *, Node *> prev;
  // vector<unordered_map<int, array<double, 2>>> prev(node);
  pq.push(pathInfo(1, 1, getNode(src, -1, 0)));

  while (!pq.empty()) {
    pathInfo path = pq.top();

    double prob = path.prob, fidelity = path.fidelity;
    Node *curNode = path.node;
    pq.pop();

    if (fidelity < threshold)
      continue;

    // find an answer
    if (curNode->from == dst) {
      printPath(prev, curNode, src);
      cout << "Prob : " << prob << " Fidelity : " << fidelity << endl;
      return;
    }
    if (used.find(curNode) != used.end())
      continue;
    used[curNode] = true;
    for (auto &nxt : curNode->neighbors) {
      Node *nextNode = nxt.node;
      double edgeFidelity = nxt.fidelity, edgeProb = nxt.prob;
      // nxt -> {next_node,purify_time,fidelity,probability}

      // 1. check nextNode and memory is visited
      if (used.find(nextNode) != used.end())
        continue;

      double nextFidelity = fidelity * edgeFidelity;
      double nextProb = prob * edgeProb;

      // 若經過非起點 終點 中繼點以外的點，就代表要做swapping
      if (curNode->from != src && curNode->from != dst && curNode->from >= 0 &&
          curNode->to == -1) {
        nextFidelity *= swapping_fidelity(fidelity, edgeFidelity);
        nextProb *= swapping_prob[curNode->from];
      }

      //  check fidelity is larger than threshold
      if (nextFidelity < threshold)
        continue;
      pq.push(pathInfo(nextFidelity, nextProb, nextNode));
      //  最後要找出path，所以標記上一跳位置
      prev[nextNode] = curNode;
    }
  }
}

int main() {
  srand(time(NULL));
  for (int _ = 0; _ < times; _++) {
    init_path(node);
    buildGraph(node, 0, node - 1);
    // printGraph();
    dijkstra(virtual_src, node - 1);
  }
}
