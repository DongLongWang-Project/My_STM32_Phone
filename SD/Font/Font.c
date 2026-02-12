#include "Font.h"

uint8_t * g_font_mem_ptr_bitmap = NULL; // 定义一个全局指针存放字库数据
uint8_t * g_font_mem_ptr_index = NULL; // 定义一个全局指针存放字库数据

LV_FONT_DECLARE( my_font_12);
LV_FONT_DECLARE( my_font_16);
LV_FONT_DECLARE( my_font_24);
LV_FONT_DECLARE( my_font_32);

#if !keil
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
        
        printf("地址挂载成功：Index=%p, Bitmap=%p\n", g_font_mem_ptr_index, g_font_mem_ptr_bitmap);
    }
}
#endif
const uint8_t * __user_font_get_bitmap_external(const lv_font_t * font, uint32_t unicode_letter) 
{
    if(unicode_letter == '\t') unicode_letter = ' ';
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;

    uint32_t gid = get_glyph_dsc_id(font, unicode_letter);
    if(!gid) return NULL;
    
    lv_font_fmt_txt_glyph_dsc_t * my_index_array;
    #if keil
    
    
    #else
    
    
    
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
    
    #endif
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
    #if keil
    
    
    
    #else
    

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
    #endif
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


static void Update_Font_to_W25Qxx(const char*file_path,uint32_t Font_addr)
{
  FIL f;
  FRESULT fr;
  UINT br;
//  uint8_t state;
  uint8_t sector_buf[4096];
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
    print("读取数据:%d...\r\n",br);
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
    if(file_size>br)
    {
      file_size-=br;
    }
    else
    {
      file_size=0;
    }
    print("剩余:%d...\r\n",file_size);
  }
  f_close(&f);
  print("%s写入完成\r\n",file_path);
}

uint8_t Read_Font_Version(const char*file_path,uint32_t Version_addr)
{
  FIL f;
  FRESULT fr;
  UINT br;
  
  uint32_t SD_version = 0;
  uint32_t W25Q_head_version = 0;
  uint32_t W25Q_end_version = 0;
  uint32_t file_size = 0;
    
  fr=f_open(&f,file_path,FA_READ);

  if(fr!=FR_OK)
  {
    print("打开文件失败\r\n");
    return 0;
  }
  else
  {
    file_size = f_size(&f);
    fr=f_read(&f,&SD_version,4,&br);
    if(fr!=FR_OK)
    {
      print("读取文件失败\r\n");
      f_close(&f);
      return 0;  
    }
  }
  f_close(&f);
  
  W25Qxx_ReadData(Version_addr,(uint8_t*)&W25Q_head_version,4);
  W25Qxx_ReadData(Version_addr+file_size-4,(uint8_t*)&W25Q_end_version,4);
    
  if(SD_version==W25Q_head_version && SD_version==W25Q_end_version)
  {
    print("W25q有最新字库,无需更新\r\n");
    return 1;
  }
  else
  { 
    print("SD有新字库,需要更新\r\n");
    return 0;
    
  }
}
void update_font(void)
{
    FATFS fs;
    FRESULT res;
     res=f_mount(&fs,"0",1);
     if(res!=FR_OK)
     {
      print("烧入字库挂载sd失败\r\n");
      return;
     }
     else
     {
        print("挂载成功\r\n");
        if(Read_Font_Version("0:/SD/Font/Font.bin",FONT_VERSION_ADDR_HEAD)==0)
        {
          Update_Font_to_W25Qxx("0:/SD/Font/Font.bin",FONT_VERSION_ADDR_HEAD);          
        }
        else
        {
          /*字库正常*/
        }
     }
     f_mount(NULL,"0",1);
}
