#include <bits/stdc++.h>
#include "../formula.h"
#include "qLeap.h"
using namespace std;
using ad = array<double,4>;

class qLeap{
public:
    qLeap(string name,double th,double beta):filename(name),threshold(th),beta(beta){;}
    void run(){
        auto start = chrono::high_resolution_clock::now();
        init();
        buildPurifyTable();
        buildGraph();
        extendDijkstra();
        string error = "";
        if (path.size() < 2){
            error = "error:no path";
        }
        bool havePath = purifyDecision();
        if (!havePath && error.empty()){
            error = "error:edge without enough memory";
        }
        bool enoughMemory = checkPath();
        if (!enoughMemory && error.empty()){
            error = "error:exceed memory limit";
        }
        auto end = chrono::high_resolution_clock::now();
        auto diff = end - start;
        double time = chrono::duration<double>(diff).count();
        write_path_info(time,error);
    }
private:
    int n;
    int s = 0,d;
    double threshold,beta;
    string filename;
    vector<int> path;
    vector<int> purifyTime;
    unordered_map<int,vector<int>> g;
    vector<double> x,y,dis,memory,swapping_prob;
    vector<vector<vector<purifyLabel>>> purifyTable; // {fidelity, fidelity improve, prob}
    double distance(int i, int j) {
        return sqrt(powf(x[i] - x[j], 2) + powf(y[i] - y[j], 2));
    }

    void init(){
        ifstream in;
        in.open(filename);
        in >> n;
        d = n-1;
        x.resize(n);
        y.resize(n);
        dis.resize(n, 0);
        memory.resize(n);
        swapping_prob.resize(n);

        for (int i = 0; i < n; i++) {
            in >> x[i] >> y[i] >> memory[i] >> swapping_prob[i];
            cout << "Node " << i << " memory : " << memory[i] << " swappingSuccessProb : " << swapping_prob[i] << endl;
        }

        in.close();

        for (int i = 1; i < n; i++)
            dis[i] = dis[i - 1] + distance(i - 1, i);
    }
    void buildPurifyTable(){
        // 2
        purifyTable.resize(n,vector<vector<purifyLabel>>(n));
        for(int i=0; i<n; i++){
            for(int j=i+1; j<n; j++){
                double entangleFidelity = entangle_fidelity(dis[j]-dis[i],beta);
                double entangleProb = entangle_success_prob(dis[j] - dis[i]);
                double curProb = entangleProb, curFidelity = entangleFidelity;

                purifyTable[i][j].push_back({curFidelity,0,curProb});
                for(int k=1; k<=min(memory[i],memory[j]); k++){
                    curProb = curProb * entangleProb * purify_success_prob(curFidelity,entangleFidelity);
                    curFidelity = purify_fidelity(curFidelity,entangleFidelity);
                    purifyTable[i][j].push_back({curFidelity, curFidelity - purifyTable[i][j][k-1].fidelity, curProb});
                }
            }
        }

        // print
        /*
        for(int i=0; i<n; i++){
            for(int j=i+1; j<n; j++){
                cout<<"i : "<< i<<" j : "<< j<<endl;
                for(int k=0; k<purifyTable[i][j].size(); k++){
                    cout<<purifyTable[i][j][k].fidelity<<" "<<purifyTable[i][j][k].fidelity_improvement<<" "<<purifyTable[i][j][k].prob<<endl;
                }
            }
        }
        */
    }
    void buildGraph(){
        for(int i=0; i<n; i++){
            for(int j=i+1; j<n; j++){
                int purifyTimes = purifyTable[i][j].size();
                // check 3
                if (purifyTable[i][j][purifyTimes-1].fidelity < threshold)
                    continue;
                double entangleFidelity = entangle_fidelity(dis[j] - dis[i],beta);
                double entangleProb = entangle_success_prob(dis[j] - dis[i]);
                double curProb = entangleProb, curFidelity = entangleFidelity;
                g[i].push_back(j);
            }
        }
    }

    void getPath(vector<double> &parent, int curNode){
        if (curNode == s) {
            path.push_back(s);
            return;
        }
        getPath(parent,parent[curNode]);
        path.push_back(curNode);
        return;
    }
    void extendDijkstra(){

        for(int i=0; i<n; i++){
            path.push_back(i);
        }

        // priority_queue<pqLabel> pq;
        // pq.push({1,1,s,-1});
        // vector<bool> used(n);
        // vector<double> parent(n,-1);
        // while(!pq.empty()){
        //     pqLabel temp = pq.top(); pq.pop();
        //     double curFidelity = temp.fidelity, curProb = temp.probability;
        //     int node = temp.node, p = temp.parent;
        //     if (used[node])
        //         continue;
        //     used[node] = true;
        //     parent[node] = p;
            
        //     for(auto &nxt:g[node]){
        //         if (used[nxt])
        //             continue;
        //         //  若走到非終點，需做swapping，所以memory至少要>=2
        //         else if (nxt != d && memory[nxt] < 2)
        //             continue;
        //         pq.push({curFidelity * purifyTable[node][nxt][0].fidelity, curProb * purifyTable[node][nxt][0].prob, nxt, node});
        //     }
        // }
        // if (!used[d])
        //     return;
        // getPath(parent,d);
        // purifyTime.resize(path.size()-1,0);
    }

    bool purifyDecision(){
        int l = path.size();
        double fidelity_avg = powf(threshold,1/(double)(l-1));

        for(int i=0; i<l-1; i++){
            int curNode = path[i];
            int nxtNode = path[i+1];

            while (purifyTime[i] < purifyTable[curNode][nxtNode].size() && (purifyTable[curNode][nxtNode][purifyTime[i]].fidelity < fidelity_avg)){
                ++purifyTime[i];
            }
            
            if (purifyTime[i] >= purifyTable[curNode][nxtNode].size())
                return false;
        }
        return true;
    }

    bool checkPath(){
        int l = path.size();

        // 起點終點的memory檢查
        if ((memory[s] < (purifyTime[0]+1)) || (memory[d] < (purifyTime[l-2]+1)))
            return false;
        // 中間的memory檢查
        for(int i=1; i<l-1; i++){
            if (memory[path[i]] < (purifyTime[i-1] + purifyTime[i] + 2))
                return false;
        }
        return true;
    }

    void write_path_info(double time,string error){
        ofstream ofs;
        ofs.open("output/qLeap.txt");
            if (!ofs.is_open()) {
            cout << "error to open qLeap.txt" << endl;
            return;
        }
        if (!error.empty()){
            ofs<<error<<endl;
            ofs<<"Time:"<<time<<endl;
            return;
        }
        ofs << "Path: ";
        for (int i = 0; i < path.size(); i++) {
            ofs << path[i] << " ";
        }
        ofs << endl << "PurTimes: ";
        for (auto &c : purifyTime)
        ofs << c << " ";
        ofs << endl;
        int l = path.size();
        double fidelityAns = 1, ProbAns = 1;
        for(int i=0; i<l-1; i++){
            int curNode = path[i];
            int nxtNode = path[i+1];

            fidelityAns *= purifyTable[curNode][nxtNode][purifyTime[i]].fidelity;
            ProbAns *= purifyTable[curNode][nxtNode][purifyTime[i]].prob;
            if (i != 0)
                ProbAns *= swapping_prob[curNode];
        }
        ofs << "Fidelity: " << fidelityAns << endl;
        ofs << "Probability: " << ProbAns << endl;
        ofs << "Time: " << time << endl;
    }
};

int main(int argc, char* argv[]){
    if (argc != 3){
        cout<<"run with ./qLeap {filename} {threshold}";
        return 1;
    }
    double beta = 0.00438471;
    qLeap qleap(argv[1],atof(argv[2]),beta);
    qleap.run();
}