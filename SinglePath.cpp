#include "formula.cpp"
#include "parameter.cpp"
#include <bits/stdc++.h>
using namespace std;

vector<int> path;               // path走法，假設是[0,1,2,....node]
vector<int> memory;             // 單點的memory
vector<array<double, 2>> point; // 點的座標，用來計算距離

unordered_map<int, unordered_map<int, vector<P>>> memo;
int mx_group_size = 0;

/*
unordered_map<double, unordered_map<int, vector<double>>> purify_memo;
// 回傳為長度為2的vector<double> ，
// v[0]為最大的fidelity，v[1]為最大fidelity那組的成功機率
vector<double> dp_purify(double fidelity, int times, int distance) {
  if (times == 1)
    return {fidelity, entangle_success_prob(distance)};

  if (purify_memo.find(fidelity) != purify_memo.end() &&
      purify_memo[fidelity].find(times) != purify_memo[fidelity].end())
    return purify_memo[fidelity][times];

  double ans_fid = 0;
  double ans_prob = 0;
  for (int i = 1; i <= (times / 2); i++) {
    vector<double> left = dp_purify(fidelity, i, distance);
    vector<double> right = dp_purify(fidelity, times - i, distance);
    double split_purify = purify_fidelity(left[0], right[0]);
    double split_prob =
        purify_success_prob(left[0], right[0]) * left[1] * right[1];

    if (ans_fid < split_purify) {
      ans_fid = split_purify;
      ans_prob = split_prob;
    }
  }

  return purify_memo[fidelity][times] = {ans_fid, ans_prob};
}

*/

// 找到vector<P> a內，第一個a.fidelity >= fid的index
int lower_bound(vector<P> &a, double fid) {
  int l = 0, r = a.size() - 1;
  int ans = a.size();
  while (l <= r) {
    int mid = (l + r) / 2;

    double cur_fid = a[mid].fidelity;
    if (cur_fid >= fid) {
      ans = mid;
      r = mid - 1;
    } else
      l = mid + 1;
  }

  return ans;
}

/*
// 計算距離用的beta，可以用desmos拉就好
double cal_beta() {
  double l = 0, r = 0.5;
  double ans = 0;
  while (l <= r) {
    double cur_beta = (l + r) / 2;
    double f = fidelity(fiber_distance, cur_beta);

    if (f < 0.5) {
      r = cur_beta - 0.00001;
    } else {
      ans = cur_beta;
      l = cur_beta + 0.00001;
    }
  }
  return ans;
}
*/

void init_path(int node) {
  path.clear();
  memory.clear();
  memo.clear();
  point.clear();
  // purify_memo.clear();
  mx_group_size = 0;

  for (int i = 0; i < node; i++) {
    path.push_back(i);
    int mem = rand() % (memory_up - memory_low + 1) + memory_low;
    memory.push_back(mem);
    double x = rand() % (coor_up - coor_low) + coor_low;
    double y = rand() % (coor_up - coor_low) + coor_low;
    point.push_back({x, y});
  }
}

// swapping跟purify選擇的dp
vector<P> dp(int l, int r) {
  if (l >= r)
    return {};
  else if (memo.find(l) != memo.end() && memo[l].find(r) != memo[l].end())
    return memo[l][r];

  vector<P> res;
  // 分段
  for (int m = l + 1; m < r; m++) {
    // 不能在這個點swapping
    if (memory[m] < 2)
      continue;

    vector<P> left = dp(l, m);
    vector<P> right = dp(m, r);

    for (auto &c : left) {
      double f1 = c.fidelity;
      double s1 = c.success_prob;
      vector<int> path1 = c.path;
      vector<int> memory1 = c.memory;

      int idx = lower_bound(right, at_least_to_meet_threshold(f1, threshold));
      for (int j = idx; j < right.size(); j++) {
        P d = right[j];
        double f2 = d.fidelity;
        double s2 = d.success_prob;
        vector<int> memory2 = d.memory;

        double fid = swapping_fidelity(f1, f2);
        if (fid < threshold)
          continue;
        else if ((memory1.back() + memory2.front()) > memory[m])
          continue;

        vector<int> temp_path = path1;
        vector<int> temp_memory = memory1;
        double success_prob = s1 * s2 * swapping_success_prob();

        temp_memory.back() += memory2.front();

        for (int k = 1; k < d.path.size(); k++) {
          temp_path.push_back(d.path[k]);
          temp_memory.push_back(d.memory[k]);
        }

        res.push_back(P(fid, temp_path, temp_memory, success_prob));
      }
    }
  }

  // 跳關
  for (int m = 1; m <= min(memory[l], memory[r]); m *= 2) {
    double distance = dis(point[l], point[r]);
    double jump_fidelity = fidelity(distance, beta);
    double success_prob = pow(entangle_success_prob(distance), m);

    for (int j = m; j > 1; j /= 2) {
      success_prob *= pow(jump_fidelity, j / 2);
      jump_fidelity = purify_fidelity(jump_fidelity, jump_fidelity);
    }

    res.push_back(P(jump_fidelity, {l, r}, {m, m}, success_prob));
  }
  sort(res.begin(), res.end(),
       [](P &a, P &b) { return a.fidelity < b.fidelity; });

  mx_group_size = max(mx_group_size, (int)res.size());
  return memo[l][r] = res;
}

void print(vector<P> &ans) {
  for (int i = 0; i < node; i++) {
    cout << memory[i] << " ";
  }
  cout << endl;
  for (int i = 0; i < ans.size(); i++) {
    auto c = ans[i];
    if (c.fidelity < threshold)
      continue;
    cout << "fidelity: " << c.fidelity << " prob: " << c.success_prob << " ";
    cout << "path = {";
    for (int j = 0; j < c.path.size(); j++) {
      if (j == (c.path.size() - 1))
        cout << c.path[j];
      else
        cout << c.path[j] << "->";
    }
    cout << "}, {";

    for (int j = 0; j < c.memory.size(); j++) {
      if (j == c.memory.size() - 1)
        cout << c.memory[j];
      else
        cout << c.memory[j] << ", ";
    }
    cout << "}" << endl;
  }

  cout << "maximum group size : " << mx_group_size << endl;
}

int main() {
  srand(time(NULL));
  double time = 0;
  for (int i = 0; i < times; i++) {
    cout << "第" << i + 1 << "次" << endl;
    init_path(node);
    int n = path.size() - 1;
    clock_t start = clock();
    vector<P> ans = dp(0, n);
    clock_t end = clock();

    // print(ans);

    time += (double(end) - double(start));
  }
  cout << "used time : " << (time / times) / CLOCKS_PER_SEC << endl;
}
