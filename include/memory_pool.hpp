#ifndef _MEMORY_POOL_
#define _MEMORY_POOL_

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

template<class T, long key>
class memory_pool
{
public:
  memory_pool()
  {
    if(head == NULL)
    {
      shm_id = shm_open(std::to_string(key).c_str(), O_EXCL | O_CREAT | O_RDWR, 0666);

      if(shm_id == -1) // already exists
      {
        shm_id = shm_open(std::to_string(key).c_str(), O_CREAT | O_RDWR, 0666);
        head = (void **)mmap((void*)key, total_size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, shm_id, 0); }
      else 
      {
        ftruncate(shm_id, total_size);
        head = (void **)mmap((void*)key, total_size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, shm_id, 0);
        memset(head, 0, total_size);
      }
    }
  }
  ~memory_pool(){}

  static T* alloc(int n)
  {
    fprintf(stderr,"alloc start() %d\n",n);
    void **tmp = head;
    void **prev = NULL;
    int i, shm_id,key_count = 0;
    while(tmp != NULL)
    {
      key_count++;
      char *now = (char*)((int*)(tmp + 1) + 1);
      T *first_obj = (T*)(now + (1<<13));
      int cnt = 0;
      for(i=0;i<65536;i++)
      {
        if(((*now) & (1<<(i%8)))==0)
        {
          cnt++;
          if(cnt==n) 
          {
            fprintf(stderr,"alloc complete() %d\n",first_obj + i);
            set_bits((char*)((int*)(tmp + 1) + 1), i, n, 1);
            return first_obj + i;
          }
        }
        else cnt=0;
        if(i && i%8==0) now++;
      }
      prev = tmp;
      tmp = (void **)*tmp;
    }
    shm_id = shm_open(std::to_string(key + key_count).c_str(), O_EXCL | O_CREAT | O_RDWR, 0666);
    ftruncate(shm_id, total_size);
    *prev = mmap((void*)key, total_size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, shm_id, 0);
     
    tmp = (void **)*prev;

    char *now = (char*)((int*)(tmp + 1) + 1);
    set_bits(now, 0, n, 1);
    fprintf(stderr,"alloc() complete  : %d\n",now + (1<<13));
    return (T*)(now + (1<<13));

  }

  static void free(T* p, size_t n)
  {
    fprintf(stderr,"free()\n");
    void **tmp = head;
    while(tmp != NULL)
    {
      if(tmp < (void**)p && (void**)p < tmp + total_size)
      {
        char *first_flag = (char*)((int*)(tmp + 1) + 1);
        T *first_obj = (T*)(first_flag + (1<<13));
        int cnt = p - first_obj;
        set_bits(first_flag, cnt, n, 0);
        return;
      }
      tmp = (void**)*tmp;
    }
    fprintf(stderr,"complete free()\n");
  }

  static void shutdown()
  {
    fprintf(stderr,"shutdown!!\n");
    void **tmp = head;
    int key_count = 0;
    while(tmp != NULL)
    {
      void *next = *tmp;
      shm_unlink(std::to_string(key + key_count).c_str());
      tmp = (void**)next;
    }
  }

private: 
  static int shm_id; 
  static void **head;
  static const int total_size;
  static void set_bits(char *first_flag, int idx, int n, int val);
};

template<class T, long key>
int memory_pool<T, key>::shm_id;

template<class T, long key>
void **memory_pool<T, key>::head = NULL;

template<class T, long key>
const int memory_pool<T, key>::total_size = sizeof(T)*(1<<16) + (1<<13) + sizeof(void*) + sizeof(int);


template<class T, long key>
void memory_pool<T, key>::set_bits(char *first_flag, int idx, int n, int val)
{
                                                                      //   0   1 1
  fprintf(stderr,"set_bits() ff: %d, idx: %d, n: %d, val: %d\n",first_flag,idx,n,val);
  int i, t = (idx%8 + n)/8 - 1;
  char *target_flag = first_flag + idx/8;

  if(val==0) for(i=idx%8;i<8 && i<idx%8+n;i++) (*target_flag)&=(~(1<<i));
  else 
  {
    for(i=idx%8;i<8 && i<idx%8+n;i++) 
    {
      fprintf(stderr,"before *target_flag:%d\n",*target_flag);
      (*target_flag)|=(1<<i);
      fprintf(stderr,"after *target_flag:%d\n\n",*target_flag);
    }
  }
  if(i==8) target_flag++;

  for(i=0;i<t;i++, target_flag++) *target_flag = (val==0 ? 0 : -1); 
  if(val==0) for(i=0;i<t;i++, target_flag++) *target_flag = 0;
  else for(i=0;i<t;i++, target_flag++) *target_flag = -1;

  if(val==0) for(i=0;i<(idx%8+n)%8;i++)(*target_flag)&=(~(1<<i));
  else for(i=0;i<(idx%8+n)%8;i++)(*target_flag)|=(1<<i);
  fprintf(stderr,"complete set_bits()\n");
}

#endif
