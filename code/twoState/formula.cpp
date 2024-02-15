#include <bits/stdc++.h>
#include <cstdlib>
#include <random>
using namespace std;

class Path_and_Memory {
public:
  int idx;
  vector<int> path, memory;
  Path_and_Memory(int i, vector<int> p, vector<int> m)
      : idx(i), path(p), memory(m) {
    ;
  }
  bool operator<(const Path_and_Memory &b) const {
    if (path != b.path)
      return path < b.path;
    else if (memory != b.memory)
      return memory < b.memory;
    return idx < b.idx;
  }
};

class P {
public:
  double fidelity;
  vector<int> path;
  vector<int> memory;
  double success_prob;
  P(double f, vector<int> p, vector<int> m, double s)
      : fidelity(f), path(p), memory(m), success_prob(s) {
    ;
  }

  bool operator<(const P &p) const {
    if (success_prob != p.success_prob)
      return success_prob < p.success_prob;
    if (path != p.path)
      return path < p.path;

    if (memory != p.memory)
      return memory < p.memory;

    if (fidelity != p.fidelity)
      return fidelity < p.fidelity;

    return false;
  }

  bool operator==(const P &p) const {
    return success_prob == p.success_prob && path == p.path &&
           memory == p.memory;
  }

  friend ostream &operator<<(ostream &cout, const P p) {
    cout << fixed << setprecision(6) << "fidelity: " << p.fidelity
         << "\tprob: " << p.success_prob;
    cout << "\t path = {";
    for (int j = 0; j < p.path.size(); j++) {
      if (j == (p.path.size() - 1))
        cout << p.path[j];
      else
        cout << p.path[j] << "->";
    }
    cout << "}, {";

    for (int j = 0; j < p.memory.size(); j++) {
      if (j == p.memory.size() - 1)
        cout << p.memory[j];
      else
        cout << p.memory[j] << ", ";
    }
    cout << "}" << endl;
    return cout;
  }
};

double entangle_fidelity(double dis, double beta) {
  return 0.5 + exp(-beta * dis);
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
  // return 0.85;
  return (1 - 0.8) * rand() / RAND_MAX + 0.8;
}
