#include <bits/stdc++.h>
#include <cstdlib>
#include <random>
using namespace std;
class Node;

class path {
public:
  double fidelity;
  double prob;
  Node *node;
  Node *node2;
  path(double f, double p, Node *n) : fidelity(f), prob(p), node(n) { ; }
  path(double f, double p, Node *n, Node *n2) : fidelity(f), prob(p), node(n),node2(n2) { ; }

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
