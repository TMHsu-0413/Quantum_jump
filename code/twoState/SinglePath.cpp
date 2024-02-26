#include "../parameter.cpp"
#include "formula.cpp"
#include <bits/stdc++.h>
using namespace std;
using tNi = tuple<Node *, int>;

vector<int> memory;           // 單點的memory
vector<double> swapping_prob; // 每個點做swapping的機率
vector<double> dis;           // 距離直接隨機給，用presum來取
unordered_map<int, unordered_map<int, Node *>> nodeMap;
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
    memory.push_back(3);

    double swapping_probaility = swapping_success_prob();
    swapping_prob.push_back(swapping_probaility);
    // nodeMap[i] = new Node(i);

    // double distance = (dist_up - dist_low) * rand() / RAND_MAX + dist_low;
    double distance = 1;
    dis.push_back(dis.back() + distance);
  }
}

Node *getNode(int idx, int mem) {
  if (nodeMap.find(idx) == nodeMap.end() ||
      nodeMap[idx].find(mem) == nodeMap[idx].end()) {
    nodeMap[idx][mem] = new Node(idx, mem);
  }
  return nodeMap[idx][mem];
}

void printPath(unordered_map<Node *, Node *> &prev, Node *curNode, int &src) {
  vector<int> Path, Memory;
  while (curNode->ID != src) {
    Path.push_back(curNode->ID);
    Memory.push_back(curNode->memory);
    curNode = prev[curNode];
  }
  Path.push_back(src);
  Memory.push_back(0);
  reverse(Path.begin(), Path.end());
  reverse(Memory.begin(), Memory.end());
  cout << "Path : ";
  for (auto &c : Path)
    cout << c << " ";
  cout << endl << "Memory : ";
  for (auto &c : Memory)
    cout << c << " ";
  cout << endl;
}

void buildGraph(int node, int src, int dst) {
  for (int i = 1; i <= memory[src]; i++) {
    Node *s = getNode(virtual_src, 0);
    Node *d = getNode(src, i);
    s->neighbors.push_back(pathInfo(1, 1, d));
  }
  for (int i = 1; i <= memory[dst]; i++) {
    Node *s = getNode(dst, i);
    Node *d = getNode(virtual_dst, 0);
    s->neighbors.push_back(pathInfo(1, 1, d));
  }
  for (int i = 0; i < node; i++) {
    for (int iMemory = 1; iMemory <= memory[i]; iMemory++) {
      Node *s = getNode(i, iMemory);
      for (int j = i + 1; j < node; j++) {
        // 若2點距離超過 fiber distance，不建邊
        if ((dis[j] - dis[i]) > fiber_distance)
          break;
        double entangleFidelity = entangle_fidelity(dis[j] - dis[i], beta);
        double entangleProb = entangle_success_prob(dis[j] - dis[i]);
        double curFidelity = entangleFidelity, curProb = entangleProb;
        for (int jMemory = 1; jMemory <= memory[j]; jMemory++) {
          // 若左邊的路用的memory + 下一條路用的memory超過節點i的memory，則跳出
          if (iMemory + jMemory > memory[i])
            break;
          Node *d = getNode(j, jMemory);

          if (curFidelity > threshold) {
            Node *middleNode = new Node(-100000 - j, jMemory);
            s->neighbors.push_back(pathInfo(curFidelity, curProb, middleNode));
            middleNode->neighbors.push_back(pathInfo(1, 1, d));
          }
          curProb = curProb * entangleProb *
                    purify_success_prob(curFidelity, entangleFidelity);
          curFidelity = purify_fidelity(curFidelity, entangleFidelity);
        }
      }
    }
  }
}
void dijkstra(int src, int dst) {
  priority_queue<pathInfo> pq; // {prob,fid,cur,used_memory}
  unordered_map<Node *, bool> used;
  unordered_map<Node *, Node *> prev;
  // vector<unordered_map<int, array<double, 2>>> prev(node);
  pq.push(pathInfo(1, 1, nodeMap[src][0]));

  while (!pq.empty()) {
    pathInfo path = pq.top();

    double prob = path.prob, fidelity = path.fidelity;
    Node *curNode = path.node;
    pq.pop();

    // cout << "Prob : " << prob << " Fid : " << fidelity << " ID : " <<
    // curNode->ID << "  Mem : " << curNode->memory << endl;

    if (fidelity < threshold)
      continue;

    // find an answer
    if (curNode->ID == dst) {
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

      // 1. check memory is enough at pratical node (virtual node don't need
      // to check memory)
      // if (curNode->ID >= 0 && (usedMemory + edgeMemory) >
      // memory[curNode->ID]) continue;
      // 2. check nextNode and memory is visited
      if (used.find(nextNode) != used.end())
        continue;

      double nextFidelity = fidelity * edgeFidelity;
      double nextProb = prob * edgeProb;

      if (curNode->ID >= 0 && curNode->ID != src && curNode->ID != dst) {
        nextFidelity *= swapping_fidelity(fidelity, edgeFidelity);
        nextProb *= swapping_prob[curNode->ID];
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
    dijkstra(virtual_src, virtual_dst);
  }
}
