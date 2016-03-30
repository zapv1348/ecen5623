#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <semaphore.h>
#include <string.h>
#include <fcntl.h>



#define SNAME "/mysem"

int main(){
    sem_t * sem=sem_open(SNAME,0);
    printf("This should free the other task\n");
    sem_post(sem);
    printf("exiting\n");
        
    return 0;
}
