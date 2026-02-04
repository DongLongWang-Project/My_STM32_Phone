/*--------------------------------------------------------------------------------↓
	@项目	: 图像文件
	@日期	: 2026-1-25
	@备注	:
↑--------------------------------------------------------------------------------*/
#include "file_img.h"

static void event_img_slider_cb(lv_event_t *e);
 
 
 /*--------------------------------------------------------------------------------↓
 	@函数	  : 操作图像文件
 	@参数	  :  父对象,路径
 	@返回值 : 无
 	@备注	  :
 ↑--------------------------------------------------------------------------------*/
void operation_file_img(lv_obj_t*parent,const char*path)
{           
        ui_img_t file_img;/*图像文件*/
        lv_fs_file_t file;
        lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_RD); /*打开文件*/
        
        file_img.image = lv_img_create(parent);/*创建图像*/
        lv_img_set_src(file_img.image, path);/*根据路径添加图像*/
        lv_obj_align(file_img.image, LV_ALIGN_CENTER, 0, 0);
        lv_img_set_zoom(file_img.image,64);/*设置缩放*/
        // slider
        file_img.slider = lv_slider_create(parent); /*图像大小滑动条*/
        
        lv_obj_align(file_img.slider, LV_ALIGN_TOP_MID, 0, scr_height*3/4);
        
        lv_slider_set_range(file_img.slider, 1, 70); /*缩放范围*/
        lv_obj_add_event_cb(file_img.slider, event_img_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);/*添加缩放事件*/
        lv_obj_set_width (file_img.slider, scr_width/2);
        //btn
        file_img.btn_dir=lv_btn_create (parent);/*按钮*/
        lv_obj_align_to (file_img.btn_dir, file_img.slider, LV_ALIGN_OUT_LEFT_MID, -20, 0);/*添加按钮事件*/
        
        lv_fs_close(&file);/*关闭文件*/
}


/*--------------------------------------------------------------------------------↓
	@函数	  :  图像滑动条的回调事件
	@参数	  :  无
	@返回值 :  无
	@备注	  :
↑--------------------------------------------------------------------------------*/
 static void event_img_slider_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);/*滑动条*/
    lv_obj_t *file_img = lv_obj_get_parent(slider);
    lv_obj_t *image = lv_obj_get_child(file_img, 0);/*得到图片*/

    if (!image) return;

    int32_t value = lv_slider_get_value(slider); /*获得滑动条的值*/
    lv_img_set_zoom(image, 6 * value);/*改变图片缩放大小*/
}
