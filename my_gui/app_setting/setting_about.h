#ifndef __SETTING_ABOUT_H
#define __SETTING_ABOUT_H


#include "../ui_app_language.h"
#include "stdio.h"

typedef enum
{
    update_none=0xFF,
    update_is_running=0xFE,
    update_is_ok=0xFC,
}update_state_t;
typedef struct 
{
    uint32_t version;
    uint32_t crc32;
    uint32_t file_size;
    char name[16];
    update_state_t update_state;   
}head_t;


void ui_app_setting_about(lv_obj_t*parent);

#endif
