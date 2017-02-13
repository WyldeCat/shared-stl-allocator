#include "memory_pool.hpp"

template<typename T, int key>
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


  template<class U, int k> struct rebind { typedef shared_stl_allocator<U, k> other; };

  pointer address(reference value)       const { return &value; }
  const_pointer address(const_reference value) const { return &value; }

  shared_stl_allocator() {}
  shared_stl_allocator(const shared_stl_allocator&) {}
  template<class U, int k> shared_stl_allocator(const shared_stl_allocator<U, k>&) {}
  ~shared_stl_allocator() {}

  size_type max_size() const throw() 
  { 
    return (1<<16);
  }

  pointer allocate(size_type n, const void* = 0)
  {
    return static_cast<pointer>(mem.alloc(n));
  }

  void construct(pointer p, const T& val)
  {
    new((void*)p)T(val);
  }

  void destroy(pointer p)
  {
    p->~T();
  }

  void deallocate(pointer p, size_type n)
  {
    mem.free(p,n);
  }

};

template<typename T, int key>
memory_pool<T, key> shared_stl_allocator<T, key>::mem;
