
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <math.h>
#include <sys/param.h>

#define BILLION 1000000000L

/********************************************************************
  Variable Declarations
 ********************************************************************/

pthread_t Thread_set;
pthread_t Thread_get;
pthread_t timeout;

pthread_attr_t schedAttr_set;
pthread_attr_t schedAttr_get;
pthread_attr_t schedAttr_time;

struct sched_param set_param;
struct sched_param get_param;
struct sched_param time_param;

pthread_mutex_t mutexLock;


int test;

/********************************************************************
  Function Declarations
 ********************************************************************/




typedef struct NAV
{
    double Acc_x,Acc_y,Acc_z;
    double roll;
    double pitch;
    double yaw;
    struct timespec ts;
}NAV;
NAV *nav;
/********************************************************************
  Threads
 ********************************************************************/
void set_XYZ(void *a)
{
    pthread_mutex_lock(&mutexLock);
    nav->Acc_x = rand();
    nav->Acc_y = rand();
    nav->Acc_z = rand();
    nav->roll = rand();
    nav->pitch = rand();
    nav->yaw = rand();
    clock_gettime(CLOCK_MONOTONIC, &(nav->ts));
    pthread_mutex_unlock(&mutexLock);

}

void timed_lock(void *a)
{
    int rc;
    struct timespec c={10,0}, b;
    while(1)
    {
        if(pthread_mutex_timedlock(&mutexLock,&c)!=0)
        {
            clock_gettime(CLOCK_REALTIME,&b);
            printf("No new data available at time: sec=%d, nsec=%d\n",(int)b.tv_sec,(int)b.tv_nsec);
            return;
        }
        else
        {
            pthread_mutex_unlock(&mutexLock);
        }
    }
}



void get_XYZ(void *a)
{

    pthread_mutex_lock(&mutexLock);
    //printf("test= %d\n",test);
    printf("Acc_x= %f\n",nav->Acc_x);
    printf("Acc_y= %f\n",nav->Acc_y);
    printf("Acc_z= %f\n",nav->Acc_z);
    printf("roll= %f\n",nav->roll);
    printf("pitch= %f\n",nav->pitch);
    printf("yaw= %f\n",nav->yaw);
    printf("time: sec=%d, nsec=%d\n",(int)nav->ts.tv_sec,(int)nav->ts.tv_nsec);
    pthread_mutex_unlock(&mutexLock);

}

/********************************************************************
  Main
 ********************************************************************/
int main (int argc, char *argv[])
{


    pthread_attr_init(&schedAttr_set);
    pthread_attr_init(&schedAttr_get);
    pthread_attr_init(&schedAttr_time);

    pthread_attr_setinheritsched(&schedAttr_set, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&schedAttr_set, SCHED_FIFO);

    pthread_attr_setinheritsched(&schedAttr_get, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&schedAttr_get, SCHED_FIFO);

    pthread_attr_setinheritsched(&schedAttr_time, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&schedAttr_time, SCHED_FIFO);


    set_param.sched_priority = 4;
    get_param.sched_priority = 3;
    time_param.sched_priority = 2;



    pthread_attr_setschedparam(&schedAttr_set, &set_param);
    pthread_attr_setschedparam(&schedAttr_get, &get_param);
    pthread_attr_setschedparam(&schedAttr_time, &time_param);
    int count = 0;
    //ts.tv_sec  = 5;
    //ts.tv_nsec = 500000000;

    nav = (NAV *) malloc(sizeof(NAV));

    while(count < 2)
    {
        if(count==1)
        {
            pthread_create(&timeout, &schedAttr_time, timed_lock, (void*) 0);
        }
        pthread_create(&Thread_set, &schedAttr_set,set_XYZ,(void*) 0);


        //pthread_mutex_lock(&mutexLock);
        pthread_create(&Thread_get, &schedAttr_get,get_XYZ,(void*) 0);
        //pthread_mutex_unlock(&mutexLock);

        //pthread_delay_np(&ts);
        //usleep(1000);
        count++;
        usleep(10000);
    }

    pthread_join(Thread_set,NULL);
    pthread_join(Thread_get,NULL);

    pthread_mutex_lock(&mutexLock);

    pthread_join(timeout,NULL);

    pthread_mutex_destroy(&mutexLock);
}

