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
    
    
}ui_app_weather_widget_t;

extern ui_app_weather_widget_t weather_widget;
lv_obj_t* update_weather_icon(lv_obj_t * parent, uint8_t weather_code,bool is_cur_day); 
#endif
