#ifndef  UI_MAIN_H
#define UI_MAIN_H

#include "lvgl.h"

#include "ui_widgets.h"
#include "app_setting/ui_app_setting.h"
#include "app_file_sys/ui_app_file.h"
#include "app_net_ass/ui_app_net.h"
#include "app_clock/ui_app_clock.h"
#include "app_weather/ui_app_weather.h"

 
//#include "HX1838.h"
//#include "key.h"
//#include "lv_port_indev.h"

#include "lvgl.h"
#include "ui_widgets.h"

//#include "HX1838.h"


#define StateBar_Percent     5
#define MainPage_Percent     87
#define ControlBar_Percent   8

/******************************************
PAGE
*****************************************/
typedef enum {
    PAGE_HOME,/** 子主界面**/
    PAGE_APP_LIST,/**app列表界面 **/
    PAGE_APP_DETAIL,/**app详情界面 **/
} UI_APP_PAGE_ENUM;

typedef enum{
APP_SETTING=0,
APP_FILE,
APP_NET,
APP_WEATHER,
APP_CLOCK,
APP_TEMP,
}UI_APP_ENUM;

typedef struct{
const void *icon;
const char *text[APP_TEMP];
}ui_app_message_t;

extern UI_APP_ENUM Cur_APP;
extern UI_APP_PAGE_ENUM Cur_Page;
extern lv_obj_t *tile_main;
extern lv_obj_t*keyboard;
 void ui_page_clear(void);
lv_obj_t* ui_page_creat_app(lv_obj_t *parent);
void ui_goto_page(UI_APP_PAGE_ENUM Page,UI_APP_ENUM APP);
lv_obj_t* ui_main_page(lv_obj_t*parent);
void event_ui_app_list_btn_cb(lv_event_t *e);
void ui_init(void);
void btn_back(void);
#endif
