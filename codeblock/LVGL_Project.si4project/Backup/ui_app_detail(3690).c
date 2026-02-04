#include "ui_app_detail.h"

static void event_slider_cb(lv_event_t *e);

/******************************************************
函数:app详情界面
参数:父对象
返回:详情页
注意:里面可自行设置,只需返回对应的主界面就行
******************************************************/
lv_obj_t* page_detail_create(lv_obj_t *parent,UI_APP_SETTING_LIST_ENUM setting_mode)
{
	lv_obj_t *page = lv_obj_create(parent);

	switch(setting_mode)
		{
			case WLAN:
				{
				lv_obj_t *label = lv_label_create(page);
				lv_label_set_text(label, "wwwwwwww");
				lv_obj_center(label);
				}
		}
    return page;
}


static FILE_TYPE_ENUM get_file_type(const char *path)
{
    const char *ext = strrchr(path, '.');
    if(!ext) return File_Unknow;
    if(strcmp(ext, ".txt") == 0) return File_TXT;
    if(strcmp(ext, ".c")   == 0) return File_C;
    if(strcmp(ext, ".h")   == 0) return File_H;
    if(strcmp(ext, ".png") == 0) return File_PNG;
    return File_Unknow;
}
lv_obj_t* page_detail_file_create(lv_obj_t *parent, const char *file_path)
{
    ui_app_file_t ctx ;
    memset(&ctx, 0, sizeof(ui_app_file_t));
    ctx.type = get_file_type(file_path);
    // 2. 创建页面对象
    lv_obj_t*page = lv_obj_create(parent);
    lv_obj_set_size(page, scr_width, scr_height);

    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, Cure_Path, LV_FS_MODE_RD);
	lv_obj_clear_flag (page, LV_OBJ_FLAG_SCROLLABLE);

    // 5. 根据文件类型创建控件
    switch(ctx.type) {
        case File_TXT:
        case File_C:
        case File_H: {
            // 文本文件
            ctx.Text= lv_textarea_create(page);
            lv_obj_set_size(ctx.Text, 200, scr_height);
            lv_obj_center(ctx.Text);
            lv_obj_set_style_text_font(ctx.Text, my_fonts.FONT_SIZE_12, LV_STATE_DEFAULT);
			lv_obj_set_style_shadow_opa (ctx.Text, 0, LV_STATE_DEFAULT);
			lv_obj_set_style_border_width(ctx.Text,0,LV_PART_MAIN);/**边框宽度0 **/

            if(res == LV_FS_RES_OK) {
                uint8_t buf[256];
                uint32_t cnt;
                while(1) {
                    res = lv_fs_read(&file, buf, sizeof(buf)-1, &cnt);
                    if(res != LV_FS_RES_OK || cnt == 0) break;
                    buf[cnt] = '\0';
                    lv_textarea_add_text(ctx.Text, buf);
                    if(cnt < sizeof(buf)-1) break;
                }
            }
            break;
        }
        case File_PNG: {
            // 图片文件
            ctx.img.image = lv_img_create(page);
            lv_img_set_src(ctx.img.image, Cure_Path);
            lv_obj_align(ctx.img.image, LV_ALIGN_CENTER, 0, 0);
			lv_img_set_zoom(ctx.img.image,64);
            // slider
            ctx.img.slider = lv_slider_create(page);
            lv_obj_align(ctx.img.slider, LV_ALIGN_TOP_MID, 0, scr_height*3/4);
            lv_slider_set_range(ctx.img.slider, 1, 70);
            lv_obj_add_event_cb(ctx.img.slider, event_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);
			lv_obj_set_width (ctx.img.slider, scr_width/2);
			//btn
			ctx.img.btn_dir=lv_btn_create (page);
			lv_obj_align_to (ctx.img.btn_dir, ctx.img.slider, LV_ALIGN_OUT_LEFT_MID, -20, 0);

            break;
        }
        default:
            ctx.Text = lv_textarea_create(page);
            lv_obj_set_size(ctx.Text, 200, scr_height);
            lv_obj_center(ctx.Text);
            lv_obj_set_style_text_font(ctx.Text, my_fonts.FONT_SIZE_24, LV_STATE_DEFAULT);
			lv_obj_set_style_shadow_opa (ctx.Text, 0, LV_STATE_DEFAULT);
			lv_obj_set_style_border_width(ctx.Text,0,LV_PART_MAIN);/**边框宽度0 **/

			lv_textarea_add_text (ctx.Text, "目前还不能读此文件,可读(txt,c,h,png)");

            // 未知文件类型，可显示提示
            break;
    }
   lv_fs_close(&file);

    return page;
}

/*****************************************************************************************************

回调函数

*****************************************************************************************************/

static void event_slider_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *img = lv_obj_get_parent(slider);
    lv_obj_t *image = lv_obj_get_child(img, 0);

    if (!image) return;

    int32_t value = lv_slider_get_value(slider);
    lv_img_set_zoom(image, 6 * value);
}


