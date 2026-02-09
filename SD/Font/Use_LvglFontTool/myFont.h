#ifndef	__MYFONT_H
#define	__MYFONT_H


#include "lvgl.h"
#include "stdio.h"

#if keil
#include "W25Qxx.h"
#include "ff.h"
#include "USART1.h"
#endif // keil


#define USE_LvglFontTool 0

typedef struct{
    uint16_t min;
    uint16_t max;
    uint8_t  bpp;
    uint8_t  reserved[3];
}x_header_t;
typedef struct{
    uint32_t pos;
}x_table_t;
typedef struct{
    uint8_t adv_w;
    uint8_t box_w;
    uint8_t box_h;
    int8_t  ofs_x;
    int8_t  ofs_y;
    uint8_t r;
}glyph_dsc_t;

void update_font(void);
extern lv_font_t myFont_12;
extern lv_font_t myFont_16;
extern lv_font_t myFont_24;
extern lv_font_t myFont_32;

#if keil

#else
void memory_load_font(void);
extern uint8_t * g_font_mem_ptr;
#endif // keil

const uint8_t * __user_font_get_bitmap_external(const lv_font_t * font, uint32_t unicode_letter) ;
#endif
