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
  typedef std::map<char, A, std::less<char>, shared_stl_allocator<std::pair<char,A>, 0xcaffe2e000> > mymp;

  /*
  shm_unlink(std::to_string(0xcaffe3e000).c_str());
  shm_unlink(std::to_string(0xcaffe2e000).c_str());
  shm_unlink(std::to_string(0xcaffe1e000).c_str());
  //shm_unlink("0xcaffe2e000");
  perror("");
  */

  mymp a;
  A b;
  b.qq=3;
  a['c']=b;
  if(a.find('c') != a.end()) printf("thereis");
  printf("%d\n",a['c'].qq);
}
