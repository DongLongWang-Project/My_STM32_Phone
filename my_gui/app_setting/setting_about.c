/*--------------------------------------------------------------------------------↓
	@项目	: app设置的选项(关于本机)
	@日期	: 2026-1-24
	@备注	: 关于本机: 
                    设备:STM32F407ZGT6
                    RAM:(内部)128K+(CCM)64K+(外部1M)
                    ROM:1M
                    SD:32GB
                    屏幕:240x320 电阻触摸屏
                    系统:FreeRTOS V11.1.0
                    GUI:LVGLV8.3
                    
↑--------------------------------------------------------------------------------*/

#include "setting_about.h"

LV_FONT_DECLARE( my_font_12);
LV_FONT_DECLARE( my_font_16);
LV_FONT_DECLARE( my_font_24);
LV_FONT_DECLARE( my_font_32);
#if keil
extern char DEAL_BUF[DEAL_BUF_SIZE];/*总数据处理缓冲区*/

#else
uint8_t DEAL_BUF[0x10000]__attribute__((section(".EXT_SRAM")));
uint32_t Continue_CRC32(uint32_t last_crc, uint8_t* data, uint32_t len) {
    uint32_t crc = last_crc; // 接力上次的结果
    uint32_t calc_len = len;   
    for (uint32_t i = 0; i < calc_len; i++) {
        crc ^= ((uint32_t)data[i] << 24);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80000000) crc = (crc << 1) ^ 0x04C11DB7;
            else crc = (crc << 1);
        }
    }
    return crc;
}

#endif

ui_setting_update_t ui_setting_update;
update_flag_info_t update_flag_info;

#if keil
/**
  * @brief  获取 W25Q 扇区内最后一条有效记录的索引
  * @param  Address: 4KB 扇区的起始地址
  * @retval 0~255: 有效索引; -1: 扇区为空; -2: 扇区异常
  */
int16_t Get_Latest_update_info_Index(uint32_t Address)
{
    uint32_t current_magic;
    uint8_t info_size = sizeof(update_flag_info_t);
    uint16_t max_slots = 4096 / info_size; // 16字节时为256

    for(int16_t i = 0; i < max_slots; i++)
    {

        W25Qxx_ReadData(Address + (i * info_size), (uint8_t*)&current_magic, 4);
      
        if(current_magic == 0xFFFFFFFF) 
        { 
            // 情况 A: 如果第一个位置就是空的，说明整个扇区没有记录
            if(i == 0) return -1; 
            
            // 情况 B: 找到了第一个空位，返回前一个有效索引
            return i - 1; 
        }
    }

    // 情况 C: 循环跑完都没 0xFF，说明 256 个格子全写满了，返回 255
    return (int16_t)(max_slots - 1); 
}

void update_flag_to_W25Qxx(update_flag_info_t *update_flag_info)
{
    uint8_t info_size=sizeof(update_flag_info_t);
    int16_t LastIndex=Get_Latest_update_info_Index(UPDATE_INFO_Addr);
    int16_t Target_index=0;
    if(LastIndex==-1)
    {
      /*直接在第一个位置写就行*/
      Target_index=0;
      printf("直接在第一个位置写就行\r\n");
    }
    else if(LastIndex==(4096/info_size)-1)
    {
      /*说明写满了,擦除扇区从头写*/
      W25Qxx_SectorErase(UPDATE_INFO_Addr,W25Qxx_SECTOR_ERASE_4KB);
      Target_index=0;
      printf("说明写满了,擦除扇区从头写\r\n");
    }
    else
    {
      Target_index=LastIndex+1;
      printf("还有空间可以直接写\r\n");
    }

    W25Qxx_WriteBuffer(UPDATE_INFO_Addr+Target_index*info_size,(const uint8_t*)update_flag_info,info_size); 
}

uint8_t Read_Latest_update_info_(update_flag_info_t *update_flag_info)
{
    uint8_t info_size = sizeof(update_flag_info_t);
    int16_t LastIndex = Get_Latest_update_info_Index(UPDATE_INFO_Addr);
    
    if(LastIndex == -1) return 0; // 整个扇区都是空的，无事可做

    // 读取最新的一条记录
    W25Qxx_ReadData(UPDATE_INFO_Addr + (LastIndex * info_size), (uint8_t*)update_flag_info, info_size);
    
    // --- 核心修正逻辑 ---
    // 只有 0x5A5A1234 代表“APP 下令：我要更新，请 Bootloader 动手”
    if (update_flag_info->update_flag == 0x5A5A1234) 
    {
        printf("检测到有效的更新请求！\r\n");
        printf("版本:%u, 大小:%u, CRC:0x%08X\r\n", 
                update_flag_info->file_version, 
                update_flag_info->file_size, 
                update_flag_info->file_crc);
        return 1; // 返回 1，触发 Moveing_file_to_flash
    }
    
    // 如果是 0x00000000，说明上次更新刚写完，或者是系统正常运行状态
    if (update_flag_info->update_flag == 0x00000000)
    {
        printf("当前记录已处理完成 (Idle/Done)\r\n");
        return 0; // 返回 0，Bootloader 不搬运，直接跳转 APP
    }
    
    printf("未知状态或数据损坏 (Flag:0x%08X)\r\n", update_flag_info->update_flag);
    return 0; 
}
 #endif // keil
void SD_get_update_file_head(const char*update_file_path)
{
      uint32_t num;
      lv_fs_res_t res=lv_fs_open(&ui_setting_update.file_p,update_file_path,LV_FS_MODE_RD|LV_FS_MODE_WR);
      if(res!=LV_FS_RES_OK)
      {
        printf("打开app文件失败\r\n");
      }
      else
      {        
          res=lv_fs_read(&ui_setting_update.file_p,&ui_setting_update.head[HEAD_SD],sizeof(head_t),&num);
         if(res==LV_FS_RES_OK && num==sizeof(head_t))
         {
           printf("读取SD卡app头部成功\r\n");
    //         printf("head[HEAD_SD].CRC32:0X%08X\r\n",head[HEAD_SD].CRC32);
    //         printf("head[HEAD_SD].version:%u\r\n",head[HEAD_SD].version);
//            lv_fs_close(&ui_setting_update.file_p);
//            ui_setting_update.file_p.drv=NULL;
         }
         else
         {
            printf("读取SD卡app头部失败\r\n");
         } 
      }
}

uint8_t get_update_file_head(head_enum head_)
{
         uint16_t size= sizeof(head_t);
        switch(head_)
        {
          case HEAD_SD:         SD_get_update_file_head(UPDATE_FILE_PATH); break;
          #if keil
          case HEAD_FLASH:      myFLASH_ReadData(APP_HEAD_Addr,&ui_setting_update.head[HEAD_FLASH],size);  break;
          case HEAD_GitHUB:     Get_GitHub_MyPhone_Update_file(Get_GitHub_MyPhone_file_head,get_update_head_str);break;
          case HEAD_W25Q_Pre:   W25Qxx_DMA_ReadData(Application_Addr_2,&ui_setting_update.head[HEAD_W25Q_Pre],size); break;
          #endif // keil
          default:break;
        }
       uint32_t buf_size = ui_setting_update.head[head_].file_size;
       print("head_:%d,buf_size:%u version:%u\r\n",head_,buf_size,ui_setting_update.head[head_].version);
       if (buf_size == 0 ||  buf_size >=0x00EFF00) return 0;  
//       else return 1;
        if(head_!=HEAD_GitHUB)
        {
         if(update_is_valid(head_)) return 1; 
        }
        else
        {
          return 1; 
        }
       
       
       return 0;   
}

uint8_t update_is_valid(head_enum head_)
{
    uint32_t buf_size = sizeof(DEAL_BUF);
    uint32_t offset = 0;
    uint32_t remain = ui_setting_update.head[head_].file_size;
    uint32_t read_len;
    uint32_t current_crc = 0XFFFFFFFF;
    uint32_t num;
    // 基本合法性检查
    if (remain == 0 ||  remain >=0x00EFF00) return 0; 
    if(head_==HEAD_SD ) 
    {
//      lv_fs_res_t res=lv_fs_open(&ui_setting_update.file_p,UPDATE_FILE_PATH,LV_FS_MODE_RD|LV_FS_MODE_WR);
//      if(res!=LV_FS_RES_OK)
//      {
//        printf("打开app文件失败\r\n");
//      }
//      else
//      {
        lv_fs_seek(&ui_setting_update.file_p,sizeof(head_t),LV_FS_SEEK_SET);
//      }
    }    
    
    while (remain > 0)
    {
        // 计算本次读取长度：取 buf_size 和 剩余长度 的最小值
        read_len = (remain > buf_size) ? buf_size : remain;

        // 从 Flash 读取当前分块
        switch(head_)
        {
          case HEAD_SD:       lv_fs_read(&ui_setting_update.file_p,DEAL_BUF,read_len,&num);break;
           #if keil
          case HEAD_FLASH:    myFLASH_ReadData(APP_Addr + offset, DEAL_BUF, read_len);num=read_len;break;
          case HEAD_W25Q_Pre: W25Qxx_DMA_ReadData(Application_Addr_2+ offset+sizeof(head_t),DEAL_BUF,read_len);num=read_len;break;
          #endif // keil
          
          default:break;
        }
        current_crc=Continue_CRC32(current_crc,(uint8_t *)DEAL_BUF,num);
//        printf("%02X %02X %02X %02X num:%d current_crc:0X%08X\r\n",DEAL_BUF[num-4],DEAL_BUF[num-3],DEAL_BUF[num-2],DEAL_BUF[num-1],num,current_crc);
        offset += num;
        remain -= num;
        if(num==0)  
        {
          print("出现读取错误,读取数据为0,将强制退出while\r\n");
          break;
        }
 
    }
    
    if(head_==HEAD_SD)
    {
        lv_fs_close(&ui_setting_update.file_p);
        ui_setting_update.file_p.drv=NULL;
    }
    // 最终校验
    if (current_crc == ui_setting_update.head[head_].crc32)
    {
        printf("Flash CRC OK: 0x%08X\r\n", current_crc);
        return 1;
    }
    printf("Flash CRC Error! Calc: 0x%08X, Target: 0x%08X\r\n", current_crc, ui_setting_update.head[head_].crc32);
    // 在 APP 校验失败时，执行这个：
    return 0;
}



update_is_ready_t update_is_ready=has_no_new;
static void event_check_update_cb(lv_event_t*e)
{
      lv_obj_t*target=lv_event_get_target(e);
        if(update_is_ready==has_no_new)
        {
            if(get_update_file_head(HEAD_SD))
            {
              if(ui_setting_update.head[HEAD_SD].version>ui_setting_update.head[HEAD_FLASH].version)
              {
                lv_label_set_text(ui_setting_update.update_obj.new_version_label,"本地发现新版本,点击更新");
                 update_is_ready=has_sd_new;
              }
              else if(ui_setting_update.head[HEAD_SD].version==ui_setting_update.head[HEAD_FLASH].version )
              {
                lv_label_set_text(ui_setting_update.update_obj.new_version_label,"已是最新版本");
                Get_GitHub_MyPhone_Update_file(Get_GitHub_MyPhone_file_head,get_update_head_str); 
              }
            }
            else
            {
              lv_label_set_text(ui_setting_update.update_obj.new_version_label,"文件错误,请重新下载");
              Get_GitHub_MyPhone_Update_file(Get_GitHub_MyPhone_file_head,get_update_head_str);
            }
        }
        else if(update_is_ready==has_git_new)
        {
            Get_GitHub_MyPhone_Update_file(Get_GitHub_MyPhone_file,get_update_file_str);
            update_is_ready=has_download;
//            lv_label_set_text(ui_setting_update.update_obj.new_version_label,"Github上找到新版本,正在下载");
            
        }
        else if(update_is_ready==has_download)
        {
           print("正在下载,请稍等\r\n");
        }

        else
        {
            printf("设置更新信息,准备复位更新\r\n");
            #if keil
            update_flag_info.file_crc=ui_setting_update.head[HEAD_SD].crc32;
            update_flag_info.file_size=ui_setting_update.head[HEAD_SD].file_size;
            update_flag_info.file_version=ui_setting_update.head[HEAD_SD].version;
            update_flag_info.update_flag=0x5A5A1234;
            update_flag_to_W25Qxx(&update_flag_info);
            
            printf("文件CRC:0X%08X \r\n",update_flag_info.file_crc);
            printf("文件大小:%u字节\r\n",update_flag_info.file_size);
            printf("文件版本:%u\r\n",update_flag_info.file_version);            
            NVIC_SystemReset(); //复位
            #endif
        }
}

extern ipd_ctx_t my_ipd_ctx; 
void download_update_timer(lv_timer_t*t)
{
  static uint32_t pre_len=0;
  uint32_t cur_len=my_ipd_ctx.total_saved;
  uint32_t file_size=ui_setting_update.head[HEAD_GitHUB].file_size+sizeof(head_t);
  
  if(pre_len!=cur_len)
  {
      uint32_t percent=(uint32_t)(((uint64_t)my_ipd_ctx.total_saved*100)/file_size);
    if(ui_setting_update.update_obj.obj_update!=NULL)
      {
       lv_bar_set_value(ui_setting_update.update_obj.progress_update_bar,percent,LV_ANIM_OFF);
       lv_label_set_text_fmt(ui_setting_update.update_obj.new_version_label,"正在下载:%d%%",percent);
       print("当前的长度:%u 进度:%d%%",my_ipd_ctx.total_saved,percent);
      }

  }
   pre_len=cur_len;
   
  if(my_ipd_ctx.total_saved>=ui_setting_update.head[HEAD_GitHUB].file_size+sizeof(head_t))
  {
    printf("下载完毕,删除定时器\r\n");
    lv_label_set_text(ui_setting_update.update_obj.new_version_label,"下载完毕,点击检查");
    update_is_ready=has_no_new;
    lv_timer_del(t);
  }
}

static void event_obj_update_cb(lv_event_t*e)
{
   ui_setting_update.update_obj.obj_update=NULL;
   ui_setting_update.update_obj.progress_update_bar=NULL;
   
    if(ui_setting_update.update_obj.timer!=NULL)
    {
      lv_timer_del(ui_setting_update.update_obj.timer);
      ui_setting_update.update_obj.timer=NULL;
    }
}
void setting_update_create(lv_obj_t*parent,update_obj_t *update_obj)
{
    update_obj->obj_update=lv_obj_create(parent);
    lv_obj_set_size(update_obj->obj_update,lv_pct(100),lv_pct(40));
    lv_obj_set_style_radius(update_obj->obj_update,0,0);
    
    update_obj->label_name=lv_label_create( update_obj->obj_update);
    lv_obj_align(update_obj->label_name,LV_ALIGN_CENTER,0,-10);
    lv_obj_set_style_text_font(update_obj->label_name,&my_font_16,0);
    
    lv_label_set_text(update_obj->label_name,"MyPhoneOS");
    #if keil
      printf("name:%s\r\n",ui_setting_update.head[HEAD_FLASH].name);
      lv_label_set_text(ui_setting_update.update_obj.label_name,ui_setting_update.head[HEAD_FLASH].name);
    #endif
    

    update_obj->progress_update_bar=lv_bar_create(update_obj->obj_update);
    lv_obj_set_size(update_obj->progress_update_bar,lv_pct(100),lv_pct(30));
    lv_obj_align(update_obj->progress_update_bar,LV_ALIGN_BOTTOM_MID,0,0);
    lv_bar_set_range(update_obj->progress_update_bar,0,100);
    
    
    update_obj->new_version_label=lv_label_create(update_obj->progress_update_bar);
    lv_obj_add_event_cb(update_obj->new_version_label,event_check_update_cb,LV_EVENT_CLICKED,NULL);  
    lv_obj_add_flag(update_obj->new_version_label,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(update_obj->new_version_label);
    lv_obj_set_style_text_color(update_obj->new_version_label,lv_color_hex(0x00FF00),0);
    if(update_is_ready==has_no_new)
    {
     lv_label_set_text(update_obj->new_version_label,"点击检查新版本"); 
    }
    else if(update_is_ready==has_download)
    {
     lv_label_set_text(update_obj->new_version_label,"Github上找到新版本,正在下载"); 
    }
//    lv_obj_add_flag(update_obj->progress_update_bar,LV_OBJ_FLAG_HIDDEN);
   
   update_obj->timer=lv_timer_create(download_update_timer,1000,NULL);
   lv_obj_add_event_cb(update_obj->obj_update,event_obj_update_cb,LV_EVENT_DELETE,NULL); 
}
/*--------------------------------------------------------------------------------↓
	@函数	  :  创建关于本机的ui
	@参数	  :  父对象
	@返回值 :  无
	@备注	  :
↑--------------------------------------------------------------------------------*/
void ui_app_setting_about(lv_obj_t*parent)
{
    lv_obj_t*list_about=lv_list_create (parent);/*创建列表*/
    
    
    lv_obj_set_size(list_about, lv_pct(100), lv_pct(100));
    lv_obj_set_style_border_width(list_about, 0, 0);
    lv_obj_set_style_radius(list_about, 0, 0);
    lv_obj_set_style_pad_all(list_about, 0, 0);
    
    lv_obj_set_scrollbar_mode(list_about, LV_SCROLLBAR_MODE_OFF);/*禁止滚动*/ 
    lv_obj_set_scroll_dir(list_about, LV_DIR_VER); /*允许纵向滚动*/             
    
    
    lv_list_add_text (list_about, _GET_UI_TEXT(APP_SET_ABOUT_LA_TABLE,APP_SET_ABOUT_TITLE));/*添加标题*/
    
    
    setting_update_create(list_about,&ui_setting_update.update_obj);
    
    for(uint8_t i=APP_SET_ABOUT_MCU;i<APP_SET_ABOUT_NUM;i++)/*循环添加对应的列表项*/
    {
       lv_list_add_btn (list_about,_GET_UI_ICON(APP_SET_ABOUT_LA_TABLE,i),_GET_UI_TEXT(APP_SET_ABOUT_LA_TABLE,i));
    }
}
