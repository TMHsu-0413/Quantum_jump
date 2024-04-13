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
void printALLACP();
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
      
      // 計算多次 purification 的 fidelity, probability
      double maxF = 0; 
      vector<double>tmpPurFi, tmpPurPrb;
      tmpPurFi.push_back(curFidelity);
      tmpPurPrb.push_back(curProb);
      double tmpF = curFidelity, tmpP = curProb;
      for(int purMem=1; purMem<=qNode[i].mem && purMem<=qNode[j].mem; purMem++){
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
  // printPurifiTable();
}

pair<double, double> countFB(vector<int>path, vector<int>purTimes){
  pair<double, double>cur = {1, 1};
  bool memOverFlow = 0;
  for(int i=0; i<path.size()-1; i++){
    int curNode = path[i];
    int nextNode = path[i+1];
    int edgeIdx = qNode[curNode].findEdge(curNode, nextNode);

    if(qNode[curNode].memUsed > qNode[curNode].mem || qNode[nextNode].memUsed > qNode[nextNode].mem){
      memOverFlow = 1;
      break;
    }
    
    // debug 
    if(purTimes[i] >= qNode[curNode].neighbor[edgeIdx].purFidelity.size()){
      cout << "node " << curNode << " to " << nextNode << '\n';
      cout << purTimes[i] << " " << qNode[curNode].neighbor[edgeIdx].purFidelity.size() << '\n';
      for(auto x:qNode[curNode].neighbor[edgeIdx].purFidelity){
        cout << x << " ";
      }
      cout << '\n';
    }
    assert(purTimes[i] < qNode[curNode].neighbor[edgeIdx].purFidelity.size());
    cur.first *= qNode[curNode].neighbor[edgeIdx].purFidelity[purTimes[i]];
    cur.second *= qNode[curNode].neighbor[edgeIdx].purProb[purTimes[i]];
    if(i > 0){
      cur.second *= qNode[i].swappingProb;
    }
  }
  if(memOverFlow){
    return {0, 0};
  }
  return cur;
}

vector<int> bfsSP(int s, int d){
	// return shortest path with s->d
	queue<int> que;
	vector<bool> vis(numQn, 0);
	vector<int> par(numQn, -1);
	vector<int> ret;
	que.push(s);
	int cur;
	while (!que.empty()){
		cur = que.front();
		que.pop();
		if (cur == d){
			break;
		}
		for (auto it = qNode[cur].neighbor.begin(); it != qNode[cur].neighbor.end(); it++){
			if (!vis[it->to] && it->canUse == 1){
				vis[it->to] = 1;
				par[it->to] = cur;
				que.push(it->to);
			}
		}
	}
	if (cur != d)
		return {};
	while (cur != s){
		ret.push_back(cur);
		cur = par[cur];
	}
	ret.push_back(s);
	reverse(ret.begin(), ret.end());
	return ret;
}

vector<vector<int>> yenKSP(int src, int dest, int K, int H){
	vector<vector<int>> A; // KSP set
	priority_queue<vector<int>, vector<vector<int>>, CompareVector> B;

	vector<int> initialPath = bfsSP(src, dest);
	if (initialPath.empty()){
		return A;
	}

	A.push_back(initialPath);

	bool overSize = 0;

	for (int k = 1; k < K && !overSize; ++k){
		for (size_t i = 0; i < A[k - 1].size() - 1 && !overSize; ++i){
			// store origin statue (canUse)
			vector<Node> qNodeBackup = qNode;
			int spurNode = A[k - 1][i];
			vector<int> rootPath(A[k - 1].begin(), A[k - 1].begin() + i + 1);

			for (auto &path : A){
				if (rootPath.size() < path.size() && equal(rootPath.begin(), rootPath.end(), path.begin())){
					int edgeIdx = qNode[path[i]].findEdge(path[i], path[i + 1]);
					qNode[path[i]].neighbor[edgeIdx].canUse = 0;
				}
			}

			vector<int> spurPath = bfsSP(spurNode, dest);

			if (!spurPath.empty()){
				vector<int> totalPath = rootPath;
				totalPath.insert(totalPath.end(), spurPath.begin() + 1, spurPath.end()); // prevent add multiple spurNode
        B.push(totalPath);
			}

			// recover
			qNode = qNodeBackup;
		}

		if (B.empty())
			break;

		A.push_back(B.top());
		B.pop();
	}

	return A;
}

int purifyDicision(vector<int>& path, vector<int>& purTimes, priority_queue<pair<double, int>, vector<pair<double, int>>>& purDicision){
  int edgeChoose = -1;
  while(!purDicision.empty()){
    edgeChoose = purDicision.top().second;
    purDicision.pop();
    int curNode = path[edgeChoose];
    int nextNode = path[edgeChoose+1];
    int edgeIdx = qNode[curNode].findEdge(curNode, nextNode);
    vector<double>edgePurF = qNode[curNode].neighbor[edgeIdx].purFidelity;

    int curNodeMemUsed = purTimes[edgeChoose] + qNode[curNode].memUsed;
    int nextNodeMemUsed = purTimes[edgeChoose] + qNode[nextNode].memUsed;

    if(curNodeMemUsed <= qNode[curNode].mem && nextNodeMemUsed <= qNode[nextNode].mem && \
      purTimes[edgeChoose] < edgePurF.size()-1){ 
      return edgeChoose;
    }
  }   
  return -1;      
}
void updEdgeCost(){
  // 如果 path Fidelity 沒過，就不斷找 purify 後 F 差異最大的進行 purify。
  for(int i=0; i<kSP.size(); i++){
    vector<int>path = kSP[i];
    vector<int>purTimes(path.size()+1, 0);
    auto tmp = countFB(path, purTimes);
    double curF = tmp.first, curP = tmp.second;
    priority_queue<pair<double, int>, vector<pair<double, int>>> purDicision;
    // 
    bool pathFailed = 0;
    while(curF < threshold && curF){
      double purIdx = 0, diff=0;
      for(int i=0; i<path.size()-1; i++){
        int curNode = path[i];
        int nextNode = path[i+1];
        int edgeIdx = qNode[curNode].findEdge(curNode, nextNode);
        vector<double>edgePurF = qNode[curNode].neighbor[edgeIdx].purFidelity;
        assert(edgePurF == qNode[curNode].neighbor[edgeIdx].purFidelity);
        if(purTimes[i] < edgePurF.size()-1){ 
          purDicision.push({edgePurF[purTimes[i+1]] - edgePurF[purTimes[i]], i});  // push i, i is edge of path[i]->path[i+1]
        }  
      }  
      // update fidelity, prb    
      int edgeChoose = purifyDicision(path, purTimes, purDicision); // node a -> node b
      if(edgeChoose == -1){
        pathFailed = 1;
        break;
      } else {
        purTimes[edgeChoose]++;
        qNode[path[edgeChoose]].memUsed++;
        qNode[path[edgeChoose+1]].memUsed++;
        auto res = countFB(path, purTimes);
        curF = res.first;
        curP = res.second;
      }  
    } 
    if(curF >= threshold && !pathFailed){
      acPaths.push_back({curF, curP, kSP[i], purTimes});
    }
  }
}
void delBKSP(int h){
  vector<vector<int>>tmp;
  for(int i=0; i<kSP.size(); i++){
    if(kSP[i].size() == h){
      tmp.push_back(kSP[i]);
    }
  }
  kSP = tmp;
}

void routing(){
  minCost = bfsSP(0, numQn-1).size();
  maxCost = numQn+1;
  for(int i=minCost; i<maxCost; i++){
    if(!kSP.empty())
      kSP.clear();
    kSP = yenKSP(0, numQn-1, numQn*2, i);
    // 不能限制 k 剛好是指定長度，可能會漏，但我猜應該只要多找一點在砍掉就可以了，另外注意 i 是 node 數量不是 hop 數
    delBKSP(i);
    updEdgeCost();
  }  
}
void init(){
  buildDisTable();
  buildGraph(); 
}
void input(){
  double inputX, inputY, inputSwProb;
  int inputMem;
  cin >> numQn;
  for(int i=0; i<numQn; i++){
    cin >> inputX >> inputY >> inputMem >> inputSwProb;
    qNode.push_back({i, inputX, inputY, inputMem, inputSwProb});
  }
}
int main(int argc, char* argv[]){
  if(freopen(argv[1], "r", stdin) == nullptr){
    cout << argv[1] << " File Open Error" << '\n';
  }
  double START, END; START = clock();
  input();
  fclose(stdin);
  init();
  routing(); 
  sort(acPaths.begin(), acPaths.end());
  printACP();
  END = clock();
  cout << "Time: " << (END-START)/CLOCKS_PER_SEC << "s\n";
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
void printKSP(){
  for(int i=0; i<kSP.size(); i++){
    cout << "kSP " << i << ": ";
    for(auto x:kSP[i]){
      cout << x << " ";
    }
    cout << '\n';
  }
}
void printACP(){
  auto x = acPaths[0];
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
void printALLACP(){
  for(int i=0; i<acPaths.size(); i++){
    auto x = acPaths[i];
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
      cout << "Node " << i << " mem " << qNode[i].mem << '\n';
      cout << "Node " << nxt.to << " mem " << qNode[nxt.to].mem << '\n';
      for(int j=0; j<nxt.purFidelity.size(); j++){
        cout << "pur times = " << j << "\nF = " << nxt.purFidelity[j] << "\n";
      }
    }
    cout << '\n';
  }
}