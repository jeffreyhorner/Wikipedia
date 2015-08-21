#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <google/dense_hash_map>
#include <string.h>
using namespace std;
using namespace google;
struct eqstr
{
  bool operator()(const char *s1, const char *s2) const
  {
    return (s1 == s2) || (s1 && s2 && strcmp(s1, s2) == 0);
  }
};

int main(int argc, char **argv){
  ifstream in(argv[1]);
  string input_string;
  char foo[] = "foo";
  chrono::time_point<chrono::system_clock> start, end;
  chrono::duration<double> t1, t2;
  dense_hash_map<const char *, const char *, hash<const char *>, eqstr> d;
  unsigned int lc=0, miss=0;
  
  d.set_empty_key(NULL);

  while(in >> input_string){
    start = chrono::system_clock::now();
    d[input_string.c_str()] = foo;
    end = chrono::system_clock::now();
    t1 += end-start;
    lc++;
  }
  in.close();

  in.open(argv[1]);
  while(in >> input_string){
    start = chrono::system_clock::now();
    if (d.find(input_string.c_str()) == d.end())
      miss++;
    end = chrono::system_clock::now();
    t2 += end-start;
  }
  in.close();

  cout << "C-Dense-Hash\t" << argv[1] << "\t" << lc <<
    "\t" << t1.count() << "\t" << t2.count() << endl;

  return 0;
}
