#ifndef	__MAX98367A_H
#define	__MAX98367A_H
#include "stm32f4xx.h"
#include "lvgl.h"
#include "USART1.h"

#define PLAY_BUF_HALF_SIZE   4096    // 内部播放缓冲半区大小（字节数）
#define PLAY_BUF_FULL_SIZE   (PLAY_BUF_HALF_SIZE*2)
#define SRAM_BUF_SIZE        (128*1024)  // 外部SRAM总大小

#define Audio_Fill_Buf_Queue_MAX_Len 4
extern volatile SemaphoreHandle_t Audio_BinSemaphore;

typedef enum {
    MUSIC_STATE_STOP = 0,
    MUSIC_STATE_PLAYING,
    MUSIC_STATE_PAUSED,
    MUSIC_STATE_ERROR,
} music_run_state_t;

typedef struct {
    uint32_t SampleRate;   // 采样率
    uint32_t DataSize;     // 音频数据总字节数
    uint16_t NumChannels;  // 声道数
    uint16_t BitsPerSample;// 位深 (通常固定16)
    char cur_playing_music[64];
    char pre_playing_music[64];
} audio_Info;
typedef struct {
  uint32_t byte_sec;
  uint16_t music_total_sec;
  uint8_t  music_hour;
  uint8_t  music_min;
  uint8_t  music_sec;
  uint32_t cur_time_ms;
  uint32_t total_played; 
}music_time_t;
typedef struct {
  audio_Info wav_data;  
  music_time_t music_time;
  lv_fs_file_t file;
  lv_obj_t *obj_music;
  lv_timer_t*music_timer;
  music_run_state_t state;
}music_control_t;
extern music_control_t music_win;

void MAX98357_Init(void);
void AudioTask(void);
void music_playing(void);
void music_pause(void);
void music_stop(void) ;

extern uint8_t play_buf[PLAY_BUF_FULL_SIZE]; // 内部DMA播放缓冲
extern uint8_t sram_ring_buf[SRAM_BUF_SIZE];

#endif
