#include <algorithm>
#include <bits/stdc++.h>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <random>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
using namespace std;

int num_of_node;
vector<vector<int>> predecessor;
vector<vector<int>> successor;
class Label {
public:
  double theta, obj_bar2;
  int i;

  Label(double t, double o, int i) : theta(t), obj_bar2(o), i(i){};

  bool operator<(const Label &b) const {
    if (theta == b.theta)
      return obj_bar2 > b.obj_bar2;
    return theta > b.theta;
  }
};

vector<int> Dijkstra2(int src, int dst, map<pair<int, int>, double> &X,
                      map<pair<int, int>, double> &Y,
                      vector<pair<double, double>> &dist) {
  double INF = numeric_limits<double>::infinity();
  vector<bool> used(num_of_node, false);
  vector<int> parent(num_of_node, -1);
  priority_queue<pair<double, int>, vector<pair<double, int>>,
                 greater<pair<double, int>>>
      pq;
  dist[src] = {0, 0}; // 找src到所有點的最短距離(針對X-obj)
  pq.push({0, src});
  while (!pq.empty()) {
    int cur_node = pq.top().second;
    pq.pop();
    if (used[cur_node])
      continue;
    used[cur_node] = true;
    for (int neigh : successor[cur_node]) {
      if (dist[cur_node].first + X[{cur_node, neigh}] < dist[neigh].first) {
        dist[neigh].first =
            dist[cur_node].first + X[{cur_node, neigh}]; //(1)store d
        dist[neigh].second = dist[cur_node].second + Y[{cur_node, neigh}];
        parent[neigh] = cur_node; //(1)store pred
        pq.push({dist[neigh].first, neigh});
      }
    }
  }

  if (dist[dst].first >= INF)
    return {};
  return parent;
}

vector<int> separation_oracle2(int src, int dst, map<pair<int, int>, double> X,
                               map<pair<int, int>, double> Y) {
  double INF = numeric_limits<double>::infinity();
  vector<pair<double, double>> dist(num_of_node, {INF, INF});
  vector<int> pred(num_of_node, -1);
  vector<int> SDpath;
  vector<double> theta_table(num_of_node, INF);
  vector<double> obj_bar1_table(num_of_node, INF);
  vector<double> obj_bar2_table(num_of_node, INF);
  vector<int> cpred_table(num_of_node, -1);
  priority_queue<Label> pq;
  pred = Dijkstra2(src, dst, X, Y, dist); //(1)
  cout << "break\n";
  double last_ratio = 0; //(2)start
  double d1 = dist[dst].first, d2 = dist[dst].second;
  int current = dst;
  while (current != -1) {
    SDpath.push_back(current);
    current = pred[current];
  }
  reverse(SDpath.begin(), SDpath.end()); //(2)end
  for (auto it : SDpath) {
    cout << it << " ";
  }
  cout << "\n";

  for (int i = 1; i < num_of_node; i++) { //(4)compute lexmin
    if (i == src) {
      continue;
    }
    for (auto &neigh : predecessor[i]) {
      double cur_obj_bar2 = dist[neigh].second + Y[{neigh, i}] - dist[i].second;
      if (cur_obj_bar2 < 0) {
        double cur_theta =
            -(dist[neigh].first + X[{neigh, i}] - dist[i].first) / cur_obj_bar2;
        if (cur_theta <= theta_table[i]) {
          if (cur_theta < theta_table[i]) {
            theta_table[i] = cur_theta;
            obj_bar1_table[i] =
                dist[neigh].first + X[{neigh, i}] - dist[i].first;
            obj_bar2_table[i] = cur_obj_bar2;
            cpred_table[i] = neigh;
          } else if (cur_obj_bar2 < obj_bar2_table[i]) {
            theta_table[i] = cur_theta;
            obj_bar1_table[i] =
                dist[neigh].first + X[{neigh, i}] - dist[i].first;
            obj_bar2_table[i] = cur_obj_bar2;
            cpred_table[i] = neigh;
          }
        }
      }
    }
    if (cpred_table[i] != -1) {
      pq.push({theta_table[i], obj_bar2_table[i], i});
      cout << "Push1: " << theta_table[i] << " " << obj_bar2_table[i] << " "
           << i << "\n";
    }
  }

  while (!pq.empty()) { //(6)

    for (auto i1 : dist) {
      cout << "Dist:" << i1.first << "," << i1.second << "   ";
    }
    cout << endl;

    for (int i = 1; i < num_of_node; i++) {
      cout << "Cpred,pred:" << cpred_table[i] << "," << pred[i] << "   ";
    }
    cout << endl;

    for (auto i2 : theta_table) {
      cout << "theta:" << i2 << "   ";
    }
    cout << endl << endl;

    Label min = pq.top(); //(7)
    double min_theta = min.theta, min_obj_bar2 = min.obj_bar2;
    int min_i = min.i;
    pq.pop();
    if (min_obj_bar2 != obj_bar2_table[min_i]) {
      continue;
    }
    cout << "pop: " << min_theta << " " << min_obj_bar2 << " " << min_i << endl;
    dist[min_i].first += obj_bar1_table[min_i];
    dist[min_i].second += obj_bar2_table[min_i];
    pred[min_i] = cpred_table[min_i];

    // pop出來後要把obj_bar2_table[min_i]改回無限大
    obj_bar2_table[min_i] = INF;

    if (min_i == dst) {             //(9)
      if (min_theta > last_ratio) { //(10)
        cout << "[" << last_ratio << "," << min_theta << "] with "
             << " D1:" << d1 << " D2:" << d2 << endl;
        last_ratio = min_theta;
        cout << "Extremepath: ";
        for (auto it : SDpath) {
          cout << it << " ";
        }
        cout << endl;
      }
      SDpath.clear();
      current = dst;
      while (current != -1) {
        SDpath.push_back(current);
        current = pred[current];
      }
      reverse(SDpath.begin(), SDpath.end()); //(12)

      d1 = dist[dst].first, d2 = dist[dst].second;

      // 這裡要更新last_ratio = min_theta
      last_ratio = min_theta;
    }

    theta_table[min_i] = INF, cpred_table[min_i] = -1; //(13)
    for (auto neigh : predecessor[min_i]) {
      double cur_obj_bar2 =
          dist[neigh].second + Y[{neigh, min_i}] - dist[min_i].second;
      if (cur_obj_bar2 < 0) {
        double cur_theta =
            -(dist[neigh].first + X[{neigh, min_i}] - dist[min_i].first) /
            cur_obj_bar2;
        if (cur_theta <= theta_table[min_i]) {
          if (cur_theta < theta_table[min_i]) {
            theta_table[min_i] = cur_theta;
            obj_bar1_table[min_i] =
                dist[neigh].first + X[{neigh, min_i}] - dist[min_i].first;
            obj_bar2_table[min_i] = cur_obj_bar2;
            cpred_table[min_i] = neigh;
          } else if (cur_obj_bar2 < obj_bar2_table[min_i]) {
            theta_table[min_i] = cur_theta; // no use just for read
            obj_bar1_table[min_i] =
                dist[neigh].first + X[{neigh, min_i}] - dist[min_i].first;
            obj_bar2_table[min_i] = cur_obj_bar2;
            cpred_table[min_i] = neigh;
          }
        }
      }
    }
    if (cpred_table[min_i] != -1) {
      pq.push({theta_table[min_i], obj_bar2_table[min_i], min_i});
      cout << "Push2: " << theta_table[min_i] << " " << obj_bar2_table[min_i]
           << " " << min_i << "\n";
    }
    for (auto neigh : successor[min_i]) {
      double temp_obj_bar2 =
          dist[min_i].second + Y[{min_i, neigh}] - dist[neigh].second;
      if (temp_obj_bar2 < 0) {
        double temp_obj_bar1 =
            dist[min_i].first + X[{min_i, neigh}] - dist[neigh].first;
        if (-(temp_obj_bar1 / temp_obj_bar2) < theta_table[neigh] ||
            (-(temp_obj_bar1 / temp_obj_bar2) == theta_table[neigh] &&
             temp_obj_bar2 < obj_bar2_table[neigh])) {
          pq.push({-temp_obj_bar1 / temp_obj_bar2, temp_obj_bar2, neigh});
          theta_table[neigh] = -(temp_obj_bar1 / temp_obj_bar2);
          obj_bar1_table[neigh] = temp_obj_bar1;
          obj_bar2_table[neigh] = temp_obj_bar2;
          cpred_table[neigh] = min_i;
          cout << "Push3: " << theta_table[neigh] << " "
               << obj_bar2_table[neigh] << " " << neigh << "\n";
        }
      }
    }
  }
  for (auto it : SDpath) {
    cout << it << " ";
  }
  cout << endl;
  cout << "D1:" << d1 << " D2:" << d2 << endl;
  cout << endl;
  return SDpath;
}

int main() {

  num_of_node = 5;
  map<pair<int, int>, double> X;
  map<pair<int, int>, double> Y;
  predecessor.resize(num_of_node);
  successor.resize(num_of_node);
  predecessor[2].push_back(1);
  predecessor[2].push_back(3);
  predecessor[3].push_back(1);
  predecessor[3].push_back(2);
  predecessor[4].push_back(2);
  predecessor[4].push_back(3);

  predecessor[1].push_back(2);
  predecessor[1].push_back(3);
  predecessor[2].push_back(3);
  predecessor[2].push_back(4);
  predecessor[3].push_back(2);
  predecessor[3].push_back(4);

  successor[2].push_back(1);
  successor[2].push_back(3);
  successor[3].push_back(1);
  successor[3].push_back(2);
  successor[4].push_back(2);
  successor[4].push_back(3);

  successor[1].push_back(2);
  successor[1].push_back(3);
  successor[2].push_back(3);
  successor[2].push_back(4);
  successor[3].push_back(2);
  successor[3].push_back(4);

  X[{1, 2}] = 0;
  Y[{1, 2}] = 2;

  X[{1, 3}] = 4;
  Y[{1, 3}] = 0;
  X[{2, 3}] = 1;
  Y[{2, 3}] = 5;
  X[{2, 4}] = 3;
  Y[{2, 4}] = 1;
  X[{3, 2}] = 1;
  Y[{3, 2}] = 0;
  X[{3, 4}] = 0;
  Y[{3, 4}] = 2;

  X[{2, 1}] = 0;
  Y[{2, 1}] = 2;
  X[{3, 1}] = 4;
  Y[{3, 1}] = 0;
  X[{3, 2}] = 1;
  Y[{3, 2}] = 5;
  X[{4, 2}] = 3;
  Y[{4, 2}] = 1;
  X[{2, 3}] = 1;
  Y[{2, 3}] = 0;
  X[{4, 3}] = 0;
  Y[{4, 3}] = 2;
  int src = 1, dst = 4;
  vector<int> path = separation_oracle2(src, dst, X, Y);
}
