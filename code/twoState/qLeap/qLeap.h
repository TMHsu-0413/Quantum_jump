#include <bits/stdc++.h>
#include <cstdlib>
#include <random>
using namespace std;
class Node;
class purifyLabel {
public:
  double fidelity, fidelity_improvement, prob;
  // Label內改成紀錄目前這條path經過的點的資訊，不然會因為threshold超過導致更新有問題

  purifyLabel(double f,double fi, double p)
      : fidelity(f), fidelity_improvement(fi), prob(p){
    ;
  }
};

class pqLabel {
public:
  double fidelity, probability;
  int node, parent;

  pqLabel(double f,double p,int n,int pa):fidelity(f),probability(p),node(n),parent(pa){;}

  bool operator<(const pqLabel &b) const{
    return fidelity < b.fidelity;
  }
};

class path {
public:
  double fidelity;
  double prob;
  Node *node;
  path(double f, double p, Node *n) : fidelity(f), prob(p), node(n) { ; }

  const bool operator<(const path &b) const { return prob < b.prob; }
};

class Node {
public:
  int ID;
  int memory;
  vector<path> neighbor, parent;
  Node() { ; }
  Node(int id, int m) : ID(id), memory(m) { ; }
};
