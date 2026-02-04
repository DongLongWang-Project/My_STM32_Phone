#ifndef __UI_APP_CLOCK_H
#define __UI_APP_CLOCK_H
#include "lvgl.h"
#include "../app_setting/setting_display.h"
#include "../ui_widgets.h"
#if keil
#include "DX_WF25.h"
#endif // keil

void ui_app_clock_creat(lv_obj_t*parent);


typedef struct
{
    lv_obj_t*meter;
    lv_meter_indicator_t *hour_indicator;
    lv_meter_indicator_t *min_indicator;
    lv_meter_indicator_t *sec_indicator; 
    lv_obj_t*time_label;
}ui_clock_widget_t;


typedef struct
{
    lv_obj_t*timer_obj;
    lv_obj_t*hour_roller;
    lv_obj_t*minute_roller;
    lv_obj_t*second_roller;
    lv_obj_t*btn_timer_switch;
    lv_obj_t*switch_label;
}ui_timer_widget_t;

typedef enum
{
    Normal=0,
    
}RINGTONE_ENUM;

typedef enum
{
    Single=0,
    EveryDay,
    
}REPEAT_ENUM;


typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t alarm_is_on;
    RINGTONE_ENUM Ringtone;
    REPEAT_ENUM repeat;
}alarm_clock_set_t;
#define SAVE_ALARM_CLOCK_MAX_NUM 10
#define ALARM_SAVE_FLAG 0XAA

typedef struct
{
  uint8_t                save_flag;
  uint8_t                save_alarm_num;  
 alarm_clock_set_t       save_alarm_clock_table[SAVE_ALARM_CLOCK_MAX_NUM];   
}alarm_clock_t;

typedef enum
{
    ALARM_ADD=0,
    ALARM_SETTING,
    
}ALARM_MODE_ENUM;
void clock_time_timer_cb(lv_timer_t *timer);
extern ui_clock_widget_t Clock_time_widget;
extern ui_timer_widget_t timer_widget;


void Alarm_System_Init(void);
void save_alarm_data(void);
#endif
