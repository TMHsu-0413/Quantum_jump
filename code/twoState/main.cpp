#include "RLBSP/RLBSP.cpp"

int main() {
  string filename = "graph.txt";
  double threshold = 0.7;
  double beta = 0.00438471;

  RLBSP_Algo RLBSP(filename, threshold, beta);
  RLBSP.run();
}
