#ifndef __FONT_H
#define __FONT_H

#include "lvgl.h"
#include "stdio.h"


#include "../../my_gui/app_setting/ui_app_setting.h"
#if keil
#include "ff.h"
#include "USART1.h"
#include "W25Qxx.h"
#endif // keil

//目标文件:G:\GitHub_Code\My_STM32_Phone\SD\Font\Font.bin   大小:0x0031F173
//各部分大小如下:

//索引                        文件名                                            大小              偏移量            强制插入
// 1      G:\GitHub_Code\My_STM32_Phone\SD\Font\Font_Version.bin              0x00000010        0x00000000             否
// 2      G:\GitHub_Code\My_STM32_Phone\SD\Font\font_index.bin                0x0004BFA8        0x00000010             否
// 3      G:\GitHub_Code\My_STM32_Phone\SD\Font\font_bitmap.bin               0x002D31AB        0x0004BFB8             否
// 4      G:\GitHub_Code\My_STM32_Phone\SD\Font\Font_Version.bin              0x00000010        0x0031F163             否


#define Font_bitmap_path        "0:/GitHub_Code/My_STM32_Phone/SD/Font/font_bitmap.bin"
#define Font_index_path         "0:/GitHub_Code/My_STM32_Phone/SD/Font/font_index.bin" 
#define Font_Version_path       "0:/GitHub_Code/My_STM32_Phone/SD/Font/Font_Version.bin"
#if keil
#define Font_path                "0:/SD/Font/Font.bin"
#else
#define Font_path       "0:/GitHub_Code/My_STM32_Phone/SD/Font/Font.bin"
#endif // keil
#define FONT_VERSION_ADDR_HEAD 0x00000000
#define FONT_VERSION_ADDR_END  0x0031F163

#define Font_Version_Size   0x00000010
#define Font_Index_Size     0x0004BFA8
#define Font_Bitmap_Size    0x002D31AB


#define FONT_12_INDEX_OFFSET   (uint32_t)(0x00000000+Font_Version_Size)
#define FONT_16_INDEX_OFFSET   (uint32_t)(0x00029750+Font_Version_Size)
#define FONT_24_INDEX_OFFSET   (uint32_t)(0x000374D8+Font_Version_Size)
#define FONT_32_INDEX_OFFSET   (uint32_t)(0x00045260+Font_Version_Size)

#define FONT_12_BITMAP_OFFSET   (uint32_t)(0x00000000+0x0004BFB8)
#define FONT_16_BITMAP_OFFSET   (uint32_t)(0x000BAE70+0x0004BFB8)
#define FONT_24_BITMAP_OFFSET   (uint32_t)(0x0012315B+0x0004BFB8)
#define FONT_32_BITMAP_OFFSET   (uint32_t)(0x00208E80+0x0004BFB8)

typedef struct {
    uint32_t version;
    uint32_t Index_Size;
    uint32_t Bitmap_Size;
    uint32_t total_Size;   
}Font_Version_t;
enum
{
  SD_data=0,
  W25Qxx_head_data,
  W25Qxx_end_data,
};

 
#if keil

#else
void memory_load_font(void);
#endif

const uint8_t * __user_font_get_bitmap_external(const lv_font_t * font, uint32_t unicode_letter);
bool __user_font_get_get_glyph_dsc_fmt_txt(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next);

extern uint8_t * g_font_mem_ptr_bitmap; // 定义一个全局指针存放字库数据
extern uint8_t * g_font_mem_ptr_index; // 定义一个全局指针存放字库数据
void update_font(void);
uint8_t Font_index_to_SDRAM(uint32_t Font_Index_Addr,uint32_t SDRAM_Addr);

#endif
