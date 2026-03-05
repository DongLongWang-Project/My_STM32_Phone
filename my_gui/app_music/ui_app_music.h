#ifndef __UI_APP_MUSIC_H
#define __UI_APP_MUSIC_H
#include "lvgl.h"

#define SD_MUSIC_PATH "0:/SD/music"
lv_obj_t* ui_app_music_list_creat(lv_obj_t*parent);
void ui_app_music_detail_creat(lv_obj_t*parent,const char*path);


#endif

