#ifndef __USART1_H
#define __USART1_H

#include "stm32f4xx.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "Queue.h"
#include "semphr.h"
#include "string.h"
#include <stdarg.h>

#define USART_Tx_BUF_SIZE     1024
void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendString(char *String);
void USART_DMA_SendString(char *String);
void print(char *format, ...);
#endif
