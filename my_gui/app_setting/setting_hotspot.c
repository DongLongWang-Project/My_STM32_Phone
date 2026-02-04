/*--------------------------------------------------------------------------------↓
	@项目	:app的设置的选项 热点设置
	@日期	:
	@备注	:
↑--------------------------------------------------------------------------------*/


#include "setting_hotspot.h"

static void event_hotspot_switch_cb(lv_event_t*e);
static void event_hotspot_users_cb(lv_event_t*e);
static void event_hotspot_name_cb(lv_event_t*e);
static void event_hotspot_pass_cb(lv_event_t*e);
static void event_hotspot_channel_cb(lv_event_t*e);
static void event_hotspot_method_cb(lv_event_t*e);
static void event_hotspot_addr_cb(lv_event_t*e);

/*************************************************************个人热点*************************************************************/


/*--------------------------------------------------------------------------------↓
	@函数	  :
	@参数	  :
	@返回值 :
	@备注	  :
↑--------------------------------------------------------------------------------*/
 void ui_app_setting_hotspot(lv_obj_t*parent)
{
    /*热点列表*/
    lv_obj_t *hotspot_list=lv_list_create(parent);
    lv_obj_set_size(hotspot_list, lv_pct(100), lv_pct(100));              
    lv_obj_set_style_border_width(hotspot_list, 0, 0);
    lv_obj_set_style_radius(hotspot_list, 0, 0);
    lv_obj_set_style_pad_all(hotspot_list, 0, 0);
    lv_obj_set_scrollbar_mode(hotspot_list, LV_SCROLLBAR_MODE_OFF); 
    lv_obj_set_scroll_dir(hotspot_list, LV_DIR_VER); 
    /*添加标题0*/
    lv_list_add_text(hotspot_list,_GET_UI_TEXT(HOTSPOT_LV_TABLE,HOTSPOT_TITLE));
    
    /*新建一个位置(放热点开关和标题)1*/
    lv_obj_t*obj=lv_obj_create(hotspot_list);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF); 
    lv_obj_set_size(obj, lv_pct(100), lv_pct(15)); 
    /*热点开关标题1.0*/
    lv_obj_t*hotspot_label=lv_label_create(obj);
    lv_obj_align(hotspot_label,LV_ALIGN_LEFT_MID,0,0); 
    lv_label_set_text(hotspot_label,_GET_UI_TEXT(HOTSPOT_LV_TABLE,HOTSPOT_TITLE));
    /*热点开关1.1*/
    lv_obj_t*hotspot_switch=lv_switch_create(obj); 
    lv_obj_align(hotspot_switch,LV_ALIGN_RIGHT_MID,0,0);
    
    /*热点列表标题2*/
    lv_list_add_text(hotspot_list,_GET_UI_TEXT(HOTSPOT_LV_TABLE,HOTSPOT_NAME));
    lv_obj_t*hotspot_name=lv_textarea_create(hotspot_list);
    lv_obj_set_size(hotspot_name, lv_pct(100), lv_pct(15)); 

    lv_obj_t*label_check_name=lv_label_create(hotspot_name);
    lv_obj_set_align(label_check_name,LV_ALIGN_RIGHT_MID);



    lv_list_add_text(hotspot_list,_GET_UI_TEXT(HOTSPOT_LV_TABLE,HOTSPOT_PASS));
    lv_obj_t*hotspot_pass=lv_textarea_create(hotspot_list);

    lv_obj_set_size(hotspot_pass, lv_pct(100), lv_pct(15)); 
    lv_obj_t*label_check_pass=lv_label_create(hotspot_pass);
    lv_obj_set_align(label_check_pass,LV_ALIGN_RIGHT_MID);
                
    lv_list_add_text(hotspot_list,_GET_UI_TEXT(HOTSPOT_LV_TABLE,HOTSPOT_CHANNEL));
    lv_obj_t*hotspot_channel=lv_dropdown_create(hotspot_list);
    lv_obj_set_size(hotspot_channel, lv_pct(100), lv_pct(15)); 


    
    lv_list_add_text(hotspot_list,_GET_UI_TEXT(HOTSPOT_LV_TABLE,HOTSPOT_METHOD));
    lv_obj_t*hotspot_method=lv_dropdown_create(hotspot_list);
    lv_obj_set_size(hotspot_method, lv_pct(100), lv_pct(15)); 


    lv_list_add_text(hotspot_list,_GET_UI_TEXT(HOTSPOT_LV_TABLE,HOTSPOT_USERS));
    lv_obj_t*hotspot_users=lv_dropdown_create(hotspot_list);
    lv_obj_set_size(hotspot_users, lv_pct(100), lv_pct(15)); 


    lv_list_add_text(hotspot_list,_GET_UI_TEXT(HOTSPOT_LV_TABLE,HOTSPOT_IP));
    lv_obj_t*hotspot_addr=lv_textarea_create(hotspot_list);
    lv_textarea_set_accepted_chars(hotspot_addr,"0123456789.");
    
    lv_obj_set_size(hotspot_addr, lv_pct(100), lv_pct(15)); 
    lv_obj_t*label_check_ip_addr=lv_label_create(hotspot_addr);
    lv_obj_set_align(label_check_ip_addr,LV_ALIGN_RIGHT_MID);
    
/*--------------------------------------------------------------------------------*/
    #if keil
    lv_textarea_add_text(hotspot_name,hotspot_data.hotspot_name);
    lv_textarea_add_text(hotspot_pass,hotspot_data.hotspot_pass);
    lv_textarea_add_text(hotspot_addr,hotspot_data.hotspot_ip);
    #endif
    lv_dropdown_set_options(hotspot_channel,"1\n6\n11");   
    lv_dropdown_set_options(hotspot_method,"OPEN\nWPA2_PSK");
    lv_dropdown_set_options(hotspot_users,"1\n2\n3\n4\n5");
    #if keil
    if(hotspot_data.hotspot_channel==1)lv_dropdown_set_selected(hotspot_channel,0);
    else  if(hotspot_data.hotspot_channel==6)lv_dropdown_set_selected(hotspot_channel,1);
    else  if(hotspot_data.hotspot_channel==11) lv_dropdown_set_selected(hotspot_channel,2);
    lv_dropdown_set_selected(hotspot_method,hotspot_data.hotspot_method);
    lv_dropdown_set_selected(hotspot_users,hotspot_data.hotspot_users-1);
    #endif
    
    #if keil
    if(display_cfg.wifi_switch_state==false)
    {
      hotspot_data.hotspot_ip_allow=true;
    }
    else
    {
      if(check_ip_conflict(connected_wifi.ip,hotspot_data.hotspot_ip)==true)
      {
       hotspot_data.hotspot_ip_allow=true;
      }
      else
       hotspot_data.hotspot_ip_allow=false;
    }
    #endif

    #if keil
    
    if(hotspot_data.hotspot_name_allow==true)  lv_label_set_text(label_check_name,LV_SYMBOL_OK);
    else                                          lv_label_set_text(label_check_name,LV_SYMBOL_CLOSE); 
    if(hotspot_data.hotspot_pass_allow==true)  lv_label_set_text(label_check_pass,LV_SYMBOL_OK);                                        
    else                                          lv_label_set_text(label_check_pass,LV_SYMBOL_CLOSE);
    if(hotspot_data.hotspot_ip_allow==true)        lv_label_set_text(label_check_ip_addr,LV_SYMBOL_OK);                                        
    else                                          lv_label_set_text(label_check_ip_addr,LV_SYMBOL_CLOSE);

        if((hotspot_data.hotspot_pass_allow==true) && (hotspot_data.hotspot_name_allow==true) \
          && (hotspot_data.hotspot_ip_allow==true))
        {
            lv_obj_clear_state(hotspot_switch, LV_STATE_DISABLED);
        }
       else if((hotspot_data.hotspot_method==0) && (hotspot_data.hotspot_ip_allow==true))
       {
         if(hotspot_data.hotspot_name_allow==true)
            {
                lv_obj_clear_state(hotspot_switch, LV_STATE_DISABLED);
            }  
       }
        else
            {
                lv_obj_add_state(hotspot_switch, LV_STATE_DISABLED);
            }
    #endif 



    if(display_cfg.hotspot_switch_state==true)
    {
            lv_obj_add_state(hotspot_switch,LV_STATE_CHECKED);
    
            lv_obj_add_state(hotspot_name,LV_STATE_DISABLED);
            lv_obj_add_state(hotspot_pass,LV_STATE_DISABLED);
            lv_obj_add_state(hotspot_channel,LV_STATE_DISABLED);
            lv_obj_add_state(hotspot_method,LV_STATE_DISABLED);
            lv_obj_add_state(hotspot_users,LV_STATE_DISABLED);
            lv_obj_add_state(hotspot_addr,LV_STATE_DISABLED);
    }
    else
    {       lv_obj_clear_state(hotspot_switch,LV_STATE_CHECKED);
    
            lv_obj_clear_state(hotspot_name,LV_STATE_DISABLED);
            lv_obj_clear_state(hotspot_pass,LV_STATE_DISABLED);
            lv_obj_clear_state(hotspot_channel,LV_STATE_DISABLED);
            lv_obj_clear_state(hotspot_method,LV_STATE_DISABLED);
            lv_obj_clear_state(hotspot_users,LV_STATE_DISABLED);
            lv_obj_clear_state(hotspot_addr,LV_STATE_DISABLED);
    } 
/*--------------------------------------------------------------------------------*/ 
    lv_obj_add_event_cb(hotspot_switch,event_hotspot_switch_cb,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(hotspot_users,event_hotspot_users_cb,LV_EVENT_VALUE_CHANGED,NULL);
    
    lv_obj_add_event_cb(hotspot_name,event_hotspot_name_cb,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(hotspot_name,event_textarea_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(hotspot_pass,event_hotspot_pass_cb,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(hotspot_pass,event_textarea_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(hotspot_addr,event_hotspot_addr_cb,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(hotspot_addr,event_textarea_cb,LV_EVENT_CLICKED,NULL); 
    
    
    lv_obj_add_event_cb(hotspot_channel,event_hotspot_channel_cb,LV_EVENT_VALUE_CHANGED,NULL); 
    lv_obj_add_event_cb(hotspot_method,event_hotspot_method_cb,LV_EVENT_VALUE_CHANGED,NULL);

}

static void event_hotspot_addr_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code=lv_event_get_code(e);
    lv_obj_t *label=lv_obj_get_child(target,1);
    lv_obj_t*list=lv_obj_get_parent(target);
    lv_obj_t*obj=lv_obj_get_child(list,1);
    lv_obj_t*hotspot_switch=lv_obj_get_child(obj,1);
    char ip[16];
    int temp[4];
        if(code==LV_EVENT_VALUE_CHANGED)
        {
            #if keil
          strcpy(ip,lv_textarea_get_text(target));
          if( (sscanf(ip,"%d.%d.%d.%d",&temp[0],&temp[1],&temp[2],&temp[3])!=4) && check_ip_conflict(connected_wifi.ip,ip)==false)
          {
            hotspot_data.hotspot_ip_allow=false;
            lv_label_set_text(label,LV_SYMBOL_CLOSE);
            lv_obj_add_state(hotspot_switch, LV_STATE_DISABLED);
          }
          else
          {
            hotspot_data.hotspot_ip_allow=true;
            lv_label_set_text(label,LV_SYMBOL_OK);
            if((hotspot_data.hotspot_pass_allow==true) && (hotspot_data.hotspot_name_allow==true) \
              && (hotspot_data.hotspot_ip_allow==true))
            {
                lv_obj_clear_state(hotspot_switch, LV_STATE_DISABLED);
                strcpy(hotspot_data.hotspot_ip,lv_textarea_get_text(target)); 
            }
          }
          #endif
        }
}

static void event_hotspot_name_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code=lv_event_get_code(e);
    lv_obj_t *label=lv_obj_get_child(target,1);
    
    lv_obj_t*list=lv_obj_get_parent(target);
    lv_obj_t*obj=lv_obj_get_child(list,1);
    lv_obj_t*hotspot_switch=lv_obj_get_child(obj,1);
        if(code==LV_EVENT_VALUE_CHANGED)
        {
            #if keil
            if( (strlen(lv_textarea_get_text(target))>32)||(strlen(lv_textarea_get_text(target))==0))
               {
                    lv_label_set_text(label,LV_SYMBOL_CLOSE);
                    
                    hotspot_data.hotspot_name_allow=false;
                   
                    lv_obj_add_state(hotspot_switch, LV_STATE_DISABLED);
               }
               else
                {
                    lv_label_set_text(label,LV_SYMBOL_OK);
                    hotspot_data.hotspot_name_allow=true;
                    if((hotspot_data.hotspot_pass_allow==true) && (hotspot_data.hotspot_name_allow==true))
                    {
                        lv_obj_clear_state(hotspot_switch, LV_STATE_DISABLED);
                    }                 
                }
                 #endif
        }
}
static void event_hotspot_pass_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code=lv_event_get_code(e);
    lv_obj_t *label=lv_obj_get_child(target,1);
    
    lv_obj_t*list=lv_obj_get_parent(target);
    lv_obj_t*obj=lv_obj_get_child(list,1);
    lv_obj_t*hotspot_switch=lv_obj_get_child(obj,1);
    
        if(code==LV_EVENT_VALUE_CHANGED)
        {
            if( (strlen(lv_textarea_get_text(target))>64)||(strlen(lv_textarea_get_text(target))<8))
               {
                    lv_label_set_text(label,LV_SYMBOL_CLOSE);
                    #if keil
                    hotspot_data.hotspot_pass_allow=false;
                    #endif
                    lv_obj_add_state(hotspot_switch, LV_STATE_DISABLED);
               }
               else
                {
                    lv_label_set_text(label,LV_SYMBOL_OK);
                    #if keil
                    hotspot_data.hotspot_pass_allow=true;
                    if((hotspot_data.hotspot_pass_allow==true) && (hotspot_data.hotspot_name_allow==true))
                    {
                        lv_obj_clear_state(hotspot_switch, LV_STATE_DISABLED);
                    }
                    #endif  
                }
        }
}

static void event_hotspot_switch_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code=lv_event_get_code(e);
    bool switch_state=lv_obj_has_state(target,LV_STATE_CHECKED);
    lv_obj_t*obj= lv_obj_get_parent(target);
    lv_obj_t*list=lv_obj_get_parent(obj);
    
    lv_obj_t*hotspot_name=lv_obj_get_child(list,3);
    lv_obj_t*hotspot_pass=lv_obj_get_child(list,5);
     lv_obj_t*hotspot_channel=lv_obj_get_child(list,7);
    lv_obj_t*hotspot_mode=lv_obj_get_child(list,9);
    lv_obj_t*hotspot_users=lv_obj_get_child(list,11); 
        
    if(code==LV_EVENT_VALUE_CHANGED)
    {
        if(switch_state==true)
        {
            display_cfg.hotspot_switch_state=true;
            lv_obj_add_state(hotspot_name,LV_STATE_DISABLED);
            lv_obj_add_state(hotspot_pass,LV_STATE_DISABLED);
            lv_obj_add_state(hotspot_channel,LV_STATE_DISABLED);
            lv_obj_add_state(hotspot_mode,LV_STATE_DISABLED);
            lv_obj_add_state(hotspot_users,LV_STATE_DISABLED);
            
                #if keil
//                snprintf(wifi_cmd_table[AT_CMD_CIPAP].cmd_str,AT_CMD_MAX_LEN,"AT+CIPAP=\"%s\"\r\n",hotspot_data.hotspot_ip);
//                DX_WF25_Send_queue(AT_CMD_CIPAP);
                DX_WF25_Send_Dynamic(AT_CMD_CIPAP,"AT+CIPAP=\"%s\"\r\n",hotspot_data.hotspot_ip);
                if(display_cfg.wifi_switch_state==true)
                {
                  DX_WF25_Send_Static(AT_MODE_STA_AP);
                }
                else 
                { 
                  DX_WF25_Send_Static(AT_MODE_AP);
                }
                printf("hotspot_users:    %d\r\n",hotspot_data.hotspot_users);

            strncpy(hotspot_data.hotspot_name, lv_textarea_get_text(hotspot_name), sizeof(hotspot_data.hotspot_name) - 1);
            hotspot_data.hotspot_name[sizeof(hotspot_data.hotspot_name) - 1] = '\0';

            strncpy(hotspot_data.hotspot_pass, lv_textarea_get_text(hotspot_pass), sizeof(hotspot_data.hotspot_pass) - 1);
            hotspot_data.hotspot_pass[sizeof(hotspot_data.hotspot_pass) - 1] = '\0';

//           snprintf(wifi_cmd_table[AT_CMD_CWSAP].cmd_str,AT_CMD_MAX_LEN,"AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n",\
//           hotspot_data.hotspot_name,hotspot_data.hotspot_pass,hotspot_data.hotspot_channel ,hotspot_data.hotspot_method);
//           DX_WF25_Send_queue(AT_CMD_CWSAP);
           
//          DX_WF25_Send_Dynamic(AT_CMD_CWSAP,"AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n",\
//           hotspot_data.hotspot_name,hotspot_data.hotspot_pass,hotspot_data.hotspot_channel ,hotspot_data.hotspot_method);
//           DX_WF25_Send_queue(AT_CMD_CWSAP);
           DX_WF25_Send_Dynamic(AT_CMD_CWSAP,"AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n",\
           hotspot_data.hotspot_name,hotspot_data.hotspot_pass,hotspot_data.hotspot_channel ,hotspot_data.hotspot_method);
                if(hotspot_data.hotspot_users==1) 
                {
                  DX_WF25_Send_Static(AT_CMD_CIPMUX_ONE);
                }
                else   
                {
                  DX_WF25_Send_Static(AT_CMD_CIPMUX_MANY);
                  DX_WF25_Send_Static(AT_CMD_CIPSERVERE);
                } 
           #endif    
        }
        else
        {
            display_cfg.hotspot_switch_state=false;
            
            lv_obj_clear_state(hotspot_name,LV_STATE_DISABLED);
            lv_obj_clear_state(hotspot_pass,LV_STATE_DISABLED);
            lv_obj_clear_state(hotspot_channel,LV_STATE_DISABLED);
            lv_obj_clear_state(hotspot_mode,LV_STATE_DISABLED);
            lv_obj_clear_state(hotspot_users,LV_STATE_DISABLED);
                #if keil
                if(display_cfg.wifi_switch_state==true)
                  {
                     DX_WF25_Send_Static(AT_MODE_STA); 
                  }
                  else
                  {
                    DX_WF25_Send_Static(AT_MODE_OFF);  
                  }
              #endif
  
        }
    }
}
static void event_hotspot_users_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code=lv_event_get_code(e);
    
    if(code==LV_EVENT_VALUE_CHANGED)
    {
        #if keil
        hotspot_data.hotspot_users=lv_dropdown_get_selected(target)+1;
        #endif
    }
}
static void event_hotspot_channel_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code=lv_event_get_code(e);
    
    if(code==LV_EVENT_VALUE_CHANGED)
    {
        #if keil
        if(lv_dropdown_get_selected(target)==0)
        {
           hotspot_data.hotspot_channel=1;
        }
        else  if(lv_dropdown_get_selected(target)==1)
        {
           hotspot_data.hotspot_channel=6;
        }
        else  if(lv_dropdown_get_selected(target)==2)
        {
           hotspot_data.hotspot_channel=11;
        }
        #endif
    }
}
static void event_hotspot_method_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code=lv_event_get_code(e);
    lv_obj_t*list=lv_obj_get_parent(target);
    lv_obj_t*textarea_pass=lv_obj_get_child(list,5);
    

    lv_obj_t*obj=lv_obj_get_child(list,1);
    lv_obj_t*hotspot_switch=lv_obj_get_child(obj,1);
    
    if(code==LV_EVENT_VALUE_CHANGED)
    {
        #if keil
        if(lv_dropdown_get_selected(target))
        {
           hotspot_data.hotspot_method=3; 
           lv_obj_clear_state(textarea_pass, LV_STATE_DISABLED);
        if((hotspot_data.hotspot_pass_allow!=true) && (hotspot_data.hotspot_name_allow!=true))
           {
             lv_obj_add_state(hotspot_switch, LV_STATE_DISABLED);  
           }
        }
        else
        {
           hotspot_data.hotspot_method=0; 
           lv_textarea_set_text(textarea_pass,"");
           lv_obj_add_state(textarea_pass, LV_STATE_DISABLED);
          lv_obj_clear_state(hotspot_switch, LV_STATE_DISABLED); 
        }
        #endif
    }
}
