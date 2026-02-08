/*--------------------------------------------------------------------------------↓
	@项目	: app设置的选项(显示设置)
	@日期	: 2026-1-24
	@备注	: 目前显示设置有:亮度设置 语言设置 字体大小 屏幕刷新方向
        
↑--------------------------------------------------------------------------------*/
#include "setting_display.h"

static void event_ui_app_setting_display_cb(lv_event_t*e); /*下拉框回调函数*/

static void ui_app_setting_display_scr_msg(lv_obj_t*parent);/*对话框*/
static void event_app_setting_scr_msg_cb(lv_event_t*e);/*对话框回调函数*/
static void display_set_scr_dir(uint16_t scr_dir); /*设置屏幕方向*/

static void display_config(lv_obj_t*parent,uint8_t index);/*根据配置设置对应的下拉框的索引*/

void set_global_font(const lv_font_t *font);/*全局字体*/
void ui_font_cn_load(DISPLAY_FONT_SIZE_ENUM size);/*加载中文*/
static void free_font(DISPLAY_LANGUAGE_ENUM Last_language,DISPLAY_FONT_SIZE_ENUM Last_size);/*设置语言和字体*/


static void display_brightness_creat(lv_obj_t*parent);/*创建亮度滑动条*/
static void event_brightness_slider_cb(lv_event_t*e); /*滑动条回调函数*/
 
 
ui_font_t ui_fonts;


/*--------------------------------------------------------------------------------↓
	@函数	  : 创建显示设置
	@参数	  : 父对象
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
void ui_app_setting_display(lv_obj_t *parent)
{
    lv_obj_t*list_display=lv_list_create (parent); /*创建显示列表*/

    lv_obj_set_size(list_display, lv_pct(100), lv_pct(100)); /*设置列表大小为父对象100%*/
    lv_obj_set_style_border_width(list_display, 0, 0);/*设置边框宽度为0*/
    lv_obj_set_style_radius(list_display, 0, 0); /*圆角为0*/
    lv_obj_set_style_pad_all(list_display, 0, 0);/*对象之间的间隔为0*/
    
    lv_obj_set_scrollbar_mode(list_display, LV_SCROLLBAR_MODE_OFF); /*禁止滚动*/
    lv_obj_set_scroll_dir(list_display, LV_DIR_VER);/*允许纵向滑动*/               

     lv_list_add_text (list_display, _GET_UI_TEXT(APP_SET_DISPLAY_LA_TABLE,DISPLAY_BRIGHT_TITLE));/*添加标题(亮度控制)*/
     display_brightness_creat(list_display);/*在列表里创建亮度设置滑动条*/
     
         lv_obj_t*droplist;/*下拉列表*/
        for(uint8_t i=1;i<SETTING_DISPLAY_NUM-2;i+=2)
        {
            lv_list_add_text (list_display, _GET_UI_TEXT(APP_SET_DISPLAY_LA_TABLE,i));/*在循环里面添加标题(语言,字体大小,屏幕方向)*/  
            droplist=lv_dropdown_create(list_display);/*在循环里面添加(语言,字体大小,屏幕方向)*/
            lv_obj_set_style_radius(droplist, 0, 0);/*无圆角*/

            lv_dropdown_set_options(droplist,_GET_UI_TEXT(APP_SET_DISPLAY_LA_TABLE,i+1));/*给下拉列表添加选项*/
            lv_obj_set_size(droplist,lv_pct(100),lv_pct(12)); 
            lv_obj_add_event_cb(droplist,event_ui_app_setting_display_cb,LV_EVENT_VALUE_CHANGED,(void*)(i+1));/*添加事件*/
            display_config(droplist,i+1);/*通过配置来设置相应的下拉列表选项*/      
        }
}

/*--------------------------------------------------------------------------------↓
	@函数	  :显示设置的下拉框选项设置
	@参数	  :parent:父对象
           index:对应的下拉列表
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
 static void display_config(lv_obj_t*parent,uint8_t index)
 {
     uint8_t sel = 0;
     switch(index)
     {
        case DISPLAY_LANGUAGE_CUR: /*语言*/
        sel = display_cfg.language;
        break;

        case DISPLAY_WORD_CUR:/*字体大小*/
        sel = display_cfg.font_size;
        break;

        case DISPLAY_SCR_CUR: /*屏幕方向*/
        sel = display_cfg.scr_dir;
        break;

        default:
        sel = 0;
        break;
     }
     lv_dropdown_set_selected(parent, sel); /*根据配置设置对象的默认选项*/
 }
 
/*************************************************************亮度*************************************************************/


/*--------------------------------------------------------------------------------↓
	@函数	  :创建改变亮度的滑动条
	@参数	  :父对象
	@返回值 :无
	@备注	  :
↑--------------------------------------------------------------------------------*/
 static void display_brightness_creat(lv_obj_t*parent)
 {
     lv_obj_t*bright_slider=lv_slider_create(parent);/*创建滑动条*/

      lv_obj_set_size(bright_slider,lv_pct(100),lv_pct(8)); 
      lv_slider_set_range(bright_slider,0,100);/*设置范围0~100*/
      lv_slider_set_value(bright_slider,display_cfg.Brightness,LV_ANIM_ON);/*根据配置设置初始滑动条值*/
      lv_obj_set_style_opa(bright_slider,0,LV_PART_KNOB);/*将滑动条的按钮去掉*/

      lv_obj_t*bringht_label=lv_label_create(bright_slider);/*在滑动条里面添加一个亮度值显示*/
      lv_obj_align_to(bringht_label,bright_slider,LV_ALIGN_CENTER,0,0);/*位于滑动条中间*/
      lv_obj_set_style_text_color(bringht_label,lv_color_hex(0x000000),LV_PART_MAIN);/*亮度值文本颜色:黑色*/
      lv_label_set_text_fmt(bringht_label,"%d",display_cfg.Brightness);/*根据配置显示初始亮度值*/
      
      lv_obj_add_event_cb(bright_slider,event_brightness_slider_cb,LV_EVENT_VALUE_CHANGED,NULL);/*滑动条的值改变*/
 }
 
 static void event_brightness_slider_cb(lv_event_t*e)
 {
     lv_obj_t*target=lv_event_get_target(e);/*对象为滑动条*/
     int32_t Bright_Value=lv_slider_get_value(target);/*获取滑动条的值*/
     lv_obj_t*label=lv_obj_get_child(target,0);/*得到子对象(亮度文本)*/
     lv_label_set_text_fmt(label,"%d",Bright_Value);/*设置相应的值*/
     //设置亮度
     #if keil
     TIM12->CCR2=Bright_Value;/*改变定时器的占空比*/
     #endif
     display_cfg.Brightness=Bright_Value;/*更新亮度配置*/
 }
/*************************************************************屏幕方向*************************************************************/


/*--------------------------------------------------------------------------------↓
	@函数	  :  下拉列表值改变的事件函数
	@参数	  :     无
	@返回值 :     无
	@备注	  :
↑--------------------------------------------------------------------------------*/
static void event_ui_app_setting_display_cb(lv_event_t*e)
 {
    lv_obj_t*droplist=lv_event_get_target(e);/*对应的下拉框(语言,字体大小,屏幕方向)*/
    lv_obj_t *list=lv_obj_get_parent(droplist);/*获得父对象列表*/
    
    lv_obj_t *page=lv_obj_get_parent(list); /*获得父对象页面*/
    SETTING_DISPLAY_ENUM list_bnt=(SETTING_DISPLAY_ENUM)lv_event_get_user_data(e);/*根据用户传入的值(语言,字体大小,屏幕方向)*/
    
    uint16_t droplist_num=lv_dropdown_get_selected(droplist);/*获得下拉列表的选中的选项*/
    switch(list_bnt)
    {
        case DISPLAY_WORD_CUR: /*字体*/
            {
                display_cfg.font_size=(DISPLAY_FONT_SIZE_ENUM)droplist_num;/*更新字体大小配置*/
                ui_set_language(display_cfg.language,display_cfg.font_size); /*根据对应语言设置字体大小*/ 
               break; 
            } 
        case DISPLAY_SCR_CUR: /*屏幕方向*/
           {                
                ui_app_setting_display_scr_msg(page);/*先弹出来对话框*/
                display_set_scr_dir(droplist_num);/*设置对应的屏幕方向*/
                display_cfg.scr_dir=(DISPLAY_SCR_DIR_ENUM)droplist_num;/*更新屏幕方向配置*/
               break;
           }
       case DISPLAY_LANGUAGE_CUR:/*语言*/
           {
                display_cfg.language=(DISPLAY_LANGUAGE_ENUM)droplist_num;/*更新语言配置*/
                ui_set_language(display_cfg.language,display_cfg.font_size); /*根据字体大小设置对应语言*/
                
                lv_obj_del(alarm_msgbox_obj);
                alarm_rem_win(lv_scr_act());
                break;
           }
        default :break;
    }
 }
 

 /*--------------------------------------------------------------------------------↓
 	@函数	  :  对话框
 	@参数	  :  父对象
 	@返回值 :  无
 	@备注	  :  改变屏幕方向会给出提示对话框:可能要重新配置触摸坐标
 ↑--------------------------------------------------------------------------------*/ 
static void ui_app_setting_display_scr_msg(lv_obj_t*parent)
{
    static const char *btn_matrix[3];/*对话框的矩阵按钮*/
    btn_matrix[0]=DISPLAY_SCR_MSG_LA_TABLE[MSG_BTN_CONTINUE].text[display_cfg.language];/*继续*/
    btn_matrix[1]=DISPLAY_SCR_MSG_LA_TABLE[MSG_BTN_CANCEL].text[display_cfg.language];/*取消*/
    btn_matrix[2]="";
    /*设置对话框的文本和矩阵按钮,取消关闭按钮(按下取消按钮就可以)*/
    lv_obj_t*msgbox=lv_msgbox_create(parent,DISPLAY_SCR_MSG_LA_TABLE[MSG_TITLE].text[display_cfg.language],DISPLAY_SCR_MSG_LA_TABLE[MSG_TEXT].text[display_cfg.language],btn_matrix,false);
    lv_obj_t*msgbox_btn=lv_msgbox_get_btns(msgbox);/*获取按钮*/
    lv_obj_add_event_cb(msgbox_btn,event_app_setting_scr_msg_cb,LV_EVENT_CLICKED,NULL); /*添加按钮点击事件*/
    
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 对话框按钮按下事件
	@参数	  : 无
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
static void event_app_setting_scr_msg_cb(lv_event_t*e)
{
    lv_obj_t *msgbox_btn = lv_event_get_current_target(e);/*获取对应按钮*/
    uint8_t btn_id=lv_btnmatrix_get_selected_btn(msgbox_btn) ;/*获取按下按钮的索引*/
    lv_obj_t*msgbox=lv_obj_get_parent(msgbox_btn); /*根据按钮的到父对象对话框*/

    Cure_Mode=APP_SETTING_DISPLAY; /*设置当前模式为显示模式*/
     if(btn_id==0)
     {
//         lv_obj_add_flag(msgbox,LV_OBJ_FLAG_HIDDEN);/*如果按下继续按钮就隐藏对话框*/
         lv_obj_del(msgbox);
     }
     else if(btn_id==1)
     {
//          lv_obj_add_flag(msgbox,LV_OBJ_FLAG_HIDDEN);/*如果按下继续按钮就隐藏对话框*/
//         ui_goto_page(PAGE_APP_DETAIL,APP_SETTING);/*按取消就返回对应的页面(根据Cure_Mode)*/
            lv_obj_del(msgbox);
     }
}

/*--------------------------------------------------------------------------------↓
	@函数	  :  设置屏幕方向
	@参数	  :  四个屏幕方向
	@返回值 :  无
	@备注	  :  改变的的硬件的扫描方向,但是也要改lvgl的扫描尺寸,也就是长和高互换
↑--------------------------------------------------------------------------------*/
static void display_set_scr_dir(uint16_t scr_dir)
{
    #if keil
    lv_disp_t *disp=lv_disp_get_default();/*获取默认显示*/
    switch(scr_dir)
    {
    case 0: /*正常竖向*/
    {
//        printf("%x\r\n",0);
      LCD_Set_Direction(Dir_Ver);/*设置正常竖向*/
      disp->driver->hor_res=240; 
      disp->driver->ver_res=320;
      break;
    }
    case 1:
    {
//        printf("%x\r\n",Dir_Rev_Ver);
      LCD_Set_Direction(Dir_Rev_Ver);/*设置相反竖向*/
      disp->driver->hor_res=240;
      disp->driver->ver_res=320;
      break;
    }
    case 2:
    {
//        printf("%x\r\n",Dir_Hor);
      LCD_Set_Direction(Dir_Hor);/*设置正常横向*/
      disp->driver->hor_res=320;
      disp->driver->ver_res=240;
      break;
    }
    case 3:
    {
//        printf("%x\r\n",Dir_Rev_Hor);  
    LCD_Set_Direction(Dir_Rev_Hor);/*设置相反竖向*/
    disp->driver->hor_res=320;
    disp->driver->ver_res=240;
    break;
    }
    default :break;
    }
    lv_disp_drv_update(lv_disp_get_default(),disp->driver);/*更新lvgl的扫描尺寸*/
    if(XPT2046_Get_Adjdata()) /*如果为1就是校准过了对应方向,否则就进入校准状态,校准过后就会自动保存在w25q的对应位置*/
    {
//    printf("%x已校准\r\n",Cur_scr_dir);
    return ;//已经校准
    }
    else
    {
//    printf("校准%x\r\n",Cur_scr_dir);
    TP_Adjust();  	//屏幕校准
    }
    #endif 
 }
/*************************************************************语言&字体设置*************************************************************/ 

LV_FONT_DECLARE( Font_CN_12);
/*--------------------------------------------------------------------------------↓
	@函数	  :  设置对应语言和字体大小
	@参数	  :  language:语言
             size    :字体大小
	@返回值 :  无
	@备注	  :
↑--------------------------------------------------------------------------------*/
void ui_set_language(DISPLAY_LANGUAGE_ENUM language,DISPLAY_FONT_SIZE_ENUM size)
{
  display_cfg.font_size=size; /*更新字体大小配置*/
  static DISPLAY_LANGUAGE_ENUM Last_language;/*上一次的语言*/
  static DISPLAY_FONT_SIZE_ENUM Last_Size;/*上一次的字体大小*/
  free_font(Last_language,Last_Size); /*释放上一次的语言配置*/
  
	switch(language)
	{
		case ENGLISH:/*英文*/
		    {
            switch(size) /*根据对应的语言和大小来设置全局字体大小*/
                {
                    case FONT_SIZE_12:set_global_font(&lv_font_montserrat_12);break;
                    case FONT_SIZE_14:set_global_font(&lv_font_montserrat_14);break;
                    case FONT_SIZE_16:set_global_font(&lv_font_montserrat_16);break;  
                    default :break;             
                }
		        display_cfg.language=ENGLISH; /*更新配置为英文*/

		        break;
		    }
		case CHINESE:
		    {
		        lv_font_t*font;
                ui_font_cn_load(size);/*加载对应的中文和大小(保存咋sd卡)*/
//                lv_font_t * my_font = lv_font_load("0:/GitHub_Code/My_STM32_Phone/SD/my_cn_font/Font_CN_12.bin");
//                my_font->fallback=&lv_font_montserrat_12;
                switch(size)
                {
//                    case FONT_SIZE_12:font=ui_fonts.FONT_SIZE_12;break; 
                    case FONT_SIZE_12:font=&Font_CN_12;break; 
//                   case FONT_SIZE_12:font=my_font;break; 
                    case FONT_SIZE_14:font=ui_fonts.FONT_SIZE_14;break;
                    case FONT_SIZE_16:font=ui_fonts.FONT_SIZE_16;break; 
//                    case FONT_SIZE_16:font=&myFont_16;break; 
                    default :break;             
                }       
                if(font!=NULL)
                    {
//                        printf("size:%d\r\n",size);
                       set_global_font(font);/*设置为全局*/
                    }
                        display_cfg.language=CHINESE;/*更新配置为中文*/               
                        break;
		    }
        default:break;
	}
  Last_language=language;/*将此次的语言和大小记录方便下一次改变来释放*/
  Last_Size=size;
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 加载对应中文
	@参数	  : 字体大小
	@返回值 :  无
	@备注	  :
↑--------------------------------------------------------------------------------*/
void ui_font_cn_load(DISPLAY_FONT_SIZE_ENUM size)
{
            switch(size)
                {
                   case FONT_SIZE_12: 
                       {
                           ui_fonts.FONT_SIZE_12=lv_font_load (FONT_Size_12_PATH);/*根据路径加载字体12*/
                           if(ui_fonts.FONT_SIZE_12==NULL) printf("use FONT_SIZE_12 to error \r\n"); /*如果加载失败就打印*/
                           else  ui_fonts.FONT_SIZE_12->fallback=&lv_font_montserrat_12;/*英文对应内置的大小*/
                           break;
                       }
                   case FONT_SIZE_14: 
                       {   
                           ui_fonts.FONT_SIZE_14=lv_font_load (FONT_Size_14_PATH);/*根据路径加载字体14*/
                           if(ui_fonts.FONT_SIZE_14==NULL) printf("use FONT_SIZE_14 to error \r\n"); /*如果加载失败就打印*/
                           else  ui_fonts.FONT_SIZE_14->fallback=&lv_font_montserrat_14;/*英文对应内置的大小*/   
                           break;
                       }
                       
                   case FONT_SIZE_16: 
                       {
                           ui_fonts.FONT_SIZE_16=lv_font_load (FONT_Size_16_PATH);/*根据路径加载字体16*/
                           if(ui_fonts.FONT_SIZE_16==NULL) printf("use FONT_SIZE_16 to error \r\n"); /*如果加载失败就打印*/
                           else  ui_fonts.FONT_SIZE_16->fallback=&lv_font_montserrat_16;/*英文对应内置的大小*/
                           break;           
                       }  
                }
}


/*--------------------------------------------------------------------------------↓
	@函数	  :  将当前的字体设置为全局
	@参数	  :  加载到的字体(也可以为内置的字体)
	@返回值 :  无
	@备注	  :
↑--------------------------------------------------------------------------------*/
void set_global_font(const lv_font_t *font)
{
    static lv_style_t default_style;/*设置样式*/
    lv_style_init(&default_style);/*初始化*/
    lv_style_set_text_font(&default_style, font);/*设置文本字体*/
    lv_obj_add_style(lv_scr_act(), &default_style, 0);/*添加为默认样式*/
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 释放上一次的字体
	@参数	  : 中文 字体大小
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
static void free_font(DISPLAY_LANGUAGE_ENUM Last_language,DISPLAY_FONT_SIZE_ENUM Last_size)
{
    if(Last_language==CHINESE)
    {
        switch(Last_size)
        {
        case FONT_SIZE_12:lv_font_free(ui_fonts.FONT_SIZE_12);break;
        case FONT_SIZE_14:lv_font_free(ui_fonts.FONT_SIZE_14);break;        
        case FONT_SIZE_16:lv_font_free(ui_fonts.FONT_SIZE_16);break;
        default :break;      
        }
    }
}

/*--------------------------------------------------------------------------------↓
	@函数	  :  单独设置文本字体
	@参数	  :  父对象,字体大小
	@返回值 :  无
	@备注	  :  用的很少,需要单独使用的时候调用,不设置的话都是默认字体大小
↑--------------------------------------------------------------------------------*/
void ui_set_obj_text_font(lv_obj_t*parent,DISPLAY_FONT_SIZE_ENUM FONT_SIZE)
{
    switch(display_cfg.language)                       /*根据参数设置大小*/
    {
    case ENGLISH:
        {
                switch(FONT_SIZE)
            {
                case FONT_SIZE_12: lv_obj_set_style_text_font(parent,&lv_font_montserrat_12,LV_STATE_DEFAULT); break;
                case FONT_SIZE_14: lv_obj_set_style_text_font(parent,&lv_font_montserrat_14,LV_STATE_DEFAULT); break;
                case FONT_SIZE_16: lv_obj_set_style_text_font(parent,&lv_font_montserrat_16,LV_STATE_DEFAULT); break;
                case FONT_SIZE_24: lv_obj_set_style_text_font(parent,&lv_font_montserrat_24,LV_STATE_DEFAULT); break;
                case FONT_SIZE_32: lv_obj_set_style_text_font(parent,&lv_font_montserrat_32,LV_STATE_DEFAULT); break; 
                default:break;                   
            }
            break;
        }
    case CHINESE:
        {
                    switch(FONT_SIZE)
            {
                case FONT_SIZE_12: lv_obj_set_style_text_font(parent,ui_fonts.FONT_SIZE_12,LV_STATE_DEFAULT); break;
                case FONT_SIZE_14: lv_obj_set_style_text_font(parent,ui_fonts.FONT_SIZE_14,LV_STATE_DEFAULT); break;
                case FONT_SIZE_16: lv_obj_set_style_text_font(parent,ui_fonts.FONT_SIZE_16,LV_STATE_DEFAULT); break;   
                default:break;           
            }
            break;
        }
        default:break;
    }

}
