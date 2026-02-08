#ifndef	__LVGL_TASK_H
#define	__LVGL_TASK_H
#include "stm32f4xx.h"
#include "LVGL_Timer.h"

#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "Delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lvgl.h"
#include "USART1.h"
#include "malloc.h"

#include "ui_main.h"
#include "key.h"
#include "lv_port_indev.h"

#include "W25Qxx.h"
#include "DX_WF25.h"

#include "Queue.h"
#include "myFont.h"

void lvgl_demo(void);

#endif


