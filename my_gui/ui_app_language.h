#ifndef     __UI_APP_LANGUAGE_H
#define    __UI_APP_LANGUAGE_H

#include "lvgl.h"
#include "app_setting/ui_app_setting_config.h"
#include "app_clock/ui_app_clock_config.h"


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
extern const ui_setting_language_t TIME_OBJ__LA_TABLE[TIME_OBJ_MAX];
extern const ui_setting_language_t WIFI_LA_TABLE[WIFI_LABEL_MAX];

#endif
