#include <cstdio>
#include <vector>
#include "../include/shared_stl.hpp"
#include "../include/shared_stl_allocator.hpp"

int main( )
{
  typedef std::vector<int, shared_stl_allocator<int,0xcaffe3e000> > myv;
  shared_stl<myv, 0xcaffe2e000> myv_allocator;

  //shm_unlink("0xcaffe3e000");
  //shm_unlink("0xcaffe2e000");
  myv *a = myv_allocator.attach();
  a->push_back(1);
  printf("%d\n",a->size());
}
