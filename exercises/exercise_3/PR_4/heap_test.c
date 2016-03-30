//#include "posix_mq.c"
#include "heap_mq.c"
#include <semaphore.h>

#define SNAME "/mysem"

int main()
{
    sem_t * sem=sem_open(SNAME,O_CREAT, 0644,0);
    heap_mq();
    sem_wait(sem);
    //mq_demo();   
    shutdown();
}
