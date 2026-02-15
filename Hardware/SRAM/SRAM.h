#ifndef 	__SRAM_H
#define	  __SRAM_H

#include "stm32f4xx.h"

#define EXT_SRAM_BASE   0x68000000
#define EXT_SRAM16      ((volatile uint16_t*)EXT_SRAM_BASE)

#define FONT_INDEX_ADDR  EXT_SRAM_BASE/*512KB*/
#define LVGL_DRAW_BUF_1    0x68080000 /*75KB*/
#define LVGL_DRAW_BUF_2    0x68092C00 /*75KB*/



void FSMC_SRAM_Init(void);
void SRAM_Write_Halfword(uint32_t Index, uint16_t data);
uint16_t SRAM_Read_Halfword(uint32_t Index);
#endif
