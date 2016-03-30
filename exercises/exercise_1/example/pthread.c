#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
//#include <sched.h>
//#include <time.h>
#include <unistd.h>

/* This is our thread function.  It is like main(), but for a thread*/
void *threadFunc(void *arg)
{
    printf("Thread 1 first\n");
    sleep(1);
    printf("Thread 1 second\n");
    return NULL;
}

void *threadFunc1(void *arg)
{
    printf("Thread 2\n");
    return NULL;
}


int main(void)
{
    pthread_attr_t attr,attr1;
    pthread_attr_t *attrp;
    pthread_attr_t *attrp1;
    pthread_t pth,pth1;     // this is our thread identifier
    
    struct sched_param param1,param2;

    attrp=&attr;
    attrp1=&attr1;
    
    param1.sched_priority=3;
    param2.sched_priority=2;

    pthread_attr_init(attrp);
    pthread_attr_init(attrp1);

    pthread_attr_setinheritsched(attrp,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(attrp1,PTHREAD_EXPLICIT_SCHED);

    pthread_attr_setschedpolicy(attrp, SCHED_FIFO);
    pthread_attr_setschedpolicy(attrp1, SCHED_FIFO);

    pthread_attr_setschedparam(attrp, &param1);
    pthread_attr_setschedparam(attrp1, &param2);

    pthread_create(&pth,attrp,threadFunc,"foo");
    pthread_create(&pth1,attrp1,threadFunc1, "boo");


    //pthread_setschedprio(pth, 1);
    //pthread_setschedprio(pth1, 2);


    pthread_attr_destroy(attrp);
    pthread_attr_destroy(attrp1);


    printf("main waiting for thread to terminate...\n");
    pthread_join(pth,NULL);
    pthread_join(pth1,NULL);

    return 0;
}
