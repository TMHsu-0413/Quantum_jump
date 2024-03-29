#include "../parameter.cpp"
#include "formula.cpp"
#include <bits/stdc++.h>
using namespace std;

vector<int> path;             // path走法，假設是[0,1,2,....node]
vector<int> memory;           // 單點的memory
vector<double> swapping_prob; // 每個點做swapping的機率
vector<double> dis;           // 距離直接隨機給，用presum來取

unordered_map<int, unordered_map<int, vector<P>>> memo;
int mx_group_size = 0;

void init_path(int node) {
  path.clear();
  memory.clear();
  memo.clear();
  swapping_prob.clear();
  mx_group_size = 0;
  dis.push_back(0);
  for (int i = 0; i < node; i++) {
    path.push_back(i);
    int mem = rand() % (memory_up - memory_low + 1) + memory_low;
    memory.push_back(mem);
    // memory.push_back(8);

    double swapping_probaility = swapping_success_prob();
    swapping_prob.push_back(swapping_probaility);

    double distance = (dist_up - dist_low) * rand() / RAND_MAX + dist_low;
    // double distance = 1.414;
    dis.push_back(dis.back() + distance);
  }
}

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

bool check_dominated(map<Path_and_Memory, array<double, 2>> &mp,
                     array<double, 2> &value) {
  for (auto &[k, v] : mp) {
    if (v[0] > value[0] && v[1] > value[1]) {
      return true;
    }
  }
  return false;
}

void delete_get_dominated(map<Path_and_Memory, array<double, 2>> &mp,
                          vector<P> &res, array<double, 2> &value) {
  for (auto it = mp.begin(); it != mp.end();) {
    if (it->second[0] < value[0] && it->second[1] < value[1]) {
      mp.erase(it++);
    } else
      it++;
  }

  for (auto it = res.begin(); it != res.end();) {
    if (it->fidelity < value[0] && it->success_prob < value[1]) {
      it = res.erase(it);
    } else
      it++;
  }
}

// swapping跟purify選擇的dp
vector<P> dp(int l, int r) {
  if (l >= r)
    return {};
  else if (memo.find(l) != memo.end() && memo[l].find(r) != memo[l].end())
    return memo[l][r];

  // 用來判斷同一個path的點有沒有被dominate
  map<Path_and_Memory, array<double, 2>> mp;
  vector<P> res;

  // 跳關 symmetric
  for (int m = 1; m <= min(memory[l], memory[r]); m *= 2) {
    double distance = dis[r] - dis[l];
    double fidelity = entangle_fidelity(distance, beta);
    double success_prob = pow(entangle_success_prob(distance), m);
    if (fidelity < 0.5) // purify只會越來越差
      continue;

    for (int j = m; j > 1; j /= 2) {
      success_prob *= pow(purify_success_prob(fidelity, fidelity), j / 2);
      fidelity = purify_fidelity(fidelity, fidelity);
    }
    if (fidelity < threshold)
      continue;
    vector<int> temp_path = {l, r}, temp_memory = {m, m};

    Path_and_Memory key(0, temp_path, temp_memory);
    array<double, 2> value = {fidelity, success_prob};
    bool dominated_by_other = check_dominated(mp, value);
    if (!dominated_by_other) {
      if (mp.find(key) == mp.end()) {
        delete_get_dominated(mp, res, value);
        mp[key] = value;
        res.push_back(P(fidelity, temp_path, temp_memory, success_prob));
      } else {
        delete_get_dominated(mp, res, mp[key]);
      }
    }
  }

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
      // 反過來，因為要盡量先組成fid與prob都大的，才能排除其他較小的
      for (int j = right.size() - 1; j >= idx; j--) {
        P d = right[j];
        double f2 = d.fidelity;
        double s2 = d.success_prob;
        vector<int> memory2 = d.memory;

        double fid = swapping_fidelity(f1, f2);
        if (fid <
            threshold) // 若swapping後小於threshold，則後續怎麼做也都補不到threshold
          continue;
        if ((memory1.back() + memory2.front()) > memory[m])
          continue;

        vector<int> temp_path = path1;
        vector<int> temp_memory = memory1;
        double success_prob = s1 * s2 * swapping_prob[m];

        temp_memory.back() += memory2.front();

        for (int k = 1; k < d.path.size(); k++) {
          temp_path.push_back(d.path[k]);
          temp_memory.push_back(d.memory[k]);
        }
        Path_and_Memory key(0, temp_path, temp_memory);
        array<double, 2> value = {fid, success_prob};

        bool dominated_by_other = check_dominated(mp, value);
        if (!dominated_by_other) {
          if (mp.find(key) == mp.end()) {
            delete_get_dominated(mp, res, value);
            mp[key] = value;
            res.push_back(P(fid, temp_path, temp_memory, success_prob));
          } else {
            delete_get_dominated(mp, res, mp[key]);
          }
        }
      }
    }
  }

  sort(res.begin(), res.end(),
       [](const P &a, const P &b) { return a.fidelity < b.fidelity; });
  mx_group_size = max(mx_group_size, (int)res.size());
  return memo[l][r] = res;
}

void print(vector<P> &ans) {
  for (int i = 0; i < node; i++) {
    cout << memory[i] << " ";
  }
  cout << "滿足fidelity限制的有" << ans.size() << "組" << endl;
  cout << endl;
  cout << "fidelity前10大的: " << endl;
  for (int i = ans.size() - 1; i >= max(0, (int)ans.size() - 10); i--)
    cout << ans[i];

  cout << endl;
  sort(ans.begin(), ans.end(),
       [](const P &a, const P &b) { return a.success_prob > b.success_prob; });

  cout << "success prob前10大的: " << endl;
  for (int i = 0; i < min((int)ans.size(), 10); i++)
    cout << ans[i];
}

void write_to_txt(vector<P> &ans) {
  ofstream ofs;
  ofs.open("output.txt");
  if (!ofs.is_open()) {
    cout << "error to open output.txt" << endl;
    return;
  }
  for (auto &c : ans) {
    ofs << c.fidelity << " " << c.success_prob << " ";
    for (int i = 0; i < c.path.size(); i++) {
      if (i != (c.path.size() - 1))
        ofs << c.path[i] << ",";
      else
        ofs << c.path[i];
    }
    ofs << " ";
    for (int i = 0; i < c.memory.size(); i++) {
      if (i != (c.memory.size() - 1))
        ofs << c.memory[i] << ",";
      else
        ofs << c.memory[i];
    }
    ofs << endl;
  }
}

int main() {
  srand(time(NULL));
  double time = 0;
  for (int i = 0; i < times; i++) {
    init_path(node);
    int n = path.size() - 1;
    clock_t start = clock();
    vector<P> ans = dp(0, n);
    clock_t end = clock();

    print(ans);
    write_to_txt(ans);

    time += (double(end) - double(start));
    cout << "maximum group size : " << mx_group_size << endl;
  }
  cout << "used time : " << (time / times) / CLOCKS_PER_SEC << endl;
}
