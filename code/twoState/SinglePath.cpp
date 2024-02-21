#include "../parameter.cpp"
#include "formula.cpp"
#include <bits/stdc++.h>
using namespace std;
using ad4 = array<double, 4>;

vector<int> memory;           // 單點的memory
vector<double> swapping_prob; // 每個點做swapping的機率
vector<double> dis;           // 距離直接隨機給，用presum來取
unordered_map<int, Node *> nodeMap;
int idx = -1;
void init_path(int node) {
  memory.clear();
  swapping_prob.clear();
  dis.push_back(0);
  for (int i = 0; i < node; i++) {
    // int mem = rand() % (memory_up - memory_low + 1) + memory_low;
    // memory.push_back(mem);
    memory.push_back(8);

    double swapping_probaility = swapping_success_prob();
    swapping_prob.push_back(swapping_probaility);
    nodeMap[i] = new Node(i);

    // double distance = (dist_up - dist_low) * rand() / RAND_MAX + dist_low;
    double distance = 1.414;
    dis.push_back(dis.back() + distance);
  }
}
void buildGraph(int node) {
  for (int i = 0; i < node; i++) {
    for (int j = i + 1; j < node; j++) {
      if ((dis[j] - dis[i]) <= fiber_distance) {
        double entangleFidelity = entangle_fidelity(dis[j] - dis[i], 0.74);
        double entangleProb = entangle_success_prob(dis[j] - dis[i]);

        double curFidelity = entangleFidelity, curProb = entangleProb;
        for (int k = 1; k <= min(memory[i], memory[j]); k++) {
          if (curFidelity > threshold) {
            Node *middleware = new Node(idx--);
            nodeMap[i]->neighbors.push_back(
                pathInfo(curFidelity, curProb, middleware, k));
            middleware->neighbors.push_back(pathInfo(1, 1, nodeMap[j], k));
          }
          curProb = curProb * entangleProb *
                    purify_success_prob(curFidelity, entangleFidelity);
          curFidelity = purify_fidelity(curFidelity, entangleFidelity);
        }
      }
    }
  }
}
void dijkstra() {
  priority_queue<pathInfo> pq; // {prob,fid,cur,used_memory}
  double source = 0, dist = node - 1;
  unordered_set<Node *> used;
  unordered_map<Node *, tuple<Node *, int>> prev;
  // vector<unordered_map<int, array<double, 2>>> prev(node);
  pq.push(pathInfo(1, 1, nodeMap[0], 0));

  while (!pq.empty()) {
    pathInfo path = pq.top();

    double prob = path.prob, fidelity = path.fidelity;
    Node *curNode = path.node;
    int usedMemory = path.memory;
    pq.pop();
    // cout << prob << " " << fidelity << " " << curNode->ID << endl;

    if (fidelity < threshold)
      continue;
    else if (used.find(curNode) != used.end())
      continue;

    // find an answer
    if (curNode->ID == dist) {
      double mem = usedMemory, prevMemory = 0;
      vector<double> tempPath, tempMemory;
      while (curNode->ID != 0) {
        // not middleware node
        if (curNode->ID >= 0) {
          tempMemory.push_back(mem + prevMemory);
          tempPath.push_back(curNode->ID);
        }
        Node *prevNode = get<0>(prev[curNode]);
        int pathMemory = get<1>(prev[curNode]);
        curNode = prevNode;
        prevMemory = mem;
        mem = pathMemory;
      }
      tempMemory.push_back(prevMemory);
      tempPath.push_back(0);
      reverse(tempPath.begin(), tempPath.end());
      reverse(tempMemory.begin(), tempMemory.end());
      cout << "path : ";
      for (auto &c : tempPath)
        cout << c << " ";
      cout << endl;
      cout << "memory : ";
      for (auto &c : tempMemory)
        cout << c << " ";
      cout << endl;

      cout << "Fidelity : " << fidelity << "\nProb : " << prob << endl;
      return;
    }
    used.insert(curNode);
    for (auto &nxt : curNode->neighbors) {
      Node *nextNode = nxt.node;
      int needMemory = nxt.memory;
      double nextFidelity = nxt.fidelity, nextProb = nxt.prob;
      // nxt -> {next_node,purify_time,fidelity,probability}

      // 1. check memory is enough at pratical node (virtual node don't need
      // to check memory)
      if (curNode->ID >= 0 && (usedMemory + needMemory) > memory[curNode->ID])
        continue;
      // 2. check nextNode and memory is visited
      else if (used.find(nextNode) != used.end())
        continue;
      // 3. check fidelity is larger than threshold
      else if ((fidelity * nextFidelity) < threshold)
        continue;

      pq.push(pathInfo(fidelity * nextFidelity, prob * nextProb, nextNode,
                       needMemory));
      // cout << prob * nextProb << " " << fidelity * nextFidelity << " "
      //<< curNode->ID << " " << nextNode->ID << endl;
      //  最後要找出path，所以標記上一跳位置
      prev[nextNode] = {curNode, needMemory};
    }
  }
}

int main() {
  srand(time(NULL));
  for (int i = 0; i < times; i++) {
    init_path(node);
    buildGraph(node);
    dijkstra();
  }
}
