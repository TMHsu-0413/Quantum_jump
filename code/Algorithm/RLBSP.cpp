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
  /*
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (i == j)
        continue;
      double cost1 = (double)rand() / (RAND_MAX + 1.0);
      double cost2 = (double)rand() / (RAND_MAX + 1.0);
      double build_edge = (double)rand() / (RAND_MAX + 1.0);
      if (build_edge >= 0.5) {
        g[i]->neighbor.push_back(new path(cost1, cost2, g[j]));
        g[j]->parent.push_back(new path(cost1, cost2, g[i]));
      }
    }
  }
  */

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
  }
  return g;
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

    if (cur == d) {
      // cout << "cost1 : " << c1 << " cost2 : " << c2 << endl;
      // printPath(g, parent, d, s);
      return true;
    }

    if (c1 > dist[cur][0])
      continue;

    for (auto &nxt : g[cur]->neighbor) {
      int nxtID = nxt->node->id;
      if ((c1 + nxt->cost1) < dist[nxtID][0]) {
        dist[nxtID] = {c1 + nxt->cost1, c2 + nxt->cost2};
        parent[nxtID] = {0, cur};
        pq.push({dist[nxtID][0], dist[nxtID][1], nxt->node->id});
      }
    }

    for (auto &prev : g[cur]->parent) {
      int prevID = prev->node->id;
      if ((c1 + prev->cost1) < dist[prevID][0]) {
        dist[prevID] = {c1 + prev->cost1, c2 + prev->cost2};
        parent[prevID] = {0, cur};
        pq.push({dist[prevID][0], dist[prevID][1], prev->node->id});
      }
    }
  }
  return false;
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
    double slope = temp.slope, diffCost2 = temp.diffCost;
    int i = temp.nodeID;
    pq.pop();

    // lazy delete
    if (minDiff[i] != diffCost2)
      continue;

    if (i == d) {
      if (slope > lastratio) {
        cout << "cost1 : " << dist[d][0] << " cost2 : " << dist[d][1] << endl;
        printPath(g, parent, d, s);
        cout << endl;
        lastratio = slope;
      }
    }

    minDiff[i] = DBL_MAX;
    dist[i][0] = dist[i][0] - (slope * diffCost2);
    dist[i][1] = dist[i][1] + diffCost2;
    theta[i] = DBL_MAX;
    parent[i][1] = parent[i][0];
    parent[i][0] = 0;

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
      theta[i] = minTheta;
    }

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
          minDiff[j] = diffCost2;
          theta[j] = curTheta;
          parent[j][0] = i;
        }
      }
    }
  }
  cout << "cost1 : " << dist[d][0] << " cost2 : " << dist[d][1] << endl;
  printPath(g, parent, d, s);
}

int main() {
  srand(time(NULL));
  int source = 1, dest = 4, n = dest + 1;
  auto g = buildgraph(dest);

  vector<array<double, 2>> dist(n, {DBL_MAX, DBL_MAX}); // {d1,d2}
  vector<array<int, 2>> parent(n, {0, -1});             // {Cpred,pred}
  bool havePath = dijkstra(g, source, dest, dist, parent);
  // 這張圖起點到終點沒連通
  if (!havePath)
    return 0;

  RLBSP(g, dist, parent, source, dest);
}
