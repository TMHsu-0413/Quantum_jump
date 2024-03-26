#include <bits/stdc++.h>
#include <cstdlib>
#include <random>
using namespace std;
class Node;
class Label {
public:
  double slope, diffCost;
  Node *node;

  Label(double s, double d, Node *n) : slope(s), diffCost(d), node(n) { ; }

  const bool operator<(const Label &b) const {
    if (slope == b.slope)
      return diffCost > b.diffCost;
    return slope > b.slope;
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
