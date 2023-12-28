#include <bits/stdc++.h>
using namespace std;

class P {
public:
  double fidelity;
  vector<int> path;
  vector<int> memory;
  P(double f, vector<int> p, vector<int> m) : fidelity(f), path(p), memory(m) {
    ;
  }
};
double fidelity(double dis,double beta) { return 0.25 + 0.75 * exp(-beta * dis); }

double cal_fidelity(double cur_beta, double dis) {
  return 0.25 + 0.75 * exp(-cur_beta * dis);
}

double swapping(double fid1, double fid2) {
  return fid1 * fid2 + ((1 - fid1) * (1 - fid2) / 3);
}

double purify(double fid1, double fid2) {
  return ((fid1 * fid2 + (1 - fid1) * (1 - fid2) / 9)) /
         ((fid1 * fid2) + ((1 - fid1) * fid2 / 3) + (fid1 * (1 - fid2) / 3) +
          (5 * (1 - fid1) * (1 - fid2) / 9));
}

double dis(array<double, 2> &a, array<double, 2> &b) {
  return sqrt(powf(a[0] - b[0], 2) + powf(a[1] - b[1], 2));
}

double at_least_to_meet_threshold(double fid,double threshold){
  // fid*b + (1-fid)/3 * (1-b) >= threshold
  double goal = threshold - (1 - fid) / 3;
  double cur_pa = fid - (1 - fid) / 3;
  goal /= cur_pa;

  return goal;
}