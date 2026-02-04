#ifndef __SETTING_DISPLAY_H
#define __SETTING_DISPLAY_H


#include "../ui_app_language.h"


#include "../ui_main.h"

typedef struct {
    lv_font_t *FONT_SIZE_12;
    lv_font_t *FONT_SIZE_14;
    lv_font_t *FONT_SIZE_16;
}ui_font_t;

extern ui_font_t ui_fonts;
void ui_app_setting_display(lv_obj_t *parent);
void ui_set_language(DISPLAY_LANGUAGE_ENUM language,DISPLAY_FONT_SIZE_ENUM size);
void ui_set_obj_text_font(lv_obj_t*parent,DISPLAY_FONT_SIZE_ENUM FONT_SIZE);

#endif

