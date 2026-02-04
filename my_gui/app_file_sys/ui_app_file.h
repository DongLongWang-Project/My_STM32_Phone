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
    File_PNG
} FILE_TYPE_ENUM;


lv_obj_t* ui_app_file_list_create(lv_obj_t *parent,const char * path);
lv_obj_t* ui_app_file_detail_create(lv_obj_t *parent, const char *file_path);


 
 
extern char Cure_Path[128];

#endif // __UI_APP_FILE_H
