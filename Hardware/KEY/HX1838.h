#ifndef	__HX1838_H
#define	__HX1838_H
#include "stm32f4xx.h"
#include "stdio.h"
#include "USART1.h"

typedef enum
{
HX1823_NULL=0,

HX1823_Num1=0x45,
HX1823_Num2=0x46,
HX1823_Num3=0x47,
HX1823_Num4=0x44,
HX1823_Num5=0x40,
HX1823_Num6=0x43,
HX1823_Num7=0x07,
HX1823_Num8=0x15,
HX1823_Num9=0x09,
HX1823_Num0=0x19,

HX1823_Star=0x16,
HX1823_Jing=0x0D,

HX1823_Up=0x18,
HX1823_Down=0x52,
HX1823_Left=0x08,
HX1823_Right=0x5A,

HX1823_OK=0x1C,
}HX1828_KeyNum;

void HX1838_Init(void);
HX1828_KeyNum Get_HX1838_KeyNum(void);
#endif
