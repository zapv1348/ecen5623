#include <FreeRTOS_ARM.h>


SemaphoreHandle_t sem1,sem2;

void writer(void * params)
{

    float ytemp[2]={3.1,5.7};
    float ymeas[2]={-1.2,4.2};
    int count1=35555;
    int count2=32010;
    int n;
    int unew;
    int time1;
    int oldtime=0;
    while(n<100000)
    {
        count1=analogRead(0);
        count2=analogRead(2);
        time1=micros();
        //mutex_lock
        xSemaphoreTake(sem1,0);
        if(unew>65535)
        {
          unew=65535;
        }
        if(unew<0)
        {
          unew=0;
        }
        analogWrite(5,(int)unew);
        xSemaphoreGive(sem1);
        //mutex unlock

        ytemp[0]=count1*355.251;
        ytemp[1]=count2*342.129;
        //mutex_lock
        xSemaphoreTake(sem2,0);
        ymeas[0]=ytemp[0];
        ymeas[1]=ytemp[1];
        xSemaphoreGive(sem2);
        //mutex unlock
        time1=micros()-time1;
        if(time1>oldtime)
        {
          oldtime=time1;
        }
        n++;
    }
    Serial.print("max time after 100000 trialswas");
    Serial.print(oldtime);
    Serial.println(" in microseconds");
    while(1);
}


void setup() {
    portBASE_TYPE s;
    s = xTaskCreate(writer, NULL, 200, NULL, 3, NULL);
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
