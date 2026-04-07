#ifndef __SETTING_ABOUT_H
#define __SETTING_ABOUT_H


#include "../ui_app_language.h"
#include "stdio.h"
#include "ui_app_setting.h"
#include "../ui_widgets.h"


#define APP_HEAD_Addr 0x08010000
#define APP_Addr 0x08010200

#if keil
#include "flash.h"
#include "W25Qxx.h"
#include "crc.h"
#define UPDATE_FILE_PATH  "0:/SD/bin/myPhone.bin"

#else
#define UPDATE_FILE_PATH "0:/GitHub_Code/My_STM32_Phone/SD/bin/myPhone.bin"
#endif
typedef enum
{
  has_no_new=0,
  has_sd_new,
  has_git_new,
}update_is_ready_t;

typedef enum
{
    update_none=0xFF,
    update_is_running=0xFE,
    update_is_ok=0xFC,
}update_state_t;
typedef struct 
{
    uint32_t version;
    uint32_t crc32;
    uint32_t file_size;
    char name[16];
    update_state_t update_state;  
    uint8_t reserved[512-32];     
}head_t;
typedef enum
{  
   HEAD_SD=0,
   HEAD_FLASH,
   HEAD_GitHUB,
   HEAD_W25Q_Pre,
   HEAD_NUM
}head_enum;
typedef struct
{
  uint32_t update_flag;
  uint32_t file_version;
  uint32_t file_crc;
  uint32_t file_size;
}update_flag_info_t;
typedef struct
{
    lv_obj_t*obj_update;
    lv_obj_t*label_name;
    lv_obj_t*progress_update_bar;
    lv_obj_t*new_version_label;
    lv_timer_t*timer;
}update_obj_t;

typedef struct
{
lv_fs_file_t  file_p;
head_t head[HEAD_NUM];

update_obj_t update_obj;
}ui_setting_update_t;

extern update_flag_info_t update_flag_info;
extern ui_setting_update_t ui_setting_update;
void ui_app_setting_about(lv_obj_t*parent);
extern update_is_ready_t update_is_ready;
uint8_t update_is_valid(head_enum head_);
void download_update_timer(lv_timer_t*t);
#endif
