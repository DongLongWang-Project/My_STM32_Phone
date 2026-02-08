/*--------------------------------------------------------------------------------↓
	@项目	:  app (文件系统)
	@日期	:
	@备注	:
↑--------------------------------------------------------------------------------*/
#include "ui_app_file.h"


#define Dir_MAX_LEN     512
char Cure_Path[128]={"0:"};

static FILE_TYPE_ENUM get_file_type(const char *path);

static lv_obj_t* open_dir_creat_btn(lv_obj_t *parent,const char * path);

static void other_file_message(lv_obj_t*parent);


static void event_ui_app_file_cb(lv_event_t *e);


/*--------------------------------------------------------------------------------↓
	@函数	  : 创建文件夹的目录
	@参数	  : 父对象,当前路径
	@返回值 : 创建的列表
	@备注	  :
↑--------------------------------------------------------------------------------*/
lv_obj_t* ui_app_file_list_create(lv_obj_t *parent,const char * path)
{
    lv_obj_t*list = lv_list_create(parent); /*文件目录(以列表形式)*/
    
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));           
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_radius(list, 0, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF); 
    lv_obj_set_scroll_dir(list, LV_DIR_VER); 
    
    lv_list_add_text(list, path);/*添加标题(路径)*/
    return open_dir_creat_btn(list,path);/*打开文件夹创建文件按钮*/
}


/*--------------------------------------------------------------------------------↓
	@函数	  :  打开文件夹创建内部目录文件按钮
	@参数	  :  父对象,路径
	@返回值 :  父对象
	@备注	  :
↑--------------------------------------------------------------------------------*/
static lv_obj_t* open_dir_creat_btn(lv_obj_t *parent,const char * path)
{
    lv_fs_res_t res;/*文件打开状态*/
    lv_fs_dir_t dir; /*文件夹*/
    lv_obj_t *btn;   /*按钮*/
    char buf[Dir_MAX_LEN];
    res=lv_fs_dir_open(&dir,path);  /*打开目录的文件夹*/
    if(res!=LV_FS_RES_OK)/*失败就打印返回*/
    {
        printf("open %s  is fail",path);
        return parent;
    }
    while(1)
    {
        memset(buf,0,sizeof(buf));
        res=lv_fs_dir_read(&dir,buf);/*读取文件夹的内容*/
//        printf("%s \r\n",buf);
        if(res!=LV_FS_RES_OK || buf[0]=='\0') break; /*失败或者读到底就打断循环*/
        else
        {
            if( buf[0]=='/')btn = lv_list_add_btn(parent, LV_SYMBOL_DIRECTORY, buf); /*  /开头为文件夹*/
            else btn = lv_list_add_btn(parent, LV_SYMBOL_FILE, buf);/*其他为文件*/
            lv_obj_add_event_cb(btn, event_ui_app_file_cb, LV_EVENT_CLICKED, NULL);/*添加事件*/
        }
    }
    lv_fs_dir_close(&dir); /*关闭文件夹*/
    
    return parent; /*返回父对象(页面,也就是列表)*/
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
