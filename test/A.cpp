#include <cstdio>
#include <vector>
#include <map>
#include "../include/shared_stl.hpp"
#include "../include/shared_stl_allocator.hpp"

class A
{
public:
  int qq;
};
int main( )
{
  shm_unlink(std::to_string(0xcaffe2e000).c_str());
  perror("");
  typedef std::map<char, int, std::less<char>, shared_stl_allocator<std::pair<char,int>, 0xcaffe2e000> > mymp;

  //shm_unlink(std::to_string(0xcaffe3e000).c_str());
  //shm_unlink(std::to_string(0xcaffe1e000).c_str());
  //perror("");


  mymp a;
  a.insert(std::make_pair('a',1));
  printf("%d\n",&a['a']);
  printf("%d\n",&a.begin()->second);
  a.insert(std::make_pair('b',2));
  printf("%d\n",&a['a']);
  printf("%d\n",&a.begin()->second);
  //a.insert(std::make_pair('c',3));
  //a.insert(std::make_pair('d',4));
  //a.insert(std::make_pair('e',5));

  //if(a.find('c') != a.end()) printf("thereis");
  //printf("%d\n",a['c']);
  /**/
  return 0;
}
