/****************************************************************************/
/* Function: Basic POSIX message queue demo from VxWorks Prog. Guide p. 78  */
/*                                                                          */
/* Sam Siewert - 9/24/97                                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

//#include <msgQLib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include "errnoLib.h" 
#include <stdio.h>
#include <errno.h>
//#include "ioLib.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

#define SNDRCV_MQ "/sendrmq"
#define MAX_MSG_SIZE 128

pthread_t thread[2];
struct mq_attr mq_attr;

void *receiver(void *arg)
{
    mqd_t mymq;
    char buffer[MAX_MSG_SIZE];
    int prio;
    int nbytes;



    /* note that VxWorks does not deal with permissions? */
    //mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, 0, &mq_attr);
    mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, 0644, &mq_attr);
    //printf("mq_open number is: %d",mymq);
    /*if(mymq != (mqd_t)0)
        perror("mq_open");*/

    /* read oldest, highest priority msg from the message queue */
    /*if((nbytes = mq_receive(mymq, buffer, MAX_MSG_SIZE, &prio)) !=0)
    {
        perror("mq_receive");
    }
    else
    {
        buffer[nbytes] = '\0';
        printf("receive: msg %s received with priority = %d, length = %d\n",
                buffer, prio, nbytes);
    }*/
    usleep(10000);
    nbytes = mq_receive(mymq, buffer, MAX_MSG_SIZE, &prio);
    buffer[nbytes]='\0';
    printf("buffer recieve: %s\n",buffer);
    return NULL;

}

static char canned_msg[] = "this is a test, and only a test, in the event of a real emergency, you would be instructed ...";

void *sender(void *arg)
{
    mqd_t mymq;
    int prio;
    int nbytes;

    /* note that VxWorks does not deal with permissions? */
    mymq = mq_open(SNDRCV_MQ, O_RDWR);

    /*if(mymq != (mqd_t)0)
        perror("mq_open");*/

    /* send message with priority=30 */
    /*if((nbytes = mq_send(mymq, canned_msg, sizeof(canned_msg), 30)) != 0)
    {
        perror("mq_send");
    }
    else
    {
        printf("send: message successfully sent\n");
    }*/
    nbytes = mq_send(mymq, canned_msg, sizeof(canned_msg), 30);
    return NULL;
}


void mq_demo(void)
{

    pthread_attr_t attr[2];
    pthread_attr_t * attrp[2];
    struct sched_param param[2];

    attrp[0] = &attr[0];
    attrp[1] = &attr[1];
    param[0].sched_priority=6;
    param[1].sched_priority=5;

    pthread_attr_init(attrp[0]);
    pthread_attr_init(attrp[1]);

    pthread_attr_setinheritsched(attrp[0],PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(attrp[1],PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(attrp[0],SCHED_FIFO);
    pthread_attr_setschedpolicy(attrp[1],SCHED_FIFO);

    pthread_attr_setschedparam(attrp[0],&param[0]);
    pthread_attr_setschedparam(attrp[1],&param[1]);


    /* setup common message q attributes */
    mq_attr.mq_maxmsg = 100;
    mq_attr.mq_msgsize = MAX_MSG_SIZE;

    mq_attr.mq_flags = 0;

    
    if(pthread_create(&thread[0],attrp[0],receiver,NULL)!=0)
    {
        printf("receiver did not open\n");

    }
    else
    {
        printf("receiver spawned\n");

    }

    if(pthread_create(&thread[1],attrp[1],sender,NULL)!=0)
    {
        printf("sender did not open\n");
    }
    else
    {
        printf("sender spawned\n");
    }
    pthread_attr_destroy(attrp[0]);
    pthread_attr_destroy(attrp[1]);
    

    pthread_join(thread[1],NULL);
    pthread_join(thread[0],NULL);
    /* receiver runs at a higher priority than the sender */
    /*if(taskSpawn("Receiver", 90, 0, 4000, receiver, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) == ERROR) {
        printf("Receiver task spawn failed\n");
    }
    else
        printf("Receiver task spawned\n");

    if(taskSpawn("Sender", 100, 0, 4000, sender, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) == ERROR) {
        printf("Sender task spawn failed\n");
    }
    else
        printf("Sender task spawned\n");
*/

}
