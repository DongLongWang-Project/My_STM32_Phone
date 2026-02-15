#ifndef __SETTING_DISPLAY_H
#define __SETTING_DISPLAY_H


#include "../ui_app_language.h"
#include "../../SD/Font/Font.h"

#include "../ui_main.h"




void ui_app_setting_display(lv_obj_t *parent);
void ui_set_language(DISPLAY_LANGUAGE_ENUM language,DISPLAY_FONT_SIZE_ENUM size);
void ui_set_obj_text_font(lv_obj_t*parent,DISPLAY_FONT_SIZE_ENUM FONT_SIZE);

#endif

