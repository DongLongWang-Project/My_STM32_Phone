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


#define MAX_LRC_LINE_NUM 100
typedef struct
{
  uint32_t cur_ms;
  lv_obj_t *lrc_label;
  char lrc_label_text[64];
}lrc_line_t;
typedef struct
{
  lv_obj_t *lrc_obj;
  lrc_line_t lrc_line[MAX_LRC_LINE_NUM];
  uint8_t total_lcr_line;
  char lrc_name[64];
}lrc_obj_t;

lv_obj_t* ui_app_music_list_creat(lv_obj_t*parent);
void ui_app_music_detail_creat(lv_obj_t*parent,const char*path);
void music_progress_timer_cb(lv_timer_t*t);
void load_lrc_file(lv_obj_t*parent,const char*path,char*lrc_buffer);
lv_obj_t* lrc_obj_create(lv_obj_t*parent);
#endif

