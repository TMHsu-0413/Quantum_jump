#include"header.h"
#include"formula.cpp"
#include"../parameter.cpp"
#define BETA 0.00438471
using namespace std;
vector<Node>qNode;
unordered_map<pair<int, int>, double, pairHash>disTable;
vector<vector<int>>kSP;
vector<acceptPath>acPaths;
int routingReq, numQn, minCost, maxCost;
struct CompareVector {
    bool operator()(const vector<int>& a, const vector<int>& b) const {
        if (a.size() == b.size()) {
            return a > b; // 如果大小相等，按字典序比较
        }
        return a.size() > b.size(); // 首先基于大小比较
    }
};
void input(int num, void* fileName);
void printDisTable();
void printGraph();
void printSP();
void printKSP();
void printPath(vector<int> &path);
void printACP();
void printPurifiTable();
void printNodeInfo(){
  for(int i=0; i<numQn; i++){
    cout << "node " << i << " x " << qNode[i].x << " y " << qNode[i].y << " neighbor " << qNode[i].neighbor.size() << '\n';
    for(int j=0; j<qNode[i].neighbor.size(); j++){
      cout << qNode[i].neighbor[j].to << " ";
    }
    cout << '\n';

  }
}
void buildDisTable(){
  for(int i=0; i<numQn-1; i++){
    disTable[make_pair(i, i+1)] = distence(qNode[i], qNode[i+1]);
  }
  for(int i=0; i<numQn-2; i++){
    for(int j=i+2; j<numQn; j++){
      double tmpDis = 0;
      for(int k=i; k<j; k++){
        tmpDis += disTable[{k, k+1}];
      }
      disTable[make_pair(i, j)] = tmpDis;
    }
  }
  // !! distence 需要重算，要透過通道
  // 計算相鄰點的，在加上去
}
void buildGraph(){
  for(int i=0; i<numQn; i++){
    for(int j=i+1; j<numQn; j++){
      qNode[i].memUsed++; // use to swap
      qNode[j].memUsed++; // use to swap
      double curProb = entangle_success_prob(disTable[{i, j}]);
      double enProb = curProb;
      double curFidelity = entangle_fidelity(disTable[{i, j}], BETA); // define
      double enFidelity = curFidelity;
      int edgeIdx = qNode[i].neighbor.size();
      int edgeIdx2 = qNode[j].neighbor.size();
      vector<double>fi, prb;
      fi.push_back(curFidelity);
      prb.push_back(curProb);
      qNode[i].addNeighbor({edgeIdx, i, j, curProb, curFidelity, fi, prb}); // 只是紀錄可以連，實際上沒有 entangle，所以 memUsed = 0
      // qNode[j].addNeighbor({edgeIdx2, j, i, curProb, curFidelity, fi, prb}); 答案會變差
      
      // 計算多次 purification 的 fidelity, probability
      double maxF = 0; 
      vector<double>tmpPurFi, tmpPurPrb;
      tmpPurFi.push_back(curFidelity);
      tmpPurPrb.push_back(curProb);
      double tmpF = curFidelity, tmpP = curProb;
      for(int purMem=1; purMem<=qNode[i].mem && purMem<qNode[j].mem; purMem++){
        double purP = purify_success_prob(tmpF, enFidelity) * enProb * tmpP; 
        double purF = purify_fidelity(tmpF, enFidelity);  
        tmpPurFi.push_back(purF);
        tmpPurPrb.push_back(purP);
        maxF = max(maxF, purF);
        tmpF = purF; // update fidelity
        tmpP = purP; // update probability
      }
      // Delete all edges(u,v) from G, if Fpur max(u,v)<Fth;
      if(maxF >= threshold){
        // add path
        qNode[i].neighbor[edgeIdx].purFidelity = tmpPurFi;
        qNode[i].neighbor[edgeIdx].purProb = tmpPurPrb;
      }
    }
  }
}

pair<double, double> countFB(vector<int>path, vector<int>purTimes){
  pair<double, double>cur = {1, 1};
  for(int i=0; i<path.size()-1; i++){
    int curNode = path[i];
    int nextNode = path[i+1];
    int edgeIdx = qNode[curNode].findEdge(curNode, nextNode);
    cur.first *= qNode[curNode].neighbor[edgeIdx].purFidelity[purTimes[i]];
    cur.second *= qNode[curNode].neighbor[edgeIdx].purProb[purTimes[i]];
    if(i > 0){
      double swappingProb = swapping_success_prob();
      cur.second *= swappingProb;
    }
  }
  return cur;
}

vector<int> extendDijkstra(int src, int dest) {
  vector<double> maxFidelity(qNode.size(), 0.0);
  vector<int> parent(qNode.size(), -1);
  auto cmp = [&maxFidelity](int l, int r) { return maxFidelity[l] < maxFidelity[r]; };
  priority_queue<int, vector<int>, decltype(cmp)> queue(cmp);

  maxFidelity[src] = 1.0;
  queue.push(src);

  while (!queue.empty()) {
    int curNode = queue.top();
    queue.pop();

    if (curNode == dest) {
      break; 
    }

    for (auto& edge : qNode[curNode].neighbor) {
      if (edge.canUse) {
        double newFidelity = maxFidelity[curNode] * edge.fidelity;

        if (newFidelity > maxFidelity[edge.to]) {
            maxFidelity[edge.to] = newFidelity;
            parent[edge.to] = curNode;
            queue.push(edge.to);
        }
      }
    }
  }

  vector<int> path;
  for (int at = dest; at != -1; at = parent[at]) {
    path.push_back(at);
  }
  reverse(path.begin(), path.end());
  if (!path.empty() && path[0] == src) {
    return path;
  }

  return {}; 
}

void updEdgeCost(){
  auto path = kSP[0];
  int len = path.size();
  vector<int>purTimes(len, 0);
  double minEdgeF = pow(threshold, 1/(len-1));
  bool memOverFlow = 0;
  for(int i=0; i<len-1; i++){
    int edgeIdx = qNode[path[i]].findEdge(path[i], path[i+1]); 
    double curEdgeF = qNode[path[i]].neighbor[edgeIdx].purFidelity[purTimes[i]];
    while(curEdgeF < minEdgeF){
      purTimes[i]++;
      qNode[path[i]].memUsed++;
      qNode[path[i+1]].memUsed++;
      if(purTimes[i] >= qNode[path[i]].neighbor[edgeIdx].purFidelity.size() || qNode[path[i]].memUsed > qNode[path[i]].mem || \
          qNode[path[i+1]].memUsed > qNode[path[i+1]].mem){
        memOverFlow = 1;
        break;
      }
      curEdgeF = qNode[path[i]].neighbor[edgeIdx].purFidelity[purTimes[i]];
    }
  }
  if(memOverFlow){      
    return;
  }
  auto cur = countFB(path, purTimes);
  acPaths.push_back({cur.first, cur.second, path, purTimes});
}

void routing(){
  kSP.push_back(extendDijkstra(0, numQn-1));
  // printPath(kSP[0]);
  updEdgeCost();
}
void init(){
  buildDisTable();
  buildGraph(); 
  // 4 Q←Priority queue according to value of min_cost;
  // 5 Find shortest path on G with H(min) by using BFS;
}
void input(){
  // node limit 70
  // test tle 
  double inputX, inputY, inputEnProb;
  int inputMem;
  cin >> numQn;
  for(int i=0; i<numQn; i++){
    cin >> inputX >> inputY >> inputMem >> inputEnProb;
    qNode.push_back({i, inputX, inputY, inputMem, inputEnProb});
  }
}
int main(int argc, char* argv[]){
  if(freopen(argv[1], "r", stdin) == nullptr){
    cout << argv[1] << " File Open Error" << '\n';
  }
  // 紀錄算法實際執行時間
  // 輸出格式 : path
  input();
  fclose(stdin);
  double START, END; START = clock();
  init();
  routing(); 
  printACP();
  END = clock();  
  cout << "Time: " << (END-START)/CLOCKS_PER_SEC << "s" << '\n';
}
void printPath(vector<int> &path){
	for (auto &p : path){
		cout << p << "  ";
	}
	cout << endl;
}
void printDisTable(){
  for(int i=0; i<numQn; i++){
    for(int j=0; j<numQn; j++){
      cout << "Node " << i  << " to " << j << " : "<< disTable[{i, j}] << "\n";
    }
    cout << '\n';
  }
}
void printGraph(){
  for(int i=0; i<numQn; i++){
    cout << "Node " << qNode[i].id << ":\n";
    for(auto it=qNode[i].neighbor.begin(); it!=qNode[i].neighbor.end(); it++){
      cout << "(" << it->from << ", " << it->to << ")\nprob: " << it->prob << "\nfidelity: " << it->fidelity << "\n";
      cout << "purFidelity: ";
      for(auto it2=it->purFidelity.begin(); it2!=it->purFidelity.end(); it2++){
        cout << *it2 << " ";
      }
      cout << '\n';
    }
    cout << '\n';
  }
}
void printACP(){
  for(auto x:acPaths){
    cout << "Path: ";
    for(auto y:x.path){
      cout << y << " ";
    }
    cout << '\n';
    cout << "PurTimes: ";
    for(int i=0; i<x.path.size()-1; i++){
      cout << x.purTimes[i] << " ";
    }
    cout << '\n';
    auto tmp = countFB(x.path, x.purTimes);
    cout << "Fidelity: " << tmp.first << '\n';
    cout << "Probability: " << tmp.second << '\n';
  }
}
void printPurifiTable(){
  for(int i=0; i<numQn; i++){
    for(auto nxt: qNode[i].neighbor){
      cout << "Node " << i << " to " << nxt.to << " : ";
      for(int j=0; j<nxt.purFidelity.size(); j++){
        cout << "pur times = " << j << "\nF = " << nxt.purFidelity[j] << "\n";
      }
    }
    cout << '\n';
  }
}