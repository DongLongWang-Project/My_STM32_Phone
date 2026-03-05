#ifndef	__MAX98367A_H
#define	__MAX98367A_H
#include "stm32f4xx.h"
#include "lvgl.h"
#include "USART1.h"

extern volatile SemaphoreHandle_t Audio_BinSemaphore;
typedef struct {
    uint32_t SampleRate;   // 采样率
    uint32_t DataSize;     // 音频数据总字节数
    uint16_t NumChannels;  // 声道数
    uint16_t BitsPerSample;// 位深 (通常固定16)
} audio_Info;
typedef struct {
  uint16_t music_total_sec;
  uint8_t  music_min;
  uint8_t  music_sec;  
}music_time_t;
typedef struct {
  audio_Info wav_data;  
  music_time_t music_time;
  lv_fs_file_t file;
  lv_obj_t *obj_music;
}music_control_t;
extern music_control_t music_win;

void MAX98357_Init(void);
void AudioTask(void);

#endif
