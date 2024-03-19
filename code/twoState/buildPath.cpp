#include "../parameter.cpp"
// #include "formula.cpp"
#include <bits/stdc++.h>
using namespace std;
class Node;
using tNi = tuple<Node *, int>;
double entangle_success_prob(double dis) { return exp(-0.0002 * dis); }

double purify_success_prob(double fid1, double fid2) {
  return ((fid1 * fid2) + ((1 - fid1) * (1 - fid2)));
}

double swapping_success_prob() {
  return 0.85;
  // return (1 - 0.8) * rand() / RAND_MAX + 0.8;
}
class pathInfo {
public:
  double prob;
  int s, sMem;
  int d, dMem;
  // pathInfo(double f, double p, Node *n) : fidelity(f), prob(p), node(n) { ; }
  const bool operator<(const pathInfo &b) const { return prob < b.prob; }
};

class Node {
public:
  int ID;
  int memory;
  vector<pathInfo> neighbor, parent;
  Node(int id, int mem) : ID(id), memory(mem) { ; }
};

vector<int> memory;           // 單點的memory
vector<double> swapping_prob; // 每個點做swapping的機率
vector<double> dis;           // 距離直接隨機給，用presum來取
struct NodeID { 
    int id, mem;
    bool operator==(const NodeID& other) const {
        return id == other.id && mem == other.mem;
    }    
};

struct NodeIDHash {
    std::size_t operator()(const NodeID& key) const {
        return std::hash<int>{}(key.id) ^ (std::hash<int>{}(key.mem) << 1);
    }
};
unordered_map<NodeID, Node *, NodeIDHash> nodeMap;
int idx = -1;

Node *getNode(int id, int mem){
  if(nodeMap.count({id, mem}) == 0){
    nodeMap[{id, mem}] = new Node(id, mem);
  }
  return nodeMap[{id, mem}];
}

void BuildGraph(int node, int src, int dst){
  // from i to j
  for (int i = 0; i < node - 1; i++) { 
    for (int j = i + 1; j < node; j++) {
      // 若 2 點距離超過 fiber distance，不建邊
      if ((dis[j] - dis[i]) > fiber_distance)
        break;
      // prob to create the edge
      double entangleProb = entangle_success_prob(dis[j] - dis[i]);
      double curProb = entangleProb;

      // iMemory: memory node i already used
      // jMemory: memory node j already used
      for (int iMemory = 1; i <= memory[i]; iMemory++) {
        Node *s = getNode(i, iMemory);

        for (int jMemory = 1; jMemory <= memory[j]; jMemory++) {
          // iMemory + jMemory = the memory node i used after create the edge
          if(iMemory + jMemory > memory[i])
            break;
          
          Node *d = getNode(j, jMemory);
          pathInfo p;
          p.prob = curProb;
          p.s = s;
          p.sMem = iMemory;
          p.d = d;
          p.dMem = jMemory;
          
          s->neighbor.push_back(p);
          
          d->parent.push_back(p);
        }
      }
    }
  }
}

int main(){

}