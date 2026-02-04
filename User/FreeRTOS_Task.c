#include "FreeRTOS_Task.h"

uint32_t sum=0;
TaskHandle_t TaskHand1;
TaskHandle_t TaskHand2;
QueueHandle_t QueueclcaHandle;

void Task1(void *pvParameters)
{
  uint32_t i;
    while(1)
    {
        for(i=0;i<10000000;i++)
        {
         sum++;
            xQueueSend(QueueclcaHandle,&sum,portMAX_DELAY);
           
        }
      
    }
}

void Task2(void *pvParameters)
{
    while(1)
    {
        
    }    
}



void Start_Task(void)
{
    Serial_Init();
    LED_init();
    xTaskCreate(Task1,"task1",128,NULL,0,&TaskHand1);
    
    QueueclcaHandle=xQueueCreate(2,sizeof(uint32_t));
    if(QueueclcaHandle == NULL)
    {
        printf("NO!\r\n");
    }
    vTaskStartScheduler();
}
