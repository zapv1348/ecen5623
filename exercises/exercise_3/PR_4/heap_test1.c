//#include "posix_mq.c"
#include "heap_mq.c"
#include <semaphore.h>

#define SNAME "/mysem"

int main()
{
    sem_t * sem=sem_open(SNAME,0);
    sem_post(sem);
    return 0;
    //mq_demo();   
    //shutdown();
}
