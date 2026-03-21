#ifndef __UI_APP_WEATHER_H
#define __UI_APP_WEATHER_H
#include "lvgl.h"
#include "../ui_widgets.h"
#include "../app_setting/setting_display.h"
#include "../app_clock/ui_app_clock_config.h"
#if keil
#include "DX_WF25.h"
#endif

#define CITY_STR_MAX_LEN 32

#define icon_width      48
#define icon_height     48
#define icon_buf_size   icon_width*icon_height*2

void ui_app_weather_create(lv_obj_t*parent);

typedef struct
{
    lv_obj_t*search_box;
    lv_obj_t*search_btn;
    lv_obj_t*place_label;
    lv_obj_t*temperature_label; 
    lv_obj_t*humidity_label;
    lv_obj_t*wind_dir_label;  
    lv_obj_t*wind_speed_label;
    
    lv_obj_t*next_day1_label;
    lv_obj_t*next_day2_label;
    
    lv_obj_t*next_day1_tem;
    lv_obj_t*next_day2_tem;
    
    lv_fs_file_t file_fp;
}ui_app_weather_widget_t;

typedef enum
{
TEXTAREA_LABEL=0,
UI_WEATHER_LA_MAX,
}UI_WEATHER_LA_ENUM;
extern ui_app_weather_widget_t weather_widget;
lv_obj_t* update_weather_icon(lv_obj_t * parent, uint8_t weather_code ); 
#endif
