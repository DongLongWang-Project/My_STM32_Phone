#ifndef 	__SRAM_H
#define	  __SRAM_H

#include "stm32f4xx.h"

#define EXT_SRAM_BASE   0x68000000
#define EXT_SRAM16      ((volatile uint16_t*)EXT_SRAM_BASE)

void FSMC_SRAM_Init(void);
void SRAM_Write_Halfword(uint32_t Index, uint16_t data);
uint16_t SRAM_Read_Halfword(uint32_t Index);
#endif
