#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

template<typename T, int key>
class memory_pool
{
public:
  memory_pool()
  {
    if(head == NULL)
    {
      shm_id = shmget((key_t)key, total_size, 0666 | IPC_CREAT);
      head = (void **)shmat(shm_id, (void *)0, 0);
      memset(head, 0, total_size);
    }
  }
  ~memory_pool(){}

  static T* alloc(int n)
  {
    void **tmp = head;
    void **prev = NULL;
    int i, shm_id;
    while(tmp != NULL)
    {
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

    shm_id = shmget(0 , total_size, 0666 | IPC_CREAT);
    *prev = shmat(shm_id, (void *)0, 0);
    tmp = (void **)*prev;

    char *now = (char*)((int*)(tmp + 1) + 1);
    set_bits(now, 0, n, 1);
    return (T*)(now + (1<<13));

  }

  static void free(T* p, size_t n)
  {
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
  }

  static void shutdown()
  {
    void **tmp = head;
    while(tmp != NULL)
    {
      void *next = *tmp;
      int shm_id = *(int*)(tmp+1);
      shmctl(shm_id, IPC_RMID, NULL);
      shmdt(tmp);
      tmp = (void**)next;
    }
  }

private: 
  static int shm_id; 
  static void **head;
  static const int total_size;
  static void set_bits(char *first_flag, int idx, int n, int val);
};

template<typename T, int key>
int memory_pool<T, key>::shm_id;

template<typename T, int key>
void **memory_pool<T, key>::head = NULL;



template<typename T, int key>
const int memory_pool<T, key>::total_size = sizeof(T)*(1<<16) + (1<<13) + sizeof(void*) + sizeof(int);


template<typename T, int key>
void memory_pool<T, key>::set_bits(char *first_flag, int idx, int n, int val)
{
  int i, t = (idx%8 + n)/8 - 1;
  char *target_flag = first_flag + idx/8;

  if(val==0) for(i=idx%8;i<8 && i<idx%8+n;i++) (*target_flag)&=(~(1<<(i-1)));
  else for(i=idx%8;i<8 && i<idx%8+n;i++) (*target_flag)|=((1<<(i-1)));
  if(i==8) target_flag++;

  for(i=0;i<t;i++, target_flag++) *target_flag = (val==0 ? 0 : -1); 
  if(val==0) for(i=0;i<t;i++, target_flag++) *target_flag = 0;
  else for(i=0;i<t;i++, target_flag++) *target_flag = -1;

  if(val==0) for(i=0;i<(idx%8+n)%8;i++)(*target_flag)&=(~(1<<(i-1)));
  else for(i=0;i<(idx%8+n)%8;i++)(*target_flag)|=((1<<(i-1)));
}
