#include"header.h"
#include"formula.cpp"
#include"../parameter.cpp"
using namespace std;
int main(){
    double BETA = 0.00438471;
    double dis = 0;
    double dis02 = 230.351;
    double dis01 = 124.114;
    double dis12 = 106.237;
    double enP02 = entangle_success_prob(dis02);
    double enP01 = entangle_success_prob(dis01);
    double enP12 = entangle_success_prob(dis12);
    double enF02 = entangle_fidelity(dis02, BETA);
    double enF01 = entangle_fidelity(dis01, BETA);
    double purF02[3] = {enF02, purify_fidelity(dis02, dis02)*enF02, purify_fidelity(purify_fidelity(dis02, dis02)*enF02, dis02)};
    for(int i = 0; i < 3; i++){
        cout << purF02[i] << endl;
    }
    // double purP02[3] = {purify_success_prob(dis02, dis02), purify_success_prob(purify_success_prob(dis02, dis02), dis02)};
}