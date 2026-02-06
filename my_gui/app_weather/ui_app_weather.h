#ifndef __UI_APP_WEATHER_H
#define __UI_APP_WEATHER_H
#include "lvgl.h"
#include "../ui_widgets.h"
#include "../app_setting/setting_display.h"
#include "../app_clock/ui_app_clock_config.h"


#define CITY_STR_MAX_LEN 32
void ui_app_weather_create(lv_obj_t*parent);

typedef struct
{
    lv_obj_t*search_box;
    lv_obj_t*place_label;
    char place_str[CITY_STR_MAX_LEN];
    lv_obj_t*template_label;
}ui_app_weather_widget_t;

extern ui_app_weather_widget_t weather_widget;
#endif
