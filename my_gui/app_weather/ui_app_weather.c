#include "ui_app_weather.h"



ui_app_weather_widget_t weather_widget=
{
    .place_str="www"
};
void ui_app_weather_create(lv_obj_t*parent)
{
    lv_obj_set_flex_flow(parent,LV_FLEX_FLOW_COLUMN);
        
    weather_widget.search_box=lv_textarea_create(parent);
    lv_obj_set_size(weather_widget.search_box,lv_pct(90),lv_pct(10));
    lv_obj_align(weather_widget.search_box,LV_ALIGN_TOP_MID,0,0);
    lv_textarea_set_one_line(weather_widget.search_box, true); 
    lv_obj_add_event_cb(weather_widget.search_box,event_textarea_cb,LV_EVENT_CLICKED,NULL);
    
    weather_widget.place_label=lv_label_create(parent);
    lv_label_set_text(weather_widget.place_label,weather_widget.place_str);
    ui_set_obj_text_font(weather_widget.place_label,FONT_SIZE_16);
    
    weather_widget.template_label=lv_label_create(parent);
    lv_label_set_text_fmt(weather_widget.template_label,"%d",Cur_Time.three_day_data[0].high_temperature);
    ui_set_obj_text_font(weather_widget.template_label,FONT_SIZE_32);
}
