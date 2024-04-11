#include "RLBSP/RLBSP.cpp"

int main() {
  string filename = "graph_RLBSP_example.txt";
  double threshold = 0.6;
  double beta = 0.00438471;

  RLBSP_Algo RLBSP(filename, threshold, beta);
  RLBSP.run();
}
