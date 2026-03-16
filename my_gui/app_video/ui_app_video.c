#include "ui_app_video.h"
#include "stdio.h"
#include "../app_file_sys/ui_app_file.h"
#include "../ui_widgets.h"

#define Video_Width  240
#define Video_Height 160
#define frame_rate  66   //15hz

uint8_t video_buf[Video_Height*Video_Width*2]__attribute__((section(".EXT_SRAM")));

play_control_t video_play_control_bar;

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
    Cure_Path[strlen(SD_VIDEO_PATH)]='\0';
    printf("当前路径:%s\r\n",Cure_Path);
    
//    return ui_app_file_list_create(parent,SD_VIDEO_PATH);
    ui_goto_page(PAGE_APP_LIST,APP_FILE);
    return NULL;
}


static void video_timer_cb(lv_timer_t * t) {
    uint32_t num;
    // 读取下一帧
    static uint32_t tick=0;
       video_time.view_cur_time++;
tick++;
if(tick>=15)
{
    tick=0;
   video_time.cur_second++;

   if(video_time.cur_second>59)
   {
      video_time.cur_second=0;
      video_time.cur_minute++;
      if(video_time.cur_minute>59)
      {
        video_time.cur_hour++;
      }
   }
  lv_bar_set_value(video_play_control_bar.progress_bar, video_time.view_cur_time,LV_ANIM_OFF);
  if(video_time.hour==0)
  {
        lv_label_set_text_fmt(video_play_control_bar.progress_label,"%02d:%02d / %02d:%02d",video_time.cur_minute,video_time.cur_second,video_time.minute,video_time.second);
  }
  else
  {
    lv_label_set_text_fmt(video_play_control_bar.progress_label,"%02d:%02d:%02d / %02d:%02d:%02d",video_time.cur_hour,video_time.cur_minute,video_time.cur_second,video_time.hour,video_time.minute,video_time.second);
 
  }
}




   
    lv_fs_res_t res = lv_fs_read(&Video_win.file, (void*)video_dsc.data, video_dsc.data_size, &num);

        if(res != LV_FS_RES_OK || num < video_dsc.data_size) {
        // 播放完了，关闭文件并停止定时器
        lv_timer_pause(t);
        
//        lv_fs_close(&Video_win.file);
//        lv_timer_del(t);
//        Video_win.timer = NULL; // 清空指针
        video_time.cur_hour=0;
        video_time.cur_minute=0;
        video_time.cur_second=0;
        video_time.view_cur_time=0;
        Video_win.is_playing=false;
        lv_fs_seek(&Video_win.file,0,LV_FS_SEEK_SET);
         lv_label_set_text(lv_obj_get_child(video_play_control_bar.pause_btn,0),LV_SYMBOL_REFRESH);
         
        printf("视频播放结束,点击重新播放\r\n");
        return;
    }
#if keil 
    // 更新显示
    lv_disp_t * disp = lv_disp_get_default();
    lv_disp_drv_t * drv = disp->driver;


    lv_area_t video_area;
    video_area.x1 = Video_win.obj_video->coords.x1;
    video_area.y1 = Video_win.obj_video->coords.y1;
    video_area.x2 = Video_win.obj_video->coords.x2;
    video_area.y2 = Video_win.obj_video->coords.y2;


    drv->flush_cb(drv, &video_area, (lv_color_t *)video_dsc.data);

#else

    lv_img_cache_invalidate_src(&video_dsc);
    lv_obj_invalidate(Video_win.obj_video);



    #endif
}

void event_video_btn(lv_event_t*e)
{

    lv_obj_t*target=lv_event_get_target(e);
    lv_event_code_t code=lv_event_get_code(e);
    if(target==video_play_control_bar.pause_btn)
    {
            if(code==LV_EVENT_CLICKED)
            {
                if(Video_win.is_playing==false)
                {
                    lv_timer_resume(Video_win.timer);
                    lv_label_set_text(lv_obj_get_child(video_play_control_bar.pause_btn,0),LV_SYMBOL_PAUSE);
                }
                else
                {
                    lv_timer_pause(Video_win.timer); 
                    lv_label_set_text(lv_obj_get_child(video_play_control_bar.pause_btn,0),LV_SYMBOL_PLAY);

                }
                Video_win.is_playing=!Video_win.is_playing;
            }
            if(code==LV_EVENT_LONG_PRESSED)
            {
                Video_win.is_playing=false;
                lv_timer_set_period(Video_win.timer,frame_rate/2);
            }
            else if(code==LV_EVENT_RELEASED)
            {
                lv_timer_set_period(Video_win.timer,frame_rate);
            }
    }
    if(target==video_play_control_bar.next_btn)
    {
				snprintf(Cure_Path, sizeof(Cure_Path), "%s/%s",  lv_fs_up(Cure_Path), FILE_BUF[1]);  /*将当前的按钮名称和缓冲区合并添加到当前路径*/
				ui_goto_page(PAGE_APP_DETAIL,APP_FILE);  /*打开文件*/
      

    
    }
    if(target==video_play_control_bar.pre_btn)
    {
				snprintf(Cure_Path, sizeof(Cure_Path), "%s/%s",  lv_fs_up(Cure_Path), FILE_BUF[0]);  /*将当前的按钮名称和缓冲区合并添加到当前路径*/
				ui_goto_page(PAGE_APP_DETAIL,APP_FILE);  /*打开文件*/
    }
    lv_obj_invalidate(lv_scr_act());
}



void ui_app_video_detail_creat(lv_obj_t*parent,const char*path)
{
   memset(video_buf,0,sizeof(video_buf));
   memset(&video_time,0,sizeof(video_time_t));
   uint8_t file_index=video_get_list_to_file(path,FILE_BUF[0],FILE_BUF[1]);
   printf("上一个文件:%s\r\n下一个文件:%s\r\n",FILE_BUF[0],FILE_BUF[1]);
  
   Video_win.obj_video=lv_img_create(parent);
//   lv_obj_add_flag(Video_win.obj_video,LV_OBJ_FLAG_HIDDEN); 
   lv_obj_set_size(Video_win.obj_video,Video_Width,Video_Height);
   lv_obj_center(Video_win.obj_video);
   lv_img_set_src(Video_win.obj_video, &video_dsc);
   ui_play_control_create(parent,&video_play_control_bar);
   lv_label_set_text(video_play_control_bar.progress_label,"");
    if(file_index==0 && file_switch_page.total_file_num==1)
    {
      lv_obj_add_flag(video_play_control_bar.next_btn,LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(video_play_control_bar.pre_btn,LV_OBJ_FLAG_HIDDEN);   
    }
    if(file_index==0 && file_switch_page.total_file_num>1)
    {
         lv_obj_add_flag(video_play_control_bar.pre_btn,LV_OBJ_FLAG_HIDDEN);
    }
    if(file_index==file_switch_page.total_file_num-1)
    {
       lv_obj_add_flag(video_play_control_bar.next_btn,LV_OBJ_FLAG_HIDDEN);
    }
    
//    Video_win.pargress_bar_label=lv_obj_get_child(bar_obj,1); 
        lv_obj_align_to(video_play_control_bar.obj_play_control,Video_win.obj_video,LV_ALIGN_OUT_BOTTOM_MID,0,0);

//    lv_obj_set_style_bg_opa(Video_win.view_btn,0,0);
//    lv_obj_t*btn_label=lv_obj_get_child(Video_win.view_btn,0);
//    lv_obj_set_style_text_color(btn_label,lv_color_hex(0x007FFE),0);
    lv_obj_add_event_cb(video_play_control_bar.pre_btn,event_video_btn,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(video_play_control_bar.next_btn,event_video_btn,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(video_play_control_bar.pause_btn,event_video_btn,LV_EVENT_CLICKED,NULL);
    
    lv_obj_add_flag(Video_win.obj_video, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(Video_win.obj_video,event_video_btn,LV_EVENT_CLICKED,NULL); 
    lv_obj_add_event_cb(Video_win.obj_video,event_video_btn,LV_EVENT_LONG_PRESSED,NULL); 
    lv_obj_add_event_cb(Video_win.obj_video,event_video_btn,LV_EVENT_RELEASED,NULL); 
    lv_obj_add_event_cb(Video_win.obj_video,event_video_close_cb,LV_EVENT_DELETE,NULL); 
   
   lv_fs_res_t res = lv_fs_open(&Video_win.file, path, LV_FS_MODE_RD); /*打开文件*/
   if(res==LV_FS_RES_OK) 
   {
       Video_win.is_playing=true;
       lv_fs_seek(&Video_win.file, 0, LV_FS_SEEK_END);
       uint32_t video_size;
       lv_fs_tell(&Video_win.file,&video_size);
       video_time.total_sec=video_size/video_dsc.header.w/video_dsc.header.h/15/2;
       video_time.hour=video_time.total_sec/3600;
       video_time.minute=video_time.total_sec/60;
       video_time.second=video_time.total_sec%60;
       printf("%d %d %d ",video_time.hour,video_time.minute,video_time.second);
       lv_bar_set_range( video_play_control_bar.progress_bar,0,video_size/video_dsc.header.w/video_dsc.header.h/2);
       
       lv_fs_seek(&Video_win.file, 0, LV_FS_SEEK_SET);
      printf("打开视频文件成功,大小:%u\r\n",video_size);
      Video_win.timer=lv_timer_create(video_timer_cb,frame_rate,NULL);
      if(Video_win.timer!=NULL)
      {
       printf("视频定时器创建成功\r\n"); 
        lv_bar_set_start_value( video_play_control_bar.progress_bar, 0, LV_ANIM_OFF );
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
