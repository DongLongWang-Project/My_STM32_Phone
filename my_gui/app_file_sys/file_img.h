#ifndef __FILE_IMG_H
#define __FILE_IMG_H
#include "lvgl.h"
#include "../ui_widgets.h"

typedef struct {
	lv_obj_t *image;
        lv_obj_t *slider;
	lv_obj_t *btn_dir;
}ui_img_t;
void operation_file_img(lv_obj_t*parent,const char*path);
#endif

