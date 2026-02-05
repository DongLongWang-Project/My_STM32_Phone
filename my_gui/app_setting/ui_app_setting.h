#ifndef     __UI_APP_SETTING_H
#define    __UI_APP_SETTING_H

#include "lvgl.h"

#include "../ui_main.h"


#include "../ui_app_language.h"

#include "setting_wifi.h"
#include "setting_hotspot.h"
#include "setting_display.h"
#include "setting_about.h"

#define keil 0

#if keil
#include "TFTLCD.h"
#include "XPT2046.h"
#include "DX_WF25.h"
#endif // keil

 /******************************************
SETTING
*****************************************/


 /******************************************
FONT
*****************************************/
#if keil
 #define FONT_Size_12_PATH      "0:/SD/my_font_only/Font_12.bin"                                                                                                                                         
#define FONT_Size_14_PATH      "0:/SD/my_font_only/Font_14.bin"
#define FONT_Size_16_PATH      "0:/SD/my_font_only/Font_16.bin"

#else

#define FONT_Size_12_PATH     "0:/GitHub_Code/My_STM32_Phone/SD/my_font_all/Font_12.bin"
#define FONT_Size_14_PATH     "0:/GitHub_Code/My_STM32_Phone/SD/my_font_all/Font_14.bin"
#define FONT_Size_16_PATH     "0:/GitHub_Code/My_STM32_Phone/SD/my_font_all/Font_16.bin"

#endif 

extern lv_obj_t*list_wifi;

lv_obj_t* ui_app_setting_create_list(lv_obj_t *parent);
lv_obj_t* ui_app_setting_create_detail(lv_obj_t *parent );
UI_APP_SETTING_ENUM ui_app_get_setting_list_btnname(const char *btn_name);



#endif // __UI_APP_SETTING_H
