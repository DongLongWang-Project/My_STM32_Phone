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
#include "flash.h"
#include "W25Qxx.h"
#define Update_bin_Path  "0:/SD/bin/myPhone.bin"

#else
#define Update_bin_Path "0:/GitHub_Code/My_STM32_Phone/SD/bin/myPhone.bin"
#endif

ui_setting_update_t ui_setting_update;
update_flag_info_t update_flag_info;

void SD_get_update_file_head(const char*update_file_path)
{
   uint32_t num;
   
          lv_fs_res_t res=lv_fs_open(&ui_setting_update.file_p,update_file_path,LV_FS_MODE_RD);
      if(res!=LV_FS_RES_OK)
      {
        printf("打开app文件失败\r\n");
      }
      else
      {        
      res=lv_fs_read(&ui_setting_update.file_p,&ui_setting_update.head[HEAD_SD],sizeof(head_t),&num);
         if(res==LV_FS_RES_OK && num==sizeof(head_t))
         {
           printf("读取app头部成功\r\n");
//           printf("head[HEAD_SD].CRC32:0X%08X\r\n",head[HEAD_SD].CRC32);
//           printf("head[HEAD_SD].version:%u\r\n",head[HEAD_SD].version);
         }
         else
         {
            printf("读取app头部失败\r\n");
         }
         
      }
//      f_unmount("0");


}
uint8_t get_update_file_head(head_enum head_)
{

        switch(head_)
        {
          case HEAD_SD:         SD_get_update_file_head(UPDATE_FILE_PATH);                                 break;
          case HEAD_FLASH:      myFLASH_ReadData(APP_HEAD_Addr,&ui_setting_update.head[HEAD_FLASH],sizeof(head_t));         break;
          case HEAD_W25Q_Cur:   W25Qxx_DMA_ReadData(Application_Addr_1,&ui_setting_update.head[HEAD_W25Q_Cur],sizeof(head_t)); break;
          case HEAD_W25Q_Pre:   W25Qxx_DMA_ReadData(Application_Addr_2,&ui_setting_update.head[HEAD_W25Q_Pre],sizeof(head_t)); break;
          default:break;
        }
       uint32_t buf_size = ui_setting_update.head[head_].file_size;
       printf("version:%u\r\n",ui_setting_update.head[head_].version);
       if (buf_size == 0 || buf_size == 0xFFFFFFFF) return 0; 
       if(update_is_valid(head_)) return 1;
       
       return 0;   
}

uint8_t update_is_valid(head_enum head_)
{
    uint32_t buf_size = sizeof(crc_buf);
    uint32_t offset = 0;
    uint32_t remain = ui_setting_update.head[head_].file_size;
    uint32_t read_len;
    uint32_t current_crc = 0XFFFFFFFF;
    uint32_t num;
    // 基本合法性检查
    if (remain == 0 || remain == 0xFFFFFFFF) return 0;
    if(head_==HEAD_SD )  
    lv_fs_seek(&ui_setting_update.file_p,512,LV_FS_SEEK_SET);
    while (remain > 0)
    {
        // 计算本次读取长度：取 buf_size 和 剩余长度 的最小值
        read_len = (remain > buf_size) ? buf_size : remain;

        // 从 Flash 读取当前分块
        switch(head_)
        {
          case HEAD_SD:       lv_fs_read(&ui_setting_update.file_p,crc_buf,read_len,&num);break;
          case HEAD_FLASH:    myFLASH_ReadData(APP_Addr + offset, crc_buf, read_len);num=read_len;break;
          case HEAD_W25Q_Cur: W25Qxx_DMA_ReadData(Application_Addr_1+ offset+sizeof(head_t),crc_buf,read_len);num=read_len;break;
          case HEAD_W25Q_Pre: W25Qxx_DMA_ReadData(Application_Addr_1+ offset+sizeof(head_t),crc_buf,read_len);num=read_len;break;
          default:break;
        }
        current_crc=Continue_CRC32(current_crc,crc_buf,num);
        printf("%02X %02X %02X %02X num:%d current_crc:0X%08X\r\n",crc_buf[num-4],crc_buf[num-3],crc_buf[num-2],crc_buf[num-1],num,current_crc);
 
        offset += num;
        remain -= num;
    }
    // 最终校验
    if (current_crc == ui_setting_update.head[head_].crc32)
    {
        printf("Flash CRC OK: 0x%08X\r\n", current_crc);
        return 1;
    }
    
    printf("Flash CRC Error! Calc: 0x%08X, Target: 0x%08X\r\n", current_crc, ui_setting_update.head[head_].crc32);
    return 0;
}



static void event_check_update_cb(lv_event_t*e)
{
        get_update_file_head(HEAD_SD);
        if(ui_setting_update.head[HEAD_SD].version>ui_setting_update.head[HEAD_FLASH].version)
        {
          lv_label_set_text(ui_setting_update.update_obj.new_version_label,"发现新版本,点击更新");
        }

}

void setting_update_create(lv_obj_t*parent,update_obj_t *update_obj)
{
    update_obj->obj_update=lv_obj_create(parent);
    lv_obj_set_size(update_obj->obj_update,lv_pct(100),lv_pct(40));
    update_obj->label_name=lv_label_create( update_obj->obj_update);
    lv_obj_align(update_obj->label_name,LV_ALIGN_CENTER,0,-10);
    lv_obj_set_style_text_font(update_obj->label_name,&my_font_16,0);
    lv_label_set_text(update_obj->label_name,"MyPhoneOS V1.0");
    #if keil
      get_update_file_head(HEAD_FLASH);
      lv_label_set_text(ui_setting_update.update_obj.label_name,ui_setting_update.head[HEAD_FLASH].name);
    #endif
    

    update_obj->progress_update_bar=lv_bar_create(update_obj->obj_update);
    lv_obj_set_size(update_obj->progress_update_bar,lv_pct(100),lv_pct(30));
    lv_obj_align(update_obj->progress_update_bar,LV_ALIGN_BOTTOM_MID,0,0);
    update_obj->new_version_label=lv_label_create(update_obj->progress_update_bar);
    lv_obj_add_event_cb(update_obj->new_version_label,event_check_update_cb,LV_EVENT_CLICKED,NULL);  
    lv_obj_add_flag(update_obj->new_version_label,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(update_obj->new_version_label);
    lv_label_set_text(update_obj->new_version_label,"点击检查新版本");
//    lv_obj_add_flag(update_obj->progress_update_bar,LV_OBJ_FLAG_HIDDEN);
    
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
