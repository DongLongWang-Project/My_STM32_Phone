/*
*---------------------------------------------------------------
*                        Lvgl Font Tool                         
*                                                               
* 注:使用unicode编码                                              
* 注:本字体文件由Lvgl Font Tool V0.4 生成                          
* 作者:阿里(qq:617622104)                                         
*---------------------------------------------------------------
*/
#include "myFont.h"




static x_header_t __g_xbf_hd = {
    .min = 0x0020,
    .max = 0xf2e0,
    .bpp = 2,
};
static uint8_t __g_font_buf[528];//如bin文件存在SPI FLASH可使用此buff

uint8_t * g_font_mem_ptr = NULL; // 定义一个全局指针存放字库数据
#if keil

#else
void memory_load_font(void)
 {
    lv_fs_file_t f;
    lv_fs_res_t res = lv_fs_open(&f,"0:/GitHub_Code/My_STM32_Phone/SD/font_w25q/Font.bin",LV_FS_MODE_RD);

//    lv_fs_res_t res = lv_fs_open(&f,"0:/GitHub_Code/My_STM32_Phone/SD/font_w25q/myFont_1.bin",LV_FS_MODE_RD); 
     
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
#endif // keil

 
static uint8_t *__user_font_getdata(const lv_font_t * font,int offset, int size){
    //如字模保存在SPI FLASH, SPIFLASH_Read(__g_font_buf,offset,size);
    //如字模已加载到SDRAM,直接返回偏移地址即可如:return (uint8_t*)(sdram_fontddr+offset);
    #if keil
    W25Qxx_ReadData((uint32_t)font->user_data+offset,__g_font_buf,size);
    #else
// 模拟器环境：直接计算指针地址返回
        if (g_font_mem_ptr != NULL) {
            // 返回地址 = 内存首地址 + 字库基地址 + 内部偏移
            return &g_font_mem_ptr[(uint32_t)font->user_data + offset];
        }
        return NULL;
    #endif
    return __g_font_buf;
}


static const uint8_t * __user_font_get_bitmap(const lv_font_t * font, uint32_t unicode_letter) {
    if( unicode_letter>__g_xbf_hd.max || unicode_letter<__g_xbf_hd.min ) {
        return NULL;
    }
    uint32_t unicode_offset = sizeof(x_header_t)+(unicode_letter-__g_xbf_hd.min)*4;
    uint32_t *p_pos = (uint32_t *)__user_font_getdata(font,unicode_offset, 4);
    if( p_pos[0] != 0 ) {
        uint32_t pos = p_pos[0];
        glyph_dsc_t * gdsc = (glyph_dsc_t*)__user_font_getdata(font,pos, sizeof(glyph_dsc_t));
        return __user_font_getdata(font,pos+sizeof(glyph_dsc_t), gdsc->box_w*gdsc->box_h*__g_xbf_hd.bpp/8);
    }
    return NULL;
}


static bool __user_font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next) {
    if( unicode_letter>__g_xbf_hd.max || unicode_letter<__g_xbf_hd.min ) {
        return NULL;
    }
    uint32_t unicode_offset = sizeof(x_header_t)+(unicode_letter-__g_xbf_hd.min)*4;
    uint32_t *p_pos = (uint32_t *)__user_font_getdata(font,unicode_offset, 4);
    if( p_pos[0] != 0 ) {
        glyph_dsc_t * gdsc = (glyph_dsc_t*)__user_font_getdata(font,p_pos[0], sizeof(glyph_dsc_t));
        dsc_out->adv_w = gdsc->adv_w;
        dsc_out->box_h = gdsc->box_h;
        dsc_out->box_w = gdsc->box_w;
        dsc_out->ofs_x = gdsc->ofs_x;
        dsc_out->ofs_y = gdsc->ofs_y;
        dsc_out->bpp   = __g_xbf_hd.bpp;
        return true;
    }
    return false;
}


//Source Han Sans CN Bold,,-1
//字模高度：18
//XBF字体,外部bin文件
lv_font_t myFont_12 = {
    .get_glyph_bitmap = __user_font_get_bitmap,
    .get_glyph_dsc = __user_font_get_glyph_dsc,
    .line_height = 18,
    .base_line = 0,
    .fallback = &lv_font_montserrat_12,
    #if keil
    .user_data = (void*)0x000000,
    #else
    .user_data = (void*)0x000000, 
    #endif
};

//Source Han Sans CN Bold,,-1
//字模高度：24
//XBF字体,外部bin文件
lv_font_t myFont_16 = {
    .get_glyph_bitmap = __user_font_get_bitmap,
    .get_glyph_dsc = __user_font_get_glyph_dsc,
    .line_height = 24,
    .base_line = 0,
    .fallback = &lv_font_montserrat_16,
    #if keil
    .user_data = (void*)0x200000,
    #else
    .user_data = (void*)0x00130697,
    #endif
};


//Source Han Sans CN Bold,,-1
//字模高度：36
//XBF字体,外部bin文件
lv_font_t myFont_24 = {
    .get_glyph_bitmap = __user_font_get_bitmap,
    .get_glyph_dsc = __user_font_get_glyph_dsc,
    .line_height = 36,
    .base_line = 0,
    .fallback = &lv_font_montserrat_24,
    
    #if keil
    .user_data = (void*)0x500000,
    #else
    .user_data = (void*)0x002E6EBC,
    #endif
};

//Source Han Sans CN Bold,,-1
//字模高度：48
//XBF字体,外部bin文件
lv_font_t myFont_32 = {
    .get_glyph_bitmap = __user_font_get_bitmap,
    .get_glyph_dsc = __user_font_get_glyph_dsc,
    .line_height = 48,
    .base_line = 0,
    .fallback = &lv_font_montserrat_32,
    #if keil
    .user_data = (void*)0x900000,
    #else
    .user_data = (void*)0x006336AB,
    #endif
};

#if keil
void Update_Font_to_W25Qxx(const char*file_path,uint32_t Font_addr)
{
  FIL f;
  FRESULT fr;
  UINT br;
//  uint8_t state;
  static uint8_t sector_buf[4096];
  uint32_t cur_addr=Font_addr;
  
  fr=f_open(&f,file_path,FA_READ);
  if(fr!=FR_OK)
  {
    print("打开文件失败\r\n");
    return;
  }
  uint32_t file_size=f_size(&f);
  print("开始写入%s,大小:%d字节\r\n",file_path,file_size);
  
  while(file_size>0)
  {
    fr=f_read(&f,sector_buf,4096,&br);
//    print("读取数据:%d...\r\n",br);
    W25Qxx_SectorErase(cur_addr);
//    state=W25Qxx_SectorErase(cur_addr);
//    if(state)
//    {
//     print("擦除扇区:%d失败\r\n",cur_addr); 
//    }
//    else
//    {
//     print("擦除扇区:%d成功\r\n",cur_addr); 
//    }
    W25Qxx_WriteBuffer(cur_addr,sector_buf,br);
    cur_addr+=br;
    file_size-=br;
    print("剩余:%d...\r\n",file_size);
  }
  f_close(&f);
  print("%s写入完成\r\n",file_path);
}


void update_font(void)
{
  Update_Font_to_W25Qxx("0:/SD/font_w25q/myFont_12.bin",(uint32_t)myFont_12.user_data);
  Update_Font_to_W25Qxx("0:/SD/font_w25q/myFont_16.bin",(uint32_t)myFont_16.user_data); 
  Update_Font_to_W25Qxx("0:/SD/font_w25q/myFont_24.bin",(uint32_t)myFont_24.user_data);
  Update_Font_to_W25Qxx("0:/SD/font_w25q/myFont_32.bin",(uint32_t)myFont_32.user_data);
}
#endif