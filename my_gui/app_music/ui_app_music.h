#ifndef __UI_APP_MUSIC_H
#define __UI_APP_MUSIC_H
#include "lvgl.h"
#include "../app_setting/ui_app_setting.h"
#include "../ui_widgets.h"



#if keil
#define SD_MUSIC_PATH "0:/SD/music"
#define MUSIC_RECORD_PATH "0:/SD/my_icon/唱片.bin"
#else
#define SD_MUSIC_PATH "0:/GitHub_Code/My_STM32_Phone/SD/music"
#define MUSIC_RECORD_PATH "0:/GitHub_Code/My_STM32_Phone/SD/my_icon/唱片.bin"
#endif //

lv_obj_t* ui_app_music_list_creat(lv_obj_t*parent);
void ui_app_music_detail_creat(lv_obj_t*parent,const char*path);

typedef struct
{
    lv_obj_t*record_icon;
    lv_fs_file_t file;
    lv_anim_t record_anim;
    lv_obj_t*pause_btn;
    lv_obj_t*next_btn;
    lv_obj_t*pre_btn;
    lv_obj_t*music_record;
    lv_obj_t*music_label;
}music_player_t;
#endif

