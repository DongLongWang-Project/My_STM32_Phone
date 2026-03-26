#ifndef	__FLASH_H
#define	__FLASH_H
#include "stm32f4xx.h"

#define Flash_buf_size 1024*64
extern uint8_t read_buf[Flash_buf_size];
uint32_t MyFLASH_ReadWord(uint32_t Address);
uint16_t MyFLASH_ReadHalfWord(uint32_t Address);
uint8_t MyFLASH_ReadByte(uint32_t Address);

void MyFLASH_EraseAllPages(void);
void MyFLASH_ErasePage(uint32_t PageAddress);

void MyFLASH_ProgramWord(uint32_t Address, uint32_t Data);
void MyFLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);

#endif
