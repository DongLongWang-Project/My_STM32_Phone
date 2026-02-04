#include "ui_app_list.h"


lv_obj_t *list,*btn;

static void list_btn_event_cb(lv_event_t *e);
lv_obj_t* page_app_list_fat_create(lv_obj_t *parent,const char * path);


static UI_APP_SETTING_LIST_ENUM ui_get_list_setting_btnname(char *btn_name)
{
	if(strcmp(btn_name,"WLAN")==0) return WLAN;
	if(strcmp(btn_name,"蓝牙")==0) return BlueTooch;
	return SETTING_NULL;
}
/**
 * @brief   app列表函数
 * @param 父对象,按钮图标,列表文本,按键文本
 * @return app列表页面
 *
 *  注意:点击app后,触发上一级面回调函数进入app列表界面
 */
lv_obj_t* page_app_Setting_list_create(lv_obj_t *parent)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_set_size(page, scr_width,scr_height);/**设置尺寸 **/
    lv_obj_t *list = lv_list_create(page);
    lv_list_add_text(list, "Setting");/**文本 **/
	ui_app_setting_list_t List_setting;
    memset(&List_setting, 0, sizeof(ui_app_setting_list_t));
	lv_obj_set_style_text_font (list, my_fonts.FONT_SIZE_16,LV_STATE_DEFAULT);
    /**添加文本按钮 **/
	List_setting.btn_WIFI= lv_list_add_btn(list, LV_SYMBOL_WIFI, "WLAN");
	List_setting.btn_BlueTooch = lv_list_add_btn(list, LV_SYMBOL_BLUETOOTH, "蓝牙");
    /**事件 **/
    lv_obj_add_event_cb(List_setting.btn_WIFI, list_btn_event_cb, LV_EVENT_CLICKED, (void*)btn_list_setting);
    lv_obj_add_event_cb(List_setting.btn_BlueTooch, list_btn_event_cb, LV_EVENT_CLICKED, (void*)btn_list_setting);

    return page;
}
static void list_btn_event_cb(lv_event_t *e)
{
	lv_obj_t*target=lv_event_get_target(e);
	char *btn_txt=lv_list_get_btn_text( list,target);
	UI_APP_BTN_ENUM type=(UI_APP_BTN_ENUM)lv_event_get_user_data(e);

	if(type==btn_list_File)
	{
		if(btn_txt[0]=='/')
			{
				snprintf(Cure_Path, sizeof(Cure_Path), "%s%s", Cure_Path, btn_txt);
				ui_goto_page(PAGE_APP_LIST,type,SETTING_NULL);
			}
		else
			{
				snprintf(Cure_Path, sizeof(Cure_Path), "%s/%s", Cure_Path, btn_txt);
				ui_goto_page(PAGE_APP_DETAIL,type,SETTING_NULL);
			}
			printf("Enter in ( %s )\r\n",Cure_Path);
	}
	else if(type==btn_list_setting)
		{
			ui_goto_page(PAGE_APP_DETAIL,type,ui_get_list_setting_btnname(btn_txt));
		}
}

lv_obj_t* page_app_list_fat_create(lv_obj_t *parent,const char * path)
{
    lv_obj_t *page = lv_obj_create(parent);
    lv_obj_set_size(page, scr_width,scr_height);/**设置尺寸 **/
    list = lv_list_create(page);
    lv_list_add_text(list, Cure_Path);/**文本 **/
	lv_obj_set_style_shadow_opa (list, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(list,0,LV_PART_MAIN);/**边框宽度0 **/

    lv_fs_res_t res;
    lv_fs_dir_t dir;

    char buf[256];
    uint8_t i=0;
    res=lv_fs_dir_open(&dir,path);
    if(res!=LV_FS_RES_OK)
    {
        return page;
    }
    while(1)
    {
        res=lv_fs_dir_read(&dir,buf);

        if(res!=LV_FS_RES_OK || buf[0]=='\0') break;
        if( buf[0]=='/')btn = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, buf);
        else btn = lv_list_add_btn(list, LV_SYMBOL_FILE, buf);
		lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, (void*)btn_list_File);
    }
    lv_fs_dir_close(&dir);

    return page;
}
