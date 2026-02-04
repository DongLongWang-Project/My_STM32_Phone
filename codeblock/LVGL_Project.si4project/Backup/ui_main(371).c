#include "ui_main.h"

/******************************************************
1 默认当前界面为子界面
2 建立平铺界面
3 根界面 app的list界面 app详细界面
******************************************************/
static lv_obj_t *tileview,*tile_main;
static lv_obj_t *home_page,*app_page,*detail_page;

my_font_t my_fonts;
char Cure_Path[100]={"F:"};
UI_APP_ENUM Cur_Page = PAGE_HOME;
UI_APP_BTN_ENUM Crue_APP=Home;
ui_setting_ctx_t sctx;
sctx.Mode=SETTING_NULL;

/******************************************************
函数:ui初始化(主函数)
参数:无
返回:无
注意:
******************************************************/
void ui_init(void)
{
    my_font_init();

    tileview = lv_tileview_create(lv_scr_act());   /**  创建平铺界面                                         **/
    tile_main = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_HOR | LV_DIR_VER);/** 创建子主界面,在0列0行,可横向竖向滑动**/
    lv_obj_set_tile(tileview, tile_main, LV_ANIM_OFF);/**动画关闭**/
    lv_obj_set_style_text_font(tile_main,my_fonts.FONT_SIZE_16,LV_STATE_DEFAULT);
	My_Ui_Creat_Navigation_bar(tileview);/**创建导航栏**/


	ui_goto_page(PAGE_HOME,Home,&sctx);

}

/******************************************************
函数:页面跳转函数
参数:1.要跳转的页面,2.页面的app按钮枚举
返回:无
注意:根据相应app按钮创建相应的app list,比如:设置按钮,会生成list,里面有各种设置项
        并且点击每个设置项都有相应的详情页
        APP 是事件回调函数传入的用户数据,每个app都有独自的
******************************************************/
void ui_goto_page(UI_APP_ENUM Page,UI_APP_BTN_ENUM APP,void*ctx)
{
    switch(Page)
    {
	case PAGE_HOME:
		{
			/** 删除app列表页,设为空**/
			if(app_page!=NULL)lv_obj_del(app_page);app_page = NULL;
			/** 删除详情页,设为空**/
			if(detail_page!=NULL)	lv_obj_del(detail_page);detail_page = NULL;
			home_page = page_child_create(tile_main);/**在主界面创建app图标按钮**/
	        lv_obj_clear_flag(home_page, LV_OBJ_FLAG_HIDDEN);/**主页面显现 **/
	        Cur_Page = PAGE_HOME;/** 设当前页为主页面**/
	        break;
		}
    case PAGE_APP_LIST:/** 列表页**/
		/** 删除app列表页,设为空**/
		if(app_page!=NULL)lv_obj_del(app_page);app_page = NULL;
        switch(APP)
        {
            case btn_list_setting:
				{

					app_page = page_app_Setting_list_create(tile_main);   break;/**创建wifi **/
				}
            case btn_list_File:
				{
					app_page = page_app_list_fat_create(tile_main,Cure_Path);break;/** 创建文件列表**/
            	}
			}
		/** 隐藏主界面,设置当前为app列表页**/
        lv_obj_add_flag(home_page, LV_OBJ_FLAG_HIDDEN);Cur_Page = PAGE_APP_LIST;
        /** 删除详情页,设为空**/
        if(detail_page!=NULL)	lv_obj_del(detail_page);detail_page = NULL;
        /**app列表页显现 **/
        lv_obj_clear_flag(app_page, LV_OBJ_FLAG_HIDDEN);
        break;
    case PAGE_APP_DETAIL:/**详情页(page_detail_create函数可以自行设置,只需返回界面就行,传入的对象作为父对象就行) **/
         switch(APP)
        {
            case btn_list_setting:
				{
					sctx=(ui_setting_ctx_t *)ctx;
					detail_page = page_detail_create(tile_main,sctx->Mode);  break;
		    	}
			case btn_list_File:
				{
					 detail_page = page_detail_file_create(tile_main,Cure_Path);       break;
				}
			default:
            break;
        }
		 /**隐藏app列表页,设置当前为详情页**/
        lv_obj_add_flag(app_page, LV_OBJ_FLAG_HIDDEN);Cur_Page = PAGE_APP_DETAIL;
        break;

    default:
        break;
    }
}











void my_font_init(void)
{
		my_fonts.FONT_SIZE_12=lv_font_load (FONT_Size_12_PATH);
		my_fonts.FONT_SIZE_16=lv_font_load (FONT_Size_16_PATH);
		my_fonts.FONT_SIZE_24=lv_font_load (FONT_Size_24_PATH);
		my_fonts.FONT_SIZE_32=lv_font_load (FONT_Size_32_PATH);

		if(my_fonts.FONT_SIZE_12==NULL) printf("use FONT_SIZE_12 to error \r\n");
		else  my_fonts.FONT_SIZE_12->fallback=&lv_font_montserrat_12;
		if(my_fonts.FONT_SIZE_16==NULL) printf("use FONT_SIZE_16 to error \r\n");
		else  my_fonts.FONT_SIZE_16->fallback=&lv_font_montserrat_16;
		if(my_fonts.FONT_SIZE_24==NULL) printf("use FONT_SIZE_24 to error \r\n");
		else  my_fonts.FONT_SIZE_24->fallback=&lv_font_montserrat_24;
		if(my_fonts.FONT_SIZE_32==NULL) printf("use FONT_SIZE_32 to error \r\n");
		else  my_fonts.FONT_SIZE_32->fallback=&lv_font_montserrat_32;
}






