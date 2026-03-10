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
    File_WAV
} FILE_TYPE_ENUM;

typedef struct
{
    lv_obj_t*list;
    lv_fs_res_t res;/*文件打开状态*/
    lv_fs_dir_t dir; /*文件夹*/
    lv_obj_t *btn;   /*按钮*/
    lv_timer_t*timer;
}file_dir_t;

lv_obj_t* ui_app_file_list_create(lv_obj_t *parent,const char * path);
lv_obj_t* ui_app_file_detail_create(lv_obj_t *parent, const char *file_path);


 
 
extern char Cure_Path[128];
extern file_dir_t file_dir;
#endif // __UI_APP_FILE_H
