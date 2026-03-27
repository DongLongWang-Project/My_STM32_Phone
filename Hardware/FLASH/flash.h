#ifndef	__FLASH_H
#define	__FLASH_H
#include "stm32f4xx.h"

#define FLASH_APP_Addr            0x08010000
#define Flash_buf_size 1024*64
extern uint8_t read_buf[Flash_buf_size];
void myFLASH_ReadData(uint32_t Address, void* buf, uint32_t len);

#endif
