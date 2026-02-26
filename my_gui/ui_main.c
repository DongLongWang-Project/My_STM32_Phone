/*--------------------------------------------------------------------------------↓
	@项目	:ui主函数
	@日期	:
	@备注	:分为三层 :状态栏 主页面  控制栏
↑--------------------------------------------------------------------------------*/


#include "ui_main.h"



lv_obj_t*tileview,*tile_main;

UI_APP_PAGE_ENUM Cur_Page = PAGE_HOME;
UI_APP_ENUM Cur_APP=APP_TEMP;

static void event_ui_page_btn_app(lv_event_t *e);

void ui_init(void)
{
    #if !keil
    memory_load_font();
    #endif // keil

//    LV_FONT_DECLARE( my_font_12);
//     lv_obj_t*label=lv_label_create(lv_scr_act());
//     lv_obj_set_style_text_font(label,&my_font_12,0);
//     lv_label_set_text(label,"王東龙");
     


    ui_set_language(display_cfg.language,display_cfg.font_size);
    #if keil
    TIM12->CCR2=display_cfg.Brightness;
    #endif
    ui_creat_statebar();
    
    ui_main_page(lv_scr_act());
    ui_creat_control_bar(lv_scr_act());
    ui_creat_keyboard();
    label_time= creat_statebar_icon("");
    lv_obj_set_flex_grow(label_time,1);
 
    lv_timer_create(clock_time_timer_cb, 1000, NULL);  
  
    ui_goto_page(PAGE_HOME,APP_TEMP);
    
    alarm_rem_win(lv_scr_act());
    
}


void ui_main_page(lv_obj_t*parent)
{
    // 1. 创建基础容器
    lv_obj_t* obj = lv_obj_create(parent);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(MainPage_Percent));
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    
    // 关键点 A：必须让这个中间层透明，否则它是一片白
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);

    // 2. 创建 Tileview
    tileview = lv_tileview_create(obj);  
    lv_obj_align_to(obj, state_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    
    // 关键点 B：Tileview 默认有背景，也得把它弄透明
    lv_obj_set_style_bg_opa(tileview, LV_OPA_TRANSP, 0);

    // 3. 添加具体的 Tile
    tile_main = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_HOR | LV_DIR_VER); 

//    lv_obj_set_style_bg_color(tile_main,lv_color_hex(0xF87B7B),LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(tile_main, 100, LV_PART_MAIN);
}
const ui_app_message_t APP_LA_TABLE[APP_TEMP]=
{
     [APP_SETTING]=
     {
             .icon=LV_SYMBOL_SETTINGS,
             .text=
             {
                     [ENGLISH]="Setting",
                     [CHINESE]="设置",
             }
     },
     [APP_FILE]=
     {
             .icon=LV_SYMBOL_FILE,
             .text=
             {
                     [ENGLISH]="File",
                     [CHINESE]="文件",
             }
     }, 
     [APP_NET]=
     {
             .icon=LV_SYMBOL_LOOP,
             .text=
             {
                     [ENGLISH]="Net",
                     [CHINESE]="网络助手",
             }
     }, 
     [APP_WEATHER]=
     {
             .icon=LV_SYMBOL_UPLOAD,
             .text=
             {
                     [ENGLISH]="Weather",
                     [CHINESE]="天气",
             }
     }, 
     [APP_CLOCK]=
     {
             .icon=LV_SYMBOL_BELL,
             .text=
             {
                     [ENGLISH]="Clock",
                     [CHINESE]="时钟",
             }
     }, 
     [APP_VIDEO]=
     {
             .icon=LV_SYMBOL_VIDEO,
             .text=
             {
                     [ENGLISH]="Video",
                     [CHINESE]="视频",
             }
     }, 
        
};

/******************************************************
函数:创建界面
参数:父对象
返回:无
注意:可设置多个app
******************************************************/
lv_obj_t* ui_page_creat_app(lv_obj_t *parent)
{
    lv_obj_t *page=lv_obj_create(parent);
    lv_obj_set_size(page,lv_pct(100),lv_pct(100));
    lv_obj_set_style_pad_all(page, 0, 0);
    lv_obj_set_style_border_width(page, 0, 0);
    
    lv_obj_set_style_bg_opa(page, 100, 0); 
    lv_obj_set_style_radius(page,0,0);
//    lv_obj_set_style_bg_color(page,lv_color_hex(0x1A6120),LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(page, "0:/icon/100.png", 0);
    
    lv_obj_set_style_bg_img_opa(page, 100, 0); // 确保图片完全显示
    lv_obj_set_style_bg_img_tiled(page, false, 0);      // 设为 false，图片不平铺，而是拉伸或居中

    uint16_t Xpos=10,Ypos=10;
    uint8_t row=0;
    
    for(uint8_t i=0;i<APP_TEMP;i++)
    {
        Xpos=(i-(row*4))*60;
        if(Xpos>=scr_width)
        {
            Xpos=0;
            row++;
            Ypos=row*70;
        }
      lv_obj_t *btn_app=ui_create_app(page,APP_LA_TABLE[i].icon,APP_LA_TABLE[i].text[display_cfg.language]); 
      lv_obj_add_event_cb(btn_app, event_ui_page_btn_app, LV_EVENT_CLICKED, (void *)i); 
      lv_obj_align(btn_app,LV_ALIGN_TOP_LEFT,Xpos,Ypos);  
    }
    return page;
}
static void event_ui_page_btn_app(lv_event_t *e)
{
    /**根据不同的用户输入数据,判断不同的app响应**/
  UI_APP_ENUM app=(UI_APP_ENUM)lv_event_get_user_data(e);
	ui_goto_page(PAGE_APP_LIST,app);
	Cur_APP=app;
}
/******************************************************
函数:页面跳转函数
参数:1.要跳转的页面,2.页面的app按钮枚举
返回:无
注意:根据相应app按钮创建相应的app list,比如:设置按钮,会生成list,里面有各种设置项
        并且点击每个设置项都有相应的详情页
        APP 是事件回调函数传入的用户数据,每个app都有独自的
******************************************************/
void ui_goto_page(UI_APP_PAGE_ENUM Page,UI_APP_ENUM APP)
{
        lv_obj_clean(tile_main);
        Clock_time_widget.meter = NULL;
        timer_widget.timer_obj=NULL;

    switch(Page)
    {
        case PAGE_HOME:
          {
                ui_page_creat_app(tile_main);list_wifi=NULL;/**在主界面创建app图标按钮**/
                Cur_Page = PAGE_HOME;/** 设当前页为主页面**/
                break;
          }
        case PAGE_APP_LIST:/** 列表页**/
            {
                Cur_Page = PAGE_APP_LIST;
                      switch(APP)
                      {
                           case APP_SETTING:   ui_app_setting_create_list(tile_main);list_wifi=NULL;   break;/**创建wifi **/
                           case APP_FILE:      ui_app_file_list_create(tile_main,Cure_Path);break;/** 创建文件列表**/
                           case APP_NET:      ui_app_net_list_creat(tile_main); break;
                           case APP_WEATHER: ui_app_weather_create(tile_main);break;
                           case APP_CLOCK   : ui_app_clock_creat(tile_main);break;
                           case APP_VIDEO :ui_app_video_list_creat(tile_main);break;
                           default:break;
                      }
              break;
            }
        case PAGE_APP_DETAIL:
            {/**详情页(page_detail_create函数可以自行设置,只需返回界面就行,传入的对象作为父对象就行) **/
                Cur_Page = PAGE_APP_DETAIL;
                 switch(APP)
                {
                    case APP_SETTING:   ui_app_setting_create_detail(tile_main);            break;
                    case APP_FILE:      ui_app_file_detail_create(tile_main,Cure_Path);       break;
                    
                    default:  break;
                }
                default:break;
               }
    }
}


 void btn_back(void)
 {
	char *end_dir=NULL;
  switch(Cur_APP)
  {
    case APP_TEMP:
      {
//        ui_goto_page(PAGE_HOME,APP_TEMP);
        break;
      }
    case APP_SETTING:
      {
        switch(Cur_Page)
          {
          case PAGE_APP_LIST:ui_goto_page(PAGE_HOME,APP_TEMP);break;
          case PAGE_APP_DETAIL:ui_goto_page(PAGE_APP_LIST,APP_SETTING);break;
//          case PAGE_HOME:ui_goto_page(PAGE_HOME,APP_TEMP);break;
                        default:break;
        }

      }break;
    case APP_FILE:

      if(strcmp(Cure_Path,"0:")==0)
      {
        ui_goto_page(PAGE_HOME,APP_TEMP);
      }
      else
        {
        end_dir=strrchr(Cure_Path,'/');
        if(end_dir)	*end_dir='\0';
//        printf("Come out ( %s )\r\n",Cure_Path);
        ui_goto_page(PAGE_APP_LIST,APP_FILE);
        }
        break;
    case APP_NET:
        {
                switch(Cur_Page)
          {
            case PAGE_APP_LIST: ui_goto_page(PAGE_HOME,APP_TEMP);break;
            case PAGE_APP_DETAIL:  ui_app_net_list_creat(tile_main);break;
            default:break;            
          }
            break;
    case APP_WEATHER:
        {
          ui_goto_page(PAGE_HOME,APP_TEMP);break;  
        }

        case APP_CLOCK:
            {
             ui_goto_page(PAGE_HOME,APP_TEMP);break;  
            }
        }
      }
 }
