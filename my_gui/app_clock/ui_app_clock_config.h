#ifndef __UI_APP_CLOCK_CONFIG_H
#define __UI_APP_CLOCK_CONFIG_H

#include "lvgl.h"

typedef struct
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  int8_t high_temperature;
  int8_t low_temperature;
  uint8_t weather_code_day;
  uint8_t weather_code_night;
  char weather_str[8];
}day_data;

typedef struct
{
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  char place_str[32];
  char wind_dir_str[8];
  uint8_t humidity;
  day_data three_day_data[3]; 
}ui_weather_time_t;


typedef struct
{
  uint8_t timer_hour;
  uint8_t timer_min;
  uint8_t timer_sec;
}timer_data_t;

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
    bool btn_timer_switch_state;
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
    bool alarm_is_on;
    RINGTONE_ENUM Ringtone;
    REPEAT_ENUM repeat;
}alarm_clock_set_t;

#define SAVE_ALARM_CLOCK_MAX_NUM 10
#define ALARM_SAVE_FLAG 0XAA

typedef struct
{
  uint8_t                 save_flag;
  uint8_t                 save_alarm_num;  
 alarm_clock_set_t       save_alarm_clock_table[SAVE_ALARM_CLOCK_MAX_NUM];   
}alarm_clock_t;

typedef enum
{
    ALARM_ADD=0,
    ALARM_SETTING,
    
}ALARM_MODE_ENUM;

extern ui_clock_widget_t Clock_time_widget;
extern ui_timer_widget_t timer_widget;
extern ui_weather_time_t Cur_Time;
extern alarm_clock_t alarm_clock;
extern timer_data_t timer_data;

extern const char * clock_set_hour_options;
extern const char * clock_set_min_options;

#endif

