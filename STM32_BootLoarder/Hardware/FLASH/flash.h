#ifndef	__FLASH_H
#define	__FLASH_H
#include "stm32f4xx.h"
#include "string.h"


void myFLASH_ReadData(uint32_t Address, void* buf, uint32_t len);
void myFLASH_WriteData(uint32_t Flash_addr, uint8_t *data_buf, uint32_t len);
#endif
