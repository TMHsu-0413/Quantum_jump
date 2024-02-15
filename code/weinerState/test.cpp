#include "formula.cpp"
#include <iostream>

using namespace std;

int main() {
  map<Path_and_Memory, int, Path_and_Memory_Compare> m;
  Path_and_Memory k1(0, {1, 2, 3}, {4, 5}), k2(0, {1, 2, 3}, {4, 5});
  m[k1] = 1;
  if (m.find(k2) != m.end())
    cout << "no";
  else
    cout << "Yes";
  m[k2] = 10;
  if (m.find(k1) != m.end())
    cout << "no";
  else
    cout << "Yes";
  cout << endl;
  for (auto &[k, v] : m) {
    for (auto &c : k.path)
      cout << c << " ";
    cout << endl;
    for (auto &c : k.memory)
      cout << c << " ";
    cout << endl;
    cout << v << endl;
  }
}
