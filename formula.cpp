#include <bits/stdc++.h>
#include <cstdlib>
#include <random>
using namespace std;

class P {
public:
  double fidelity;
  vector<int> path;
  vector<int> memory;
  double success_prob;
  P(double f, vector<int> p, vector<int> m) : fidelity(f), path(p), memory(m) {
    ;
  }
  P(double f, vector<int> p, vector<int> m, double s)
      : fidelity(f), path(p), memory(m), success_prob(s) {
    ;
  }
  bool operator<(const P &b) const{
    return fidelity < b.fidelity;
  }

  friend ostream& operator<<(ostream &cout,const P& p){
    cout << fixed << setprecision(6) << "fidelity: " << p.fidelity << "\tprob: " << p.success_prob;
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

double fidelity(double dis, double beta) {
  return 0.25 + 0.75 * exp(-beta * dis);
}

double swapping_fidelity(double fid1, double fid2) {
  return fid1 * fid2 + ((1 - fid1) * (1 - fid2) / 3);
}

double purify_fidelity(double fid1, double fid2) {
  return ((fid1 * fid2 + (1 - fid1) * (1 - fid2) / 9)) /
         ((fid1 * fid2) + ((1 - fid1) * fid2 / 3) + (fid1 * (1 - fid2) / 3) +
          (5 * (1 - fid1) * (1 - fid2) / 9));
}

double dis(array<double, 2> &a, array<double, 2> &b) {
  return sqrt(powf(a[0] - b[0], 2) + powf(a[1] - b[1], 2));
}

double at_least_to_meet_threshold(double fid, double threshold) {
  // fid*b + (1-fid)/3 * (1-b) >= threshold
  double goal = threshold - (1 - fid) / 3;
  double b_constant = fid - (1 - fid) / 3;
  double b = goal / b_constant;

  return b;
}

double entangle_success_prob(double dis) { return exp(-0.0002 * dis); }

double purify_success_prob(double fid1, double fid2) {
  return ((fid1 * fid2) + ((1 - fid1) * fid2 / 3) + (fid1 * (1 - fid2) / 3) +
          (5 * (1 - fid1) * (1 - fid2) / 9));
}

double swapping_success_prob() { return (1 - 0.8) * rand() / RAND_MAX + 0.8; }
