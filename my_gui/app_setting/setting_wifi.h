#ifndef __SETTING_WIFI_H
#define __SETTING_WIFI_H


#include "../ui_app_language.h"

#include "../ui_widgets.h"

void ui_app_setting_wifi(lv_obj_t* parent);
void wifi_scan_done_cb(void *user_data);
void wifi_icon_show(void *user_data);


extern lv_obj_t*list_wifi;

#endif

