#ifndef __UI_APP_VIDEO_H
#define __UI_APP_VIDEO_H

#include "lvgl.h"
#include "../app_setting/ui_app_setting.h"

lv_obj_t* ui_app_video_list_creat(lv_obj_t*parent);
void ui_app_video_detail_creat(lv_obj_t*parent,const char*path);

#if keil
#define SD_VIDEO_PATH "0:/SD/video"
#else
#define SD_VIDEO_PATH "0:/GitHub_Code/My_STM32_Phone/SD/video"
#endif // 

typedef struct {
  lv_fs_file_t file;
  lv_obj_t *obj_video;
  lv_timer_t *timer; // 保存定时器指针，方便删除
  bool is_playing;
}video_control_t;

typedef struct {
  uint16_t hour;
  uint8_t minute;
  uint8_t second;
  uint32_t total_sec;
  uint32_t view_cur_time;
  uint16_t cur_hour;
  uint8_t cur_minute;
  uint8_t cur_second;
}video_time_t;

extern video_control_t Video_win;
extern video_time_t video_time;
extern uint8_t video_buf[];


#endif
