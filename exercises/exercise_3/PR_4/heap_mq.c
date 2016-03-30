/****************************************************************************/
/*                                                                          */
/* Sam Siewert - 10/14/97                                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

// Edited version for POSIX linux c Zachary and Vidya 2/27/2016
//
//#include "msgQLib.h"
#include <mqueue.h>
#include <sys/stat.h> //constants
#include <fcntl.h> //constants
//#include "errnoLib.h" 
#include <stdio.h>
#include <errno.h>
#include <string.h> //replacing ioLIB
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>

#define SNDRCV_MQ "/send_receive_mq"

pthread_t thread[2];
struct mq_attr mq_attr;
mqd_t mymq;

/* receives pointer to heap, reads it, and deallocate heap memory */

void *receiver(void *arg)
{
    char buffer[sizeof(void *)+sizeof(int)];
    void *buffptr; 
    int prio;
    int nbytes;
    int count = 0;
    int id;
    //mymq = mq_open(SNDRCV_MQ, O_RDWR);
    while(1) {

        /* read oldest, highest priority msg from the message queue */

        //printf("Reading %ld bytes\n", sizeof(void *));

        //if((nbytes = mq_receive(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), &prio)) !=0)
            /*
               if((nbytes = mq_receive(mymq, (void *)&buffptr, (size_t)sizeof(void *), &prio)) == ERROR)
               */
        //{
        //    perror("mq_receive");
       // }
         //      else
         //
              // {
        //mq_receive(mymq,buffptr,(size_t)(sizeof(void *)+sizeof(int)),&prio);
        nbytes=mq_receive(mymq,buffer,(sizeof(void *)+sizeof(int)),&prio);
        if(nbytes>0){
        
            printf("receive nbytes=%d\n",nbytes);
            memcpy(buffptr, buffer, sizeof(void *));
            memcpy((void *)&(id), &(buffer[sizeof(void *)]), sizeof(int));
            printf("receive: ptr msg 0x%X received with priority = %d, length = %d, id = %d\n", buffptr,prio,nbytes, id);
            printf("contents of ptr = \n%s\n", (char *)buffptr);
            free(buffptr);
            printf("heap space memory freed\n");
        }
        usleep(100);
            //}
        //else{
            //sched_yield();
        //}
    }

}


static char imagebuff[4096];

void *sender(void *arg)
{
    static char buffer[sizeof(void *)+sizeof(int)];
    void *buffptr;
    int prio;
    int nbytes;
    int id1 = 36;
    size_t size=sizeof(void *)+sizeof(int);
    //mymq = mq_open(SNDRCV_MQ, O_RDWR);
    //printf("size of buffer %d\n",sizeof(buffer));
    while(1) {

        /* send malloc'd message with priority=30 */
        //printf("size of imagebuff is %d\n",sizeof(imagebuff));
        buffptr = (void *)malloc(sizeof(imagebuff));
        strcpy(buffptr, imagebuff);
        //printf("Size of void * is %d\n",sizeof(void *));
        printf("Message to send = %s\n", (char *)buffptr);

        printf("Sending %ld bytes\n", sizeof(buffptr));

        memcpy(buffer, buffptr, sizeof(void *));
        printf("this is the string that is supposed to get sent %s\n",buffer);
        memcpy(&(buffer[sizeof(void *)]), &id1, sizeof(int));
        printf("this is the string that is supposed to get sent %s\n",buffer);
        printf("bout to mq_send\n");
        nbytes = mq_send(mymq,buffer,size,9);
        //nbytes = mql_send(mymq,buffer,sizeof(buffer),30);
        printf("mq_send finished\n");
        printf("send nbytes=%d\n",nbytes);
        usleep(200);

    }

}



void heap_mq(void)
{
    int i, j;
    char pixel = 'A';

    pthread_attr_t attr[2];
    pthread_attr_t * attrp[2];
    struct sched_param param[2];
    attrp[0]=&attr[0];
    attrp[1]=&attr[1];
    param[0].sched_priority=20;
    param[1].sched_priority=19;

    pthread_attr_init(attrp[0]);
    pthread_attr_init(attrp[1]);

    pthread_attr_setinheritsched(attrp[0],PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(attrp[1],PTHREAD_EXPLICIT_SCHED);

    pthread_attr_setschedpolicy(attrp[0],SCHED_FIFO);
    pthread_attr_setschedpolicy(attrp[1],SCHED_FIFO);

    pthread_attr_setschedparam(attrp[0],&param[0]);
    pthread_attr_setschedparam(attrp[1],&param[1]);

    for(i=0;i<4096;i+=64) {
        pixel = 'A';
        for(j=i;j<i+64;j++) {
            imagebuff[j] = (char)pixel++;
        }
        imagebuff[j-1] = '\n';
    }
    imagebuff[4095] = '\0';
    imagebuff[63] = '\0';

    //printf("buffer =\n%s", imagebuff);

    /* setup common message q attributes */
    mq_attr.mq_maxmsg = 100;
    mq_attr.mq_msgsize = sizeof(void *)+sizeof(int);

    mq_attr.mq_flags = 0;

    /* note that VxWorks does not deal with permissions? */
    mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, 0644, &mq_attr);
    /*if(mymq != (mqd_t)0)
        perror("mq_open");
    */
    printf("%d\n",mymq);
    if(pthread_create(&thread[0],attrp[0],receiver,NULL))
    {
        printf("you suck, reciever didn't open\n");
    }
    else
    {
        printf("receiver spawned\n");
    }
    if(pthread_create(&thread[1],attrp[1],sender,NULL))
    {
        printf("you suck, sender didn't open\n");
    }
    else
    {
        printf("sender spawned\n");
    }
    /* receiver runs at a higher priority than the sender */
    /*if((rid=taskSpawn("Receiver", 90, 0, 4000, receiver, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)) !=0) {
      printf("Receiver task spawn failed\n");
      }
      else
      printf("Receiver task spawned\n");

      if((sid=taskSpawn("Sender", 100, 0, 4000, sender, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)) !=0) {
      printf("Sender task spawn failed\n");
      }
      else
      printf("Sender task spawned\n");
      */
    pthread_attr_destroy(attrp[0]);
    pthread_attr_destroy(attrp[1]);
}

void shutdown(void)
{
    mq_close(mymq);
    pthread_cancel(thread[0]);
    pthread_cancel(thread[1]);
}
