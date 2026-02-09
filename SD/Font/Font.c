#include "Font.h"

uint8_t * g_font_mem_ptr = NULL; // 定义一个全局指针存放字库数据

void memory_load_font(void)
 {
    lv_fs_file_t f;

    lv_fs_res_t res = lv_fs_open(&f,Font_path,LV_FS_MODE_RD);
     
    if(res == LV_FS_RES_OK) {
        // 1. 获取文件总大小
        uint32_t size;
        lv_fs_seek(&f, 0, LV_FS_SEEK_END);
        lv_fs_tell(&f, &size);
        lv_fs_seek(&f, 0, LV_FS_SEEK_SET);

        // 2. 申请对应的内存
        g_font_mem_ptr = (uint8_t *)lv_mem_alloc(size);

        if(g_font_mem_ptr) {
            uint32_t br;
            lv_fs_read(&f, g_font_mem_ptr, size, &br); // 一次性读入内存
            printf("字库全量加载成功，大小: %d bytes\r\n", size);
        }
        lv_fs_close(&f);
    } else {
        
        printf("加载字库失败！\n");
        lv_fs_close(&f);
        return;
    }
}
const uint8_t * __user_font_get_bitmap_external(const lv_font_t * font, uint32_t unicode_letter) 
{
    if(unicode_letter == '\t') unicode_letter = ' ';
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
    uint32_t gid = get_glyph_dsc_id(font, unicode_letter);
    if(!gid) return NULL;

    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &fdsc->glyph_dsc[gid];
    
    #if keil
//    uint32_t size = (gdsc->box_w * gdsc->box_h * fdsc->bpp + 7) / 8;
//    if(size == 0) return NULL;
//

//    uint32_t flash_addr = (uint32_t)font->user_data + gdsc->bitmap_index;

//    W25QXX_Read(font_pixel_buf, flash_addr, size);
//
//    return font_pixel_buf; 

//        uint32_t data_start_offset = *(uint32_t*)g_font_mem_ptr;
//        
//    if(fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN)
//        {
//        return &g_font_mem_ptr[gdsc->bitmap_index];
//    }
    #else
    if(fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN) {
        return &g_font_mem_ptr[gdsc->bitmap_index+(uint32_t)font->user_data];
    }
    
    #endif // keil
}

