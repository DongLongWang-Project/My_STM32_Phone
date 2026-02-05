#ifndef     __UI_APP_LANGUAGE_H
#define    __UI_APP_LANGUAGE_H

#include "lvgl.h"
#include "app_setting/ui_app_setting_config.h"



typedef struct{
const void *icon;
const char *text[LANGUAGE_NUM];
}ui_setting_language_t;


#define _GET_UI_TEXT(TABLE,ID)           (TABLE[ID].text[display_cfg.language])
#define _GET_UI_ICON(TABLE,ID)          (TABLE[ID].icon)

extern const ui_setting_language_t APP_SET_ABOUT_LA_TABLE[];
extern const ui_setting_language_t APP_SET_LA_TABLE[];
extern const ui_setting_language_t APP_SET_DISPLAY_LA_TABLE[];
extern const ui_setting_language_t DISPLAY_SCR_MSG_LA_TABLE[];
extern const ui_setting_language_t HOTSPOT_LV_TABLE[];
extern const ui_setting_language_t ALARM_MSGBOX_LA_TABLE[ALARM_MSGBOX_NUM];
#endif
