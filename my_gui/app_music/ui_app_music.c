#include "ui_app_music.h"
#include "stdio.h"
#include "../app_file_sys/ui_app_file.h"
#if keil
#include "MAX98357A.h"
music_control_t music_win;
#endif
extern char Cure_Path[128];




lv_obj_t* ui_app_music_list_creat(lv_obj_t*parent)
{
    memcpy(Cure_Path,SD_MUSIC_PATH,strlen(SD_MUSIC_PATH));
    return ui_app_file_list_create(parent,SD_MUSIC_PATH);
}

void ui_app_music_detail_creat(lv_obj_t*parent,const char*path)
{
    #if keil
   lv_fs_res_t res = lv_fs_open(&music_win.file, path, LV_FS_MODE_RD); /*打开文件*/
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
                           
   }
   else
   {
      printf("打开音频文件失败\r\n");
   }
//    lv_fs_close(&music_win.file);
    #endif
}
