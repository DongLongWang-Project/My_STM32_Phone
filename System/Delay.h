#ifndef __DELAY_H
#define __DELAY_H
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
void Delay_init(u8 SYSCLK);

void Delay_ms(u32 nms);
void Delay_xms(u32 nms);
void Delay_us(u32 nus);

#endif

