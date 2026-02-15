#ifndef __UI_APP_SETTING_CONFIG_H
#define __UI_APP_SETTING_CONFIG_H

#include "lvgl.h"

typedef enum{
ENGLISH=0,
CHINESE,
LANGUAGE_NUM,
}DISPLAY_LANGUAGE_ENUM;

typedef enum{
    APP_SET_ABOUT_TITLE=0,
    APP_SET_ABOUT_DEVICE,
    APP_SET_ABOUT_MCU,
    APP_SET_ABOUT_RAM,
    APP_SET_ABOUT_ROM,
    APP_SET_ABOUT_SD,
    APP_SET_ABOUT_SCREEN,
    APP_SET_ABOUT_SYSTEM,
    APP_SET_ABOUT_GUI,
    APP_SET_ABOUT_NUM,
}UI_APP_SET_ABOUT_ID_ENUM;

typedef enum{
    APP_SETTING_TITLE,
    APP_SETTING_WLAN,
    APP_SETTING_DISPLAY,
    APP_SETTING_HOTSPOT,
    APP_SETTING_ABOUT,
    APP_SETTING_NUM,
}UI_APP_SETTING_ENUM;



typedef enum{
    DISPLAY_TITLE,
    DISPLAY_LANGUAGE_TITLE,
    DISPLAY_LANGUAGE_CUR,
    
    DISPLAY_WORD_TITLE,
    DISPLAY_WORD_CUR,
    
    DISPLAY_SCR_TITLE,
    DISPLAY_SCR_CUR,
    

 
    DISPLAY_BRIGHT_TITLE,
    DISPLAY_BRIGHT_CUR,
    
    SETTING_DISPLAY_NUM,
}SETTING_DISPLAY_ENUM;



typedef enum{
MSG_TITLE=0,
MSG_BTN_CONTINUE,
MSG_BTN_CANCEL,
MSG_TEXT,
MSG_NUM,
}MSG_ENUM;
typedef enum{
FONT_SIZE_12=0,
FONT_SIZE_16,
FONT_SIZE_24,
FONT_SIZE_32,
}DISPLAY_FONT_SIZE_ENUM;
typedef enum{
VER=0,
REV_VER,
HOR,
REV_HOR,
}DISPLAY_SCR_DIR_ENUM;
typedef enum{
LOW_FPS=0,
MID_FPS,
HIGH_FPS,
}DISPLAY_FPS_ENUM;

typedef enum{
 HOTSPOT_TITLE=0,
 HOTSPOT_NAME,
 HOTSPOT_PASS,
 HOTSPOT_CHANNEL,
 HOTSPOT_METHOD,
 HOTSPOT_USERS,
 HOTSPOT_IP,  
 HOTSPOT_NUM, 
}HOTSPOT_ENUM;

typedef enum{
 ALARM_MSGBOX_BTN_OK,
ALARM_MSGBOX_BTN_WAIT,
ALARM_MSGBOX_TITLE,
ALARM_MSGBOX_LABEL,
ALARM_MSGBOX_NUM,
}ALARM_MSGBOX_ENUM;


typedef struct {
    DISPLAY_LANGUAGE_ENUM           language;
    DISPLAY_FONT_SIZE_ENUM            font_size;
    DISPLAY_SCR_DIR_ENUM                 scr_dir;
    uint8_t                                             Brightness;
    bool                                                 wifi_switch_state;
    bool                                                hotspot_switch_state;
}ui_app_setting_config_t;

extern  ui_app_setting_config_t display_cfg;
extern UI_APP_SETTING_ENUM Cure_Mode;
#endif

