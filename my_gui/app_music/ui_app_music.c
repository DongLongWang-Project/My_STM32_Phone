#include "ui_app_music.h"
#include "stdio.h"
#include "../app_file_sys/ui_app_file.h"
#include "../app_video/ui_app_video.h"
#include "../../SD/Font/Font.h"
#include "string.h"

#if keil
#include "MAX98357A.h"
music_control_t music_win;
#else
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib") // 链接winmm.lib，必须加
#endif
extern char Cure_Path[Dir_MAX_LEN];

#define record_width   160
#define record_length   160

#define record_buf_size 160*160*2

uint8_t musci_record_buf[record_buf_size]__attribute__((section(".EXT_SRAM")));
LV_FONT_DECLARE( my_font_16);

music_player_t music_play;
     static   lv_img_dsc_t music_record = {
        .header.always_zero = 0,           // 必须为 0
        .header.w = record_width,           // 宽
        .header.h = record_length,          // 高
        .data_size = record_buf_size,
        .header.cf = LV_IMG_CF_TRUE_COLOR, // 核心：使用当前系统配置的颜色格式
        .data = musci_record_buf,                      // 初始化设为 NULL，稍后在函数里赋值
    };
 
 
 void get_music_name(const char *path,char*buf);
 static void event_music_btn_cb(lv_event_t*e);
void music_record_anim_config(void);

lv_obj_t* ui_app_music_list_creat(lv_obj_t*parent)
{
    memcpy(Cure_Path,SD_MUSIC_PATH,strlen(SD_MUSIC_PATH));
    return ui_app_file_list_create(parent,SD_MUSIC_PATH);
}

void ui_app_music_detail_creat(lv_obj_t*parent,const char*path)
{
    
    char music_name[64];
    get_music_name(path,music_name);
    lv_fs_res_t res;
    #if keil
    res = lv_fs_open(&music_win.file, path, LV_FS_MODE_RD); /*打开文件*/
   if(res==LV_FS_RES_OK) 
   {
        printf("打开音频文件成功\r\n");
           uint32_t num;
           uint8_t wav_head[128];
            // 读取下一帧
            res = lv_fs_read(&music_win.file, wav_head, sizeof(wav_head), &num);
              if(res==LV_FS_RES_OK) 
           {
             printf("num:%d\r\n",num);
             music_win.wav_data.NumChannels=*(uint16_t*)(&wav_head[22]);  
             music_win.wav_data.SampleRate=*(uint32_t*)(&wav_head[24]);
             music_win.wav_data.BitsPerSample=*(uint16_t*)(&wav_head[34]);
             for(uint8_t i=34;i<128;i++)
             {
              if(wav_head[i] == 0x64 && wav_head[i+1] == 0x61 && wav_head[i+2] == 0x74 && wav_head[i+3] == 0x61)
                 {
                  music_win.wav_data.DataSize=*(uint32_t*)(&wav_head[i+4]); 
                  lv_fs_seek(&music_win.file,i+8,LV_FS_SEEK_SET);
                   music_win.wav_data.DataSize=music_win.wav_data.DataSize-(i+8);
                  break;                  
                 }
             }
             
           }
       music_win.music_time.music_total_sec=music_win.wav_data.DataSize/(music_win.wav_data.SampleRate*music_win.wav_data.NumChannels*music_win.wav_data.BitsPerSample/8);  
       music_win.music_time.music_min=music_win.music_time.music_total_sec/60;
       music_win.music_time.music_sec=music_win.music_time.music_total_sec%60; 
       printf("music_total_sec:%d NumChannels:%d SampleRate:%d BitsPerSample:%d DataSize:%d\r\n",music_win.music_time.music_total_sec,
       music_win.wav_data.NumChannels,music_win.wav_data.SampleRate,music_win.wav_data.BitsPerSample,music_win.wav_data.DataSize);
       DMA_Cmd(DMA1_Stream4,ENABLE);                    
   }
   else
   {
      printf("打开音频文件失败\r\n");
   }
//    lv_fs_close(&music_win.file);

#else
char music_path[256];
snprintf(music_path,sizeof(music_path),"G:\\GitHub_Code\\My_STM32_Phone\\SD\\music\\%s.wav",music_name);
    PlaySound(TEXT(music_path), NULL, SND_FILENAME | SND_ASYNC);
  
    #endif
    
    music_play.record_icon=lv_img_create(parent);
    
     res= lv_fs_open(&music_play.file,MUSIC_RECORD_PATH,LV_FS_MODE_RD);
   if(res!=LV_FS_RES_OK)
   {
       printf("打开唱片文件失败 或者唱片文件被删除\r\n");
       //return ;
   }
   else
   {
       uint32_t len;
        res=lv_fs_read(&music_play.file,musci_record_buf,record_buf_size,&len);
        if(res==LV_FS_RES_OK)
        {
            printf("打开唱片\r\n");
            lv_fs_close(&music_play.file);
            lv_img_set_src(music_play.record_icon,&music_record);
            lv_obj_align(music_play.record_icon,LV_ALIGN_CENTER,0,-30);
                
            music_record_anim_config();
             lv_anim_start(&music_play.record_anim);
        }
   }
   
   music_play.music_label=lv_label_create(parent);
   lv_obj_align(music_play.music_label,LV_ALIGN_CENTER,0,80);
    lv_obj_set_style_text_font(music_play.music_label,&my_font_16,0);
    

    lv_label_set_text(music_play.music_label,music_name);
//   music_play.music_record=ui_progress_bar(parent);
   lv_obj_align_to(music_play.music_record,music_play.music_label,LV_ALIGN_OUT_BOTTOM_MID,0,10);
 music_play.pause_btn=ui_widgets_btn_create(parent,LV_SYMBOL_PAUSE,lv_color_hex(0x007FFE));
 music_play.next_btn=ui_widgets_btn_create(parent,LV_SYMBOL_NEXT,lv_color_hex(0x007FFE));
  music_play.pre_btn=ui_widgets_btn_create(parent,LV_SYMBOL_PREV,lv_color_hex(0x007FFE)); 
  
//  lv_obj_align_to(music_play.pause_btn,music_play.music_record,LV_ALIGN_OUT_BOTTOM_MID,0,0);
  lv_obj_align_to(music_play.next_btn,music_play.pause_btn,LV_ALIGN_OUT_RIGHT_MID,0,0);
  lv_obj_align_to(music_play.pre_btn,music_play.pause_btn,LV_ALIGN_OUT_LEFT_MID,0,0);
  
  lv_obj_add_event_cb(music_play.pause_btn,event_music_btn_cb,LV_EVENT_CLICKED,NULL);
  lv_obj_add_event_cb(music_play.pre_btn,event_music_btn_cb,LV_EVENT_CLICKED,NULL);
  lv_obj_add_event_cb(music_play.next_btn,event_music_btn_cb,LV_EVENT_CLICKED,NULL);
}

void music_record_anim_config(void)
{
            lv_anim_init(&music_play.record_anim);
            lv_anim_set_var(&music_play.record_anim, music_play.record_icon);
            lv_anim_set_exec_cb(&music_play.record_anim,(lv_anim_exec_xcb_t)lv_img_set_angle);
            lv_anim_set_values(&music_play.record_anim,0,3600);
            lv_anim_set_time(&music_play.record_anim,3000);
            lv_anim_set_repeat_count(&music_play.record_anim,LV_ANIM_REPEAT_INFINITE);
            lv_anim_start(&music_play.record_anim);
}
void get_music_name(const char *path,char*buf)
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

static void event_music_btn_cb(lv_event_t*e)
{
        static bool music_state=true;
    lv_obj_t*target=lv_event_get_target(e);
    if(target==music_play.pause_btn)
    {
          lv_obj_t *label=lv_obj_get_child(music_play.pause_btn,0);
        if(music_state==true)
        {
            printf("暂停音乐\r\n");
            music_state=false;
         
          lv_label_set_text(label,LV_SYMBOL_PLAY);

               lv_anim_del(music_play.record_icon,(lv_anim_exec_xcb_t)lv_img_set_angle); 
             
            
        }
        else
        {
            music_state=true;
            printf("继续播放\r\n");
            music_record_anim_config();
             lv_label_set_text(label,LV_SYMBOL_PAUSE);
        }   
    }
    else if(target==music_play.next_btn)
    {
        printf("下一首\r\n");
        music_state=true;
            if(music_play.file.drv!=NULL)
            {
                lv_fs_close(&music_play.file);
                music_play.file.drv=NULL;
            }
    }
    else if(target==music_play.pre_btn)
    {
          printf("上一首\r\n");
          music_state=true;
            if(music_play.file.drv!=NULL)
            {
                lv_fs_close(&music_play.file);
                music_play.file.drv=NULL;
            }   
    }
}
