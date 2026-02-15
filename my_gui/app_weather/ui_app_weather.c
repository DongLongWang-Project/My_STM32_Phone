#include "ui_app_weather.h"



ui_app_weather_widget_t weather_widget;
static lv_obj_t* temperature_slider(lv_obj_t*parent,int32_t temperature_min,int32_t temperature_max);
static void event_weather_search_btn_cb(lv_event_t*e)
{
    lv_event_code_t code=lv_event_get_code(e);
    if(code==LV_EVENT_CLICKED)
    {
        /*获取城市名字*/
        #if keil
        Get_Weather_data(weather_api_str,weather_api_key_str,lv_textarea_get_text(weather_widget.search_box));
        #endif // keil
        
    }
}
void ui_app_weather_create(lv_obj_t*parent)
{
    lv_obj_t*img_icon;
    lv_obj_set_flex_flow(parent,LV_FLEX_FLOW_COLUMN);
    lv_obj_t*list_weather=lv_list_create(parent);
    lv_obj_set_size(list_weather,lv_pct(100),lv_pct(100));
    lv_obj_set_style_pad_all(list_weather,0,0);
    lv_obj_set_style_bg_opa(list_weather,100,LV_STATE_DEFAULT);
    /*搜索框(文本区域部件)*/
    lv_obj_t*obj_search_box=lv_obj_create(list_weather);
    lv_obj_set_style_pad_all(obj_search_box,0,0);

    lv_obj_set_size(obj_search_box,lv_pct(100),lv_pct(15));
   lv_obj_set_scrollbar_mode(obj_search_box,LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(obj_search_box,10,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj_search_box,lv_color_hex(0x949494),0); 
    
    weather_widget.search_box=lv_textarea_create(obj_search_box);
    lv_obj_set_style_bg_opa(weather_widget.search_box,100,LV_STATE_DEFAULT);
    lv_obj_set_size(weather_widget.search_box,lv_pct(80),lv_pct(100));
    lv_obj_align(weather_widget.search_box,LV_ALIGN_LEFT_MID,0,0);
    lv_textarea_set_one_line(weather_widget.search_box, true); 
    lv_textarea_set_placeholder_text(weather_widget.search_box,"Please to input city...");
    lv_obj_add_event_cb(weather_widget.search_box,event_textarea_cb,LV_EVENT_CLICKED,NULL);

    weather_widget.search_btn=ui_widgets_btn_create(obj_search_box,"OK");
    lv_obj_set_style_bg_opa(weather_widget.search_btn,100,LV_STATE_DEFAULT);
    lv_obj_set_size(weather_widget.search_btn,lv_pct(20),lv_pct(80)); 
    lv_obj_align_to(weather_widget.search_btn,weather_widget.search_box,LV_ALIGN_OUT_RIGHT_MID,0,0);
    lv_obj_add_event_cb(weather_widget.search_btn,event_weather_search_btn_cb,LV_EVENT_CLICKED,NULL);
    
    lv_obj_t*obj_cur_day=lv_obj_create(list_weather);
    lv_obj_set_style_bg_opa(obj_cur_day,10,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj_cur_day,lv_color_hex(0x949494),0); 
    lv_obj_set_style_pad_all(obj_cur_day,0,0);
    lv_obj_set_size(obj_cur_day,lv_pct(100),lv_pct(65));
    
    weather_widget.place_label=lv_label_create(obj_cur_day);
    #if keil
    lv_label_set_text(weather_widget.place_label,Cur_Time.place_str);
    #else
    lv_label_set_text(weather_widget.place_label,"UNKNOW");
    #endif // keil
    ui_set_obj_text_font(weather_widget.place_label,FONT_SIZE_16);
    lv_obj_align(weather_widget.place_label,LV_ALIGN_TOP_LEFT,10,20);
    
    weather_widget.temperature_label=lv_label_create(obj_cur_day);
    ui_set_obj_text_font(weather_widget.temperature_label,FONT_SIZE_32);
    lv_obj_align_to(weather_widget.temperature_label,weather_widget.place_label,LV_ALIGN_OUT_BOTTOM_LEFT,0,20);
    
    #if keil
    lv_label_set_text_fmt(weather_widget.temperature_label,"%s\n%d~%d°C",Cur_Time.three_day_data[0].weather_str,Cur_Time.three_day_data[0].low_temperature,Cur_Time.three_day_data[0].high_temperature);
     img_icon=update_weather_icon(obj_cur_day,Cur_Time.three_day_data[0].weather_code_day,true);
    lv_obj_align(img_icon,LV_ALIGN_TOP_RIGHT,-10,5);
    #else
    lv_label_set_text(weather_widget.temperature_label,"晴\n0~0°C");
    img_icon=update_weather_icon(obj_cur_day,0,true);
    lv_obj_align(img_icon,LV_ALIGN_TOP_RIGHT,-10,5);
    #endif // keil
    
//    weather_widget.humidity_label=lv_label_create(obj_cur_day);
//    ui_set_obj_text_font(weather_widget.humidity_label,FONT_SIZE_16);
//    lv_obj_align(weather_widget.humidity_label,LV_ALIGN_BOTTOM_LEFT,0,0);
//    #if keil
//    lv_label_set_text_fmt(weather_widget.temperature_label,"%d~%d°C",Cur_Time.three_day_data[0].low_temperature,Cur_Time.three_day_data[0].high_temperature);
//lv_label_set_text_fmt(weather_widget.humidity_label,"%s:%d%%",LV_SYMBOL_BELL);
//    #else
////    lv_label_set_text(weather_widget.humidity_label,"humidity:00%");
//lv_label_set_text_fmt(weather_widget.humidity_label,"humidity:00%%");
//    #endif // keil
    

  

    
    lv_obj_t*obj_next_day=lv_obj_create(list_weather);
    lv_obj_set_style_bg_opa(obj_next_day,10,LV_STATE_DEFAULT); 
    lv_obj_set_style_bg_color(obj_next_day,lv_color_hex(0x949494),0);
    lv_obj_set_style_pad_all(obj_next_day,0,0);
    lv_obj_set_style_radius(obj_next_day,0,0);
    lv_obj_set_size(obj_next_day,lv_pct(100),lv_pct(20));
    
    weather_widget.next_day1_label=lv_label_create(obj_next_day);
    weather_widget.next_day2_label=lv_label_create(obj_next_day);
    lv_obj_set_size(weather_widget.next_day1_label,lv_pct(50),lv_pct(50));
    lv_obj_set_size(weather_widget.next_day2_label,lv_pct(50),lv_pct(50));
    lv_obj_align_to(weather_widget.next_day2_label, weather_widget.next_day1_label,LV_ALIGN_OUT_BOTTOM_MID,0,0);
    ui_set_obj_text_font(weather_widget.next_day1_label,FONT_SIZE_16);
    ui_set_obj_text_font(weather_widget.next_day2_label,FONT_SIZE_16);

    
    
    #if keil
    lv_label_set_text_fmt(weather_widget.next_day1_label,"Day 1:%s",Cur_Time.three_day_data[1].weather_str);
    lv_label_set_text_fmt(weather_widget.next_day2_label,"Day2:%s",Cur_Time.three_day_data[2].weather_str);
    
    weather_widget.next_day1_tem=temperature_slider(obj_next_day,Cur_Time.three_day_data[1].low_temperature,Cur_Time.three_day_data[1].high_temperature);
    weather_widget.next_day2_tem=temperature_slider(obj_next_day,Cur_Time.three_day_data[2].low_temperature,Cur_Time.three_day_data[2].high_temperature);
    lv_obj_align_to(weather_widget.next_day1_tem,weather_widget.next_day1_label,LV_ALIGN_OUT_RIGHT_MID,0,-5);
    lv_obj_align_to(weather_widget.next_day2_tem,weather_widget.next_day2_label,LV_ALIGN_OUT_RIGHT_MID,0,-5);

    #else
     


   
    
    lv_label_set_text_fmt(weather_widget.next_day1_label,"Next_day 1:%s","晴");
    lv_label_set_text_fmt(weather_widget.next_day2_label,"Next_day2:%s","阴");
    
    weather_widget.next_day1_tem=temperature_slider(obj_next_day,-10,20);
    weather_widget.next_day2_tem=temperature_slider(obj_next_day,-20,10);

    lv_obj_align_to(weather_widget.next_day1_tem,weather_widget.next_day1_label,LV_ALIGN_OUT_RIGHT_MID,0,-5);
    lv_obj_align_to(weather_widget.next_day2_tem,weather_widget.next_day2_label,LV_ALIGN_OUT_RIGHT_MID,0,-5);


    #endif // keil
//    
//    weather_widget.template_label=lv_label_create(list_weather);
//    lv_label_set_text_fmt(weather_widget.template_label,"%d",Cur_Time.three_day_data[0].high_temperature);
//    ui_set_obj_text_font(weather_widget.template_label,FONT_SIZE_32);
}

lv_obj_t* update_weather_icon(lv_obj_t * parent, uint8_t weather_code,bool is_cur_day) 
{
    char icon_path[128];
    lv_obj_t * img_icon = lv_img_create(parent);
    if(is_cur_day==true)
    {
    #if keil
    snprintf(icon_path, sizeof(icon_path), "0:SD/my_icon/xinzhi_icon/white/%d@1x.png", weather_code);
    #else
//    snprintf(icon_path, sizeof(icon_path), "0:/GitHub_Code/My_STM32_Phone/SD/my_icon/weather_icon/%d.png", weather_code);
     snprintf(icon_path, sizeof(icon_path), "0:/GitHub_Code/My_STM32_Phone/SD/my_icon/xinzhi_icon/white/%d@1x.png", weather_code);
//     snprintf(icon_path, sizeof(icon_path), "0:/GitHub_Code/My_STM32_Phone/SD/my_icon/xinzhi_icon/old/%d.png", weather_code);
   
//    snprintf(icon_path, sizeof(icon_path), "0:/icon/%d.png", weather_code);
    #endif // keil 

    }
    else
    {
    #if keil
    snprintf(icon_path, sizeof(icon_path), "0:SD/my_icon/xinzhi_icon/white/%d@1x.png", weather_code);
    #else
//    snprintf(icon_path, sizeof(icon_path), "0:/GitHub_Code/My_STM32_Phone/SD/my_icon/weather_icon/%d.png", weather_code);
     snprintf(icon_path, sizeof(icon_path), "0:/GitHub_Code/My_STM32_Phone/SD/my_icon/xinzhi_icon/white/%d@1x.png", weather_code);
//     snprintf(icon_path, sizeof(icon_path), "0:/GitHub_Code/My_STM32_Phone/SD/my_icon/xinzhi_icon/old/%d.png", weather_code);
   
//    snprintf(icon_path, sizeof(icon_path), "0:/icon/%d.png", weather_code);
    #endif // keil
    }
    // 关键一步：重新设置图片源
    lv_img_set_src(img_icon, icon_path);
    return img_icon;
}

static lv_obj_t* temperature_slider(lv_obj_t*parent,int32_t temperature_min,int32_t temperature_max)
{
   lv_obj_t*slider=lv_slider_create(parent);
   lv_obj_set_width(slider,lv_pct(50));

   lv_slider_set_mode(slider,LV_SLIDER_MODE_RANGE);
   lv_slider_set_range(slider,-30,50);
    lv_obj_set_style_opa(slider,0,LV_PART_KNOB);
    lv_obj_add_state(slider,LV_STATE_DISABLED);
    lv_slider_set_left_value(slider,temperature_min,LV_ANIM_OFF);
    lv_slider_set_value(slider,temperature_max,LV_ANIM_OFF);

    lv_obj_t *label=lv_label_create(slider);
   
    lv_obj_set_style_text_color(label,lv_color_hex(0),0);
    lv_label_set_text_fmt(label,"%d°~%d°",temperature_min,temperature_max);
     lv_obj_align_to(label,slider,LV_ALIGN_CENTER,0,0);
    
   return slider;
}
