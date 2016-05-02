#include <math.h>
#include <cstdlib>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>


const float A[5][5]={{1.0, 5.2, 5.7, 3.7,4.8},
                    {1.2, 1.5, 1.7,2.3,2.4},
                    {4.2,4.7,3.3,10.1,7.1},
                    {8.2,9.4,10.7,3.8,7.2},
                    {-10.3,12.3,-47.3,84.0,94.0}};
const float B[]={1.0,
                2.0,
                -3.0,
                -4.0,
                8.7};
const float C[]={-2.0,3.0,-4.7,-5.7,-32};
const float L[]={-2.0,
                -3.0,
                -4.0,
                -7.0,
                -8.0};
const float K[]={4.8,3.3,4.2,7.7,8.1};

//some test code for 
int main()
{
    struct timespec a,b;
    float x[]={1.0,3.3,4.4,2.7,1.8};
    float xnew[5]={1.0,2.9,3.8,4.7,5.6};
    float u=1.0;
    float unew=2.7;
    float utemp=0.0;
    float yest=0.0;
    float yerr=0.0;
    float ymeas=0;
    float temp[5];
    int i;
    while(1)
    {   
        ymeas=(float)rand();
        //measure time
        clock_gettime(CLOCK_REALTIME,&a);
        u=unew;
        for(i=0;i<5;i++)
        {
            //delay state
            x[i]=xnew[i];
            //new system input
            utemp+=x[i]*K[i];
        }
        //mutex_lock
        unew=utemp;
        //mutex_unlock
        
        //mutex_lock
        yerr=ymeas-yest;
        //mutex_unlock
        for(i=0;i<5;i++)
        {
            //input from output error and previous input
            temp[i]=u*B[i]+yerr*L[i];
            //output estimate
            yest+=C[i]*x[i];
        }
        for(i=0;i<5;i++)
        {
            //This is the A matrix multiply to find the future state
            xnew[i]=temp[i]+x[0]*A[i][0]+x[1]*A[i][1]+x[2]*A[i][2]+x[3]*A[i][3]+x[4]*A[i][4];    
        }

        clock_gettime(CLOCK_REALTIME,&b);
        printf("that took %d seconds and %d nanoseconds\n",b.tv_sec-a.tv_sec,b.tv_nsec-a.tv_nsec);
    }
    return 0;
}
