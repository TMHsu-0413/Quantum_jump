#include "../parameter.cpp"
#include "formula.cpp"
#include "header.h"
using namespace std;
vector<Node> qNode;
unordered_map<pair<int, int>, double, pairHash> disTable;
vector<vector<int>> kSP;
vector<acceptPath> acPaths;
int routingReq, numQn, minCost, maxCost;

void printPath(vector<int> &path) {
  for (auto &p : path) {
    cout << p << "  ";
  }
  cout << endl;
}

vector<int> extendDijkstra(int src, int dest) {
  vector<double> maxFidelity(qNode.size(), 0.0);
  vector<int> parent(qNode.size(), -1);
  priority_queue<pair<double, int>, vector<pair<double, int>>> pq;

  pq.push({0, src});

  while (!pq.empty()) {
    auto [curF, curNode] = pq.top();
    pq.pop();

    if (curNode == dest) {
      break;
    }

    for (auto &edge : qNode[curNode].neighbor) {
      if (edge.canUse) {
        double newFidelity = curF * edge.fidelity;
        if (newFidelity > maxFidelity[edge.to]) {
          maxFidelity[edge.to] = newFidelity;
          parent[edge.to] = curNode;
          pq.push({newFidelity, edge.to});
        }
      }
    }
  }

  vector<int> path;
  for (int at = dest; at != -1; at = parent[at]) {
    path.push_back(at);
  }
  reverse(path.begin(), path.end());
  if (!path.empty() && path[0] == src) {
    return path;
  }

  return {};
}

void testInit() {
  numQn = 6;
  for (int i = 0; i < 6; i++) {
    Node *tmp = new Node;
    tmp->id = i;
    qNode.push_back(*tmp);
  }
  qNode[0].addNeighbor({0, 0, 1, 0, 0, {0}, {0}}); // c
  disTable[{0, 1}] = 3;
  qNode[0].addNeighbor({1, 0, 2, 0, 0, {0}, {0}});
  disTable[{0, 2}] = 2;
  qNode[1].addNeighbor({2, 1, 3, 0, 0, {0}, {0}}); // d
  disTable[{1, 3}] = 4;
  qNode[2].addNeighbor({3, 2, 1, 0, 0, {0}, {0}}); // e
  disTable[{2, 1}] = 1;
  qNode[2].addNeighbor({4, 2, 3, 0, 0, {0}, {0}});
  disTable[{2, 3}] = 2;
  qNode[2].addNeighbor({5, 2, 4, 0, 0, {0}, {0}});
  disTable[{2, 4}] = 3;
  qNode[3].addNeighbor({6, 3, 4, 0, 0, {0}, {0}}); // f
  disTable[{3, 4}] = 2;
  qNode[3].addNeighbor({7, 3, 5, 0, 0, {0}, {0}});
  disTable[{3, 5}] = 1;
  qNode[4].addNeighbor({8, 4, 5, 0, 0, {0}, {0}});
  disTable[{4, 5}] = 2;
}

int main() {
  testInit();
  auto path = extendDijkstra(0, 5);
  printPath(path);
  return 0;
}
