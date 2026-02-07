#ifndef     __UI_WIDGETS_H
#define    __UI_WIDGETS_H
#include "lvgl.h"
#include "ui_main.h"
#include "ui_app_language.h"

#define scr_width   lv_obj_get_width(lv_scr_act())
#define scr_height  lv_obj_get_height(lv_scr_act())

lv_obj_t* ui_create_app(lv_obj_t *parent,const void *icon, const char *text);
lv_obj_t* ui_creat_control_bar(lv_obj_t *parent);
void ui_control_bar_toggle(lv_obj_t *parent);

void ui_creat_keyboard(void);
void ui_creat_statebar(void);
lv_obj_t* creat_statebar_icon(const char*text);


extern lv_obj_t*label_time,*label_wifi,*state_bar;
extern lv_obj_t*keyboard;
extern lv_obj_t*alarm_msgbox_obj;
extern void event_textarea_cb(lv_event_t*e);
void alarm_rem_win(lv_obj_t* parent);

lv_obj_t* ui_widgets_btn_create(lv_obj_t*parent,const char*btn_text);
#endif // __UI_WIDGETS_H
