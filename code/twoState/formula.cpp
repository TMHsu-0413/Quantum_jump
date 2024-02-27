#include <bits/stdc++.h>
#include <cstdlib>
#include <random>
using namespace std;
class Node;

class pathInfo {
public:
  double fidelity;
  double prob;
  Node *node;
  pathInfo(double f, double p, Node *n) : fidelity(f), prob(p), node(n) { ; }

  const bool operator<(const pathInfo &b) const { return prob < b.prob; }
};

class Node {
public:
  // from = 1 , to = 2，代表從1走到2的virtual node
  // from = 3 , to = -1，代表3的實體node
  int from;
  int to;
  int memory;
  vector<pathInfo> neighbors;
  Node(int from, int to, int mem) : from(from), to(to), memory(mem) { ; }
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
