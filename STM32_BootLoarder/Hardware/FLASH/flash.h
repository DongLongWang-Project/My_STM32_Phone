#ifndef	__FLASH_H
#define	__FLASH_H
#include "stm32f4xx.h"


void myFLASH_ReadData(uint32_t Address, void* buf, uint32_t len);

#endif
