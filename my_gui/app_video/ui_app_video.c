#include "ui_app_video.h"
#include "stdio.h"
#include "../app_file_sys/ui_app_file.h"
#include "../ui_widgets.h"

#define Video_Width  240
#define Video_Height 160
#define frame_rate  66   //15hz
uint8_t video_buf[Video_Height*Video_Width*2]__attribute__((section(".EXT_SRAM")));


static lv_img_dsc_t video_dsc = {
    .header.always_zero = 0,           // 必须为 0
    .header.w = Video_Width,           // 宽
    .header.h = Video_Height,          // 高
    .data_size = Video_Width * Video_Height * 2,
    .header.cf = LV_IMG_CF_TRUE_COLOR, // 核心：使用当前系统配置的颜色格式
    .data = video_buf,                      // 初始化设为 NULL，稍后在函数里赋值
};


video_control_t Video_win;
video_time_t video_time;
extern char Cure_Path[Dir_MAX_LEN];
static void event_video_close_cb(lv_event_t*e);

lv_obj_t* ui_app_video_list_creat(lv_obj_t*parent)
{
    memcpy(Cure_Path,SD_VIDEO_PATH,strlen(SD_VIDEO_PATH));
    return ui_app_file_list_create(parent,SD_VIDEO_PATH);
}


static void video_timer_cb(lv_timer_t * t) {
    uint32_t num;
    // 读取下一帧
   
   video_time.view_cur_time++;
  lv_bar_set_value(Video_win.video_pargress_bar, video_time.view_cur_time,LV_ANIM_ON);

   
    lv_fs_res_t res = lv_fs_read(&Video_win.file, (void*)video_dsc.data, video_dsc.data_size, &num);

        if(res != LV_FS_RES_OK || num < video_dsc.data_size) {
        // 播放完了，关闭文件并停止定时器
        lv_fs_close(&Video_win.file);
        lv_timer_del(t);
         video_time.view_cur_time=0;
        Video_win.timer = NULL; // 清空指针
         lv_label_set_text(lv_obj_get_child(Video_win.view_btn,0),LV_SYMBOL_PLAY);
        printf("视频播放结束\r\n");
        return;
    }

    // 更新显示
    lv_img_cache_invalidate_src(&video_dsc);
    lv_obj_invalidate(Video_win.obj_video);
}

void event_video_btn(lv_event_t*e)
{
    static bool video_state=true;
    lv_event_code_t code=lv_event_get_code(e);
    if(code==LV_EVENT_CLICKED)
    {
        if(video_state==false)
        {
            lv_timer_resume(Video_win.timer);
            lv_label_set_text(lv_obj_get_child(Video_win.view_btn,0),LV_SYMBOL_PAUSE);
        }
        else
        {
            lv_timer_pause(Video_win.timer); 
            lv_label_set_text(lv_obj_get_child(Video_win.view_btn,0),LV_SYMBOL_PLAY);

        }
        video_state=!video_state;
    }
    if(code==LV_EVENT_LONG_PRESSED)
    {
        video_state=false;
        lv_timer_set_period(Video_win.timer,frame_rate/2);
    }
    else if(code==LV_EVENT_RELEASED)
    {
        lv_timer_set_period(Video_win.timer,frame_rate);
    }
}



void ui_app_video_detail_creat(lv_obj_t*parent,const char*path)
{
    memset(video_buf,0,sizeof(video_buf));
    Video_win.obj_video=lv_img_create(parent);
   lv_obj_set_size(Video_win.obj_video,Video_Width,Video_Height);
   lv_obj_center(Video_win.obj_video);
   lv_img_set_src(Video_win.obj_video, &video_dsc);
   
    Video_win.video_pargress_bar=ui_progress_bar(parent);
        lv_obj_align_to(Video_win.video_pargress_bar,Video_win.obj_video,LV_ALIGN_OUT_BOTTOM_MID,0,0);
    Video_win.view_btn=ui_widgets_btn_create(parent,LV_SYMBOL_PAUSE,lv_color_hex(0x007FFE));
    lv_obj_align_to(Video_win.view_btn,Video_win.video_pargress_bar,LV_ALIGN_OUT_BOTTOM_MID,0,0);
//    lv_obj_set_style_bg_opa(Video_win.view_btn,0,0);
//    lv_obj_t*btn_label=lv_obj_get_child(Video_win.view_btn,0);
//    lv_obj_set_style_text_color(btn_label,lv_color_hex(0x007FFE),0);
    
    lv_obj_add_event_cb(Video_win.view_btn,event_video_btn,LV_EVENT_CLICKED,NULL);
    lv_obj_add_flag(Video_win.obj_video, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(Video_win.obj_video,event_video_btn,LV_EVENT_CLICKED,NULL); 
    lv_obj_add_event_cb(Video_win.obj_video,event_video_btn,LV_EVENT_LONG_PRESSED,NULL); 
    lv_obj_add_event_cb(Video_win.obj_video,event_video_btn,LV_EVENT_RELEASED,NULL); 
    lv_obj_add_event_cb(Video_win.obj_video,event_video_close_cb,LV_EVENT_DELETE,NULL); 
   
   lv_fs_res_t res = lv_fs_open(&Video_win.file, path, LV_FS_MODE_RD); /*打开文件*/
   if(res==LV_FS_RES_OK) 
   {
       lv_fs_seek(&Video_win.file, 0, LV_FS_SEEK_END);
       uint32_t video_size;
       lv_fs_tell(&Video_win.file,&video_size);
       video_time.total_sec=video_size/video_dsc.header.w/video_dsc.header.h/15/2;
       video_time.hour=video_time.total_sec/3600;
       video_time.minute=video_time.total_sec/60;
       video_time.total_sec=video_time.total_sec%60;
       printf("%d %d %d ",video_time.hour,video_time.minute,video_time.total_sec);
       lv_bar_set_range( Video_win.video_pargress_bar,0,video_size/video_dsc.header.w/video_dsc.header.h/2);
       
       lv_fs_seek(&Video_win.file, 0, LV_FS_SEEK_SET);
      printf("打开视频文件成功,大小:%u\r\n",video_size);
      Video_win.timer=lv_timer_create(video_timer_cb,frame_rate,NULL);
      if(Video_win.timer!=NULL)
      {
       printf("视频定时器创建成功\r\n"); 
        lv_bar_set_start_value( Video_win.video_pargress_bar, 0, LV_ANIM_OFF );
      }
   }
   else
   {
      printf("打开视频文件失败\r\n");

   }
}

static void event_video_close_cb(lv_event_t*e)
{
        if(Video_win.timer !=NULL)
        {
         lv_timer_del(Video_win.timer);
         Video_win.timer = NULL; // 清空指针
          video_time.view_cur_time=0;
        printf("关闭视频刷新定时器\r\n");
        }
        if(Video_win.file.drv!=NULL)
        {
          lv_fs_close(&Video_win.file);
          Video_win.file.drv==NULL;
            printf("关闭视频文件\r\n");
        }
}
