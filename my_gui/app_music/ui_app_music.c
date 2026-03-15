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

LV_FONT_DECLARE( my_font_16);

void get_music_name(const char *path,char*buf);
static void event_music_btn_cb(lv_event_t*e);

lv_obj_t* ui_app_music_list_creat(lv_obj_t*parent)
{
    
    memcpy(Cure_Path,SD_MUSIC_PATH,strlen(SD_MUSIC_PATH));
    Cure_Path[strlen(SD_MUSIC_PATH)]='\0';
     printf("当前路径:%s\r\n",Cure_Path);
    ui_goto_page(PAGE_APP_LIST,APP_FILE);
}

void ui_app_music_detail_creat(lv_obj_t*parent,const char*path)
{
    
    char music_name[64];
    get_music_name(path,music_name);
   uint8_t file_index=video_get_list_to_file(path,FILE_BUF[0],FILE_BUF[1]);
   printf("上一个文件:%s\r\下一个文件:%s\r\n",FILE_BUF[0],FILE_BUF[1]);
   
    ui_play_control_create(parent,&play_control_bar); 
   lv_label_set_text(play_control_bar.progress_label,"");
    if(file_index==0 && file_switch_page.total_file_num==1)
    {
      lv_obj_add_flag(play_control_bar.next_btn,LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(play_control_bar.pre_btn,LV_OBJ_FLAG_HIDDEN);   
    }
    if(file_index==0 && file_switch_page.total_file_num>1)
    {
         lv_obj_add_flag(play_control_bar.pre_btn,LV_OBJ_FLAG_HIDDEN);
    }
    if(file_index==file_switch_page.total_file_num-1)
    {
       lv_obj_add_flag(play_control_bar.next_btn,LV_OBJ_FLAG_HIDDEN);
    }
    
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
       music_win.music_time.music_hour=music_win.music_time.music_total_sec/3600;
       music_win.music_time.music_min=(music_win.music_time.music_total_sec%3600)/60;
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
    snprintf(music_path,sizeof(music_path),"G:\\GitHub_Code\\My_STM32_Phone\\SD\\music\\%s",lv_fs_get_last(path));
        PlaySound(TEXT(music_path), NULL, SND_FILENAME | SND_ASYNC);
#endif
    

   lv_obj_align(play_control_bar.obj_play_control,LV_ALIGN_BOTTOM_MID,0,0);
   
    lv_obj_set_style_text_font(play_control_bar.progress_label,&my_font_16,0);
    

    lv_label_set_text(play_control_bar.progress_label,music_name);
    
  lv_obj_add_event_cb(play_control_bar.pause_btn,event_music_btn_cb,LV_EVENT_CLICKED,NULL);
  lv_obj_add_event_cb(play_control_bar.pre_btn,event_music_btn_cb,LV_EVENT_CLICKED,NULL);
  lv_obj_add_event_cb(play_control_bar.next_btn,event_music_btn_cb,LV_EVENT_CLICKED,NULL);
  
  music_win.music_timer=lv_timer_create(music_progress_timer_cb,200,NULL);
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
    if(target==play_control_bar.pause_btn)
    {
          lv_obj_t *label=lv_obj_get_child(play_control_bar.pause_btn,0);
        if(music_state==true)
        {
            printf("暂停音乐\r\n");
             DMA_Cmd(DMA1_Stream4,DISABLE); 
            music_state=false;
            lv_label_set_text(label,LV_SYMBOL_PLAY);    
        }
        else
        {
            music_state=true;
            printf("继续播放\r\n");
             DMA_Cmd(DMA1_Stream4,ENABLE); 
             lv_label_set_text(label,LV_SYMBOL_PAUSE);
        }   
    }
    else if(target==play_control_bar.next_btn)
    {
        printf("下一首\r\n");
        music_state=true;
        DMA_Cmd(DMA1_Stream4,DISABLE);
         
				snprintf(Cure_Path, sizeof(Cure_Path), "%s/%s",  lv_fs_up(Cure_Path), FILE_BUF[1]);  /*将当前的按钮名称和缓冲区合并添加到当前路径*/
				ui_goto_page(PAGE_APP_DETAIL,APP_FILE);  /*打开文件*/
    }
    else if(target==play_control_bar.pre_btn)
    {
          printf("上一首\r\n");
          music_state=true;
          DMA_Cmd(DMA1_Stream4,DISABLE);
				snprintf(Cure_Path, sizeof(Cure_Path), "%s/%s",  lv_fs_up(Cure_Path), FILE_BUF[0]);  /*将当前的按钮名称和缓冲区合并添加到当前路径*/
				ui_goto_page(PAGE_APP_DETAIL,APP_FILE);  /*打开文件*/   
    }
}

void music_progress_timer_cb(lv_timer_t*t)
{
  static uint8_t run_tick=0;
  run_tick++;
  uint32_t total_sec=music_win.music_time.cur_time_ms/1000;
  uint8_t  cur_hour=total_sec/3600;
  uint8_t  cur_min=(total_sec%3600)/60;
  uint8_t  cur_sec=total_sec%60;
  
  if(run_tick>5)
  {
    run_tick=0;
    if(cur_hour)
    {
      lv_label_set_text_fmt(play_control_bar.progress_label,"%02d:%02d:%02d / %02d:02d:%02d",cur_hour,cur_min,cur_sec,
      music_win.music_time.music_hour,music_win.music_time.music_min,music_win.music_time.music_sec);
    }
    else
    {
      lv_label_set_text_fmt(play_control_bar.progress_label,"%02d:%02d / %02d:%02d",cur_min,cur_sec,
      music_win.music_time.music_min,music_win.music_time.music_sec);
    }  
  }


}