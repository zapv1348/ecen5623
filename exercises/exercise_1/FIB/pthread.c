#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>

struct timespec start_time={0,0};
struct timespec current_time={0,0};
double diff_time;
struct timespec F10_start={0,0};
struct timespec F20_start={0,0};
const struct timespec mil10={0,1000000};


void FIB(unsigned int iter){
	unsigned int i=0;
	unsigned int j=0;
	unsigned int fib1=1;
	unsigned int fib2=0;
	unsigned int fib3=0;

	for(i=0;i<iter;i++){
		while(j<47){
			fib3=fib1+fib2;
			fib1=fib2;
			fib2=fib3;
			j++;
		}
		fib1=1;
		fib2=0;
		j=0;
	}			
}

void *F10(void * arg){
	clock_gettime(CLOCK_REALTIME, &start_time);
	int i=0;
	while (diff_time <90000000){
		clock_gettime(CLOCK_REALTIME, &F20_start);
		FIB(12000); //should last 10 milliseconds
		
		i++;
		clock_gettime(CLOCK_REALTIME, &current_time);
		printf("F10 has completed %d iterations\n",i);
		printf("This iteration took %ld seconds, %ld nanoseconds\n",current_time.tv_sec-F20_start.tv_sec,current_time.tv_nsec-F20_start.tv_nsec);
		fflush(stdout);
		nanosleep(&mil10,NULL);
		//sleep here for 10 milliseconds till this needs to go again
		clock_gettime(CLOCK_REALTIME,&current_time);
		diff_time=(start_time.tv_sec-current_time.tv_sec)*1000000000+start_time.tv_nsec-current_time.tv_nsec;
	}
	return NULL;
}


void *F20(void * arg){
	int i=0;
	while (diff_time <80000000){
		clock_gettime(CLOCK_REALTIME, &F10_start);
		FIB(24000); //should last 20 milliseconds
	
		i++;
		clock_gettime(CLOCK_REALTIME, &current_time);
		printf("F20 has completed %d iterations\n",i);
		printf("This iteration took %ld seconds, %ld nanoseconds\n",current_time.tv_sec-F10_start.tv_sec,current_time.tv_nsec-F10_start.tv_nsec);
		fflush(stdout);
		clock_gettime(CLOCK_REALTIME,&current_time);
		diff_time=(start_time.tv_sec-current_time.tv_sec)*1000000000+start_time.tv_nsec-current_time.tv_nsec;
	}
	return NULL;
}



int main(void){

	pthread_attr_t attr,attr1;
	pthread_attr_t * attrp;
	pthread_attr_t * attrp1;
	pthread_t pth,pth1;
	struct sched_param param1,param2;
	attrp=&attr;
	attrp1=&attr1;

	param1.sched_priority=5;
	param2.sched_priority=6;
	
	pthread_attr_init(attrp);
	pthread_attr_init(attrp1);

	pthread_attr_setinheritsched(attrp,PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setinheritsched(attrp1,PTHREAD_EXPLICIT_SCHED);
	
	pthread_attr_setschedpolicy(attrp,SCHED_FIFO);
	pthread_attr_setschedpolicy(attrp1,SCHED_FIFO);

	pthread_attr_setschedparam(attrp, &param1);
	pthread_attr_setschedparam(attrp1, &param2);

	pthread_create(&pth,attrp,F10,NULL);
	pthread_create(&pth1,attrp1,F20,NULL);

	pthread_attr_destroy(attrp);
	pthread_attr_destroy(attrp1);

	printf("main waiting for thread to terminate...\n");

	pthread_join(pth1,NULL);
	pthread_join(pth,NULL);
	clock_gettime(CLOCK_REALTIME, &current_time);
	printf("both threads took %ld seconds, %ld nanoseconds\n",current_time.tv_sec-start_time.tv_sec,current_time.tv_nsec-start_time.tv_nsec);
	return 0;
}
