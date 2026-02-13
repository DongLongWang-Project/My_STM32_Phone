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


void lvgl_demo(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    Delay_init(168);
    TIM2_Int_Init(10000-1,8400-1);
    Serial_Init();
    Key_Init();
    W25Qxx_SPI_Init();
    FSMC_SRAM_Init();
    DX_WF25_Init();
    update_font();

//    my_mem_init(SRAMIN);
      lv_init();                                          /* lvgl系统初始化 */
      lv_port_disp_init();                                /* lvgl显示接口初始化,放在lv_init()的后面 */
      lv_port_indev_init();                               /* lvgl输入接口初始化,放在lv_init()的后面 */

//    char*t=lv_mem_alloc(100);
//    print("新建的测试内存位置:0x%x",(uint32_t)t);
//    lv_mem_free(t); 
    
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

//   /* DX_WF25测试任务 */
//   xTaskCreate((TaskFunction_t )DX_WF25_task,
//               (const char*    )"KEY_task",
//               (uint16_t       )DX_WF25_STK_SIZE,
//               (void*          )NULL,
//               (UBaseType_t    )DX_WF25_TASK_PRIO,
//               (TaskHandle_t*  )&DX_WF25_Task_Handler);
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
//         if(alarm_time_is_on)
         vTaskDelay(5);
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
     DX_WF25_CMD_Queue=xQueueCreate(DX_WF25_Queue_MAX_LEN,sizeof(wifi_cmd_t));


     memset(&wifi_save_list, 0, sizeof(wifi_save_t));
//     W25Qxx_SectorErase(WIFI_SAVE_Addr);  
     W25Qxx_ReadData(WIFI_SAVE_Addr,(uint8_t*)&wifi_save_list,sizeof(wifi_save_t)); 
     if(wifi_save_list.save_count==255)
     {
      wifi_save_list.save_count=0;
     }
     #if keil
     Alarm_System_Init();
     #endif
     print("save_count:%d",wifi_save_list.save_count);
     DX_WF25_Send_Static(AT_CMD_ATE1);
     DX_WF25_Send_Static(AT_CMD_CIPMUX_ONE); 
     DX_WF25_Send_Static(AT_CMD_RST);

     while(1)
     {
//       if(wifi_scan_list.connected==1)
//       {
//          if(xSemaphoreTake(Timer_Send_AT_BinSemaphore, 0) == pdTRUE)
//          {
//            DX_WF25_Send_Static(AT_CMD_CIPMODE_0);
//            DX_WF25_Send_Static(AT_CMD_CIPSTART);
//            DX_WF25_Send_Dynamic(AT_CMD_CIPSEND,"AT+CIPSEND=%d\r\n",strlen(get_time_weather_str)); 
//          }
//       }
        wifi_cmd_stateMACHINE();
     }
   }

