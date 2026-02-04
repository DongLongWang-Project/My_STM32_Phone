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
}
