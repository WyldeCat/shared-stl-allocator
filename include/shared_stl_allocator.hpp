#ifndef _SHARED_STL_ALLOCATOR_
#define _SHARED_STL_ALLOCATOR_

#include "memory_pool.hpp"

template<typename T,long key>
class shared_stl_allocator
{
private:
    static memory_pool<T, key> mem;
public:
  typedef T value_type;
  typedef T* pointer;
  typedef T& reference;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef const T* const_pointer;
  typedef const T& const_reference;

  template<class U, long k=key> struct rebind { typedef shared_stl_allocator<U, k> other; };

  pointer address(reference value)             const { return &value; }
  const_pointer address(const_reference value) const { return &value; }

  shared_stl_allocator() {}
  shared_stl_allocator(const shared_stl_allocator&) {}
  template<class U, long k> shared_stl_allocator(const shared_stl_allocator<U, k>&) {}
  ~shared_stl_allocator() {}

  size_type max_size() const throw() 
  { 
    return (1<<16);
  }

  pointer allocate(size_type n, const void* = 0)
  {
    if(n==0) return NULL;
    fprintf(stderr,"allocate()\n");
    pointer p = (pointer)mem.alloc(n);
    fprintf(stderr,"complete allocate()\n");
    return p;
  }

  void construct(pointer p, const T& val)
  {
    fprintf(stderr,"construct()\n");
    new((void*)p)T(val);
    fprintf(stderr,"complete construct()\n");
  }

  void destroy(pointer p)
  {
    fprintf(stderr,"destroy()\n");
    p->~T();
    fprintf(stderr,"complete destroy()\n");
  }

  void deallocate(pointer p, size_type n)
  {
    fprintf(stderr,"deallocate()\n");
    mem.free(p,n);
    fprintf(stderr,"complete deallocate()\n");
  }

  static void shutdown()
  {
    mem.shutdown();
  }
};

template<class T, long key>
memory_pool<T, key> shared_stl_allocator<T, key>::mem;

template<typename T, long key>
inline bool operator==(const shared_stl_allocator<T, key>&, const shared_stl_allocator<T, key> &){ return true; }

template<typename T, long key>
inline bool operator!=(const shared_stl_allocator<T, key>&, const shared_stl_allocator<T, key> &){ return false; }
#endif
