#ifndef __UI_ENUM_STRUCT_H
#define __UI_ENUM_STRUCT_H

typedef enum {
    FILE_UNKNOW = 0,
    FILE_TXT,
    FILE_C,
    FILE_H,
    FILE_PNG
} file_type_t;

typedef struct {
	lv_obj_t *image;
    lv_obj_t *slider;
	lv_obj_t *btn_dir;
}ui_img_compose;

typedef struct {
    lv_obj_t *Text;
	ui_img_compose img;
    file_type_t type;    // 文件类型
} ui_app_file_t;

typedef struct{
	lv_obj_t*btn_WIFI;
	lv_obj_t*btn_BlueTooch;
}ui_app_setting_t;

typedef enum
{
	NO_state=0,
	WLAN,
	BlueTooch,
}ui_app_setting_content;

/******************************************************
页面枚举
******************************************************/
typedef enum {
    PAGE_CHILD_HOME,/** 子主界面**/
    PAGE_APP_LIST,/**app列表界面 **/
    PAGE_DETAIL,/**app详情界面 **/
} ui_page_t;

/******************************************************
app按钮枚举
******************************************************/
typedef enum{
	Home=0,
    btn_list_setting,/**设置 **/
	btn_list_File,
 }ui_app_btn_t;

 typedef struct {
    lv_font_t *FONT_Size_12;
    lv_font_t *FONT_Size_16;
    lv_font_t *FONT_Size_24;
    lv_font_t *FONT_Size_32;
}my_font_t;

#endif // __UI_ENUM_STRUCT_H
