#include <bits/stdc++.h>
using namespace std;

class P{
public:
    double a,b;
    string name;
    P(double a,double b,string n):a(a),b(b),name(n){;}

    bool operator<(const P &p) const{
        cout<<a<<" "<<p.a<<endl;
        return a < p.a;
    }

    bool operator==(const P &p) const{
        return a == p.a && b == p.b && name == p.name;
    }
};

int main(){
    set<P> s;
    P p1(1.166,2.1,"p"), p2(1.122,1.9,"a");
    s.insert(p1);
    if (s.find(p2) == s.end())
        cout<<"not found";
    else
        cout<<"found";
    return 1;
    for(auto &c:s)
        cout<<c.a<<" "<<c.b<<" "<<c.name<<endl;
}