#include "formula.cpp"
#include "parameter.cpp"
#include <bits/stdc++.h>
using namespace std;

class Compare {
public:
  bool operator()(P &a, P &b) { return a.fidelity > b.fidelity; }
};

vector<int> path;               // path走法，假設是[0,1,2,....node]
vector<int> memory;             // 單點的memory
vector<array<double, 2>> point; // 點的座標，用來計算距離

unordered_map<int, unordered_map<int, vector<P>>> memo;
unordered_map<double, unordered_map<int, double>> purify_memo;
int mx_group_size = 0;

double dp_purify(double fidelity, int times) {
  if (times == 1)
    return fidelity;
  else if (times == 2)
    return purify_fidelity(fidelity, fidelity);

  if (purify_memo.find(fidelity) != purify_memo.end() &&
      purify_memo[fidelity].find(times) != purify_memo[fidelity].end())
    return purify_memo[fidelity][times];

  double ans = 0;
  for (int i = 1; i <= (times / 2); i++)
    ans = max(ans, purify_fidelity(dp_purify(fidelity, i),
                                   dp_purify(fidelity, times - i)));

  return purify_memo[fidelity][times] = ans;
}

int lower_bound(vector<P> &a, double fid1) {
  int l = 0, r = a.size() - 1;
  int ans = a.size();
  while (l <= r) {
    int mid = (l + r) / 2;

    double cur_fid = a[mid].fidelity;
    if (cur_fid >= fid1) {
      ans = mid;
      r = mid - 1;
    } else
      l = mid + 1;
  }

  return ans;
}

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

void init_path(int node) {
  path.clear();
  memory.clear();
  memo.clear();
  point.clear();
  purify_memo.clear();
  mx_group_size = 0;

  for (int i = 0; i < node; i++) {
    path.push_back(i);
    int mem = rand() % (memory_up - memory_low) + memory_low;
    memory.push_back(mem);
    double x = rand() % (coor_up - coor_low) + coor_low;
    double y = rand() % (coor_up - coor_low) + coor_low;
    point.push_back({x, y});
  }
}

vector<P> dp(int l, int r) {
  if (l >= r)
    return {};
  else if (memo.find(l) != memo.end() && memo[l].find(r) != memo[l].end())
    return memo[l][r];

  vector<P> res;
  priority_queue<P, vector<P>, Compare> pq;
  // 分段
  for (int m = l + 1; m < r; m++) {
    // 不能在這個點swapping
    if (memory[m] < 2)
      continue;

    vector<P> left = dp(l, m);
    vector<P> right = dp(m, r);

    for (auto &c : left) {
      double f1 = c.fidelity;
      vector<int> path1 = c.path;
      vector<int> memory1 = c.memory;

      int idx = lower_bound(right, at_least_to_meet_threshold(f1, threshold));
      for (int i = idx; i < right.size(); i++) {
        P d = right[i];
        double f2 = d.fidelity;
        vector<int> memory2 = d.memory;

        double fid = swapping_fidelity(f1, f2);
        if (fid < threshold)
          continue;
        else if (memory1.back() + memory2.front() > memory[i])
          continue;

        vector<int> temp_path = path1;
        vector<int> temp_memory = memory1;
        temp_memory.back() += memory2.front();

        for (int j = 1; j < d.path.size(); j++) {
          temp_path.push_back(d.path[j]);
          temp_memory.push_back(d.memory[j]);
        }
        pq.push(P(fid, temp_path, temp_memory));
        if (pq.size() > k)
          pq.pop();
      }
    }
  }

  // 跳關
  for (int m = min(memory[l], memory[r]); m > 0; m--) {
    double single_fidelity = fidelity(dis(point[l], point[r]), beta);
    double purify_fidelity = dp_purify(single_fidelity, m);
    pq.push(P(purify_fidelity, {l, r}, {m, m}));
    if (pq.size() > k)
      pq.pop();
  }

  while (!pq.empty()) {
    P temp = pq.top();
    pq.pop();
    res.push_back(temp);
  }
  mx_group_size = max(mx_group_size, (int)res.size());
  return memo[l][r] = res;
}

void print(vector<P> &ans) {
  for (auto &c : ans) {
    cout << c.fidelity << " ";
    cout << "path = {";
    for (int i = 0; i < c.path.size(); i++) {
      if (i == (c.path.size() - 1))
        cout << c.path[i];
      else
        cout << c.path[i] << "->";
    }
    cout << "}, {";

    for (int i = 0; i < c.memory.size(); i++) {
      if (i == c.memory.size() - 1)
        cout << c.memory[i];
      else
        cout << c.memory[i] << ", ";
    }
    cout << "}" << endl;
  }
  cout << "maximum group size : " << mx_group_size << endl;
}

int main() {
  beta = cal_beta();
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
