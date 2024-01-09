#include <bits/stdc++.h>
using namespace std;

class P{
public:
    double a,b;
    string name;
    P(double a,double b,string n):a(a),b(b),name(n){;}

    bool operator<(const P &p) const{
        return a < p.a;
    }

    bool operator==(const P &p) const{
        return a == p.a && b == p.b;
    }
};

int main(){
    set<P> s;
    P p1(1.1,1.1,"p"), p2 (1.1,1.1,"p");
    if (p1 == p2)
        cout<<"same";
    s.insert(p1);
    s.insert(p2);

    for(auto &c:s)
        cout<<c.a<<" "<<c.b<<" "<<c.name<<endl;
}