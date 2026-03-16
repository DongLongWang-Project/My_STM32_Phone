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
#define LRC_BUF_SIZE 4096
uint8_t LRC_BUF[LRC_BUF_SIZE]__attribute__((section(".EXT_SRAM"))) __attribute__((aligned(4)));  // 外部SRAM大缓冲

play_control_t music_play_control_bar;

void get_music_name(const char *path,char*buf);
static void event_music_btn_cb(lv_event_t*e);

lv_obj_t* ui_app_music_list_creat(lv_obj_t*parent)
{
    
    memcpy(Cure_Path,SD_MUSIC_PATH,strlen(SD_MUSIC_PATH));
    Cure_Path[strlen(SD_MUSIC_PATH)]='\0';
     printf("当前路径:%s\r\n",Cure_Path);
    ui_goto_page(PAGE_APP_LIST,APP_FILE);
    return NULL;
}

void ui_app_music_detail_creat(lv_obj_t*parent,const char*path)
{
    
   
   get_file_name(path,music_win.wav_data.cur_playing_music);
   uint8_t file_index=video_get_list_to_file(path,FILE_BUF[0],FILE_BUF[1]);
   printf("上一个文件:%s\r\n下一个文件:%s\r\n",FILE_BUF[0],FILE_BUF[1]);

    if(music_win.state!=MUSIC_STATE_PLAYING || strcmp(music_win.wav_data.pre_playing_music,music_win.wav_data.cur_playing_music)!=0)
    {
          music_stop();
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
                         
                         music_win.music_time.byte_sec=(music_win.wav_data.SampleRate*music_win.wav_data.NumChannels*music_win.wav_data.BitsPerSample/8);
                         music_win.music_time.music_total_sec=music_win.wav_data.DataSize/music_win.music_time.byte_sec;  
                         music_win.music_time.music_hour=music_win.music_time.music_total_sec/3600;
                         music_win.music_time.music_min=(music_win.music_time.music_total_sec%3600)/60;
                         music_win.music_time.music_sec=music_win.music_time.music_total_sec%60; 
                         printf("music_total_sec:%d NumChannels:%d SampleRate:%d BitsPerSample:%d DataSize:%d\r\n",music_win.music_time.music_total_sec,
                         music_win.wav_data.NumChannels,music_win.wav_data.SampleRate,music_win.wav_data.BitsPerSample,music_win.wav_data.DataSize);
                          strcpy(music_win.wav_data.pre_playing_music,music_win.wav_data.cur_playing_music);
                          music_win.wav_data.pre_playing_music[strlen(music_win.wav_data.cur_playing_music)]='\0';
                         music_playing();
                        break;                  
                       }
                   }
                   
                 }
                         
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
    }    
 
 
    
  ui_play_control_create(parent,&music_play_control_bar);
    
  if(file_index==0 && file_switch_page.total_file_num==1)
  {
    lv_obj_add_flag(music_play_control_bar.next_btn,LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(music_play_control_bar.pre_btn,LV_OBJ_FLAG_HIDDEN);   
  }
  if(file_index==0 && file_switch_page.total_file_num>1)
  {
       lv_obj_add_flag(music_play_control_bar.pre_btn,LV_OBJ_FLAG_HIDDEN);
  }
  if(file_index==file_switch_page.total_file_num-1)
  {
     lv_obj_add_flag(music_play_control_bar.next_btn,LV_OBJ_FLAG_HIDDEN);
  }
    
  lv_label_set_text(music_play_control_bar.progress_label,music_win.wav_data.cur_playing_music);
  lv_bar_set_range(music_play_control_bar.progress_bar,0,music_win.music_time.music_total_sec);
    
  lv_obj_add_event_cb(music_play_control_bar.pause_btn,event_music_btn_cb,LV_EVENT_CLICKED,NULL);
  lv_obj_add_event_cb(music_play_control_bar.pre_btn,event_music_btn_cb,LV_EVENT_CLICKED,NULL);
  lv_obj_add_event_cb(music_play_control_bar.next_btn,event_music_btn_cb,LV_EVENT_CLICKED,NULL);
 

   
}


static void event_music_btn_cb(lv_event_t*e)
{
    lv_obj_t*target=lv_event_get_target(e);
    if(target==music_play_control_bar.pause_btn)
    {
         lv_obj_t *label=lv_obj_get_child(music_play_control_bar.pause_btn,0);
        if(music_win.state==MUSIC_STATE_PLAYING)
        {
            printf("暂停音乐\r\n");
            music_pause();
            lv_label_set_text(label,LV_SYMBOL_PLAY);    
        }
        else
        {
            printf("继续播放\r\n");
            music_playing(); 
            lv_label_set_text(label,LV_SYMBOL_PAUSE);
        }   
    }
    else if(target==music_play_control_bar.next_btn)
    {
        printf("下一首\r\n");
        music_stop();

				snprintf(Cure_Path, sizeof(Cure_Path), "%s/%s",  lv_fs_up(Cure_Path), FILE_BUF[1]);  /*将当前的按钮名称和缓冲区合并添加到当前路径*/
				ui_goto_page(PAGE_APP_DETAIL,APP_FILE);  /*打开文件*/
    }
    else if(target==music_play_control_bar.pre_btn)
    {
          printf("上一首\r\n");
          music_stop();
          
				snprintf(Cure_Path, sizeof(Cure_Path), "%s/%s",  lv_fs_up(Cure_Path), FILE_BUF[0]);  /*将当前的按钮名称和缓冲区合并添加到当前路径*/
				ui_goto_page(PAGE_APP_DETAIL,APP_FILE);  /*打开文件*/   
    }
}

void music_progress_timer_cb(lv_timer_t*t)
{
  static uint32_t list_sec=0;
  

  uint32_t total_sec=music_win.music_time.cur_time_ms/1000;
  uint8_t  cur_hour=total_sec/3600;
  uint8_t  cur_min=(total_sec%3600)/60;
  uint8_t  cur_sec=total_sec%60;
  
  if(total_sec!=list_sec)
  {
    if(lv_obj_is_valid(music_play_control_bar.progress_label))
    {
      if(cur_hour)
      {
        lv_label_set_text_fmt(music_play_control_bar.progress_label,"%02d:%02d:%02d / %02d:%02d:%02d",cur_hour,cur_min,cur_sec,
        music_win.music_time.music_hour,music_win.music_time.music_min,music_win.music_time.music_sec);
      }
      else
      {
        lv_label_set_text_fmt(music_play_control_bar.progress_label,"%02d:%02d / %02d:%02d",cur_min,cur_sec,
        music_win.music_time.music_min,music_win.music_time.music_sec);
      }
       list_sec= total_sec;
     lv_bar_set_value(music_play_control_bar.progress_bar, total_sec,LV_ANIM_OFF);
    }

  }
}

#define MAX_LRC_LINE_NUM 100
typedef struct
{
  uint32_t cur_ms;
  lv_obj_t *lrc_label;
  char lrc_label_text[64];
}lrc_line_t;
typedef struct
{
  lv_obj_t *lrc_obj;
  lrc_line_t lrc_line[MAX_LRC_LINE_NUM];
  uint8_t total_lcr_line;
  char lrc_name[64];
}lrc_obj_t;


lrc_obj_t lrc_obj;
void lrc_file_analysis(char* lrc_buffer)
{
    char*p=lrc_buffer;
    uint32_t min,sec,ms;
    if(strncmp(p,"[ti:",strlen("[ti:"))==0)
    {
      p+=4;
      char *end=strchr(p,']');
      uint8_t len=end-p;
      strncpy(lrc_obj.lrc_name,p,len);
      lrc_obj.lrc_name[len]='\0';
      p=end;
    }
   while(p!=NULL)
   {
      p=strchr(p,'[');
      if(!p) break;
      if(sscanf(p,"[%d:%d.%d]",&min,&sec,&ms)==3)
      {
         lrc_obj.lrc_line[lrc_obj.total_lcr_line].cur_ms= (min*60+sec)*1000+ms*10;  
      }
      char *start=strchr(p,']');
      if(start) p+=1;
      char *end=strchr(p,'\n');
      uint8_t len=end-p;
      if(p==NULL)break; 
      strncpy(lrc_obj.lrc_line[lrc_obj.total_lcr_line].lrc_label_text,p,len);
      p=end;
   }  
}
void load_lrc_file(const char*path,char*lrc_buffer)
{
  uint32_t num;
  lv_fs_file_t file_lrc;;
  lv_fs_res_t res= lv_fs_read(&file_lrc,lrc_buffer,sizeof(lrc_buffer),&num);
  if(res==LV_FS_RES_OK)
  {
    printf("读取歌词文件成功,大小:%d\r\n",num);
    lrc_buffer[num]='\0';
  }
  else
  {
    printf("读取歌词文件失败,或者该路径错误\r\n");
  }
   
}
