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


#if keil
#define Update_Dir_Path  "0:/SD/bin/myPhone.bin"
#else
#define Update_bin_Path "0:/GitHub_Code/My_STM32_Phone/SD/bin/myPhone.bin"
#endif

typedef enum
{
   HEAD_SD=0,
   HEAD_FLASH,
   HEAD_W25Q,
   HEAD_NUM
}head_enum;

typedef struct
{
lv_fs_file_t  file_p;
head_t head[HEAD_NUM];

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
    
}

static void event_check_update_cb(lv_event_t*e)
{
        get_update_file_head(Update_bin_Path);
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
    for(uint8_t i=1;i<APP_SET_ABOUT_NUM;i++)/*循环添加对应的列表项*/
    {
       lv_list_add_btn (list_about,_GET_UI_ICON(APP_SET_ABOUT_LA_TABLE,i),_GET_UI_TEXT(APP_SET_ABOUT_LA_TABLE,i));
    }
    lv_obj_t*btn_check_update=lv_obj_get_child(list_about,APP_SET_ABOUT_UPDATE);
    lv_obj_add_event_cb(btn_check_update,event_check_update_cb,LV_EVENT_CLICKED,NULL);
}
