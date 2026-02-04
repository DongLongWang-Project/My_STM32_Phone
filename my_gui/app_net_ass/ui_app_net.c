 #include "ui_app_net.h"

 typedef enum
 {
    NET_
 }APP_NET_LIST_ENUM;
void ui_app_net_list_creat(lv_obj_t*parent)
{
   lv_obj_t*list_net=lv_list_create(parent);
   lv_obj_set_size(list_net,lv_pct(100),lv_pct(100));
   lv_obj_set_style_radius(list_net,0,0);
   
    
}
