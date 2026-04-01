#include "LVGL_Task.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置

 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
   */
   #define START_TASK_PRIO     1           /* 任务优先级 */
   #define START_STK_SIZE      128         /* 任务堆栈大小 */
   TaskHandle_t StartTask_Handler;         /* 任务句柄 */
   void start_task(void *pvParameters);    /* 任务函数 */

/* LV_DEMO_TASK 任务 配置

 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
   */
   #define LV_DEMO_TASK_PRIO   3           /* 任务优先级 */
   #define LV_DEMO_STK_SIZE    2048         /* 任务堆栈大小 */
   TaskHandle_t LV_DEMOTask_Handler;       /* 任务句柄 */
   void lv_demo_task(void *pvParameters);  /* 任务函数 */

/* KEY_TASK 任务 配置

 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
   */
   #define DX_WF25_TASK_PRIO       2           /* 任务优先级 */
   #define DX_WF25_STK_SIZE        1024         /* 任务堆栈大小 */
   TaskHandle_t DX_WF25_Task_Handler;           /* 任务句柄 */
   void DX_WF25_task(void *pvParameters);      /* 任务函数 */
   /******************************************************************************************************/

/* AUDIO_Task 任务 配置

 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
   */
   #define AUDIO_Task_PRIO       4           /* 任务优先级 */
   #define AUDIO_Task_STK_SIZE        512         /* 任务堆栈大小 */
   TaskHandle_t AUDIO_Task_Handler;           /* 任务句柄 */
   void AUDIO_task(void *pvParameters);      /* 任务函数 */
   /******************************************************************************************************/

void MyPhone_stm32_task(void)
{
 
     lv_init();                                          /* lvgl系统初始化 */
     lv_port_disp_init();                                /* lvgl显示接口初始化,放在lv_init()的后面 */
     lv_port_indev_init();                               /* lvgl输入接口初始化,放在lv_init()的后面 */

 
    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传递给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */
    
    vTaskStartScheduler();                              /* 开启任务调度 */

}

/**

 * @brief       start_task

 * @param       pvParameters : 传入参数(未用到)

 * @retval      无
   */
   void start_task(void *pvParameters)
   {
   pvParameters = pvParameters;
   taskENTER_CRITICAL();           /* 进入临界区 */

   /* 创建LVGL任务 */
   xTaskCreate((TaskFunction_t )lv_demo_task,
               (const char*    )"lv_demo_task",
               (uint16_t       )LV_DEMO_STK_SIZE, 
               (void*          )NULL,
               (UBaseType_t    )LV_DEMO_TASK_PRIO,
               (TaskHandle_t*  )&LV_DEMOTask_Handler);

   /* DX_WF25测试任务 */
   xTaskCreate((TaskFunction_t )DX_WF25_task,
               (const char*    )"KEY_task",
               (uint16_t       )AUDIO_Task_STK_SIZE,
               (void*          )NULL,
               (UBaseType_t    )DX_WF25_TASK_PRIO,
               (TaskHandle_t*  )&DX_WF25_Task_Handler);
               
   /* AUDIO_Task测试任务 */
   xTaskCreate((TaskFunction_t )AUDIO_task,
               (const char*    )"AUDIO_task",
               (uint16_t       )DX_WF25_STK_SIZE,
               (void*          )NULL,
               (UBaseType_t    )AUDIO_Task_PRIO,
               (TaskHandle_t*  )&AUDIO_Task_Handler);            
               
   taskEXIT_CRITICAL();            /* 退出临界区 */
   vTaskDelete(StartTask_Handler); /* 删除开始任务 */
   }

/**

 * @brief       LVGL运行例程

 * @param       pvParameters : 传入参数(未用到)

 * @retval      无
   */
   void lv_demo_task(void *pvParameters)
   {
     pvParameters = pvParameters;

     ui_init();
   

     while(1)
     {
         lv_timer_handler(); /* LVGL计时器 */
         vTaskDelay(2);
     }
   }

/**

 * @brief       KEY_task

 * @param       pvParameters : 传入参数(未用到)

 * @retval      无
   */
   void DX_WF25_task(void *pvParameters)
   {
     pvParameters = pvParameters;

     while(1)
     {

        wifi_cmd_stateMACHINE();
     }
   }
/**

 * @brief       AUDIO_task

 * @param       pvParameters : 传入参数(未用到)

 * @retval      无
   */
void AUDIO_task(void *pvParameters)
{
  pvParameters = pvParameters;
  
  while(1)
  {
      AudioTask(); 
  }
}
