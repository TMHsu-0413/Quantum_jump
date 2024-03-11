#include <bits/stdc++.h>
using namespace std;

class Node;

class path {
public:
  double cost1, cost2;
  Node *node;
  path(double c1, double c2, Node *n) : cost1(c1), cost2(c2), node(n) { ; }
};

class Label {
public:
  double slope, diffCost;
  int nodeID;

  Label(double s, double d, int n) : slope(s), diffCost(d), nodeID(n) { ; }

  const bool operator<(const Label &b) const {
    if (slope == b.slope)
      return diffCost > b.diffCost;
    return slope > b.slope;
  }
};

class Node {
public:
  int id;
  vector<path *> parent, neighbor;
  Node() { ; }
  Node(int id) : id(id) { ; }
};
