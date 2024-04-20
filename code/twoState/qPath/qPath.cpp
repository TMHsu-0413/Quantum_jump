#include "formula.cpp"
#include "header.h"
#define BETA 0.00438471
using namespace std;
vector<Node> qNode;
vector<acceptPath> acPaths;
vector<int> path;
int routingReq, numQn, minCost, maxCost;
double threshold;
int edgeIdMap[1000][1000];
double dis[100][100];
void input(int num, void *fileName);
void printACP(double time);
void printPurifiTable();
void buildDisTable() {
  for (int i = 0; i < numQn - 1; i++) {
    dis[i][i + 1] = distence(qNode[i], qNode[i + 1]);
  }
}
void buildGraph() {
  int edgeCnt = 0;
  for (int i = 0; i < numQn; i++) {
    edgeCnt = 0;
    int j=i+1;
    edgeIdMap[i][j] = edgeCnt++;

    double curProb = entangle_success_prob(dis[i][j]);
    double enProb = curProb;
    double curFidelity = entangle_fidelity(dis[i][j], BETA); // define
    double enFidelity = curFidelity;
    int edgeIdx = qNode[i].neighbor.size();
    int edgeIdx2 = qNode[j].neighbor.size();
    vector<double> fi, prb;
    fi.push_back(curFidelity);
    prb.push_back(curProb);
    qNode[i].addNeighbor(
        {edgeIdx, i, j, curProb, curFidelity, fi,
         prb}); // 只是紀錄可以連，實際上沒有 entangle，所以 memUsed = 0

    double maxF = 0;
    vector<double> tmpPurFi, tmpPurPrb;
    tmpPurFi.push_back(curFidelity);
    tmpPurPrb.push_back(curProb);
    double tmpF = curFidelity, tmpP = curProb;
    for (int purMem = 1; purMem <= qNode[i].mem && purMem <= qNode[j].mem;
         purMem++) {
      double purP = purify_success_prob(tmpF, enFidelity) * enProb * tmpP;
      double purF = purify_fidelity(tmpF, enFidelity);
      tmpPurFi.push_back(purF);
      tmpPurPrb.push_back(purP);
      maxF = max(maxF, purF);
      tmpF = purF; // update fidelity
      tmpP = purP; // update probability
    }

    if (maxF >= threshold) {
      assert(qNode[i].neighbor.size() > edgeIdx);
      qNode[i].neighbor[edgeIdx].purFidelity = tmpPurFi;
      qNode[i].neighbor[edgeIdx].purProb = tmpPurPrb;
    }
  }
}
void resetMemUsed() {
  for (int i = 0; i < numQn; i++) {
    qNode[i].memUsed = 0;
  }
}

bool checkMem(int curNode, int nextNode) {
  if(curNode >= numQn || nextNode >= numQn){
    cout << "node exceed : " << curNode << ' ' << nextNode << '\n';
  }
  if (qNode[curNode].memUsed > qNode[curNode].mem ||
      qNode[nextNode].memUsed > qNode[nextNode].mem) {
    cout << "curNode " << curNode << '\n';
    cout << "mem overflow " << qNode[curNode].memUsed << " "
              << qNode[nextNode].memUsed << '\n';
    cout << "mem i = " << qNode[curNode].mem
              << " j = " << qNode[nextNode].mem << '\n';
    resetMemUsed();
    return 0;
  }
  return 1;
}

pair<double, double> countFB(vector<int> purTimes) {
  pair<double, double> cur = {1, 1};
  bool memOverFlow = 0;
  for (int i = 0; i < path.size() - 1; i++) {
    int curNode = path[i];
    int nextNode = path[i + 1];
    int edgeIdx = edgeIdMap[curNode][nextNode];
    assert(checkMem(curNode, nextNode));
    assert(purTimes[i] < qNode[curNode].neighbor[edgeIdx].purFidelity.size());
    cur.first *= qNode[curNode].neighbor[edgeIdx].purFidelity[purTimes[i]];
    cur.second *= qNode[curNode].neighbor[edgeIdx].purProb[purTimes[i]];
    if (i > 0) {
      cur.second *= qNode[path[i]].swappingProb;
    }
  }
  return cur;
}

int purifyDicision(
    vector<int> &path, vector<int> &purTimes,
    priority_queue<pair<double, int>, vector<pair<double, int>>> &purDicision) {
  int edgeChoose = -1;

  while (!purDicision.empty()) {
    edgeChoose = purDicision.top().second;
    double diff = purDicision.top().first;
    purDicision.pop();
    int curNode = path[edgeChoose];
    int nextNode = path[edgeChoose + 1];
    assert(curNode < numQn && nextNode < numQn);
    int edgeIdx = edgeIdMap[curNode][nextNode];
    assert(qNode[curNode].neighbor.size() > edgeIdx);
    vector<double> edgePurF = qNode[curNode].neighbor[edgeIdx].purFidelity;

    int curNodeMemUsed = 1 + qNode[curNode].memUsed; // purTimes[edgeChoose]
    int nextNodeMemUsed = 1 + qNode[nextNode].memUsed;

    if (curNodeMemUsed <= qNode[curNode].mem &&
        nextNodeMemUsed <= qNode[nextNode].mem) {
      return edgeChoose; // && purTimes[edgeChoose] < edgePurF.size() - 1
    }
  }
  return -1;
}
void updEdgeCost() {

  vector<int> purTimes(path.size(), 0);
  auto tmp = countFB(purTimes);
  double curF = tmp.first, curP = tmp.second;
  priority_queue<pair<double, int>, vector<pair<double, int>>> purDicision;
  // add swap mem used
  for (int i = 0; i < path.size() - 1; i++) {
    qNode[path[i]].memUsed++;
    qNode[path[i + 1]].memUsed++;
  }
  bool pathFailed = 0;
  while (curF < threshold && curF) {
    // push {fidelity diff, edge idx} into pq
    for (int i = 0; i < path.size() - 1; i++) {
      int curNode = path[i];
      int nextNode = path[i + 1];
      int edgeIdx = edgeIdMap[curNode][nextNode];
      vector<double> edgePurF = qNode[curNode].neighbor[edgeIdx].purFidelity;
      if (purTimes[i] + 1 < edgePurF.size()) {
        purDicision.push({edgePurF[purTimes[i] + 1] - edgePurF[purTimes[i]],
                          i}); // push i, i is edge of path[i]->path[i+1]
      }
    }
    // update fidelity, prb
    int edgeChoose = purifyDicision(path, purTimes, purDicision);
    if (edgeChoose < 0 || edgeChoose > path.size() - 1) {
      pathFailed = 1;
      break;
    } else {
      assert(checkMem(path[edgeChoose], path[edgeChoose + 1]));
      purTimes[edgeChoose]++;
      qNode[path[edgeChoose]].memUsed++;
      qNode[path[edgeChoose + 1]].memUsed++;
      auto res = countFB(purTimes);
      curF = res.first;
      curP = res.second;
    }
    while(!purDicision.empty()) purDicision.pop();
  }
  resetMemUsed();
  if (curF >= threshold && !pathFailed) {
    acPaths.push_back({curF, curP, path, purTimes});
  }
}

void routing() {
  buildDisTable();
  buildGraph();
  for(int i=0; i<numQn; i++){
    path.push_back(i);
    qNode[i].memUsed = 0;
  }
  updEdgeCost();
}
void input() {
  double inputX, inputY, inputSwProb;
  int inputMem;
  cin >> numQn;
  for (int i = 0; i < numQn; i++) {
    cin >> inputX >> inputY >> inputMem >> inputSwProb;
    qNode.push_back({i, inputX, inputY, inputMem, inputSwProb});
  }
}

int main(int argc, char *argv[]) {
  if (freopen(argv[1], "r", stdin) == nullptr) {
    cout << argv[1] << " File Open Error" << '\n';
  }
  if (argc < 3) {
    cout << "need threshold\n";
    return 0;
  }
  threshold = atof(argv[2]);
  cout << "threshold: " << threshold << '\n';
  input();
  auto start = chrono::high_resolution_clock::now();
  routing();
  auto end = chrono::high_resolution_clock::now();
  auto diff = end - start;
  double time = chrono::duration<double>(diff).count();
  printACP(time);
  // printPurifiTable();
  fclose(stdin);
}
void printACP(double time) {
  ofstream ofs;
  ofs.open("output/qPath.txt");
  if (!ofs.is_open()) {
    cout << "error to open output.txt" << endl;
    return;
  }
  if (acPaths.empty()) {
    ofs << "error:no path\n";
    ofs << "Time:" << time << endl;
    return;
  }
  auto x = acPaths[0];
  ofs << "Path: ";
  for (auto y : x.path) {
    ofs << y << " ";
  }
  ofs << '\n';
  ofs << "PurTimes: ";
  for (int i = 0; i < x.path.size() - 1; i++) {
    ofs << x.purTimes[i] << " ";
  }
  ofs << '\n';
  auto tmp = countFB(x.purTimes);
  ofs << "Fidelity: " << tmp.first << '\n';
  ofs << "Probability: " << tmp.second << '\n';
  ofs << "Time: " << time << '\n';
}
void printPurifiTable() {
  for (int i = 0; i < numQn; i++) {
    for (auto nxt : qNode[i].neighbor) {
      cout << "Node " << i << " to " << nxt.to << "\n";
      cout << "Node " << i << " mem " << qNode[i].mem << '\n';
      cout << "Node " << nxt.to << " mem " << qNode[nxt.to].mem << '\n';
      for (int j = 0; j < nxt.purFidelity.size(); j++) {
        cout << "pur times = " << j << "\nF = " << nxt.purFidelity[j]
             << "\nP = " << nxt.purProb[j] << "\n";
      }
    }
    cout << '\n';
  }
}