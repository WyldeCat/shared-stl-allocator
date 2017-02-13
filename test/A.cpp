#include <cstdio>
#include <vector>
#include <map>
#include <functional>

#include "../include/shared_stl.hpp"
#include "../include/shared_stl_allocator.hpp"

int main( )
{
  typedef std::map<int, int, std::less<int>, shared_stl_allocator<std::pair<int,int>, 5678> > my_map;
  shared_stl<my_map, 1543> my_allocator;

  my_map *abc = my_allocator.attach();
  (*abc)[0]++;
  printf("%d\n",(*abc)[0]);
  return 0;
}
