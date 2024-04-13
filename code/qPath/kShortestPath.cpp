#include "header.h"
#include "formula.cpp"
#include "../parameter.cpp"
using namespace std;
vector<Node> qNode;
unordered_map<pair<int, int>, double, pairHash> disTable;
vector<vector<int>> kSP;
vector<acceptPath> acPaths;
int routingReq, numQn, minCost, maxCost;
struct CompareVector {
	bool operator()(const vector<int>& a, const vector<int>& b) const {
		if (a.size() == b.size()) {
			return a > b; // 如果大小相等，按字典序比较
		}
		return a.size() > b.size(); // 首先基于大小比较
	}
};

void testInit(){
	numQn = 6;
	for (int i = 0; i < 6; i++){
		Node *tmp = new Node;
		tmp->id = i;
		qNode.push_back(*tmp);
	}
	qNode[0].addNeighbor({0, 0, 1, 0, 0, {0}, {0}}); // c
	disTable[{0, 1}] = 3;
	qNode[0].addNeighbor({1, 0, 2, 0, 0, {0}, {0}});
	disTable[{0, 2}] = 2;
	qNode[1].addNeighbor({2, 1, 3, 0, 0, {0}, {0}}); // d
	disTable[{1, 3}] = 4;
	qNode[2].addNeighbor({3, 2, 1, 0, 0, {0}, {0}}); // e
	disTable[{2, 1}] = 1;
	qNode[2].addNeighbor({4, 2, 3, 0, 0, {0}, {0}});
	disTable[{2, 3}] = 2;
	qNode[2].addNeighbor({5, 2, 4, 0, 0, {0}, {0}});
	disTable[{2, 4}] = 3;
	qNode[3].addNeighbor({6, 3, 4, 0, 0, {0}, {0}}); // f
	disTable[{3, 4}] = 2;
	qNode[3].addNeighbor({7, 3, 5, 0, 0, {0}, {0}});
	disTable[{3, 5}] = 1;
	qNode[4].addNeighbor({8, 4, 5, 0, 0, {0}, {0}});
	disTable[{4, 5}] = 2;
}

void printPath(vector<int> &path){
	for (auto &p : path){
		cout << p << "  ";
	}
	cout << endl;
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
	cout << "initial path: ";
	printPath(initialPath);

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
				cout << k << ' ' << i << ' ' << "total path :";
				printPath(totalPath);
				if(totalPath.size() <= H)
					B.push(totalPath);
				else {
					overSize = 1;
					break;
				}
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

int main(){
	testInit();
	vector<int> path = bfsSP(0, 5);
	vector<vector<int>> A = yenKSP(0, 5, 20, 5);
	sort(A.begin(), A.end(), [](vector<int> &a, vector<int> &b){return a.size() < b.size();});
	for (auto &path : A){
		for (auto &p : path){
			cout << p << "  ";
		}
		cout << endl;
	}

	return 0;
}