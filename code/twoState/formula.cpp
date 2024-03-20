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
  Node(int id, int m) : ID(id), memory(m) { ; }
};

double entangle_fidelity(double dis, double beta) {
  return 0.5 + 0.5 * exp(-beta * dis);
}

double swapping_fidelity(double fid1, double fid2) { return fid1 * fid2; }

double purify_fidelity(double fid1, double fid2) {
  return (fid1 * fid2) / ((fid1 * fid2) + ((1 - fid1) * (1 - fid2)));
}

// 可能需要改
double entangle_success_prob(double dis) { return exp(-0.0002 * dis); }

double purify_success_prob(double fid1, double fid2) {
  return ((fid1 * fid2) + ((1 - fid1) * (1 - fid2)));
}

double swapping_success_prob() {
  return 0.85;
  // return (1 - 0.8) * rand() / RAND_MAX + 0.8;
}

double ln(double x) { return log(x) / log(exp(1)); }
