#include <cstdio>
#include <vector>

#include "../include/shared_stl.hpp"
#include "../include/shared_stl_allocator.hpp"

int main( )
{
  std::vector<int, shared_stl_allocator<int> > v;
  v.push_back(1);
  v.push_back(2);
  printf("%d\n",v.front());
  printf("%d\n",v.back());
  return 0;
}
