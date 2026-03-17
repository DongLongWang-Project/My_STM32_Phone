/*--------------------------------------------------------------------------------↓
	@项目	:  app (文件系统)
	@日期	:
	@备注	:
↑--------------------------------------------------------------------------------*/
#include "ui_app_file.h"
#include "../app_music/ui_app_music.h"

static FILE_TYPE_ENUM get_file_type(const char *path);
static void other_file_message(lv_obj_t*parent);
static void event_ui_app_file_cb(lv_event_t *e);

static void event_file_close_cb(lv_event_t*e);
lv_obj_t* ui_file_switch_page_create(lv_obj_t*parent);
static void event_switch_file_page_cb(lv_event_t*e);


char FILE_BUF[2][FILE_NAME_MAX_LEN];

#if keil 
char Cure_Path[Dir_MAX_LEN]={"0:"};
#else
char Cure_Path[Dir_MAX_LEN]={"0:/GitHub_Code/My_STM32_Phone/SD"};
#endif // keil

file_dir_t file_dir;
file_switch_page_t file_switch_page;
char name_buf[SAVE_FILE_NAME_NUM][FILE_NAME_MAX_LEN]__attribute__((section(".EXT_SRAM")));

/*--------------------------------------------------------------------------------↓
	@函数	  : 创建文件夹的目录
	@参数	  : 父对象,当前路径
	@返回值 : 创建的列表
	@备注	  :
↑--------------------------------------------------------------------------------*/
lv_obj_t* ui_app_file_list_create(lv_obj_t *parent,const char * path)
{       
    file_dir.list= lv_list_create(parent); /*文件目录(以列表形式)*/
    file_switch_page.obj=ui_file_switch_page_create(parent);
    file_switch_page.next_page_btn=lv_obj_get_child(file_switch_page.obj,1);
    file_switch_page.pre_page_btn=lv_obj_get_child(file_switch_page.obj,0);
    file_switch_page.page_label=lv_obj_get_child(file_switch_page.obj,2);
    file_switch_page.cur_page=1;

    lv_label_set_text( file_switch_page.page_label,"第1页");
    


        lv_fs_res_t res=lv_fs_dir_open(&file_dir.dir,path);  /*打开目录的文件夹*/
        if(res!=LV_FS_RES_OK)/*失败就打印返回*/
        {
            printf("open %s  is fail",path);
            
        }  
    

    lv_obj_set_size(file_dir.list, lv_pct(100), lv_pct(80));  
    lv_obj_set_style_border_width(file_dir.list, 0, 0);
    lv_obj_set_style_radius(file_dir.list, 0, 0);
    lv_obj_set_style_pad_all(file_dir.list, 0, 0);
    
    lv_obj_set_size(file_switch_page.obj, lv_pct(100), lv_pct(20));  
    lv_obj_set_style_border_width(file_switch_page.obj, 0, 0);
    lv_obj_set_style_radius(file_switch_page.obj, 0, 0);
    lv_obj_set_style_pad_all(file_switch_page.obj, 0, 0);
   
    lv_obj_align( file_dir.list,LV_ALIGN_TOP_MID,0,0);
    lv_obj_align_to(file_switch_page.obj,file_dir.list,LV_ALIGN_OUT_BOTTOM_MID,0,0);
    
    
    lv_obj_set_scrollbar_mode(file_dir.list, LV_SCROLLBAR_MODE_OFF); 
    lv_obj_set_scroll_dir(file_dir.list, LV_DIR_VER); 
    
    lv_list_add_text(file_dir.list, path);/*添加标题(路径)*/
    

    memset(name_buf,0,sizeof(name_buf));
    file_dir.timer=lv_timer_create(load_dir_timer,10,NULL);
//    lv_timer_resume(file_dir.timer);

    lv_obj_add_event_cb(file_dir.list,event_file_close_cb,LV_EVENT_DELETE,NULL);
    return file_dir.list;/*打开文件夹创建文件按钮*/
}


/*--------------------------------------------------------------------------------↓
	@函数	  :  打开文件夹创建内部目录文件按钮
	@参数	  :  父对象,路径
	@返回值 :  父对象
	@备注	  :
↑--------------------------------------------------------------------------------*/
void load_dir_timer(lv_timer_t *timer)
{
    static uint8_t read_num=0;
    char *buf=name_buf[read_num];
    if(read_num<SAVE_FILE_NAME_NUM )
    {
        lv_fs_res_t res=lv_fs_dir_read(&file_dir.dir,buf);/*读取文件夹的内容*/
        if(res!=LV_FS_RES_OK || buf[0]=='\0') 
        {
             if(file_dir.timer!=NULL)
            {
            lv_timer_del(file_dir.timer); 
            file_dir.timer=NULL; 
            }
            
//             lv_timer_pause(file_dir.timer);
            if(file_dir.dir.drv!=NULL)
            {
            lv_fs_dir_close(&file_dir.dir); /*关闭文件夹*/  
            file_dir.dir.drv=NULL;
            }

            file_switch_page.total_file_num=read_num;
            if(read_num%SHOW_FILE_MAX_NUM)
            {
                file_switch_page.total_page_num=file_switch_page.total_file_num/SHOW_FILE_MAX_NUM+1;
            }
            else
            {
                file_switch_page.total_page_num=file_switch_page.total_file_num/SHOW_FILE_MAX_NUM;
            }
           if(file_switch_page.total_page_num>1)
            {
               lv_obj_clear_flag(file_switch_page.next_page_btn,LV_OBJ_FLAG_HIDDEN);         
            }
            read_num=0; 
            printf("文件数量:%d\r\n",file_switch_page.total_file_num);
            printf("页数:%d\r\n",file_switch_page.total_page_num); 
        }
        else
        {
            
//            printf("文件后缀名:%s\r\n",);
              if(read_num<SHOW_FILE_MAX_NUM)
                {

                  if(strlen(buf)==1 || strcmp(buf,"/System Volume Information")==0)
                  {
                    return;
                  }
                  else if(timer->user_data!=NULL)
                  {
                      if(strcmp(lv_fs_get_ext(buf),timer->user_data)!=0) 
                      {
                            return;
                      }
                      else
                      {
                      if( buf[0]=='/')
                      {
                              file_dir.btn = lv_list_add_btn(file_dir.list, LV_SYMBOL_DIRECTORY, buf); /*  /开头为文件夹*/ 
                              }
                              else 
                              {
                              file_dir.btn = lv_list_add_btn(file_dir.list, LV_SYMBOL_FILE, buf);/*其他为文件*/
                              }
                              lv_obj_add_event_cb(file_dir.btn, event_ui_app_file_cb, LV_EVENT_CLICKED, NULL);/*添加事件*/
                      }
                  }
                  else
                  {
                      if( buf[0]=='/')
                      {
                      file_dir.btn = lv_list_add_btn(file_dir.list, LV_SYMBOL_DIRECTORY, buf); /*  /开头为文件夹*/ 
                      }
                      else 
                      {
                      file_dir.btn = lv_list_add_btn(file_dir.list, LV_SYMBOL_FILE, buf);/*其他为文件*/
                      }
                      lv_obj_add_event_cb(file_dir.btn, event_ui_app_file_cb, LV_EVENT_CLICKED, NULL);/*添加事件*/                        
                  }  
                }
                read_num++; 

                  
         }
    }
    else
    {
         if(file_dir.timer!=NULL)
        {
        lv_timer_del(file_dir.timer); 
        file_dir.timer=NULL; 
        }
//        timer->user_data=NULL;
//         lv_timer_pause(file_dir.timer);
        if(file_dir.dir.drv!=NULL)
        {
        lv_fs_dir_close(&file_dir.dir); /*关闭文件夹*/  
        file_dir.dir.drv=NULL;
        }

        file_switch_page.total_file_num=read_num;
        if(read_num%SHOW_FILE_MAX_NUM)
        {
            file_switch_page.total_page_num=file_switch_page.total_file_num/SHOW_FILE_MAX_NUM+1;
        }
        else
        {
            file_switch_page.total_page_num=file_switch_page.total_file_num/SHOW_FILE_MAX_NUM;
        }
           if(file_switch_page.total_page_num>1)
            {
               lv_obj_clear_flag(file_switch_page.next_page_btn,LV_OBJ_FLAG_HIDDEN);         
            }
            read_num=0;  
//        printf("文件数量:%d\r\n",file_switch_page.total_file_num);
//        printf("页数:%d\r\n",file_switch_page.total_page_num); 
    }
     

}
/*--------------------------------------------------------------------------------↓
	@函数	  : 点击文件夹或者文件的事件回调函数
	@参数	  : 无
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
static void event_ui_app_file_cb(lv_event_t *e)
{
	lv_obj_t*target=lv_event_get_target(e); /*文件按钮*/
	lv_obj_t *list= lv_obj_get_parent(target); /*得到父对象列表*/
	const char *btn_txt=lv_list_get_btn_text( list,target);/*得到按钮名称*/

	char path[sizeof(Cure_Path)];
    snprintf(path, sizeof(path), "%s", Cure_Path); /*现将当前路径添加到缓存区path*/
		if(btn_txt[0]=='/') /*如果是文件夹*/
			{
				snprintf(Cure_Path, sizeof(Cure_Path), "%s%s", path, btn_txt);/*将当前的按钮名称和缓冲区合并添加到当前路径*/
				ui_goto_page(PAGE_APP_LIST,APP_FILE);   /*进入新的文件夹*/
			}
		else  /*是文件*/
			{
				snprintf(Cure_Path, sizeof(Cure_Path), "%s/%s", path, btn_txt);  /*将当前的按钮名称和缓冲区合并添加到当前路径*/
				ui_goto_page(PAGE_APP_DETAIL,APP_FILE);  /*打开文件*/
			}
			printf("Enter in ( %s )\r\n",Cure_Path);/*打印路径*/	
}




/*--------------------------------------------------------------------------------↓
	@函数	  : 打开文件
	@参数	  : 父对象 文件路径
	@返回值 : 页面
	@备注	  :
↑--------------------------------------------------------------------------------*/
lv_obj_t* ui_app_file_detail_create(lv_obj_t *parent, const char *file_path)
{
    lv_obj_t*page = lv_obj_create(parent);/*创建文件的页面*/
    lv_obj_set_size(page, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(page, 0, 0);
	lv_obj_clear_flag (page, LV_OBJ_FLAG_SCROLLABLE);
	

        FILE_TYPE_ENUM FILE_TYPE=get_file_type(file_path); /*获得文件类型*/
    // 5. 根据文件类型创建控件
    switch(FILE_TYPE) {
        case FILE_LRC :
        case File_TXT:
        case File_C:
        case File_H: {
            // 文本文件
            operation_file_txt(page,file_path); /*文本文件 c文件 h文件 都可以直接打开*/
            break;
        }
        case File_PNG: {
            // 图片文件

            operation_file_img(page,file_path); /*打开图像文件*/
            break;
        }
        case File_RGB: {
            // 图片文件
            ui_app_video_detail_creat(page,file_path);
            break;
        }
        case File_WAV:
            ui_app_music_detail_creat(page,file_path);
            break;
        default:

            other_file_message(page); /*其他文件*/
            // 未知文件类型，可显示提示
            break;
    }


    return page;
}


/*--------------------------------------------------------------------------------↓
	@函数	  :  获得文件类型
	@参数	  :  路径
	@返回值 :  文件类型枚举
	@备注	  :
↑--------------------------------------------------------------------------------*/
static FILE_TYPE_ENUM get_file_type(const char *path)
{
    const char *ext = strrchr(path, '.');/*找到路径最后的.和后面的字符*/
    if(!ext) return File_Unknow;
    if(strcmp(ext, ".txt") == 0) return File_TXT;/*文本文件*/
    if(strcmp(ext, ".c")   == 0) return File_C; /*c文件*/
    if(strcmp(ext, ".h")   == 0) return File_H; /*h文件*/
    if(strcmp(ext, ".png") == 0) return File_PNG;/*png文件*/
    if(strcmp(ext, ".rgb") == 0) return File_RGB;/*rgb文件*/
    if(strcmp(ext, ".wav") == 0) return File_WAV;/*wav文件*/
    if(strcmp(ext, ".lrc") == 0) return FILE_LRC;/*wav文件*/
    return File_Unknow;
}

/*--------------------------------------------------------------------------------↓
	@函数	  : 其他文件的信息
	@参数	  : 父对象
	@返回值 : 无
	@备注	  : 其他还没来的及开发的文件就直接显示这个
↑--------------------------------------------------------------------------------*/
static void other_file_message(lv_obj_t*parent)
{
    lv_obj_t*file_txt = lv_textarea_create(parent);/*textarea*/

    lv_obj_set_size( file_txt, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(file_txt, 0, 0);

    lv_obj_set_style_shadow_opa (file_txt, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(file_txt,0,LV_PART_MAIN);/**边框宽度0 **/

    lv_textarea_add_text (file_txt, "目前还不能读此文件,可读(txt,c,h,png)");   /*显示*/
}

static void event_file_close_cb(lv_event_t*e)
{
        if(file_dir.timer!=NULL)
        {
            lv_timer_del(file_dir.timer); 
            file_dir.timer=NULL; 
        }
//          file_dir.timer->user_data=NULL;  
//         lv_timer_pause(file_dir.timer);
        if(file_dir.dir.drv!=NULL)
        {
            lv_fs_dir_close(&file_dir.dir); /*关闭文件夹*/  
            file_dir.dir.drv=NULL;
        } 
        
}



lv_obj_t* ui_file_switch_page_create(lv_obj_t*parent)
{
    
   lv_obj_t * obj=lv_obj_create(parent); 
   lv_obj_t * pre_page_btn= ui_widgets_btn_create(obj,LV_SYMBOL_LEFT,lv_color_hex(0x74B5F3));
   lv_obj_t * next_page_btn= ui_widgets_btn_create(obj,LV_SYMBOL_RIGHT,lv_color_hex(0x74B5F3)); 
   lv_obj_t *page_label=lv_label_create(obj);
   
   lv_obj_add_flag(pre_page_btn,LV_OBJ_FLAG_HIDDEN);  
   lv_obj_add_flag(next_page_btn,LV_OBJ_FLAG_HIDDEN);
     
   lv_obj_align(pre_page_btn,LV_ALIGN_CENTER,-30,0);
   lv_obj_align(next_page_btn,LV_ALIGN_CENTER,30,0);
   lv_obj_align(page_label,LV_ALIGN_CENTER,0,0);
   
  lv_obj_add_event_cb(pre_page_btn,event_switch_file_page_cb,LV_EVENT_CLICKED,NULL);
  lv_obj_add_event_cb(next_page_btn,event_switch_file_page_cb,LV_EVENT_CLICKED,NULL);
   
   return obj;
}
extern void ui_goto_page(UI_APP_PAGE_ENUM Page,UI_APP_ENUM APP);

static void return_to_pre_page_timer_cb(lv_timer_t*t);

static void event_switch_file_page_cb(lv_event_t*e)
{
    static uint8_t page_index;
    lv_obj_t*target=lv_event_get_target(e);
    lv_obj_clean(file_dir.list);
    lv_list_add_text(file_dir.list, Cure_Path);/*添加标题(路径)*/ 
    if(target==file_switch_page.pre_page_btn)
    {  
    file_switch_page.cur_page--; 
    page_index=file_switch_page.cur_page;
    if(page_index<=1)
    {
        page_index=1;
        lv_obj_add_flag(file_switch_page.pre_page_btn,LV_OBJ_FLAG_HIDDEN);
    }
            if(lv_obj_has_flag(file_switch_page.next_page_btn,LV_OBJ_FLAG_HIDDEN))
        {
            lv_obj_clear_flag(file_switch_page.next_page_btn,LV_OBJ_FLAG_HIDDEN);
        }
            file_switch_page.timer=lv_timer_create(return_to_pre_page_timer_cb,10,(void*)SHOW_FILE_MAX_NUM);

               
    }
    else if(target==file_switch_page.next_page_btn)
    {
        file_switch_page.cur_page++;  
        if(file_switch_page.cur_page>=file_switch_page.total_page_num)
        {
            file_switch_page.cur_page=file_switch_page.total_page_num;
            lv_obj_add_flag(file_switch_page.next_page_btn,LV_OBJ_FLAG_HIDDEN);
        }
        if(lv_obj_has_flag(file_switch_page.pre_page_btn,LV_OBJ_FLAG_HIDDEN))
        {
            lv_obj_clear_flag(file_switch_page.pre_page_btn,LV_OBJ_FLAG_HIDDEN);
        }
        if(file_switch_page.cur_page<file_switch_page.total_page_num)
        {
            file_switch_page.timer=lv_timer_create(return_to_pre_page_timer_cb,10,(void*)SHOW_FILE_MAX_NUM);
        }
        else
        {  
            uint8_t num=file_switch_page.total_file_num-(file_switch_page.cur_page-1)*SHOW_FILE_MAX_NUM; 
            file_switch_page.timer=lv_timer_create(return_to_pre_page_timer_cb,10,(void*)num);
            printf("下一页文件数量:%d\r\n",num);
        }
    }
    lv_label_set_text_fmt(file_switch_page.page_label,"第%d页",file_switch_page.cur_page);
}

//        ui_goto_page(PAGE_APP_LIST,APP_FILE);


static void return_to_pre_page_timer_cb(lv_timer_t*t)
{
    static uint8_t run_num=0;
    uint8_t num=(uint8_t)t->user_data;
    char*buf=name_buf[(SHOW_FILE_MAX_NUM*(file_switch_page.cur_page-1))+run_num];
//     printf("文件:%s\r\n",buf);
    if( buf[0]=='/')
    {
    file_dir.btn = lv_list_add_btn(file_dir.list, LV_SYMBOL_DIRECTORY, buf); /*  /开头为文件夹*/ 
    }
    else 
    {
    file_dir.btn = lv_list_add_btn(file_dir.list, LV_SYMBOL_FILE, buf);/*其他为文件*/
    }
    run_num++;
    if(num==run_num)
    {
        run_num=0;
        lv_timer_del(t);
    }
    lv_obj_add_event_cb(file_dir.btn, event_ui_app_file_cb, LV_EVENT_CLICKED, NULL);/*添加事件*/ 
}

void get_file_name(const char *path,char*buf)
{
    const char* name_start=strrchr(path,'/');
    if(!name_start) name_start=strrchr(path,'\\');
    name_start = (name_start)?(name_start+1) : path;
    char* name_end=strrchr(path,'.');
    
    if(name_end && (name_end>name_start))
    {
        uint8_t len=name_end-name_start;
        strncpy(buf,name_start,len);
        buf[len]='\0';
    }
    else
    {
        strcpy(buf, name_start);
    }
}
