#include"header.h"
using namespace std;

class Node;
class pathInfo {
public:
  int idx;
  int from;
  int to;
  double fidelity;
  double prob;
  bool canUse = 1; // for k-shortest path
  // purFidelity record fidelity after "idx+1" times purification
  vector<double>purFidelity;
  vector<double>purProb;
  pathInfo(int idx, int from, int to, double prob, double fidelity, vector<double> purF, vector<double> purP) : idx(idx), from(from), to(to), fidelity(fidelity), prob(prob), purFidelity(purF), purProb(purP) {;}
  const bool operator<(const pathInfo &b) const { return prob < b.prob; }
  bool operator==(const pathInfo &b) const {
    return idx == b.idx && from == b.from && to == b.to &&
           fidelity == b.fidelity && prob == b.prob && canUse == b.canUse &&
           purFidelity == b.purFidelity && purProb == b.purProb;
  }
};

class Node{
public: 
  int id;
  double x;
  double y;
  int mem;
  double swappingProb;
  int memUsed;
  vector<pathInfo>neighbor;
  Node(): id(-1), x(-1), y(-1), mem(0), memUsed(0), swappingProb(-1) {;}
  Node(int idx, double a, double b, int c, double d) : id(idx), x(a), y(b), mem(c), memUsed(0), swappingProb(d) {;}
  const bool operator<(const Node &b) const { 
      if(x == b.x) return y < b.y;
      return y < b.y;
  }
  void addNeighbor(pathInfo p){
    neighbor.push_back(p);
  }
  // O(N) to find edge and return edgeIdx
  // int findEdge(int u, int v){ 
  //   for(int i = 0; i < neighbor.size(); i++){
  //     if(neighbor[i].from == u && neighbor[i].to == v)
  //       return i;
  //   }
  // }
};

class acceptPath {
public:
  double fidelity;
  double probability;
  vector<int>path;
  vector<int>purTimes;
  acceptPath(): fidelity(-1), probability(-1), path(vector<int>()), purTimes(vector<int>()) {;}
  acceptPath(double f, double p, vector<int> pa, vector<int> pt) : fidelity(f), probability(p), path(pa), purTimes(pt) {;}
  bool operator<(const acceptPath &b) const { return probability > b.probability; }
};

bool cmpNode(Node a, Node b){
  if(a.x == b.x) return a.y > b.y;
  return a.x > b.x;
}

double distence(Node a, Node b){
  return sqrt(pow((a.x-b.x), 2) + pow((a.y-b.y), 2));
}

class pairHash {
public:
    size_t operator()(const std::pair<int, int>& p) const {
        auto hash1 = hash<double>{}(p.first);
        auto hash2 = hash<double>{}(p.second);
        return hash1 ^ (hash2 * 0x9e3779b9);
    }
};

double entangle_fidelity(double dis, double beta) {
  return 0.5 + 0.5 * exp(-beta * dis);
}

double swapping_fidelity(double fid1, double fid2) { return fid1 * fid2; }

double purify_fidelity(double fid1, double fid2) {
  return (fid1 * fid2) / ((fid1 * fid2) + ((1 - fid1) * (1 - fid2)));
}

double entangle_success_prob(double dis) { return exp(-0.0002 * dis); }

double purify_success_prob(double fid1, double fid2) {
  return ((fid1 * fid2) + ((1 - fid1) * (1 - fid2)));
}

double swapping_success_prob() {
  return 0.85;
  // return (1 - 0.8) * rand() / RAND_MAX + 0.8;
}

double getPurFidelity(Node u, int edgeIdx, int time){
  if(time >= u.neighbor[edgeIdx].purFidelity.size())
    return 0;
  return u.neighbor[edgeIdx].purFidelity[time];
}

