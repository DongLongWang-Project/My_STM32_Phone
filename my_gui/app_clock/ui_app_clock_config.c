#include "ui_app_clock_config.h"

ui_weather_time_t Cur_Time=
{
    .year=2026,
    .month=2,
    .day=3,
    .hour=0,
    .min=0,
    .sec=50,
    
    .temperature=2,
    .weather_code=1,
    
    .timer_hour=0,
    .timer_min=0,
    .timer_sec=0,
};

ui_clock_widget_t Clock_time_widget=
{
     .meter=NULL,
     .hour_indicator=NULL,
     .min_indicator=NULL,
     .sec_indicator=NULL, 
     .time_label=NULL,
};
 
 ui_timer_widget_t timer_widget=
 {
    .timer_obj=NULL,
     .hour_roller=NULL,
     .minute_roller=NULL,
     .second_roller=NULL,
     .btn_timer_switch=NULL,
     .switch_label=NULL,
 };

alarm_clock_t alarm_clock;

     
// 小时选项：00 - 23
const char * clock_set_hour_options = 
    "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n"
    "12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23";

// 分钟选项：00 - 59
const char * clock_set_min_options = 
    "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n"
    "20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n"
    "40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59";
