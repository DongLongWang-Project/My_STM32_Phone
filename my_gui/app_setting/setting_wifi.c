/*--------------------------------------------------------------------------------↓
	@项目	: app设置的选项(wifi的连接)
	@日期	: 2026-1-24
	@备注	:
↑--------------------------------------------------------------------------------*/
#include "setting_wifi.h"

/*************************************************************WIFI*************************************************************/

/*wifi*/
static void ui_event_wifi_refresh_btn_cb(lv_event_t*e);
static void event_wifi_switch_cb(lv_event_t*e);
void ui_app_setting_wifi(lv_obj_t*parent);

lv_obj_t*list_wifi;

static lv_obj_t* ui_app_setting_wifi_refresh_btn_creat(lv_obj_t*parent);/*wifi扫描刷新按钮*/

/*--------------------------------------------------------------------------------↓
	@函数	  : 创建wifi的ui
	@参数	  : 父对象
	@返回值 :  无
	@备注	  :
↑--------------------------------------------------------------------------------*/
void ui_app_setting_wifi(lv_obj_t* parent)
{
    list_wifi = lv_list_create(parent);/*创建wifi列表(全局变量)*/
    lv_obj_set_size(list_wifi, lv_pct(100), lv_pct(100));              
    lv_obj_set_style_border_width(list_wifi, 0, 0);
    lv_obj_set_style_radius(list_wifi, 0, 0);
    lv_obj_set_style_pad_all(list_wifi, 0, 0);
    
    lv_obj_set_scrollbar_mode(list_wifi, LV_SCROLLBAR_MODE_OFF); 
    lv_obj_set_scroll_dir(list_wifi, LV_DIR_VER); /*允许纵向滚动*/
    
    // 添加文本
    lv_obj_t*wifi_text=lv_list_add_text(list_wifi,"WLAN");/*添加列表标题WLAN*/
    
    lv_obj_t*obj=lv_obj_create(list_wifi);/*在列表里面新建一个obj区域放开关*/
    
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
//    lv_obj_set_style_pad_all(obj, 0, 0);
    
    ui_app_setting_wifi_refresh_btn_creat(parent);/*创建刷新按钮*/
    
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);/*关闭obj滚动*/ 
    lv_obj_set_size(obj, lv_pct(100), lv_pct(15)); 
    lv_obj_t*wifi_switch=lv_switch_create(obj);/*创建开关*/ 
    lv_obj_align(wifi_switch,LV_ALIGN_RIGHT_MID,0,0);
    lv_obj_t*wifi_label=lv_label_create(obj);/*添加文本*/
    lv_obj_align(wifi_label,LV_ALIGN_LEFT_MID,0,0); 
    lv_label_set_text(wifi_label,"WIFI");
    if(display_cfg.wifi_switch_state==true)/*如果(硬件)wifi是打开的,就将开关状态置开*/
    {
      lv_obj_add_state(wifi_switch,LV_STATE_CHECKED);  
    }
    else /*否则就为关*/
    {
      lv_obj_clear_state(wifi_switch,LV_STATE_CHECKED);
    }
    lv_obj_add_event_cb(wifi_switch,event_wifi_switch_cb,LV_EVENT_VALUE_CHANGED,NULL);/*开关的回调函数*/
    
    // 添加 WiFi 项
//    lv_obj_t* btn_wifi;
//    btn_wifi = lv_list_add_btn(list_wifi, LV_SYMBOL_WIFI, "ww"); 
//    lv_obj_add_event_cb(btn_wifi, ui_event_wifi_btn_cb, LV_EVENT_CLICKED, NULL);
     if(display_cfg.wifi_switch_state==true) /*如果wifi是打开的就自动*/
     {
        wifi_scan_done_cb(NULL);/*自己添加环境中的wifi到列表中*/
     }
     
}


/*--------------------------------------------------------------------------------↓
	@函数	  : wifi密码输入框中的连接按钮回调函数
	@参数	  : 无
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
static void event_wifi_connect_btn_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);/*获取按钮*/
    lv_event_code_t code=lv_event_get_code(e);/*获取状态*/
    lv_obj_t*obj=lv_obj_get_parent(target);/*获取装连接按钮的obj区域*/
    lv_obj_t*parent=lv_obj_get_parent(obj); /*获取对话框*/
    lv_obj_t*textarea_pass=lv_obj_get_child(obj,1);/*获取对话框的子对象(密码textarea)*/
    
    if(code==LV_EVENT_CLICKED)
    {
        #if keil 
         strcpy(wifi_scan_list.ready_wifi_password,lv_textarea_get_text(textarea_pass));/*从textarea中获取密码,并复制到准备连接wifi的结构体的成员密码*/
        if(strlen(wifi_scan_list.ready_wifi_password)!=0)/*如果密码长度不为0*/
        {
            /*将密码,wifi,和AT+CWJAP=拼装*/
//           snprintf(wifi_cmd_table[AT_CMD_CWJAP].cmd_str,AT_CMD_MAX_LEN,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifi_scan_list.ready_wifi_ssid,wifi_scan_list.ready_wifi_password);
//           DX_WF25_Send_queue(AT_CMD_CWJAP);/*发送连接wifi的命令到队列*/
            DX_WF25_Send_Dynamic(AT_CMD_CWJAP,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifi_scan_list.ready_wifi_ssid,wifi_scan_list.ready_wifi_password);
//           printf("pass:%s",wifi_cmd_table[AT_CMD_CWJAP].cmd_str) ; 
//           wifi_scan_list.connected=0;
        }
       lv_obj_add_flag(keyboard,LV_OBJ_FLAG_HIDDEN);/*自动隐藏键盘*/         
       lv_obj_del(parent);/*删除对话框*/   
       #endif // keil
    }
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 创建对话框背面的阴影的回调函数
	@参数	  : 无
	@返回值 : 无
	@备注	  : 点击阴影,删除对话框,删除自己
↑--------------------------------------------------------------------------------*/
static void event_bg_clicked_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code==LV_EVENT_CLICKED)
    {
      lv_obj_del(lv_obj_get_child(target,0)); 
      lv_obj_del(target); 
    }
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 密码textarea的回调函数
	@参数	  : 无
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
static void event_textarea_pass_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);/*textarea*/
    lv_event_code_t code= lv_event_get_code(e);
    lv_obj_t*obj=lv_obj_get_parent(target);/*获取对话框*/
     if(code==LV_EVENT_VALUE_CHANGED)
    {
        if(strlen(lv_textarea_get_text(target))>=8) /*如果密码输入长度大于=8*/
        { 
           lv_obj_clear_state(lv_obj_get_child(obj,2),LV_STATE_DISABLED);/*解除对连接按钮的无法选中状态*/
        }
    }
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 查看密码的按钮回调函数
	@参数	  : 无
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
static void event_seepass_btn_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);/*小眼睛*/
    lv_event_code_t code= lv_event_get_code(e);
    lv_obj_t *textarea=lv_obj_get_parent(target);/*对话框*/
    lv_obj_t *label=lv_obj_get_child(target,0); /*对应小眼睛的label*/
    if(code==LV_EVENT_CLICKED)
    {
        if(lv_textarea_get_password_mode(textarea)==true)/*如果是密码模式*/
        {
          lv_textarea_set_password_mode(textarea,false); /*取消密码模式*/ 
          lv_label_set_text(label,LV_SYMBOL_EYE_OPEN);  /*眼睛为打开*/
        }
        else
        {
            lv_textarea_set_password_mode(textarea,true); /*不是密码模式*/
             lv_label_set_text(label,LV_SYMBOL_EYE_CLOSE); /*眼睛为关闭*/
        }
        
    }
}


/*--------------------------------------------------------------------------------↓
	@函数	  : 创建密码输入框
	@参数	  : 父对象  ssid:对应wifi的名字(显示在对话框)
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
void ui_creat_wifi_textarea(lv_obj_t *parent,const char*ssid)
{
    lv_obj_t*obj=lv_obj_create(parent);/*创建背景区域*/
    lv_obj_set_size(obj,lv_pct(90),lv_pct(90));
    lv_obj_set_pos(obj,lv_pct(5),lv_pct(5));
    lv_obj_set_style_border_width(obj,0,LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(obj,0,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj,100,LV_STATE_DEFAULT);
    lv_obj_add_event_cb(obj,event_bg_clicked_cb,LV_EVENT_CLICKED,NULL);/*添加背景点击回调*/
    
    lv_obj_t*win_passText=lv_obj_create(obj); /*创建密码输入框区域*/
    lv_obj_set_size(win_passText,lv_pct(100),lv_pct(50));
    lv_obj_set_style_pad_all(win_passText,0,LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(win_passText,LV_FLEX_FLOW_COLUMN);/*在里面新建的子对象为纵向排列*/
      
    lv_obj_t*label_ssid=lv_label_create(win_passText);/*创建wifi名称的label在密码输入框*/
    lv_label_set_text(label_ssid,ssid);/*设置对应wifi的文本*/
    lv_obj_set_align(label_ssid,LV_ALIGN_TOP_RIGHT);
    lv_obj_set_size(label_ssid,lv_pct(100),lv_pct(15));
    lv_obj_set_style_text_font(label_ssid,&lv_font_montserrat_24,LV_STATE_DEFAULT);/*设置对应字体*/
    
     lv_obj_t*wifi_pass=lv_textarea_create(win_passText);/*创建密码输入textarea*/
     lv_obj_set_size(wifi_pass,lv_pct(100),lv_pct(10));
     lv_textarea_set_placeholder_text(wifi_pass,"PASSWORD:");/*textarea 的背景为密码*/
     lv_textarea_set_one_line(wifi_pass,true);/*单行模式*/
     lv_textarea_set_password_mode(wifi_pass,true);/*密码模式*/
     lv_textarea_set_password_show_time(wifi_pass,400);/*密码显示时间*/
     
     lv_obj_t*seepass_btn=lv_btn_create(wifi_pass);/*创建小眼睛按钮*/
     lv_obj_set_size(seepass_btn,lv_pct(20),lv_pct(20));
     
     lv_obj_t*seepass=lv_label_create(seepass_btn);/*在按钮里面创建小眼睛label*/
     lv_label_set_text(seepass,LV_SYMBOL_EYE_CLOSE);/*设置小眼睛图标*/
     lv_obj_set_align(seepass_btn,LV_ALIGN_RIGHT_MID);
      lv_obj_set_align(seepass,LV_ALIGN_CENTER);
      lv_obj_set_style_pad_all(seepass_btn,12,LV_STATE_DEFAULT); 
      
      lv_obj_set_style_bg_opa(seepass_btn,0,LV_STATE_DEFAULT); 
      lv_obj_set_style_shadow_width(seepass_btn,0,LV_STATE_DEFAULT); 
      lv_obj_set_style_text_color(seepass,lv_color_hex(0x9B9B9B),LV_STATE_DEFAULT); 
     lv_obj_add_event_cb(seepass_btn,event_seepass_btn_cb,LV_EVENT_CLICKED,NULL);/*添加小眼睛回调函数*/
     
     lv_obj_add_event_cb(wifi_pass,event_textarea_cb,LV_EVENT_CLICKED,NULL);/*添加点击密码textarea的触发键盘的回调*/
     lv_obj_add_event_cb(wifi_pass,event_textarea_pass_cb,LV_EVENT_VALUE_CHANGED,NULL);/*密码textarea的变化回调*/
     
     lv_obj_t*btn_connect=lv_btn_create(win_passText);/*创建连接按钮*/
     lv_obj_t*btn_text=lv_label_create(btn_connect);/*按钮文本*/
//     lv_obj_set_size(btn_connect,scr_width*2/3,scr_height/10);
     lv_obj_set_size(btn_connect,lv_pct(100),lv_pct(20));
//     lv_obj_align(btn_text,LV_ALIGN_TOP_MID,-10,-5);
     lv_label_set_text(btn_text,"Connect");
     lv_obj_center(btn_text);
     lv_obj_add_state(btn_connect,LV_STATE_DISABLED);/*连接按钮默认为无法操作*/
     
     lv_obj_add_event_cb(btn_connect,event_wifi_connect_btn_cb,LV_EVENT_CLICKED,NULL); /*添加连接按钮的回调函数*/
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 点击列表中对应wifi按钮回调函数
	@参数	  :
	@返回值 :
	@备注	  :
↑--------------------------------------------------------------------------------*/
 static void ui_event_wifi_btn_cb(lv_event_t*e)
{
   uint8_t found=0;/*判断是否找到密码在w25q*/
    lv_obj_t* btn=lv_event_get_target(e);/*wifi按钮*/
    lv_obj_t* list=lv_obj_get_parent(btn);/*wifi所在列表*/
      lv_obj_t* parent_list=lv_obj_get_parent(list);/*获得最外层列表(list_wifi)*/
    if(lv_event_get_code(e)==LV_EVENT_CLICKED)
    {
        #if keil
         wifi_scan_list.connected=0;/*现将连接状态断开*/
         strcpy(wifi_scan_list.ready_wifi_ssid,lv_list_get_btn_text(list,btn));//*将wifi的名字复制到对应的准备链接wifi的结构体的ssid*/
        for(uint8_t i=0;i<wifi_save_list.save_count;i++)/*便利保存的wifi*/
      {
         if((strcmp(wifi_save_list.save_list[i].ssid,wifi_scan_list.ready_wifi_ssid)==0))/*如果有和保存的wifi一样的ssid*/
         {
//            printf("找到了名字:%s\r\n",wifi_save_list.save_list[i].ssid);
//            printf("找到了密码:%s\r\n",wifi_save_list.save_list[i].password); /*将密码名字指令拼接*/
//          snprintf(wifi_cmd_table[AT_CMD_CWJAP].cmd_str,AT_CMD_MAX_LEN,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifi_save_list.save_list[i].ssid,wifi_save_list.save_list[i].password);
//           DX_WF25_Send_queue(AT_CMD_CWJAP);/*发送连接命令*/
           DX_WF25_Send_Dynamic(AT_CMD_CWJAP,"AT+CWJAP=\"%s\",\"%s\"\r\n",wifi_save_list.save_list[i].ssid,wifi_save_list.save_list[i].password);
           found=1; /*找到了*/
           return;                          
         }
      }
      if(!found)/*没找到,创建密码输入框,点击连接按钮连接wifi*/
      {
        ui_creat_wifi_textarea(list_wifi,lv_list_get_btn_text(list,btn));
      } 
      #endif // keil
      
    }
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 创建wifi的图标在顶部状态栏
	@参数	  :  用户数据(无)
	@返回值 :
	@备注	  :  用于不是lvgl的线程
↑--------------------------------------------------------------------------------*/
void wifi_icon_show(void *user_data)
{
//    printf("%d",wifi_scan_list.connected);
    #if keil
    if(wifi_scan_list.connected==0)/*未连接wifi,或者断开了*/
    {
        if(label_wifi!=NULL)/*如果创建过了*/
        {
            lv_obj_del(label_wifi); /*删除*/
            label_wifi=NULL;/*置为NULL*/
        }
    }
    else
    {
       if(label_wifi==NULL)/*如果为NULL*/
        {
          label_wifi=creat_statebar_icon(LV_SYMBOL_WIFI); /*创建*/
        }
        if(label_wifi!=NULL) /*创建过了就退出*/
        {
          return;
        }
    }
    #endif // keil
    
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 创建wifi列表
	@参数	  :  无
	@返回值 :
	@备注	  :  用于不是lvlg的线程
↑--------------------------------------------------------------------------------*/
void wifi_scan_done_cb(void *user_data)
{
    lv_obj_t*btn_wifi;
     lv_obj_t*obj;
    if(list_wifi==NULL) /*如果退出了wifi页面也就没必要刷新wifi列表页面了*/
    {
      printf("已经退出wifi界面,没必要刷新页面了\r\n");
      return ;
    }
   obj=lv_obj_get_child(list_wifi,2); /*获取wifi里面的区域*/
   if(obj!=NULL)
   {
      lv_obj_del(obj);/*删除,方便更新*/
      obj=NULL; /**/
   }
    obj=lv_obj_create(list_wifi); /*创建区域*/
    lv_obj_set_size(obj,lv_pct(100),lv_pct(80));
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0); 
    
    lv_obj_t*list=lv_list_create(obj);/*创建列表*/
     lv_obj_set_size(list,lv_pct(100),lv_pct(100));
      lv_obj_set_style_radius(list, 0, 0);
     lv_obj_set_style_pad_all(list, 0, 0);
      
//    printf("连接状态%d\r\n",wifi_scan_list.connected);
    #if keil
    if(wifi_scan_list.connected==0) /*未连接wifi*/
    {
         for(uint8_t i=0;i<wifi_scan_list.scan_count;i++) /*根据扫描的个数*/
         {  
           btn_wifi=lv_list_add_btn(list,LV_SYMBOL_CLOSE,wifi_scan_list.scan_list[i].ssid); /*创建相应的wifi按钮*/
           lv_obj_add_event_cb(btn_wifi,ui_event_wifi_btn_cb,LV_EVENT_CLICKED,NULL);/*添加点击事件*/
//           printf("未连接:%s\r\n",wifi_scan_list.scan_list[i].ssid);
         }
    }
    else      /*连接了wifi*/
    {      
       btn_wifi=lv_list_add_btn(list,LV_SYMBOL_OK,connected_wifi.ssid);/*单独先添加这个wifi*/
        for(uint8_t i=0;i<wifi_scan_list.scan_count;i++) /*接着便利*/
        {
           if(strcmp(connected_wifi.ssid,wifi_scan_list.scan_list[i].ssid)==0) /*如果是已添加的已连接的wifi,就跳过*/
           {
           printf("已连接:%s\r\n",connected_wifi.ssid); 
           continue;
           }
           else 
           {
//             printf("未连接:%s\r\n",wifi_scan_list.scan_list[i].ssid);
             btn_wifi=lv_list_add_btn(list,LV_SYMBOL_CLOSE,wifi_scan_list.scan_list[i].ssid);/*添加未连接的*/
             lv_obj_add_event_cb(btn_wifi,ui_event_wifi_btn_cb,LV_EVENT_CLICKED,NULL);
           } 
        }
      } 
      #endif // keil
}

/*--------------------------------------------------------------------------------↓
	@函数	  :  创建刷新wifi扫描的按钮
	@参数	  :  父对象
	@返回值 :  刷新按钮
	@备注	  :
↑--------------------------------------------------------------------------------*/
static lv_obj_t* ui_app_setting_wifi_refresh_btn_creat(lv_obj_t*parent)
{
        
    lv_obj_t* btn_refresh = lv_btn_create(parent);/*创建按钮*/
    lv_obj_set_size(btn_refresh, 30, 30); 
    lv_obj_align(btn_refresh, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    
    lv_obj_t *btn_label = lv_label_create(btn_refresh);/*按钮文本*/
    lv_label_set_text(btn_label, LV_SYMBOL_REFRESH);
    lv_obj_center(btn_label);

    lv_obj_add_event_cb(btn_refresh, ui_event_wifi_refresh_btn_cb, LV_EVENT_CLICKED, NULL);/*点击事件*/
    return btn_refresh;
}

/*--------------------------------------------------------------------------------↓
	@函数	  :  wifi开关的回调函数
	@参数	  :  无
	@返回值 :  无
	@备注	  :
↑--------------------------------------------------------------------------------*/
static void event_wifi_switch_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);/*开关*/
    lv_event_code_t code=lv_event_get_code(e);
    bool switch_state=lv_obj_has_state(target,LV_STATE_CHECKED);/*查看开启的状态*/
    
    lv_obj_t*obj=lv_obj_get_child(list_wifi,2);/*对应wifi列表*/ 

    if(code==LV_EVENT_VALUE_CHANGED)
    {
        if(switch_state==true)/*打开*/
        {  
            #if keil
            display_cfg.wifi_switch_state=true;    /*更新wifi开关状态*/
            if(display_cfg.hotspot_switch_state==true) /*如果热点已打开*/
            {
                 DX_WF25_Send_Static(AT_MODE_STA_AP); /*发送sta+ap的命令,模式3*/
                 DX_WF25_Send_Static(AT_CMD_CWLAP); /*发送扫描wifi命令*/
            }
            else   /*如果热点没打开*/
              {
//                 DX_WF25_Send_queue(AT_CMD_ATE1);
                 DX_WF25_Send_Static(AT_MODE_STA);/*发送sta命令*/
                 DX_WF25_Send_Static(AT_CMD_CWLAP); /*发送扫描*/
              }            

              #endif
        }
        else  /*关闭*/
        {
            display_cfg.wifi_switch_state=false; /*更新wifi开关状态*/

            
                #if keil
                lv_obj_del(obj);  /*删除wifi列表,置为NULL*/
                obj=NULL;
                  if(display_cfg.hotspot_switch_state==true) /*如果热点开的,发送AP命令*/
                  {
                     DX_WF25_Send_Static(AT_MODE_AP); 
                  }
                  else    /*热点为关的,直接发送关闭命令模式0*/
                  {
                    DX_WF25_Send_Static(AT_MODE_OFF);  
                  }
              #endif

        }
    }
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 刷新按钮的回调函数
	@参数	  :  无
	@返回值 :  无
	@备注	  :
↑--------------------------------------------------------------------------------*/
static void ui_event_wifi_refresh_btn_cb(lv_event_t*e)
{
  lv_event_code_t code=lv_event_get_code(e);
   if(code==LV_EVENT_CLICKED)
   {
       #if keil
     if(display_cfg.wifi_switch_state==true)/*如果wifi的开关打开*/
     {
        memset(&wifi_scan_list.scan_list, 0, sizeof(wifi_cwlap_info_t));
        DX_WF25_Send_Static(AT_CMD_CWLAP); /*大宋扫描命令*/
     }
     else  /*没有就返回*/
     {
         return;
     }
    #endif
   }  
}
