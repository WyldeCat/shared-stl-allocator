#include <sys/shm.h>
#include <sys/ipc.h>

template<class T, int key>
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

template<class T, int key> int shared_stl<T,key>::shm_id;
template<class T, int key> T*  shared_stl<T,key>::ptr;

template<class T, int key>
T* shared_stl<T,key>::attach()
{
  
  shm_id = shmget((key_t)key, sizeof(T), 0666 | IPC_CREAT | IPC_EXCL);
  if(shm_id == -1)
  {
    shm_id = shmget((key_t)key, sizeof(T), 0666 | IPC_CREAT);
    fprintf(stderr,"already exists..! %d\n",shm_id);
    ptr = (T*)shmat(shm_id, (void *)0, 0);
    return ptr;
  }
  ptr = (T*)shmat(shm_id, (void *)0, 0);
  new(ptr) T;
  return ptr;
}

template<class T, int key>
void shared_stl<T,key>::detach()
{
  shmdt((void *)ptr);
}

template<class T, int key>
void shared_stl<T,key>::shutdown()
{
  shmctl(shm_id, IPC_RMID, NULL);
}
