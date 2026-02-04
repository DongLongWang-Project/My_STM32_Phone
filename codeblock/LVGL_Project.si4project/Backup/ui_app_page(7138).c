#include "ui_app_page.h"

static void btnmatrix_event_cb(lv_event_t *e);
static void app_btn_event_cb(lv_event_t *e);

/**矩阵按钮样式 数组**/
static const char*btnm_map[]={LV_SYMBOL_HOME,LV_SYMBOL_LEFT,""};

/**
 * @brief   创建APP按钮
 * @param 父对象,对应图标,app文本
 * @return 创建的app按钮
 *
 *  注意:app是由3个基本部件组成的
        1按钮
        2文本
        3图标(LVGL内置图标
 */
lv_obj_t* ui_create_app_btn(lv_obj_t *parent,const void *icon, const char *text)
{
    /**创建按钮 **/
    lv_obj_t*btn=lv_btn_create(parent);
    lv_obj_set_style_bg_opa(btn,25,LV_STATE_DEFAULT);/**设置透明度(按下有反应 **/
    lv_obj_set_style_bg_color(btn,lv_color_hex(0xFEFEFE),LV_STATE_DEFAULT);/**设置背景色(默认状态 **/
    lv_obj_set_style_bg_color(btn,lv_color_hex(0x696969),LV_STATE_PRESSED);/**设置按下的背景色(搭配透明度 **/

    lv_obj_set_size(btn,60,60);/**设置尺寸 **/
    lv_obj_set_style_shadow_width(btn,0,LV_STATE_DEFAULT);/**去除按钮阴影 **/
    lv_obj_set_style_border_width(btn,0,LV_STATE_DEFAULT);/**设置按钮边框厚度为0 **/
    /**创建图标 **/
    lv_obj_t * img=lv_img_create(btn);
    lv_img_set_src(img,icon);
    lv_obj_set_style_text_font(img,&lv_font_montserrat_32,LV_STATE_DEFAULT);/**设置大小 **/
    lv_obj_set_style_text_color(img,lv_color_hex(0x000000),LV_STATE_DEFAULT);/**设置颜色 **/
    /**创建文本 **/
    lv_obj_t *label=lv_label_create(btn);
    lv_label_set_text(label,text);/**设置文本 **/
    lv_obj_set_style_text_font(label,&lv_font_montserrat_16,LV_STATE_DEFAULT);/**设置字体大小 **/
    lv_obj_set_style_text_color(label,lv_color_hex(0x000000),LV_STATE_DEFAULT);/**设置字体颜色 **/

    lv_obj_align(img,LV_ALIGN_TOP_MID,0,-10);/**设置图标位置 **/
    lv_obj_align_to(label,img,LV_ALIGN_BOTTOM_MID,0,15);/**设置文本相对于图标的位置 **/

    return btn;

}

/******************************************************
函数:创建导航栏
参数:父对象
返回:无
注意:导航栏有三个按钮
2:返回按钮
1:主界面按钮
******************************************************/
void My_Ui_Creat_Navigation_bar(lv_obj_t *parent)
{
    /**创建按钮 **/
    lv_obj_t*btnmatrix=lv_btnmatrix_create(parent);
    lv_btnmatrix_set_map(btnmatrix,btnm_map);/**将数组填入,生成按钮 **/
    lv_btnmatrix_set_btn_width(btnmatrix,1,1);/**按钮相对宽度 **/
    lv_obj_set_size(btnmatrix,scr_width,scr_height/7);/**矩阵按钮的宽高 宽:屏幕宽度 高:屏幕高度/6**/
    lv_obj_align(btnmatrix,LV_ALIGN_BOTTOM_MID,0,0);/**父对象中间的底部 **/
    /**优化外观 **/
    lv_obj_set_style_border_width(btnmatrix,0,LV_PART_MAIN);/**边框宽度0 **/
    lv_obj_set_style_bg_opa(btnmatrix,0,LV_PART_ITEMS);/**背景透明度0(按钮) **/
    lv_obj_set_style_bg_opa(btnmatrix,25,LV_PART_ITEMS|LV_STATE_PRESSED);/**按下后的透明度(按钮 **/
    lv_obj_set_style_shadow_width(btnmatrix,0,LV_PART_ITEMS);/**按钮边框宽度 **/

	lv_obj_set_style_bg_opa(btnmatrix,15,LV_PART_MAIN);/**背景透明度0(按钮) **/

	lv_obj_set_style_text_color (btnmatrix, lv_color_hex (0x000000), LV_PART_ITEMS);
	lv_obj_set_style_text_opa (btnmatrix, 200, LV_PART_ITEMS);

//    lv_btnmatrix_set_btn_ctrl_all(btnmatrix,LV_BTNMATRIX_CTRL_RECOLOR);/**可重色 **/
    /**事件 **/
    lv_obj_add_event_cb(btnmatrix,btnmatrix_event_cb,LV_EVENT_CLICKED,SETTING_NULL);/** **/
}
static void btnmatrix_event_cb(lv_event_t *e)
{
    uint16_t id = lv_btnmatrix_get_selected_btn(lv_event_get_target(e));
	char *end_dir=NULL;
    if(id == 1)/**如果返回按钮按下 **/
		{
		switch(Crue_App)
			{
				case Home:ui_goto_page(PAGE_HOME,Home,SETTING_NULL); Crue_App=Home;break;
				case btn_list_setting:
					{
						switch(Cur_Page)
							{
							case PAGE_APP_LIST:ui_goto_page(PAGE_HOME,Home,SETTING_NULL);Crue_App=Home;break;
							case PAGE_APP_DETAIL:ui_goto_page(PAGE_APP_LIST,btn_list_setting,SETTING_NULL);Crue_App=btn_list_setting;break;
						}

					}break;
				case btn_list_File:
					if(strcmp(Cure_Path,"F:")==0)
					{
						ui_goto_page(PAGE_HOME,Home,SETTING_NULL);
						Crue_App=Home;
					}
					else
						{
						end_dir=strrchr(Cure_Path,'/');
						if(end_dir)	*end_dir='\0';
						printf("Come out ( %s )\r\n",Cure_Path);
						Crue_App=btn_list_File;
						ui_goto_page(PAGE_APP_LIST,btn_list_File,SETTING_NULL);
						}

	        }
    	}
	else if(id ==0)
		{
		ui_goto_page(PAGE_HOME,Home,SETTING_NULL);
		}
}



/******************************************************
函数:创建界面
参数:父对象
返回:无
注意:可设置多个app
******************************************************/
lv_obj_t* page_child_create(lv_obj_t *parent)
{
    lv_obj_t *page=lv_obj_create(parent);
    lv_obj_set_size(page,scr_width,scr_height);
    /**app创建位置 **/
    lv_obj_t *btn_setting = ui_create_app_btn(page, LV_SYMBOL_SETTINGS, "Setting");
    lv_obj_t *btn_file = ui_create_app_btn(page, LV_SYMBOL_FILE, "FILE");

    /**app间的位置(自行调整 **/
    lv_obj_align_to(btn_file,btn_setting,LV_ALIGN_OUT_RIGHT_MID,0,0);

    /**各个按钮共享一个事件 **/
    lv_obj_add_event_cb(btn_setting, app_btn_event_cb, LV_EVENT_CLICKED, (void*)btn_list_setting);

    lv_obj_add_event_cb(btn_file, app_btn_event_cb, LV_EVENT_CLICKED,(void *) btn_list_File);

    return page;
}
static void app_btn_event_cb(lv_event_t *e)
{
    /**根据不同的用户输入数据,判断不同的app响应**/
    UI_APP_BTN_ENUM type=(UI_APP_BTN_ENUM)lv_event_get_user_data(e);
	if(type==btn_list_setting) Crue_App=btn_list_setting;
	else if(type==btn_list_File) Crue_App=btn_list_File;
    ui_goto_page(PAGE_APP_LIST,type,SETTING_NULL);
}
