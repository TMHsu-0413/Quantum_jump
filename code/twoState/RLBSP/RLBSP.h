#include <bits/stdc++.h>
#include <cstdlib>
#include <random>
using namespace std;
class Node;
class Label {
public:
  double slope, diffCost;
  array<Node*, 2> prev_parent;
  array<double, 2> prev_dist;
  double prev_minDiff;
  double prev_theta;
  Node *node;

  Label(double s, double d, Node *n, double minDiff, double theta, array<Node*, 2> parent, array<double,2> dist) : slope(s), diffCost(d), node(n),prev_theta(theta), prev_minDiff(minDiff), prev_parent(parent), prev_dist(dist) { ; }

  const bool operator<(const Label &b) const {
    if (slope == b.slope)
      return diffCost < b.diffCost;
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
