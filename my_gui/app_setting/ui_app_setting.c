#include "ui_app_setting.h"



 static void event_app_setting_cb(lv_event_t*e);
UI_APP_SETTING_ENUM ui_app_get_setting_list_btnname(const char *btn_name);


/*************************************************************设置(总)*************************************************************/

/**
 * @brief   app列表函数
 * @param 父对象,按钮图标,列表文本,按键文本
 * @return app列表页面
 *
 *  注意:点击app后,触发上一级面回调函数进入app列表界面
 */
lv_obj_t* ui_app_setting_create_list(lv_obj_t *parent)
{
    lv_obj_t *list = lv_list_create(parent);
        
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF); // 关掉那根条
    lv_obj_set_scroll_dir(list, LV_DIR_VER);               // 锁定只许上下动

    // 去除外观干扰
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_radius(list, 0, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    
//        lv_list_add_text(list, APP_SET_LA_TABLE[0].text[display_cfg.language]);/**文本 **/

        lv_list_add_text(list, _GET_UI_TEXT(APP_SET_LA_TABLE,0));/**文本 **/
//          lv_obj_set_style_text_font(list,ui_fonts.FONT_SIZE_14,LV_STATE_DEFAULT); 
//        ui_set_obj_text_font(list,FONT_SIZE_14);
        for(uint8_t i=1;i<APP_SETTING_NUM;i++)
        {
            lv_obj_t *btn_setting=lv_list_add_btn(list,_GET_UI_ICON(APP_SET_LA_TABLE,i) ,_GET_UI_TEXT(APP_SET_LA_TABLE,i) );  
            lv_obj_add_event_cb(btn_setting, event_app_setting_cb, LV_EVENT_CLICKED, NULL);
        }
          return list;
    }
 
 static void event_app_setting_cb(lv_event_t*e)
{
	lv_obj_t*target=lv_event_get_target(e);
	lv_obj_t *list= lv_obj_get_parent(target);
	const char *btn_txt=lv_list_get_btn_text( list,target);
  
    Cure_Mode=ui_app_get_setting_list_btnname(btn_txt);
    ui_goto_page(PAGE_APP_DETAIL,APP_SETTING);
}
 
UI_APP_SETTING_ENUM ui_app_get_setting_list_btnname(const char *btn_name)
{

	 if(strcmp(btn_name,APP_SET_LA_TABLE[APP_SETTING_ABOUT].text[display_cfg.language])==0) return APP_SETTING_ABOUT;
	else if(strcmp(btn_name,APP_SET_LA_TABLE[APP_SETTING_DISPLAY].text[display_cfg.language])==0) return APP_SETTING_DISPLAY;
    else if(strcmp(btn_name,APP_SET_LA_TABLE[APP_SETTING_WLAN].text[display_cfg.language])==0) return APP_SETTING_WLAN;
    else if(strcmp(btn_name,APP_SET_LA_TABLE[APP_SETTING_HOTSPOT].text[display_cfg.language])==0) return APP_SETTING_HOTSPOT;
    
	return APP_SETTING_NUM;
}

lv_obj_t* ui_app_setting_create_detail(lv_obj_t *parent)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_set_size (page, lv_pct(100), lv_pct(100));
    lv_obj_set_style_border_width(page, 0, 0);
    lv_obj_set_style_radius(page, 0, 0);
    lv_obj_set_style_pad_all(page, 0, 0);
    
	switch(Cure_Mode)
		{
			case APP_SETTING_ABOUT:
                    {
                        ui_app_setting_about(page);
                        break;
                    }
            case APP_SETTING_DISPLAY:
                {
                        ui_app_setting_display(page);
                        break;
                 }
            case APP_SETTING_WLAN:
                {
                        ui_app_setting_wifi(page);
                        break;
                 }
            case APP_SETTING_HOTSPOT:
                {
                ui_app_setting_hotspot(page);
                        break;
                }
             
			default:break;

		}
    return page;
}


