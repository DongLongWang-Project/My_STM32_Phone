#include "ui_app_video.h"
#include "stdio.h"
#include "../app_file_sys/ui_app_file.h"

#define Video_Width  240
#define Video_Height 160

uint8_t video_buf[Video_Height*Video_Width*2]__attribute__((section(".EXT_SRAM")));
lv_obj_t*video_win;
static lv_img_dsc_t video_dsc = {
    .header.always_zero = 0,           // 必须为 0
    .header.w = Video_Width,           // 宽
    .header.h = Video_Height,          // 高
    .data_size = Video_Width * Video_Height * 2,
    .header.cf = LV_IMG_CF_TRUE_COLOR, // 核心：使用当前系统配置的颜色格式
    .data = video_buf,                      // 初始化设为 NULL，稍后在函数里赋值
};


video_control_t Video_win;
extern char Cure_Path[128];

lv_obj_t* ui_app_video_list_creat(lv_obj_t*parent)
{
    memcpy(Cure_Path,SD_VIDEO_PATH,strlen(SD_VIDEO_PATH));
    return ui_app_file_list_create(parent,SD_VIDEO_PATH);
}


static void video_timer_cb(lv_timer_t * t) {
    uint32_t num;
    // 读取下一帧
    lv_fs_res_t res = lv_fs_read(&Video_win.file, (void*)video_dsc.data, video_dsc.data_size, &num);
    
    if(res != LV_FS_RES_OK || num < video_dsc.data_size) {
        // 播放完了，关闭文件并停止定时器
        lv_fs_close(&Video_win.file);
        lv_timer_del(t); 
        Video_win.timer = NULL; // 清空指针
        printf("视频播放结束\r\n");
        return;
    }

    // 更新显示
    lv_img_cache_invalidate_src(&video_dsc);
    lv_obj_invalidate(Video_win.obj_video);
}


void ui_app_video_detail_creat(lv_obj_t*parent,const char*path)
{
    Video_win.obj_video=lv_img_create(parent);
   lv_obj_set_size(Video_win.obj_video,Video_Width,Video_Height);
   lv_obj_center(Video_win.obj_video);
   lv_img_set_src(Video_win.obj_video, &video_dsc);
   

   lv_fs_res_t res = lv_fs_open(&Video_win.file, path, LV_FS_MODE_RD); /*打开文件*/
   if(res==LV_FS_RES_OK) 
   {
      printf("打开视频文件成功\r\n");
      Video_win.timer=lv_timer_create(video_timer_cb,66,NULL);
      if(Video_win.timer!=NULL)
      {
       printf("视频定时器创建成功\r\n"); 
      }
   }
   else
   {
      printf("打开视频文件失败\r\n");

   }
    
}
