#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>

#include <fcntl.h>
#include <unistd.h>

template<class T, long key>
class shared_stl
{
typedef T* pointer;

public:
  static pointer attach();
  static void    detach();
  static void  shutdown();
  shared_stl(){}

private:
  static int  shm_id;
  static pointer ptr;
};

template<class T, long key> int shared_stl<T,key>::shm_id;
template<class T, long key> T*  shared_stl<T,key>::ptr;

template<class T, long key>
T* shared_stl<T,key>::attach()
{
  shm_id = shm_open(std::to_string(key).c_str(), O_EXCL | O_CREAT | O_RDWR, 0666);
  if(shm_id == -1)
  {
    shm_id = shm_open(std::to_string(key).c_str(), O_CREAT | O_RDWR, 0666);
    ptr = (T*)mmap((void*)key, sizeof(T), PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, shm_id, 0);
    return ptr;
  }
  int sz = sizeof(T);
  ftruncate(shm_id, sizeof(T));
  ptr = (T*)mmap((void*)key, sizeof(T), PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, shm_id, 0);
  new(ptr) T;
  return ptr;
}

template<class T, long key>
void shared_stl<T,key>::detach()
{
  // TODO 
  //shmdt((void *)ptr);
}

template<class T, long key>
void shared_stl<T,key>::shutdown()
{
  shm_unlink(std::to_string(key).c_str());
}
