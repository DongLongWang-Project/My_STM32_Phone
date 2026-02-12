#ifndef __FONT_H
#define __FONT_H

#include "lvgl.h"
#include "stdio.h"
#include "ff.h"
#include "USART1.h"
#include "W25Qxx.h"

#define  Font_bitmap_path   "0:/GitHub_Code/My_STM32_Phone/SD/Font/font_bitmap.bin"
#define Font_index_path       "0:/GitHub_Code/My_STM32_Phone/SD/Font/font_index.bin" 
#define Font_Version_path       "0:/GitHub_Code/My_STM32_Phone/SD/Font/Font_Version.bin" 
 

#define FONT_VERSION_ADDR_HEAD 0x00000000
#define FONT_VERSION_ADDR_END  0x0031F157

#define FONT_12_INDEX_OFFSET   ((uint32_t)0x00000000+0x00000004)
#define FONT_16_INDEX_OFFSET   ((uint32_t)0x00029750+0x00000004)
#define FONT_24_INDEX_OFFSET   ((uint32_t)0x000374D8+0x00000004)
#define FONT_32_INDEX_OFFSET   ((uint32_t)0x00045260+0x00000004)

#define FONT_12_BITMAP_OFFSET   ((uint32_t)0x00000000+0x0004BFAC)
#define FONT_16_BITMAP_OFFSET   ((uint32_t)0x000BAE70+0x0004BFAC)
#define FONT_24_BITMAP_OFFSET   ((uint32_t)0x0012315B+0x0004BFAC)
#define FONT_32_BITMAP_OFFSET   ((uint32_t)0x00208E80+0x0004BFAC)


 

void memory_load_font(void);
const uint8_t * __user_font_get_bitmap_external(const lv_font_t * font, uint32_t unicode_letter);
bool __user_font_get_get_glyph_dsc_fmt_txt(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next);

extern uint8_t * g_font_mem_ptr_bitmap; // 定义一个全局指针存放字库数据
extern uint8_t * g_font_mem_ptr_index  ; // 定义一个全局指针存放字库数据
void update_font(void);
#endif
