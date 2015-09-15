#include "map.hpp"
#include <string>
#include <iostream>

using namespace cs540;
using namespace std;

int main() {
    Map<int, int> simap;
    simap[0] = 1;
    simap[2] = 3;
    simap[3] = 4;
    /* 
    simap[string("jjhow")] = 1;
    simap[string("jerry")] = 2;
    simap[string("jason")] = 3;
    simap[string("jimmy")] = 4;
    */
    //pair<string, int> value(string("david"), 5);
    //simap.insert(value);

    Map<int, int>::Iterator simap_iter = simap.begin();
    for (; simap_iter != simap.end(); ++simap_iter) 
        cout << simap_iter->first << ' ' << simap_iter->second << endl;
}


/*
bool fncomp (char lhs, char rhs) {return lhs<rhs;}

struct classcomp {
  bool operator() (const char& lhs, const char& rhs) const
  {return lhs<rhs;}
};

int main ()
{
    cs540::Map<char,int> first;

  first['a']=10;
  first['b']=30;
  first['c']=50;
  first['d']=70;

  cs540::Map<char,int> second (first.begin(),first.end());

  cs540::Map<char,int> third (second);

  cs540::Map<char,int,classcomp> fourth;                 // class as Compare

  bool(*fn_pt)(char,char) = fncomp;
  cs540::Map<char,int,bool(*)(char,char)> fifth (fn_pt); // function pointer as Compare

  return 0;
}
*/
