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




typedef enum
{
   HEAD_SD=0,
   HEAD_FLASH,
   HEAD_W25Q_1,
   HEAD_W25Q_2,
   HEAD_NUM
}head_enum;

typedef struct
{
    lv_obj_t*obj_update;
    lv_obj_t*label_name;
    lv_obj_t*progress_update_bar;
    lv_obj_t*new_version_label;
}update_obj_t;

typedef struct
{
lv_fs_file_t  file_p;
head_t head[HEAD_NUM];

update_obj_t update_obj;
}ui_setting_update_t;



ui_setting_update_t ui_setting_update;


void get_update_file_head(const char*path)
{
    lv_fs_res_t res;
    uint32_t num;
    res=lv_fs_open(&ui_setting_update.file_p,path,LV_FS_MODE_RD);
    if(res==LV_FS_RES_OK)
    {
         printf("读取文件\r\n");
        res=lv_fs_read(&ui_setting_update.file_p,&ui_setting_update.head[HEAD_SD],sizeof(head_t),&num);
        if(res==LV_FS_RES_OK)
        {
            printf("crc32:0x%08X\r\n",ui_setting_update.head[HEAD_SD].crc32);
            printf("file_size:%d\r\n",ui_setting_update.head[HEAD_SD].file_size);
            printf("name:%s\r\n",ui_setting_update.head[HEAD_SD].name);
            printf("update_state:%d\r\n",ui_setting_update.head[HEAD_SD].update_state);
            printf("version:%d\r\n",ui_setting_update.head[HEAD_SD].version);
            lv_fs_close(&ui_setting_update.file_p);
            ui_setting_update.file_p.drv=NULL;
        }
        else
        {
            printf("读取文件失败\r\n");
        }
    }
    else
    {
        printf("打开文件失败\r\n");
    }
    
#if keil

  W25Qxx_DMA_ReadData(Application_Addr_1,&ui_setting_update.head[HEAD_W25Q_1],sizeof(head_t));
  W25Qxx_DMA_ReadData(Application_Addr_2,&ui_setting_update.head[HEAD_W25Q_2],sizeof(head_t));
      
      if(ui_setting_update.head[HEAD_SD].version>ui_setting_update.head[HEAD_FLASH].version)
      {
        //说明有新版本了
        if(ui_setting_update.head[HEAD_W25Q_1].version==ui_setting_update.head[HEAD_SD].version)
        {
          //新版本已下载
          lv_label_set_text(ui_setting_update.update_obj.new_version_label,"立即重启更新");
        }
        else
        {
          //未下载,提示可以下载新版本
            lv_label_set_text(ui_setting_update.update_obj.new_version_label,"点击下载新版本");
          
        }
      }
      else if(ui_setting_update.head[HEAD_SD].version==ui_setting_update.head[HEAD_FLASH].version)
      {
        //版本一样
         lv_label_set_text(ui_setting_update.update_obj.new_version_label,"当前已是最新版本");
      }
      else
      {
        //旧版本
        lv_label_set_text(ui_setting_update.update_obj.new_version_label,"旧版本");
      }
      

    
#endif
    
}



static void event_check_update_cb(lv_event_t*e)
{
        get_update_file_head(Update_bin_Path);
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
      myFLASH_ReadData(FLASH_APP_Addr,&ui_setting_update.head[HEAD_FLASH],sizeof(head_t));
      lv_label_set_text(ui_setting_update.update_obj.label_name,ui_setting_update.head[HEAD_FLASH].name);
    #endif
    
    lv_obj_add_event_cb(update_obj->obj_update,event_check_update_cb,LV_EVENT_CLICKED,NULL);  
    update_obj->progress_update_bar=lv_bar_create(update_obj->obj_update);
    lv_obj_set_size(update_obj->progress_update_bar,lv_pct(100),lv_pct(30));
    lv_obj_align(update_obj->progress_update_bar,LV_ALIGN_BOTTOM_MID,0,0);
    update_obj->new_version_label=lv_label_create(update_obj->progress_update_bar);
    lv_obj_center(update_obj->new_version_label);
    lv_label_set_text(update_obj->new_version_label,"点击下载新版本");
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
    get_update_file_head(Update_bin_Path);
    for(uint8_t i=APP_SET_ABOUT_MCU;i<APP_SET_ABOUT_NUM;i++)/*循环添加对应的列表项*/
    {
       lv_list_add_btn (list_about,_GET_UI_ICON(APP_SET_ABOUT_LA_TABLE,i),_GET_UI_TEXT(APP_SET_ABOUT_LA_TABLE,i));
    }
}
