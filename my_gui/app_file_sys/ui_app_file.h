#ifndef     __UI_APP_FILE_H
#define     __UI_APP_FILE_H
#include <lvgl.h>
#include <stdio.h>
#include "../ui_widgets.h"
#include "file_txt.h"
#include "file_img.h"

 /******************************************
FILE
*****************************************/
typedef enum {
    File_Unknow = 0,
    File_TXT,
    File_C,
    File_H,
    File_PNG,
    File_RGB,
    File_WAV,
    FILE_BIN,
} FILE_TYPE_ENUM;

#define SHOW_FILE_MAX_NUM      8
#define FILE_NAME_MAX_LEN 64
#define SAVE_FILE_NAME_NUM 100
#define Dir_MAX_LEN     256
#if keil
#define SD_PATH "0:"
#else
#define SD_PATH "0:/GitHub_Code/My_STM32_Phone/SD"
#endif // keil
typedef struct
{
    lv_obj_t*list;
    lv_fs_dir_t dir; /*文件夹*/
    lv_obj_t *btn;   /*按钮*/
    lv_timer_t*timer;
}file_dir_t;
typedef struct
{
    lv_obj_t*obj;
    lv_obj_t*pre_page_btn; 
    lv_obj_t*next_page_btn; 
    lv_obj_t*page_label; 
    uint8_t cur_page;
    uint8_t total_file_num; 
    uint8_t total_page_num;
    lv_timer_t*timer;
    bool creat_enable;
}file_switch_page_t;


lv_obj_t* ui_app_file_list_create(lv_obj_t *parent,const char * path);
lv_obj_t* ui_app_file_detail_create(lv_obj_t *parent, const char *file_path);

void load_dir_timer(lv_timer_t *timer);
 
extern char Cure_Path[Dir_MAX_LEN];
extern file_dir_t file_dir;
extern file_switch_page_t file_switch_page;
extern char name_buf[SAVE_FILE_NAME_NUM][FILE_NAME_MAX_LEN];
extern char FILE_BUF[2][FILE_NAME_MAX_LEN];
#endif // __UI_APP_FILE_H
