#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <semaphore.h>
#include <string.h>
#include <fcntl.h>

#define SNAME "/mysem"

int main(void)
{
    sem_t * sem1=sem_open(SNAME,O_CREAT, 0644,0);

    pid_t user_lev=fork();
    if(user_lev>=0)
    {
        if(user_lev==0)
        {
           setpriority(PRIO_USER,user_lev,-11);//sets priority to default user level with standard niceness
           printf("first child, pid %d, now waiting for other task to post sem\n",getpid());
           sem_wait(sem1);
           printf("first child, pid %d, is now completing\n",getpid());
           exit(0);
        }
        //parent, generate another child and then finish
        else
        {
            int returnstat;
            printf("parent, id %d, waiting for child\n",getpid());
            waitpid(-1,&returnstat,0);
            printf("child finish, parent finishing\n");
        }
    }
    else{
        printf("First fork failed\n");
        return 1;
    }
    return 0;
}
