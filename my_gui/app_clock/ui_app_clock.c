#include "ui_app_clock.h"


lv_obj_t* ui_widgets_btn_create(lv_obj_t*parent,const char*btn_text);

static void event_btn_add_clock_cb(lv_event_t*e);
static void event_clock_obj_cb(lv_event_t*e);
lv_obj_t*tab_clock_creat(lv_obj_t*parent,alarm_clock_set_t *alarm_table);
static void clock_create(lv_obj_t*parent);
static void alarm_clock_create(lv_obj_t*parent);
static void timer_create(lv_obj_t*parent);
static void event_add_alarm_cb(lv_event_t*e);
static void event_timer_cb(lv_event_t *e);
static void event_alarm_time_is_on_cb(lv_event_t*e);
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
     
void ui_app_clock_creat(lv_obj_t*parent)
{
    lv_obj_t*clock_tabview=lv_tabview_create(parent,LV_DIR_BOTTOM,30);
    
    lv_obj_t*tab_Clock=lv_tabview_add_tab(clock_tabview,"Alarm Clock");
    lv_obj_t*tab_Time=lv_tabview_add_tab(clock_tabview,"Clock");
    lv_obj_t*tab_Timing=lv_tabview_add_tab(clock_tabview,"Timer");
    
    lv_obj_set_style_pad_all(tab_Clock,0,0);  
    lv_obj_set_style_pad_all(tab_Time,0,0);  
    lv_obj_set_style_pad_all(tab_Timing,0,0);  

   alarm_clock_create(tab_Clock); 
   clock_create(tab_Time);
    timer_create(tab_Timing);  
}

static void alarm_clock_create(lv_obj_t*parent)
{
    lv_obj_t*Clock_list=lv_list_create(parent);
    lv_obj_set_size(Clock_list,lv_pct(100),lv_pct(100));
    lv_obj_set_style_pad_all(Clock_list,0,0);
    lv_list_add_text(Clock_list,"Alarm Clock");
    lv_obj_t*btn_add_clock=lv_btn_create(parent);
    lv_obj_set_size(btn_add_clock,lv_pct(20),lv_pct(7));
    lv_obj_t*btn_add_clock_label=lv_label_create(btn_add_clock);
    lv_label_set_text(btn_add_clock_label,LV_SYMBOL_PLUS);
    lv_obj_center(btn_add_clock_label);
    lv_obj_align(btn_add_clock,LV_ALIGN_TOP_RIGHT,0,0);
   
   for(uint8_t i=0;i<alarm_clock.save_alarm_num;i++)
   {
        tab_clock_creat(Clock_list,&alarm_clock.save_alarm_clock_table[i]);
   }
    lv_obj_add_event_cb(btn_add_clock,event_btn_add_clock_cb,LV_EVENT_CLICKED,NULL);
}

static void clock_create(lv_obj_t*parent)
{
    lv_obj_t*Time_list=lv_list_create(parent);
    lv_obj_set_size(Time_list,lv_pct(100),lv_pct(100));
    lv_obj_set_style_pad_all(Time_list,0,0);
    lv_list_add_text(Time_list,"Clock");
   
    lv_obj_t*time_obj=lv_obj_create(Time_list);
    lv_obj_set_size(time_obj,lv_pct(100),lv_pct(90));
    lv_obj_set_style_pad_all(time_obj,0,0);
    
    
    Clock_time_widget.meter=lv_meter_create(time_obj);
     lv_obj_set_size(Clock_time_widget.meter,160,160);
     lv_obj_center(Clock_time_widget.meter);
//    lv_obj_align(Clock_time_widget.meter,LV_ALIGN_TOP_MID,0,5);
    lv_meter_scale_t *scale= lv_meter_add_scale(Clock_time_widget.meter);

  
  lv_meter_set_scale_ticks(Clock_time_widget.meter,scale,60,1,8,lv_color_hex(0x9D9D9D));
   lv_meter_set_scale_major_ticks(Clock_time_widget.meter,scale,5,2,8,lv_color_hex(0),10);
   
    lv_meter_set_scale_range(Clock_time_widget.meter,scale,0,60,360,-90);
   
   Clock_time_widget.sec_indicator=lv_meter_add_needle_line(Clock_time_widget.meter,scale,2,lv_color_hex(0x599FFD),0);
   Clock_time_widget.min_indicator=lv_meter_add_needle_line(Clock_time_widget.meter,scale,3,lv_color_hex(0),-6);
    Clock_time_widget.hour_indicator=lv_meter_add_needle_line(Clock_time_widget.meter,scale,4,lv_color_hex(0),-30);
   
   #if keil
   lv_meter_set_indicator_value(Clock_time_widget.meter,Clock_time_widget.sec_indicator,Cur_Time.sec);
   lv_meter_set_indicator_value(Clock_time_widget.meter,Clock_time_widget.min_indicator,Cur_Time.min);
   
    uint8_t hour=(Cur_Time.hour%12)*5+(Cur_Time.min/12);
    lv_meter_set_indicator_value(Clock_time_widget.meter, Clock_time_widget.hour_indicator,hour);
   
   Clock_time_widget.time_label=lv_label_create(time_obj);
   lv_obj_align_to(Clock_time_widget.time_label,Clock_time_widget.meter,LV_ALIGN_OUT_BOTTOM_LEFT,45,10);
   lv_label_set_text_fmt(Clock_time_widget.time_label,"Time:%d:%d:%d",Cur_Time.hour,Cur_Time.min,Cur_Time.sec);
   #endif

}

static void timer_create(lv_obj_t*parent)
{
    lv_obj_t*timer_list=lv_list_create(parent);
    lv_obj_set_size(timer_list,lv_pct(100),lv_pct(100));
    lv_obj_set_style_pad_all(timer_list,0,0);
    lv_list_add_text(timer_list,"Timer");
    
    timer_widget.timer_obj=lv_obj_create(timer_list);
    lv_obj_set_size(timer_widget.timer_obj,lv_pct(100),lv_pct(90));
    #if keil
    /*创建小时滚动条*/
    timer_widget.hour_roller=lv_roller_create(timer_widget.timer_obj);
    lv_obj_set_size(timer_widget.hour_roller,lv_pct(33),lv_pct(60));
    lv_obj_set_style_text_line_space(timer_widget.hour_roller,10,LV_STATE_DEFAULT);
    lv_obj_set_align(timer_widget.hour_roller,LV_ALIGN_TOP_LEFT);
    lv_roller_set_options(timer_widget.hour_roller,clock_set_hour_options,LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(timer_widget.hour_roller,Cur_Time.timer_hour,LV_ANIM_ON);
    /*创建分钟滚动条*/
    timer_widget.minute_roller=lv_roller_create(timer_widget.timer_obj);
    lv_obj_set_size(timer_widget.minute_roller,lv_pct(33),lv_pct(60));
    lv_obj_set_style_text_line_space(timer_widget.minute_roller,10,LV_STATE_DEFAULT);
    lv_obj_align_to(timer_widget.minute_roller,timer_widget.hour_roller,LV_ALIGN_OUT_RIGHT_MID,0,0);
    lv_roller_set_options(timer_widget.minute_roller,clock_set_min_options,LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(timer_widget.minute_roller,Cur_Time.timer_min,LV_ANIM_ON);
    /*创建秒钟滚动条*/
    timer_widget.second_roller=lv_roller_create(timer_widget.timer_obj);
    lv_obj_set_size(timer_widget.second_roller,lv_pct(33),lv_pct(60));
    lv_obj_set_style_text_line_space(timer_widget.second_roller,10,LV_STATE_DEFAULT);
    lv_obj_align_to(timer_widget.second_roller,timer_widget.minute_roller,LV_ALIGN_OUT_RIGHT_MID,0,0);
    lv_roller_set_options(timer_widget.second_roller,clock_set_min_options,LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(timer_widget.second_roller,Cur_Time.timer_sec,LV_ANIM_ON);
    
    timer_widget.btn_timer_switch=lv_switch_create(timer_widget.timer_obj);
    lv_obj_align(timer_widget.btn_timer_switch,LV_ALIGN_BOTTOM_MID,0,0);
    lv_obj_set_size(timer_widget.btn_timer_switch,lv_pct(40),lv_pct(20));
    timer_widget.switch_label=lv_label_create(timer_widget.btn_timer_switch); 
    lv_obj_center(timer_widget.switch_label);
    lv_label_set_text(timer_widget.switch_label,"");
        if(Cur_Time.timer_hour  || Cur_Time.timer_min  || Cur_Time.timer_sec )
    {
        lv_obj_add_state(timer_widget.btn_timer_switch,LV_STATE_CHECKED);
    }
     /*按钮添加事件*/
     lv_obj_add_event_cb(timer_widget.btn_timer_switch,event_timer_cb,LV_EVENT_VALUE_CHANGED,NULL); 
     #endif  
}

static void event_timer_cb(lv_event_t *e)
{
    #if keil
    lv_obj_t*target=lv_event_get_target(e);
        
    if(lv_obj_has_state(target,LV_STATE_CHECKED))
    {
    
        Cur_Time.timer_hour=lv_roller_get_selected(timer_widget.hour_roller);
        Cur_Time.timer_min=lv_roller_get_selected(timer_widget.minute_roller);
        Cur_Time.timer_sec=lv_roller_get_selected(timer_widget.second_roller);
        lv_obj_add_state(timer_widget.hour_roller,LV_STATE_DISABLED);
        lv_obj_add_state(timer_widget.minute_roller,LV_STATE_DISABLED);
        lv_obj_add_state(timer_widget.second_roller,LV_STATE_DISABLED);
    }
    else
    {
       lv_obj_clear_state(timer_widget.hour_roller,LV_STATE_DISABLED);
        lv_obj_clear_state(timer_widget.minute_roller,LV_STATE_DISABLED);
        lv_obj_clear_state(timer_widget.second_roller,LV_STATE_DISABLED);
       Cur_Time.timer_hour=0; 
       Cur_Time.timer_min=0;
       Cur_Time.timer_sec=0;
       lv_label_set_text(timer_widget.switch_label,"");
    }
    #endif // keil
}


    #if keil
static void alarm_is_time(void)
{

  static uint8_t last_min=61;
  if(last_min!=Cur_Time.min)
  {
     for(uint8_t i=0;i<alarm_clock.save_alarm_num;i++)
     {
      if(alarm_clock.save_alarm_clock_table[i].alarm_is_on)
      {
         if(alarm_clock.save_alarm_clock_table[i].hour==Cur_Time.hour &&
            alarm_clock.save_alarm_clock_table[i].min==Cur_Time.min)
          {
            /*闹钟时间到了*/
            /*创建一个窗口,单次的闹钟关闭,每天的闹钟继续*/
            
            lv_obj_clear_flag(alarm_msgbox_obj,LV_OBJ_FLAG_HIDDEN);
          }  
      }
     }
      last_min=Cur_Time.min;
  }

}


static void clock_timer(void)
{
   Cur_Time.sec++;
   if(Cur_Time.sec>=60)
   {
       Cur_Time.sec=0;
       Cur_Time.min++;
       if(Cur_Time.min>=60)
       {
           Cur_Time.min=0;
           Cur_Time.hour++;
           Cur_Time.hour = (Cur_Time.hour + 1) % 24;
       }
   }
    alarm_is_time();
   if(Clock_time_widget.meter!=NULL)
   {
       lv_meter_set_indicator_value(Clock_time_widget.meter,Clock_time_widget.sec_indicator,Cur_Time.sec);
       lv_meter_set_indicator_value(Clock_time_widget.meter,Clock_time_widget.min_indicator,Cur_Time.min);
       uint8_t hour=(Cur_Time.hour%12)*5+(Cur_Time.min/12);
       lv_meter_set_indicator_value(Clock_time_widget.meter, Clock_time_widget.hour_indicator,hour);
       lv_label_set_text_fmt(Clock_time_widget.time_label,"Time:%d:%d:%d",Cur_Time.hour,Cur_Time.min,Cur_Time.sec);
   }
}
static void timer_timer(void)
{
    // 1. 检查是否已经归零（停止条件）
    if(Cur_Time.timer_hour == 0 && Cur_Time.timer_min == 0 && Cur_Time.timer_sec == 0)
    {
        if(timer_widget.timer_obj!=NULL)
        {
            /* 结束逻辑 */
            lv_obj_clear_state(timer_widget.hour_roller, LV_STATE_DISABLED);
            lv_obj_clear_state(timer_widget.minute_roller, LV_STATE_DISABLED);
            lv_obj_clear_state(timer_widget.second_roller, LV_STATE_DISABLED);
            // 关闭开关
            lv_obj_clear_state(timer_widget.btn_timer_switch, LV_STATE_CHECKED);
            lv_label_set_text(timer_widget.switch_label,"");
        }
        return;
    }

    // 2. 统一减 1 秒逻辑（处理借位）
    if (Cur_Time.timer_sec > 0) {
        Cur_Time.timer_sec--;
    } else {
        if (Cur_Time.timer_min > 0) {
            Cur_Time.timer_min--;
            Cur_Time.timer_sec = 59; // 秒钟借位
        } else {
            if (Cur_Time.timer_hour > 0) {
                Cur_Time.timer_hour--;
                Cur_Time.timer_min = 59; // 分钟借位
                Cur_Time.timer_sec = 59; // 秒钟借位
            }
        }
    }

    // 3. 实时刷新 UI 显示
    // 建议使用 %02d 让显示更整齐，比如 01:05:09
            if(timer_widget.timer_obj!=NULL)
        {
            lv_label_set_text_fmt(timer_widget.switch_label, "%02d:%02d:%02d", 
                                  Cur_Time.timer_hour, Cur_Time.timer_min, Cur_Time.timer_sec);  
        }
}

  
 void clock_time_timer_cb(lv_timer_t *timer)
{
    clock_timer();
    timer_timer();
}
  #endif // keil
lv_obj_t* ui_widgets_btn_create(lv_obj_t*parent,const char*btn_text)
{
    lv_obj_t*btn=lv_btn_create(parent);
    lv_obj_t*btn_label=lv_label_create(btn);
    lv_label_set_text(btn_label,btn_text);
    lv_obj_set_size(btn,35,35);
    lv_obj_center(btn_label);
//    lv_obj_set_style_bg_opa(btn,10,LV_STATE_DEFAULT);
//    lv_obj_set_style_border_width(btn,0,LV_STATE_DEFAULT);
//    lv_obj_set_style_border_opa(btn,0,LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn,0,LV_STATE_DEFAULT);
//    lv_obj_set_style_text_color(btn_label,lv_color_hex(0),LV_STATE_DEFAULT);
    return btn;
}
void clock_time_create(lv_obj_t*parent,ALARM_MODE_ENUM alarm_mode)
{
    /*创建一个区域*/
    lv_obj_t*obj=lv_obj_create(parent);
    lv_obj_set_size(obj,lv_pct(100),lv_pct(100));
    lv_obj_set_style_bg_opa(obj,100,LV_STATE_DEFAULT);
   /*在区域里创建闹钟列表*/
    lv_obj_t*clock_set_list=lv_list_create(obj);
    lv_obj_set_size(clock_set_list,lv_pct(100),lv_pct(100));
    if(alarm_mode==ALARM_ADD)
    {
          lv_list_add_text(clock_set_list,"Add Clock");  
    }
    else if(alarm_mode == ALARM_SETTING)
    {
        lv_list_add_text(clock_set_list,"Clock Setting");   
    }

    lv_obj_t*clock_label=lv_label_create(clock_set_list);/**/
    lv_obj_set_align(clock_label,LV_ALIGN_TOP_MID); 

    /*滚动条区域*/
    lv_obj_t*roller_obj=lv_obj_create(clock_set_list);
    lv_obj_set_size(roller_obj,lv_pct(100),lv_pct(40));
    lv_obj_set_style_pad_all(roller_obj,0,0);
    /*创建小时滚动条*/
    lv_obj_t*hour_roller=lv_roller_create(roller_obj);
    lv_obj_set_size(hour_roller,lv_pct(35),lv_pct(100));
    lv_obj_set_style_text_line_space(hour_roller,10,LV_STATE_DEFAULT);
    lv_obj_set_align(hour_roller,LV_ALIGN_BOTTOM_LEFT);
    lv_roller_set_options(hour_roller,clock_set_hour_options,LV_ROLLER_MODE_NORMAL);
    /*创建分钟滚动条*/
    lv_obj_t*minute_roller=lv_roller_create(roller_obj);
    lv_obj_set_size(minute_roller,lv_pct(35),lv_pct(100));
    lv_obj_set_style_text_line_space(minute_roller,10,LV_STATE_DEFAULT);
    lv_obj_align_to(minute_roller,hour_roller,LV_ALIGN_OUT_RIGHT_MID,0,0);
    lv_roller_set_options(minute_roller,clock_set_min_options,LV_ROLLER_MODE_NORMAL);
    /*确定创建闹钟按钮*/
   lv_obj_t*btn_ok=ui_widgets_btn_create(roller_obj,LV_SYMBOL_OK);
   lv_obj_align(btn_ok,LV_ALIGN_RIGHT_MID,0,-20);
   /*取消创建按钮*/
   lv_obj_t*btn_cancel=ui_widgets_btn_create(roller_obj,LV_SYMBOL_CLOSE);
   lv_obj_align(btn_cancel,LV_ALIGN_RIGHT_MID,0,20);
    /*铃声的下拉列表*/
    lv_list_add_text(clock_set_list,"Ringtone");
    lv_obj_t*Ringtone_drop=lv_dropdown_create(clock_set_list);
    lv_obj_set_width(Ringtone_drop,lv_pct(100));
    lv_dropdown_set_options(Ringtone_drop,"1\n2");
    /*重复次数的下拉列表*/
    lv_list_add_text(clock_set_list,"Repeat");
    lv_obj_t*Repeat_drop=lv_dropdown_create(clock_set_list);
    lv_obj_set_width(Repeat_drop,lv_pct(100));
    lv_dropdown_set_options(Repeat_drop,"Everyday\nSingle");
    
    lv_obj_add_event_cb(btn_ok,event_add_alarm_cb,LV_EVENT_CLICKED,(void *)"OK");
    lv_obj_add_event_cb(btn_cancel,event_add_alarm_cb,LV_EVENT_CLICKED,(void *)"OFF");
     
}
static void event_add_alarm_cb(lv_event_t*e)
{
       lv_obj_t*target=lv_event_get_target(e);
     lv_obj_t*roller_obj= lv_obj_get_parent(target);
     lv_obj_t*clock_set_list= lv_obj_get_parent(roller_obj); 
     lv_obj_t*obj= lv_obj_get_parent(clock_set_list); 
     lv_obj_t*tab_Clock= lv_obj_get_parent(obj);  
     
      lv_obj_t*hour_roller= lv_obj_get_child(roller_obj,0);
      lv_obj_t*minute_roller=lv_obj_get_child(roller_obj,1);
      lv_obj_t*Ringtone_drop= lv_obj_get_child(clock_set_list,0);
      lv_obj_t*Repeat_drop=lv_obj_get_child(clock_set_list,1);
      
      lv_obj_t*clock_list=lv_obj_get_child(tab_Clock,0);
      if( strcmp( lv_event_get_user_data(e),"OK") ==0)
      {
        alarm_clock.save_alarm_clock_table[alarm_clock.save_alarm_num].hour=lv_roller_get_selected(hour_roller);  
        alarm_clock.save_alarm_clock_table[alarm_clock.save_alarm_num].min=lv_roller_get_selected(minute_roller); 
        alarm_clock.save_alarm_clock_table[alarm_clock.save_alarm_num].repeat=lv_dropdown_get_selected(Ringtone_drop);
        alarm_clock.save_alarm_clock_table[alarm_clock.save_alarm_num].Ringtone=lv_dropdown_get_selected(Repeat_drop);  
        lv_obj_del_async(obj); 
        
        tab_clock_creat(clock_list,&alarm_clock.save_alarm_clock_table[alarm_clock.save_alarm_num]); 
        alarm_clock.save_alarm_num++;
        #if keil
        save_alarm_data();
        #endif // keil
      }
      else
      {
        lv_obj_del_async(obj);
      }
}

static void event_btn_add_clock_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_obj_t*tab_Clock=lv_obj_get_parent(target);
    lv_obj_t*Clock_list=lv_obj_get_child(tab_Clock,0);
    clock_time_create(tab_Clock,ALARM_ADD); 
}
static void del_alarm_and_shift(int16_t index_alarm)
{
    if(index_alarm <0 || index_alarm>alarm_clock.save_alarm_num)
    {
        return;
    }
    for(uint16_t i=index_alarm;i<alarm_clock.save_alarm_num-1;i++)
    {
        alarm_clock.save_alarm_clock_table[i]=alarm_clock.save_alarm_clock_table[i+1];
    }
    alarm_clock.save_alarm_num--;
}
static void event_clock_set_cb(lv_event_t*e)
{
    
    lv_obj_t*target=lv_event_get_target(e);
    lv_obj_t*Clock_list=lv_obj_get_parent(target);
    lv_obj_t*tab_Clock=lv_obj_get_parent(Clock_list);
    lv_event_code_t code = lv_event_get_code(e);
//        if(code == LV_EVENT_CLICKED)
//        {
//         clock_time_create(tab_Clock,ALARM_SETTING); 
//        }
    if(code == LV_EVENT_LONG_PRESSED) 
        {
             lv_obj_del_async(target);  
             uint16_t index_alarm=lv_obj_get_index(target);
             del_alarm_and_shift(index_alarm);   
        }
        if(code==LV_EVENT_VALUE_CHANGED)
        {
            /*状态变化打开或者关闭闹钟*/
            
        }
        #if keil
            save_alarm_data();
        #endif // keil
}

//static void event_clock_obj_cb(lv_event_t*e)
//{
//        lv_obj_t*target=lv_event_get_target(e);
//        lv_event_code_t code=lv_event_get_code(e);
//        if(code==LV_EVENT_CLICKED)
//        {
//            lv_obj_del(target);
//        }
//}
lv_obj_t*tab_clock_creat(lv_obj_t*parent,alarm_clock_set_t *alarm_table)
{
    lv_obj_t*obj=lv_obj_create(parent);
    lv_obj_set_size(obj,lv_pct(100),lv_pct(20));
    lv_obj_set_style_pad_all(obj,0,0);
    lv_obj_set_style_radius(obj,0,0);
    
    lv_obj_t*clock_label=lv_label_create(obj);
    lv_label_set_text_fmt(clock_label,"%02d:%02d",alarm_table->hour,alarm_table->min);
    lv_obj_align(clock_label,LV_ALIGN_LEFT_MID,0,0);
    ui_set_obj_text_font(clock_label,FONT_SIZE_24);

//    lv_obj_t*ringtone_label=lv_label_create(obj);
//    lv_label_set_text_fmt(ringtone_label,"Ring");
//    lv_label_set_text(ringtone_label,"叮当响");
//    lv_obj_align(ringtone_label,LV_ALIGN_CENTER,0,-10);

    
    lv_obj_t*repeat_label=lv_label_create(obj);
    lv_label_set_text_fmt(repeat_label,"每天");
    lv_obj_align(repeat_label,LV_ALIGN_CENTER,0,0);

        
        
    lv_obj_t*clock_switch=lv_switch_create(obj);
    lv_obj_align(clock_switch,LV_ALIGN_RIGHT_MID,0,0);
    
    
    if(alarm_table->alarm_is_on)
    {
      lv_obj_add_state(clock_switch,LV_STATE_CHECKED);
    }
    lv_obj_add_event_cb(obj,event_clock_set_cb,LV_EVENT_LONG_PRESSED,NULL);
//    lv_obj_add_event_cb(obj,event_clock_set_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(clock_switch,event_clock_set_cb,LV_EVENT_VALUE_CHANGED,NULL);
    
    return  obj;
}           


#if keil
/*--------------------------------------------------------------------------------↓
	@函数	  :  读取保存的闹钟
	@参数	  :
	@返回值 :
	@备注	  :
↑--------------------------------------------------------------------------------*/
// 返回最新数据的索引 (0-31)，如果没有数据则返回 0xFF
uint8_t Get_Latest_Alarm_Index(uint32_t Address)
{
    uint8_t flag;
    
    for(uint8_t i = 0; i < (4096 / ALARM_SAVE_BLOT_SIZE); i++)
    {
        // 我们只需要读取每个 Slot 的第一个字节（标志位）即可，没必要读整个结构体，速度更快
        W25Qxx_ReadData(Address + (i * ALARM_SAVE_BLOT_SIZE), &flag, 1);
        
        if(flag == 0xFF) // 发现空位
        { 
            if(i == 0) return 0xFF; // 情况1：第0个就是空的，说明扇区完全没写过
            return i - 1;          // 情况2：找到了空位，那上一个就是最新的
        }
    }
    
    // 情况3：循环走完了都没发现 0xFF，说明 32 个槽位全满了
    return (4096 / ALARM_SAVE_BLOT_SIZE) - 1; 
}


/*--------------------------------------------------------------------------------↓
	@函数	  :
	@参数	  :
	@返回值 :
	@备注	  :
↑--------------------------------------------------------------------------------*/

void Alarm_System_Init(void)
{
  uint8_t LastIndex=Get_Latest_Alarm_Index(ALARM_CLOCK_SAVE_Addr);
  if(LastIndex==0XFF)
  {
    /*没有闹钟*/
    print("没有闹钟\r\n");
  }
  else
  {
    W25Qxx_ReadData(ALARM_CLOCK_SAVE_Addr+LastIndex*ALARM_SAVE_BLOT_SIZE,(uint8_t*)&alarm_clock,sizeof(alarm_clock_t));
    if(alarm_clock.save_flag==ALARM_SAVE_FLAG)
    {
      /*读取成功*/
      print("读取成功\r\n");
    }
    else
    {
     alarm_clock.save_alarm_num=0;
     print("读取闹钟失败\r\n");
    }
  }
}


/*--------------------------------------------------------------------------------↓
	@函数	  : 保存闹钟
	@参数	  :
	@返回值 :
	@备注	  :
↑--------------------------------------------------------------------------------*/
void save_alarm_data(void)
{
    uint8_t LastIndex=Get_Latest_Alarm_Index(ALARM_CLOCK_SAVE_Addr);
    uint8_t Target_index=0;
    if(LastIndex==0XFF)
    {
      /*直接在第一个位置写就行*/
      Target_index=0;
      print("直接在第一个位置写就行\r\n");
    }
    else if(LastIndex==(4096/ALARM_SAVE_BLOT_SIZE)-1)
    {
      /*说明写满了,擦除扇区从头写*/
      W25Qxx_SectorErase(ALARM_CLOCK_SAVE_Addr);
      Target_index=0;
      print("说明写满了,擦除扇区从头写\r\n");
    }
    else
    {
      /*还有空间可以直接写*/
      Target_index=LastIndex+1;
      print("还有空间可以直接写\r\n");
    }
    alarm_clock.save_flag=ALARM_SAVE_FLAG;
    W25Qxx_WriteBuffer(ALARM_CLOCK_SAVE_Addr+Target_index*ALARM_SAVE_BLOT_SIZE,(const uint8_t*)&alarm_clock,sizeof(alarm_clock_t));
    
}

#endif