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
        fprintf(stderr,"shm_id: %d\n",shm_id);
        perror("open");
        head = (void **)mmap((void*)key, total_size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, shm_id, 0); 
        fprintf(stderr,"%d head: %p %p\n",sizeof(T),head,key);

        if(sizeof(T) == 1) fprintf(stderr,"%c\n",*(char*)(0xccffe29be9));
        perror("mmap");
      }
      else 
      {
        ftruncate(shm_id, total_size);
        head = (void **)mmap((void*)key, total_size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, shm_id, 0);

        shm_id = shm_open(std::to_string(key).c_str(), O_CREAT | O_RDWR, 0666);
        fprintf(stderr,"shm_id: %d\n",shm_id);
        fprintf(stderr,"%d head: %p %p\n",sizeof(T),head,key);
        perror("mmap");
        memset(head, 0, total_size);
        perror("memset");
        fprintf(stderr,"head: %p totalsize: %d lim: %p\n",head,total_size,head+total_size);
      }
    }
  }
  ~memory_pool(){}

  static T* alloc(int n)
  {
    void **tmp = head;
    void **prev = NULL;
    int i, shm_id,key_count = 0;
    while(tmp != NULL)
    {
      key_count++;
      char *now = (char*)((int*)(tmp + 1) + 1);
      T *first_obj = (T*)(now + (1<<15));
      int cnt = 0;
      for(i=0;i<262144;i++)
      {
        if(((*now) & (1<<(i%8)))==0)
        {
          cnt++;
          if(cnt==n) 
          {
            set_bits((char*)((int*)(tmp + 1) + 1), i - n  + 1, n, 1);
            return first_obj + i - n -1;
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
    return (T*)(now + (1<<15));

  }

  static void free(T* p, size_t n)
  {
    void **tmp = head;
    while(tmp != NULL)
    {
      if(tmp < (void**)p && (void**)p < tmp + total_size)
      {
        char *first_flag = (char*)((int*)(tmp + 1) + 1);
        T *first_obj = (T*)(first_flag + (1<<15));
        int cnt = p - first_obj;
        set_bits(first_flag, cnt, n, 0);
        return;
      }
      tmp = (void**)*tmp;
    }
  }

  static void shutdown()
  {
    void **tmp = head;
    int key_count = 0;
    while(tmp != NULL)
    {
      void *next = *tmp;
      shm_unlink(std::to_string(key + key_count).c_str());
      tmp = (void**)next;
    }
  }

public:
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
const int memory_pool<T, key>::total_size = sizeof(T)*(1<<18) + (1<<15) + sizeof(void**) + sizeof(int);


template<class T, long key>
void memory_pool<T, key>::set_bits(char *first_flag, int idx, int n, int val)
{
  int i, t = (idx%8 + n)/8 - 1;
  char *target_flag = first_flag + idx/8;

  if(val==0) for(i=idx%8;i<8 && i<idx%8+n;i++) (*target_flag)&=(~(1<<i));
  else for(i=idx%8;i<8 && i<idx%8+n;i++) (*target_flag)|=(1<<i);
  if(i==8) target_flag++;
  for(i=0;i<t;i++, target_flag++) *target_flag = (val==0 ? 0 : -1); 
  if(val==0) for(i=0;i<t;i++, target_flag++) *target_flag = 0;
  else for(i=0;i<t;i++, target_flag++) *target_flag = -1;

  if(val==0) for(i=0;i<(idx%8+n)%8;i++)(*target_flag)&=(~(1<<i));
  else for(i=0;i<(idx%8+n)%8;i++)(*target_flag)|=(1<<i);

}

#endif
