#include "Font.h"

uint8_t * g_font_mem_ptr_bitmap = NULL; // 定义一个全局指针存放字库数据
uint8_t * g_font_mem_ptr_index = NULL; // 定义一个全局指针存放字库数据
uint8_t * Font_mem = NULL; // 定义一个全局指针存放字库数据

LV_FONT_DECLARE( my_font_12);
LV_FONT_DECLARE( my_font_16);
LV_FONT_DECLARE( my_font_24);
LV_FONT_DECLARE( my_font_32);

Font_Version_t Font_Data;
#define FONT_BUFFER_SIZE 256
static uint8_t Font_Bitmap_1[FONT_BUFFER_SIZE]; 

#if !keil
//void memory_load_font(void)
// {
//    lv_fs_file_t f;
//    lv_fs_res_t res;
//    
//    res = lv_fs_open(&f,Font_bitmap_path,LV_FS_MODE_RD);
//     
//    if(res == LV_FS_RES_OK) {
//        // 1. 获取文件总大小
//        uint32_t size;
//        lv_fs_seek(&f, 0, LV_FS_SEEK_END);
//        lv_fs_tell(&f, &size);
//        lv_fs_seek(&f, 0, LV_FS_SEEK_SET);
//
//        // 2. 申请对应的内存
//        g_font_mem_ptr_bitmap = (uint8_t *)lv_mem_alloc(size);
//
//        if(g_font_mem_ptr_bitmap) {
//            uint32_t br;
//            lv_fs_read(&f, g_font_mem_ptr_bitmap, size, &br); // 一次性读入内存
//            printf("字库点阵加载成功，大小: %d bytes\r\n", size);
//        }
//        lv_fs_close(&f);
//    } else {
//        
//        printf("加载字库点阵失败！\n");
//        lv_fs_close(&f);
//    }
//    
//    res = lv_fs_open(&f,Font_index_path,LV_FS_MODE_RD);
//     
//    if(res == LV_FS_RES_OK) {
//        // 1. 获取文件总大小
//        uint32_t size;
//        lv_fs_seek(&f, 0, LV_FS_SEEK_END);
//        lv_fs_tell(&f, &size);
//        lv_fs_seek(&f, 0, LV_FS_SEEK_SET);
//
//        // 2. 申请对应的内存
//        g_font_mem_ptr_index = (uint8_t *)lv_mem_alloc(size);
//
//        if(g_font_mem_ptr_index) {
//            uint32_t br;
//            lv_fs_read(&f, g_font_mem_ptr_index, size, &br); // 一次性读入内存
//            printf("字库索引加载成功，大小: %d bytes\r\n", size);
//        }
//        lv_fs_close(&f);
//    } else {
//        
//        printf("加载字库索引失败！\n");
//        lv_fs_close(&f);
//
//    }
//
// if(g_font_mem_ptr_bitmap && g_font_mem_ptr_index) {
//        
//        printf("地址挂载成功：Index=%p, Bitmap=%p\n", g_font_mem_ptr_index, g_font_mem_ptr_bitmap);
//    }
void memory_load_font(void)
 {
    lv_fs_file_t f;
    lv_fs_res_t res;
    
    res = lv_fs_open(&f,Font_path,LV_FS_MODE_RD);
     
    if(res == LV_FS_RES_OK) {
        // 1. 获取文件总大小
        uint32_t size;
        lv_fs_seek(&f, 0, LV_FS_SEEK_END);
        lv_fs_tell(&f, &size);
        lv_fs_seek(&f, 0, LV_FS_SEEK_SET);

        // 2. 申请对应的内存
        Font_mem = (uint8_t *)lv_mem_alloc(size);

        if(Font_mem) {
            uint32_t br;
            lv_fs_read(&f, Font_mem, size, &br); // 一次性读入内存
            printf("字库点阵加载成功，大小: 0x%x bytes\r\n", size);
        }
        lv_fs_close(&f);
            printf("地址挂载成功：%p\n", Font_mem, g_font_mem_ptr_bitmap);
            memcpy(&Font_Data, Font_mem, sizeof(Font_Version_t));  
   
      printf("版本:%d\r\n索引大小:0x%x\r\n点阵大小:0x%x\r\n总大小:0x%x\r\n",Font_Data.version,Font_Data.Index_Size,Font_Data.Bitmap_Size,Font_Data.total_Size) ;  
//                 uint8_t *p = (uint8_t *)Font_mem;
//          for(uint32_t i = 0; i < 10000; i++)
//              {
//                  printf("%02X ", p[i]);
//                  if((i + 1) % 16 == 0) printf("\r\n"); // 每16个字节换一行
//              } 
    } else {
        
        printf("加载字库点阵失败！\n");
        lv_fs_close(&f);
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
    uint8_t * index_ptr = NULL; // 定义一个全局指针存放字库数据
    #if keil
    index_ptr=(uint8_t *)EXT_SRAM_BASE;   
    #else
    index_ptr=Font_mem;
    #endif
    if(font==&my_font_12)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(index_ptr+FONT_12_INDEX_OFFSET);   
    }
    else if(font==&my_font_16)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(index_ptr+FONT_16_INDEX_OFFSET);   
    }
   else if(font==&my_font_24)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(index_ptr+FONT_24_INDEX_OFFSET);   
    }
    else if(font==&my_font_32)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(index_ptr+FONT_32_INDEX_OFFSET);   
    }
    
    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &my_index_array[gid];
    uint32_t total_bits = (uint32_t)gdsc->box_w * gdsc->box_h * fdsc->bpp;
    uint32_t read_len = (total_bits + 7) >> 3; // 理论字节
    if (read_len > FONT_BUFFER_SIZE) read_len = FONT_BUFFER_SIZE;
    
#if keil

    if(fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN) {
    
//    print("box_w:%d\r\ngbox_h:%d\r\nadv_w:%d\r\nbitmap_index:%d\r\n",gdsc->box_w,gdsc->box_h,gdsc->adv_w,gdsc->bitmap_index);

//     print("read_len:%d\r\n",read_len);
//     print("bitmap_index:%d\r\n",gdsc->bitmap_index);   
    uint32_t flash_addr = gdsc->bitmap_index + (uint32_t)font->user_data;

      W25Qxx_ReadData(flash_addr,Font_Bitmap_1,sizeof(Font_Bitmap_1));
//      W25Qxx_ReadData(flash_addr,Font_Bitmap_1,read_len);


      return Font_Bitmap_1;      
    }

#else
//     printf("bitmap_index:%d\r\n",gdsc->bitmap_index); 
     memcpy(Font_Bitmap_1,&Font_mem[gdsc->bitmap_index+(uint32_t)font->user_data],read_len); 
    if(fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN) {
//        return &Font_mem[gdsc->bitmap_index+(uint32_t)font->user_data];
        return Font_Bitmap_1;     
    }
#endif
       return NULL;
     
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
    uint8_t * index_ptr = NULL; // 定义一个全局指针存放字库数据
    #if keil
    index_ptr=(uint8_t *)EXT_SRAM_BASE;   
    #else
    index_ptr=Font_mem;
    #endif
    if(font==&my_font_12)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(index_ptr+FONT_12_INDEX_OFFSET);   
    }
    else if(font==&my_font_16)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(index_ptr+FONT_16_INDEX_OFFSET);   
    }
   else if(font==&my_font_24)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(index_ptr+FONT_24_INDEX_OFFSET);   
    }
    else if(font==&my_font_32)
    {
          my_index_array = (lv_font_fmt_txt_glyph_dsc_t *)(index_ptr+FONT_32_INDEX_OFFSET);   
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

#if keil
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
  
  uint32_t file_size = 0;
  Font_Version_t Font_Data[3];

  
  fr=f_open(&f,file_path,FA_READ);

  if(fr!=FR_OK)
  {
    print("打开文件失败\r\n");
    return 0;
  }
  else
  {
    file_size = f_size(&f);
    fr=f_read(&f,&Font_Data[SD_data],sizeof(Font_Version_t),&br);
    if(fr!=FR_OK)
    {
      print("读取文件失败\r\n");
      f_close(&f);
      return 0;  
    }
  }
  f_close(&f);
  
  W25Qxx_ReadData(Version_addr,(uint8_t*)&Font_Data[W25Qxx_head_data],sizeof(Font_Version_t));
  W25Qxx_ReadData(Version_addr+file_size-sizeof(Font_Version_t),(uint8_t*)&Font_Data[W25Qxx_end_data],sizeof(Font_Version_t));
    
  if(Font_Data[SD_data].version==Font_Data[W25Qxx_head_data].version && Font_Data[SD_data].version==Font_Data[W25Qxx_end_data].version)
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
        if(Read_Font_Version(Font_path,FONT_VERSION_ADDR_HEAD)==0)
        {
          Update_Font_to_W25Qxx(Font_path,FONT_VERSION_ADDR_HEAD); 
          if(Font_index_to_SDRAM(FONT_VERSION_ADDR_HEAD,EXT_SRAM_BASE))
          {
           print("字库索引已搬运到SDRAM:0x%x\r\n",(uint32_t)EXT_SRAM_BASE); 
//           uint8_t *p = (uint8_t *)EXT_SRAM_BASE;
//          for(uint32_t i = 0; i < 100; i++)
//              {
//                  printf("%02X ", p[i]);
//                  if((i + 1) % 16 == 0) printf("\r\n"); // 每16个字节换一行
//              }
          }
            
        }
        else
        {
          /*字库正常*/
          if(Font_index_to_SDRAM(FONT_VERSION_ADDR_HEAD,EXT_SRAM_BASE))
          {
           print("字库索引已搬运到SDRAM:0x%x\r\n",(uint32_t)EXT_SRAM_BASE);
//           uint8_t *p = (uint8_t *)EXT_SRAM_BASE;
//          for(uint32_t i = 0; i < 10000; i++)
//              {
//                  printf("%02X ", p[i]);
//                  if((i + 1) % 16 == 0) printf("\r\n"); // 每16个字节换一行
//              }           
          }
        }
     }
     f_mount(NULL,"0",1);
}

uint8_t Font_index_to_SDRAM(uint32_t Font_Index_Addr,uint32_t SDRAM_Addr)
{
    uint32_t total_size;
    uint32_t cur_size;
    uint32_t total_num=0;
//    W25Qxx_ReadData(FONT_VERSION_ADDR_HEAD,(uint8_t*)&Font_Data,sizeof(Font_Version_t));
//    total_size=Font_Data.Index_Size;
    total_size=0x0004BFA8;
    print("字库索引大小:%d\r\n",(uint32_t)total_size);
    while(total_size>0)
    {
      cur_size=total_size>65532 ? 65532 :total_size;
      if(W25Qxx_DMA_ReadData(Font_Index_Addr+total_num,SDRAM_Addr+total_num,cur_size))
      {
            total_size-=cur_size;
            total_num+=cur_size;
            print("当前搬运进度:%d\r\n",(uint32_t)total_num);
      }
      else
      {

        print("搬运失败\r\n");
        return 0;    
      }
    }
    return 1;  
}
#endif
