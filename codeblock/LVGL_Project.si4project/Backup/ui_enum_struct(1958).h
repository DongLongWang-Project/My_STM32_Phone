#ifndef __UI_ENUM_STRUCT_H
#define __UI_ENUM_STRUCT_H

typedef enum {
    File_Unknow = 0,
    File_TXT,
    File_C,
    File_H,
    File_PNG
} FILE_TYPE_ENUM;

typedef struct {
	lv_obj_t *image;
    lv_obj_t *slider;
	lv_obj_t *btn_dir;
}ui_img_t;

typedef struct {
    lv_obj_t *Text;
	ui_img_t img;
    FILE_TYPE_ENUM type;    // 文件类型
} ui_app_file_t;

typedef struct{
	lv_obj_t*btn_WIFI;
	lv_obj_t*btn_BlueTooch;
}ui_app_setting_list_t;

typedef enum
{
	SETTING_NULL=0,
	WLAN,
	BlueTooch,
}UI_APP_SETTING_LIST_ENUM;

/******************************************************
页面枚举
******************************************************/
typedef enum {
    PAGE_HOME,/** 子主界面**/
    PAGE_APP_LIST,/**app列表界面 **/
    PAGE_APP_DETAIL,/**app详情界面 **/
} UI_APP_ENUM;

/******************************************************
app按钮枚举
******************************************************/
typedef enum{
	Home=0,
    btn_list_setting,/**设置 **/
	btn_list_File,
 }UI_APP_BTN_ENUM;

 typedef struct {
    lv_font_t *FONT_SIZE_12;
    lv_font_t *FONT_SIZE_16;
    lv_font_t *FONT_SIZE_24;
    lv_font_t *FONT_SIZE_32;
}my_font_t;

#endif // __UI_ENUM_STRUCT_H
