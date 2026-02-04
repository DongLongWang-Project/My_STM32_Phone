#ifndef	__LVGL_TIMER_H
#define	__LVGL_TIMER_H
#include "stm32f4xx.h"

void TIM3_Int_Init(uint16_t arr, uint16_t psc);
void TIM2_Int_Init(uint16_t arr, uint16_t psc);
void TIM12_Init(void);

#endif
