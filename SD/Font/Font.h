#ifndef __FONT_H
#define __FONT_H

#include "lvgl.h"
#include "stdio.h"

#define signed_font_path    "0:/GitHub_Code/My_STM32_Phone/SD/Font/font/FONT12.bin"

#define  usigned_font_path "0:/GitHub_Code/My_STM32_Phone/SD/Font/Only_bin/font12.bin"

#define Font_path signed_font_path

void memory_load_font(void);
const uint8_t * __user_font_get_bitmap_external(const lv_font_t * font, uint32_t unicode_letter);
#endif
