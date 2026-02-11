#include "Font.h"

uint8_t * g_font_mem_ptr_bitmap = NULL; // 定义一个全局指针存放字库数据
uint8_t * g_font_mem_ptr_index = NULL; // 定义一个全局指针存放字库数据

LV_FONT_DECLARE( my_font_12);
LV_FONT_DECLARE( my_font_16);
LV_FONT_DECLARE( my_font_24);
LV_FONT_DECLARE( my_font_32);
void memory_load_font(void)
 {
    lv_fs_file_t f;
    lv_fs_res_t res;
    
    res = lv_fs_open(&f,Font_bitmap_path,LV_FS_MODE_RD);
     
    if(res == LV_FS_RES_OK) {
        // 1. 获取文件总大小
        uint32_t size;
        lv_fs_seek(&f, 0, LV_FS_SEEK_END);
        lv_fs_tell(&f, &size);
        lv_fs_seek(&f, 0, LV_FS_SEEK_SET);

        // 2. 申请对应的内存
        g_font_mem_ptr_bitmap = (uint8_t *)lv_mem_alloc(size);

        if(g_font_mem_ptr_bitmap) {
            uint32_t br;
            lv_fs_read(&f, g_font_mem_ptr_bitmap, size, &br); // 一次性读入内存
            printf("字库点阵加载成功，大小: %d bytes\r\n", size);
        }
        lv_fs_close(&f);
    } else {
        
        printf("加载字库点阵失败！\n");
        lv_fs_close(&f);
    }
    
    res = lv_fs_open(&f,Font_index_path,LV_FS_MODE_RD);
     
    if(res == LV_FS_RES_OK) {
        // 1. 获取文件总大小
        uint32_t size;
        lv_fs_seek(&f, 0, LV_FS_SEEK_END);
        lv_fs_tell(&f, &size);
        lv_fs_seek(&f, 0, LV_FS_SEEK_SET);

        // 2. 申请对应的内存
        g_font_mem_ptr_index = (uint8_t *)lv_mem_alloc(size);

        if(g_font_mem_ptr_index) {
            uint32_t br;
            lv_fs_read(&f, g_font_mem_ptr_index, size, &br); // 一次性读入内存
            printf("字库索引加载成功，大小: %d bytes\r\n", size);
        }
        lv_fs_close(&f);
    } else {
        
        printf("加载字库索引失败！\n");
        lv_fs_close(&f);

    }

 if(g_font_mem_ptr_bitmap && g_font_mem_ptr_index) {
        // 假设你的字体变量名是 my_font_12
        lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)my_font_12.dsc;
        
//        fdsc->glyph_bitmap = (const uint8_t *)g_font_mem_ptr_bitmap;
//        fdsc->glyph_dsc = (const lv_font_fmt_txt_glyph_dsc_t *)g_font_mem_ptr_index;
        
        printf("地址挂载成功：Index=%p, Bitmap=%p\n", g_font_mem_ptr_index, g_font_mem_ptr_bitmap);
    }
}
const uint8_t * __user_font_get_bitmap_external(const lv_font_t * font, uint32_t unicode_letter) 
{
    if(unicode_letter == '\t') unicode_letter = ' ';
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;

    uint32_t gid = get_glyph_dsc_id(font, unicode_letter);
    if(!gid) return NULL;
    
    lv_font_fmt_txt_glyph_dsc_t * my_index_array;
    
    if(font==&my_font_12)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(g_font_mem_ptr_index+FONT_12_INDEX_OFFSET);   
    }
    else if(font==&my_font_16)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(g_font_mem_ptr_index+FONT_16_INDEX_OFFSET);   
    }
   else if(font==&my_font_24)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(g_font_mem_ptr_index+FONT_24_INDEX_OFFSET);   
    }
    else if(font==&my_font_32)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(g_font_mem_ptr_index+FONT_32_INDEX_OFFSET);   
    }
    
    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &my_index_array[gid];

    if(fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN) {
        return &g_font_mem_ptr_bitmap[gdsc->bitmap_index+(uint32_t)font->user_data];
    }
}

bool __user_font_get_get_glyph_dsc_fmt_txt(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
    bool is_tab = false;
    if(unicode_letter == '\t') {
        unicode_letter = ' ';
        is_tab = true;
    }
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
    uint32_t gid = get_glyph_dsc_id(font, unicode_letter);
    if(!gid) return false;

    int8_t kvalue = 0;
    if(fdsc->kern_dsc) {
        uint32_t gid_next = get_glyph_dsc_id(font, unicode_letter_next);
        if(gid_next) {
            kvalue = get_kern_value(font, gid, gid_next);
        }
    }
    lv_font_fmt_txt_glyph_dsc_t * my_index_array;

    if(font==&my_font_12)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(g_font_mem_ptr_index+FONT_12_INDEX_OFFSET);   
    }
    else if(font==&my_font_16)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(g_font_mem_ptr_index+FONT_16_INDEX_OFFSET);   
    }
   else if(font==&my_font_24)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(g_font_mem_ptr_index+FONT_24_INDEX_OFFSET);   
    }
    else if(font==&my_font_32)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(g_font_mem_ptr_index+FONT_32_INDEX_OFFSET);   
    }
    
    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &my_index_array[gid];
    
    int32_t kv = ((int32_t)((int32_t)kvalue * fdsc->kern_scale) >> 4);

    uint32_t adv_w = gdsc->adv_w;
    if(is_tab) adv_w *= 2;

    adv_w += kv;
    adv_w  = (adv_w + (1 << 3)) >> 4;

    dsc_out->adv_w = adv_w;
    dsc_out->box_h = gdsc->box_h;
    dsc_out->box_w = gdsc->box_w;
    dsc_out->ofs_x = gdsc->ofs_x;
    dsc_out->ofs_y = gdsc->ofs_y;
    dsc_out->bpp   = (uint8_t)fdsc->bpp;
    dsc_out->is_placeholder = false;

    if(is_tab) dsc_out->box_w = dsc_out->box_w * 2;

    return true;
}