#include <cstdio>
#include <vector>
#include <map>
#include <functional>

#include "../include/shared_stl.hpp"
#include "../include/shared_stl_allocator.hpp"

int main( )
{
  //typedef std::map<int, int, std::less<int>, shared_stl_allocator<std::pair<int,int>, 5678> > my_map;
  typedef std::vector<int, shared_stl_allocator<int,1234> > my_vector;
  shared_stl<my_vector,1111> vv;

  //my_map *abc = my_allocator.attach();
  my_vector *abcd = vv.attach();
  fprintf(stderr,"front: %d\n",&abcd[0]);
  abcd->push_back(1);
  fprintf(stderr,"size: %d\n",abcd->size());
  return 0;
}
