#include "formula.h"
#include <bits/stdc++.h>
using t = tuple<double, double, int>;
using namespace std;

// 有向圖with cycle
unordered_map<int, Node *> buildgraph(int d) {
  unordered_map<int, Node *> g;
  g.clear();
  for (int i = 0; i <= d; i++) {
    g[i] = new Node(i);
  }

  // random graph

  /*
  for (int i = 1; i <= d; i++) {
    for (int j = 1; j <= d; j++) {
      if (i == j)
        continue;
      double cost1 = (double)rand() / (RAND_MAX + 1.0);
      double cost2 = (double)rand() / (RAND_MAX + 1.0);

      g[i]->neighbor.push_back(new path(cost1, cost2, g[j]));
      g[j]->parent.push_back(new path(cost1, cost2, g[i]));
      // g[j]->neighbor.push_back(new path(cost1, cost2, g[i]));
      // g[i]->parent.push_back(new path(cost1, cost2, g[j]));
    }
  }
  */

  // 2015 dijkstra example graph
  vector<array<int, 4>> edge = {
      {1, 2, 0, 2}, {1, 3, 4, 0}, {2, 3, 1, 5}, {3, 2, 1, 0},
      {2, 4, 3, 1}, {3, 4, 0, 2}}; //{i,j,cost1,cost2} 2015那篇的example

  vector<array<int, 4>> edge_rev = {
      {1, 2, 2, 0}, {1, 3, 0, 4}, {2, 3, 5, 1}, {3, 2, 0, 1},
      {2, 4, 1, 3}, {3, 4, 2, 0}}; //{i,j,cost1,cost2} 2015那篇的example
                                   // 但把cost1與cost2交換
  for (auto &e : edge) {
    g[e[0]]->neighbor.push_back(new path(e[2], e[3], g[e[1]]));
    g[e[1]]->parent.push_back(new path(e[2], e[3], g[e[0]]));
    // g[e[0]]->parent.push_back(new path(e[2], e[3], g[e[1]]));
    // g[e[1]]->neighbor.push_back(new path(e[2], e[3], g[e[0]]));
  }
  return g;
}
vector<array<double, 2>> dfsAns, RLBSPAns;
void dfs(unordered_map<int, Node *> &g, vector<bool> &used, int cur, int &d,
         double c1, double c2) {
  if (cur == d) {
    dfsAns.push_back({c1, c2});
    return;
  }
  used[cur] = true;
  for (auto &nxt : g[cur]->neighbor) {
    int nxtID = nxt->node->id;
    if (used[nxtID])
      continue;
    dfs(g, used, nxtID, d, c1 + nxt->cost1, c2 + nxt->cost2);
  }
  used[cur] = false;
}

void printPath(unordered_map<int, Node *> &g, vector<array<int, 2>> &p, int cur,
               int &source) {
  if (cur == source) {
    cout << "path : " << cur << " ";
    return;
  }
  printPath(g, p, p[cur][1], source);
  cout << cur << " ";
}

// 求Dijkstra找cost1的解
bool dijkstra(unordered_map<int, Node *> &g, int s, int d,
              vector<array<double, 2>> &dist, vector<array<int, 2>> &parent) {
  dist[s] = {0, 0};

  priority_queue<t, vector<t>, greater<t>> pq;
  pq.push({0, 0, s});
  while (!pq.empty()) {
    t temp = pq.top();
    pq.pop();
    double c1 = get<0>(temp), c2 = get<1>(temp);
    int cur = get<2>(temp);

    if (c1 > dist[cur][0] || c2 > dist[cur][1])
      continue;

    if (cur == d) {
      return true;
    }

    for (auto &nxt : g[cur]->neighbor) {
      int nxtID = nxt->node->id;
      if ((c1 + nxt->cost1) <= dist[nxtID][0]) {
        if (c1 + nxt->cost1 < dist[nxtID][0]) {
          dist[nxtID] = {c1 + nxt->cost1, c2 + nxt->cost2};
          parent[nxtID] = {0, cur};
          pq.push({dist[nxtID][0], dist[nxtID][1], nxt->node->id});
        } else if (c2 + nxt->cost2 < dist[nxtID][1]) {
          dist[nxtID] = {c1 + nxt->cost1, c2 + nxt->cost2};
          parent[nxtID] = {0, cur};
          pq.push({dist[nxtID][0], dist[nxtID][1], nxt->node->id});
        }
      }
    }
  }
  return false;
}

// 求Dijkstra找cost2的解
array<double, 2> dijkstra_on_cost2(unordered_map<int, Node *> &g, int s,
                                   int d) {

  vector<array<double, 2>> dist2(d + 1, {DBL_MAX, DBL_MAX});
  dist2[s] = {0, 0};

  priority_queue<t, vector<t>, greater<t>> pq;
  pq.push({0, 0, s});
  while (!pq.empty()) {
    t temp = pq.top();
    pq.pop();
    double c2 = get<0>(temp), c1 = get<1>(temp);
    int cur = get<2>(temp);

    if (cur == d) {
      return dist2[d];
    }

    if (c2 > dist2[cur][0])
      continue;

    for (auto &nxt : g[cur]->neighbor) {
      int nxtID = nxt->node->id;
      if ((c2 + nxt->cost2) < dist2[nxtID][0]) {
        dist2[nxtID] = {c2 + nxt->cost2, c1 + nxt->cost1};
        // parent[nxtID] = {0, cur};
        pq.push({dist2[nxtID][0], dist2[nxtID][1], nxt->node->id});
      }
    }
  }
  return dist2[d];
}

void RLBSP(unordered_map<int, Node *> &g, vector<array<double, 2>> &dist,
           vector<array<int, 2>> &parent, int s, int d) {
  vector<double> theta(d + 1, DBL_MAX);
  vector<double> minDiff(d + 1, DBL_MAX);
  double lastratio = 0;
  priority_queue<Label> pq;
  // Algo1中的4,5行
  for (int i = s + 1; i <= d; i++) {
    double minTheta = DBL_MAX, minCost2 = DBL_MAX;
    for (auto &prev : g[i]->parent) {
      int j = prev->node->id;
      double newCost1 = dist[j][0] + prev->cost1,
             newCost2 = dist[j][1] + prev->cost2;

      double diffCost1 = newCost1 - dist[i][0],
             diffCost2 = newCost2 - dist[i][1];

      if (diffCost2 < 0) {
        double curTheta = -diffCost1 / diffCost2;
        if (curTheta == minTheta) {
          minCost2 = min(minCost2, diffCost2);
          parent[i][0] = j;
        } else if (curTheta < minTheta) {
          minTheta = curTheta;
          minCost2 = diffCost2;
          parent[i][0] = j;
        }
      }
    }
    if (parent[i][0] != 0) {
      pq.push({minTheta, minCost2, i});
      theta[i] = minTheta;
      minDiff[i] = minCost2;
    }
  }

  while (!pq.empty()) {
    Label temp = pq.top();
    double slope = temp.slope, diffCost = temp.diffCost;
    int i = temp.nodeID;

    pq.pop();

    // lazy delete
    if (minDiff[i] != diffCost)
      continue;

    // output part
    cout << endl;
    cout << "pop : " << slope << " " << diffCost << " " << i << endl;
    for (auto &d : dist) {
      cout << "Dist: (" << d[0] << " " << d[1] << ") \t";
    }
    cout << endl;
    for (auto &d : parent) {
      cout << "Cpred, pred: (" << d[0] << "," << d[1] << ") \t";
    }
    cout << endl;
    for (auto &d : theta) {
      if (d == DBL_MAX)
        cout << "theta: inf \t";
      else
        cout << "theta: " << d << "\t";
    }
    cout << endl;

    if (i == d) {
      if (slope >= lastratio) {
        cout << "cost1 : " << dist[d][0] << " cost2 : " << dist[d][1] << endl;
        RLBSPAns.push_back({dist[d][0], dist[d][1]});
        printPath(g, parent, d, s);
        cout << endl;
        lastratio = slope;
      }
    }

    minDiff[i] = DBL_MAX;
    dist[i][0] = dist[i][0] - (slope * diffCost);
    dist[i][1] = dist[i][1] + diffCost;
    theta[i] = DBL_MAX;
    parent[i][1] = parent[i][0];
    parent[i][0] = 0;

    // 13行
    double minTheta = DBL_MAX, minCost2 = DBL_MAX;
    for (auto &prev : g[i]->parent) {
      int j = prev->node->id;
      double newCost1 = dist[j][0] + prev->cost1,
             newCost2 = dist[j][1] + prev->cost2;

      double diffCost1 = newCost1 - dist[i][0],
             diffCost2 = newCost2 - dist[i][1];

      if (diffCost2 < 0) {
        double curTheta = -diffCost1 / diffCost2;
        if (curTheta == minTheta) {
          if (diffCost2 < minCost2) {
            minCost2 = diffCost2;
            parent[i][0] = j;
          }
        } else if (curTheta < minTheta) {
          minTheta = curTheta;
          minCost2 = diffCost2;
          parent[i][0] = j;
        }
      }
    }

    if (parent[i][0] != 0) {
      pq.push({minTheta, minCost2, i});
      cout << "Push 13: [" << minTheta << " ," << minCost2 << " ," << i << endl;
      theta[i] = minTheta;
      minDiff[i] = minCost2;
    }

    // 15行
    for (auto &nxt : g[i]->neighbor) {
      int j = nxt->node->id;
      double newCost1 = dist[i][0] + nxt->cost1,
             newCost2 = dist[i][1] + nxt->cost2;

      double diffCost1 = newCost1 - dist[j][0],
             diffCost2 = newCost2 - dist[j][1];

      double curTheta = -diffCost1 / diffCost2;
      if (diffCost2 < 0) {
        if ((curTheta < theta[j]) ||
            ((curTheta == theta[j]) && (diffCost2 < minDiff[j]))) {
          pq.push({curTheta, diffCost2, j});
          cout << "Push 15: [" << curTheta << " ," << diffCost2 << " ," << j
               << "] " << endl;
          minDiff[j] = diffCost2;
          theta[j] = curTheta;
          parent[j][0] = i;
        }
      }
    }
  }
  cout << "cost1 : " << dist[d][0] << " cost2 : " << dist[d][1] << endl;
  RLBSPAns.push_back({dist[d][0], dist[d][1]});
  printPath(g, parent, d, s);
}

// 寫入txt，印圖用
void write_to_txt(vector<array<double, 2>> &v, string name) {
  ofstream ofs;
  ofs.open(name);
  if (!ofs.is_open()) {
    cout << "error to open output.txt" << endl;
    return;
  }
  for (auto &e : v)
    ofs << e[0] << " " << e[1] << endl;
}

int main() {
  srand(time(NULL));
  int source = 1, dest = 4, n = dest + 1;
  auto g = buildgraph(dest);
  vector<array<double, 2>> dist(n, {DBL_MAX, DBL_MAX}); // {d1,d2}
  vector<array<int, 2>> parent(n, {0, -1});             // {Cpred,pred}
  bool havePath = dijkstra(g, source, dest, dist, parent);
  array<double, 2> cost2_sp = dijkstra_on_cost2(g, source, dest);
  // 這張圖起點到終點沒連通
  if (!havePath)
    return 0;
  cout << "Initial shortest path cost : " << dist[dest][0] << " "
       << dist[dest][1] << endl;
  vector<bool> used(n, false);
  dfs(g, used, source, dest, 0, 0);
  RLBSP(g, dist, parent, source, dest);
  write_to_txt(dfsAns, "allpoint.txt");
  write_to_txt(RLBSPAns, "RLBSPpoint.txt");

  cout << "\n\ncost2 shortest path cost : " << cost2_sp[1] << " " << cost2_sp[0]
       << endl;
}
