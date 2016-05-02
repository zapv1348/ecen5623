#include <FreeRTOS_ARM.h>

const float A[5][5]={{1.3,1.7,4.2,3.1,8.9},
                    {2.8,-1.7,-4.1,-33.47,42.11},
                    {0.008,2203.0,120.7,-4653.3,0.7823},
                    {8.2,9.4,10.7,-3.8,-7.2},
                    {-10.3,12.3,-47.3,84.0,94.381}};
const float B[]={1.0,
                 2.0,
                 4.7,
                 -5.8,
                 -0.013};
const float C[2][5]={{-2.3,3.4,-4.2,-5.891,3.395},
                      {-4.8,5.3,7.1,2.97,3.18}};

const float L[5][2]={{-1.105,1.751},
                    {32.47,-41.58},
                    {950.1,-31.5},
                    {85.7,-48.3},
                    {77.1,34.8}};
const float K[]={4.8,3.3,-4.2,-7.7,8.1};

SemaphoreHandle_t sem1,sem2;

void Observer(void * params)
{
    float x[]={1.0,3,3,4.4,2.7,1.8};
    float xnew[5]={1.3,2.2,4.1,-3.7,8.8};
    float u=1.0;
    float utemp=2.7;
    float unew=1.8
    float yest[2]={3.8,4.8};
    float yerr[2]={3.1,5.7};
    float ymeas[2]={-1.2,4.2};
    float temp[5];
    int i;
    int time1;
    int maxtime=0;
    int n;
    while(n<100000)
    {
        ymeas[0]=(float)(analogRead(10));
        ymeas[1]=(float)(analogRead(5));
        time1=micros();
        u=unew;
        for(i=0;i<5;i++)
        {
          x[i]=xnew[i];
          utemp+=x[i]*K[i];
        }
        //mutex_lock
        xSemaphoreTake(sem1,0);
        unew=utemp;
        xSemaphoreGive(sem1);
        //mutex unlock

        //mutex_lock
        xSemaphoreTake(sem2,0);
        yerr[0]=ymeas[0]-yest[0];
        yerr[1]=ymeas[1]-yest[1];
        //mutex unlock
        xSemaphoreGive(sem2);
        yest[0]=0;
        yest[1]=0;
        
        for(i=0;i<5;i++)
        {
          temp[i]=u*B[i]+yerr[0]*L[i][0]+yerr[1]*L[i][1];
          yest[0]+=C[0][i]*x[i];
          yest[1]+=C[1][i]*x[i];
        }
        for(i=0;i<5;i++)
        {
            xnew[i]=temp[i]+x[0]*A[i][0]+x[1]*A[i][1]+x[2]*A[i][2]+x[3]*A[i][3]+x[4]*A[i][4];
        }
        time1=micros()-time1;
        if(time1>maxtime)
        {
          maxtime=time1;
        }
        n++;
    }
    Serial.print("max time after 100000 trialswas");
    Serial.print(maxtime);
    Serial.println(" in microseconds");
    while(1);
}


void setup() {
    portBASE_TYPE s;
    s = xTaskCreate(Observer, NULL, 200, NULL, 3, NULL);
    sem1=xSemaphoreCreateMutex();
    sem2=xSemaphoreCreateMutex();
    Serial.begin(115200);
  // start tasks
  vTaskStartScheduler();
  Serial.println("Scheduler failed");
  while(1);
}
//------------------------------------------------------------------------------
// WARNING idle loop has a very small stack (configMINIMAL_STACK_SIZE)
// loop must never block
void loop() {
  // not used
}
