#ifndef __UI_APP_MUSIC_H
#define __UI_APP_MUSIC_H
#include "lvgl.h"
#include "ui_app_setting.h"


#if keil
#define SD_MUSIC_PATH "0:/SD/music"

#else
#define SD_MUSIC_PATH "0:/GitHub_Code/My_STM32_Phone/SD/music"
#endif //

lv_obj_t* ui_app_music_list_creat(lv_obj_t*parent);
void ui_app_music_detail_creat(lv_obj_t*parent,const char*path);


#endif

