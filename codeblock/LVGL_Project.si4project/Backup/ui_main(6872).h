#ifndef  UI_MAIN_H
#define UI_MAIN_H

#include "lvgl.h"
#include "ui_app_page.h"
#include "ui_app_detail.h"
#include "ui_app_list.h"
#include "ui_enum_struct.h"

/******************************************************
当前屏幕的宽高
******************************************************/
#define scr_width   lv_obj_get_width(lv_scr_act())
#define scr_height  lv_obj_get_height(lv_scr_act())

#define CRUE_PATH                  "F:"
#define FONT_Size_12_PATH    "F:/End_Design/SD/my_font/Font_12.bin"
#define FONT_Size_16_PATH    "F:/End_Design/SD/my_font/Font_16.bin"
#define FONT_Size_24_PATH    "F:/End_Design/SD/my_font/Font_24.bin"
#define FONT_Size_32_PATH    "F:/End_Design/SD/my_font/Font_32.bin"






extern my_font_t my_fonts;
extern char Cure_Path[100];
extern UI_APP_BTN_ENUM Crue_App;
extern UI_APP_ENUM Cur_Page;

void ui_init(void);
void ui_goto_page(UI_APP_ENUM page,UI_APP_BTN_ENUM btn_type,UI_APP_SETTING_LIST_ENUM setting_mode);
void my_font_init(void);

#endif
