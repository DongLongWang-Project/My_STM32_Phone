#ifndef __UI_APP_CLOCK_H
#define __UI_APP_CLOCK_H
#include "lvgl.h"
#include "../app_setting/setting_display.h"
#include "../ui_widgets.h"
#include "ui_app_clock_config.h"

void ui_app_clock_creat(lv_obj_t*parent);
void clock_time_timer_cb(lv_timer_t *timer);
void Alarm_System_Init(void);
void save_alarm_data(void);
#endif
