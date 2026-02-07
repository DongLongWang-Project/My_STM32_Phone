#include "ui_widgets.h"


/**矩阵按钮样式 数组**/
static const char*btnm_map[]={LV_SYMBOL_HOME,LV_SYMBOL_LEFT,""};

lv_obj_t*label_time,*label_wifi=NULL,*state_bar;
lv_obj_t*keyboard;
lv_obj_t*alarm_msgbox_obj;

 void event_control_bar_cb(lv_event_t *e);
/**
 * @brief   创建APP按钮
 * @param 父对象,对应图标,app文本
 * @return 创建的app按钮
 *
 *  注意:app是由3个基本部件组成的
        1按钮
        2文本
        3图标(LVGL内置图标
 */
lv_obj_t* ui_create_app(lv_obj_t *parent,const void *icon, const char *text)
{
    /**创建按钮 **/
    lv_obj_t*btn=lv_btn_create(parent);
    lv_obj_set_style_bg_opa(btn,25,LV_STATE_DEFAULT);/**设置透明度(按下有反应 **/
    lv_obj_set_style_bg_color(btn,lv_color_hex(0xFEFEFE),LV_STATE_DEFAULT);/**设置背景色(默认状态 **/
    lv_obj_set_style_bg_color(btn,lv_color_hex(0x696969),LV_STATE_PRESSED);/**设置按下的背景色(搭配透明度 **/

    lv_obj_set_size(btn,60,60);/**设置尺寸 **/
    lv_obj_set_style_shadow_width(btn,0,LV_STATE_DEFAULT);/**去除按钮阴影 **/
    lv_obj_set_style_border_width(btn,0,LV_STATE_DEFAULT);/**设置按钮边框厚度为0 **/
    /**创建图标 **/
    lv_obj_t * img=lv_img_create(btn);
    lv_img_set_src(img,icon);
    lv_obj_set_style_text_font(img,&lv_font_montserrat_32,LV_STATE_DEFAULT);/**设置大小 **/
    lv_obj_set_style_text_color(img,lv_color_hex(0x000000),LV_STATE_DEFAULT);/**设置颜色 **/
    /**创建文本 **/
    lv_obj_t *label=lv_label_create(btn);
    lv_label_set_text(label,text);/**设置文本 **/
    lv_obj_set_style_text_color(label,lv_color_hex(0x000000),LV_STATE_DEFAULT);/**设置字体颜色 **/

    lv_obj_align(img,LV_ALIGN_TOP_MID,0,-10);/**设置图标位置 **/
    lv_obj_align_to(label,img,LV_ALIGN_BOTTOM_MID,0,15);/**设置文本相对于图标的位置 **/

    return btn;

}
/******************************************************
函数:创建导航栏
参数:父对象
返回:无
注意:导航栏有三个按钮
2:返回按钮
1:主界面按钮
******************************************************/
lv_obj_t* ui_creat_control_bar(lv_obj_t *parent)
{
    /**创建按钮 **/
    lv_obj_t*btnmatrix=lv_btnmatrix_create(parent);
    lv_btnmatrix_set_map(btnmatrix,btnm_map);/**将数组填入,生成按钮 **/
    lv_btnmatrix_set_btn_width(btnmatrix,1,1);/**按钮相对宽度 **/
    lv_obj_set_size(btnmatrix,scr_width,scr_height/7);/**矩阵按钮的宽高 宽:屏幕宽度 高:屏幕高度/6**/
    lv_obj_align(btnmatrix,LV_ALIGN_BOTTOM_MID,0,0);/**父对象中间的底部 **/
    
    lv_obj_set_size(btnmatrix,lv_pct(100),lv_pct(ControlBar_Percent));
    lv_obj_set_style_pad_all(btnmatrix, 0, 0);

    /**优化外观 **/
    lv_obj_set_style_border_width(btnmatrix,0,LV_PART_MAIN);/**边框宽度0 **/
    lv_obj_set_style_bg_opa(btnmatrix,0,LV_PART_ITEMS);/**背景透明度0(按钮) **/
    lv_obj_set_style_bg_opa(btnmatrix,25,LV_PART_ITEMS|LV_STATE_PRESSED);/**按下后的透明度(按钮 **/
    lv_obj_set_style_shadow_width(btnmatrix,0,LV_PART_ITEMS);/**按钮边框宽度 **/

	lv_obj_set_style_bg_opa(btnmatrix,15,LV_PART_MAIN);/**背景透明度0(按钮) **/

	lv_obj_set_style_text_color (btnmatrix, lv_color_hex (0x000000), LV_PART_ITEMS);
	lv_obj_set_style_text_opa (btnmatrix, 200, LV_PART_ITEMS);

//    lv_btnmatrix_set_btn_ctrl_all(btnmatrix,LV_BTNMATRIX_CTRL_RECOLOR);/**可重色 **/
    /**事件 **/
    lv_obj_add_event_cb(btnmatrix,event_control_bar_cb,LV_EVENT_CLICKED,NULL);/** **/
    
    /*取消焦点*/
    lv_group_remove_obj(btnmatrix);
    return btnmatrix;
}

 void event_control_bar_cb(lv_event_t *e)
{
    uint16_t id = lv_btnmatrix_get_selected_btn(lv_event_get_target(e));
    lv_obj_add_flag(keyboard,LV_OBJ_FLAG_HIDDEN);
    if(id == 1)/**如果返回按钮按下 **/
		{
      btn_back();
    }
	else if(id ==0 )
		{
      ui_goto_page(PAGE_HOME,APP_TEMP);
		}
}

void ui_control_bar_toggle(lv_obj_t *parent)
{
  static lv_obj_t*bar=NULL;
  if(bar==NULL)
  {
   bar=ui_creat_control_bar(parent); 
  }
  else
  {
    if(lv_obj_has_flag(bar,LV_OBJ_FLAG_HIDDEN))
    {
       lv_obj_clear_flag(bar,LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
      lv_obj_add_flag(bar,LV_OBJ_FLAG_HIDDEN);
    }
  }
}

static void event_keyboard_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code=lv_event_get_code(e);
    if(code==LV_EVENT_VALUE_CHANGED)
    {
       const char *txt=lv_keyboard_get_btn_text(target,lv_keyboard_get_selected_btn(target)); 
       if(strcmp(txt,LV_SYMBOL_KEYBOARD)==0)
       {
           if(lv_keyboard_get_mode(target)==LV_KEYBOARD_MODE_NUMBER)
           {
               lv_keyboard_set_mode(target,LV_KEYBOARD_MODE_TEXT_LOWER);
           }
           else
           {
               lv_keyboard_set_mode(target,LV_KEYBOARD_MODE_NUMBER);
           }
       }
       else  if(strcmp(txt,LV_SYMBOL_OK)==0)
       {
           lv_keyboard_set_textarea(target,NULL);
           lv_obj_add_flag(target,LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(NULL);
       }
    }
    
}
void ui_creat_keyboard(void)
{
    keyboard=lv_keyboard_create(lv_scr_act());
    lv_keyboard_set_popovers(keyboard,true);
    lv_obj_add_event_cb(keyboard,event_keyboard_cb,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_flag(keyboard,LV_OBJ_FLAG_HIDDEN);
        
}
void ui_creat_statebar(void)
{
    state_bar=lv_obj_create(lv_scr_act());
    lv_obj_set_size(state_bar,lv_pct(100),lv_pct(StateBar_Percent));
    lv_obj_align(state_bar,LV_ALIGN_TOP_MID,0,0);
    lv_obj_set_style_pad_all(state_bar, 0, 0);
    lv_obj_set_style_border_width(state_bar, 0, 0);
     lv_obj_set_style_radius(state_bar, 0, 0);
     
    lv_obj_set_scrollbar_mode(state_bar, LV_SCROLLBAR_MODE_OFF); 
    lv_obj_set_style_bg_color(state_bar,lv_color_hex(0x000000),LV_STATE_DEFAULT);
  
    lv_obj_set_flex_flow(state_bar,LV_FLEX_FLOW_ROW);
}

lv_obj_t* creat_statebar_icon(const char*text)
{
    lv_obj_t*label=lv_label_create(state_bar);
    lv_label_set_text(label,text);
    lv_obj_set_style_text_color(label,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT); 
    return label;
}


 void event_textarea_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code= lv_event_get_code(e);
   lv_obj_t*win= lv_obj_get_parent(target);
   
    if(code==LV_EVENT_CLICKED)
    {
        lv_keyboard_set_textarea(keyboard,target);
        lv_obj_clear_flag(keyboard,LV_OBJ_FLAG_HIDDEN);
    }
}

static void event_alarm_msgbox_cb(lv_event_t*e);

void alarm_rem_win(lv_obj_t* parent)
{
   static const char*btns[3];
   btns[0]=_GET_UI_TEXT(ALARM_MSGBOX_LA_TABLE,ALARM_MSGBOX_BTN_OK);
   btns[1]=_GET_UI_TEXT(ALARM_MSGBOX_LA_TABLE,ALARM_MSGBOX_BTN_WAIT); 
   btns[2]="";
    // 1. 创建窗口
    alarm_msgbox_obj=lv_obj_create(parent);
     lv_obj_set_size(alarm_msgbox_obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(alarm_msgbox_obj,0,0);
    
   lv_obj_t*msgbox=lv_msgbox_create(alarm_msgbox_obj,_GET_UI_TEXT(ALARM_MSGBOX_LA_TABLE,ALARM_MSGBOX_TITLE),_GET_UI_TEXT(ALARM_MSGBOX_LA_TABLE,ALARM_MSGBOX_LABEL),btns,false);
    lv_obj_add_flag(alarm_msgbox_obj,LV_OBJ_FLAG_HIDDEN);
   lv_obj_center(msgbox); 
   lv_obj_add_event_cb(msgbox,event_alarm_msgbox_cb,LV_EVENT_CLICKED,NULL);
}

static void event_alarm_msgbox_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_current_target(e);
    lv_obj_t*obj=lv_obj_get_parent(target);
    uint8_t index=lv_msgbox_get_active_btn(target);
    printf("index:%d\r\n",index); 
    if(index==0)
    {
       /*关闭闹铃*/
          lv_obj_add_flag(obj,LV_OBJ_FLAG_HIDDEN);
       printf("关闭闹铃\r\n"); 
    }
    else
    {
        /*延时闹铃*/
           lv_obj_add_flag(obj,LV_OBJ_FLAG_HIDDEN);
          printf("延时闹铃\r\n"); 
    }
}

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

