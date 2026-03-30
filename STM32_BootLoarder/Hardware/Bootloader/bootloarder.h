#ifndef	__BOOTLOARDER_H
#define	__BOOTLOARDER_H
#include "stm32f4xx.h"
#include "ff.h"
#include "stdio.h"
#include <stdint.h>
#include "W25Qxx.h"
#include "flash.h"

#define APP_HEAD_Addr 0x08010000
#define APP_Addr 0x08010200
#define UPDATE_FILE_PATH   "0:SD/bin/myPhone.bin"
typedef void (*iapfun)(void);
typedef enum
{
    update_none=0xFF,
    update_is_running=0xFE,
    update_is_ok=0xFC,
}update_state_t;
typedef struct 
{
    uint32_t version;
    uint32_t CRC32;
    uint32_t file_size;
    char name[16];
    update_state_t update_state; 
    uint8_t reserved[512-32];     
}head_t;
typedef enum
{  
   HEAD_SD=0,
   HEAD_FLASH,
   HEAD_W25Q_Cur,
   HEAD_W25Q_Pre,
   HEAD_NUM
}head_enum;
typedef struct
{
  uint32_t update_flag;
  uint32_t file_version;
  uint32_t file_crc;
  uint32_t file_size;
}update_flag_info_t;


void load_app(u32 appxaddr);
uint8_t get_update_file_head(head_enum head_);
uint8_t update_is_valid(head_enum head_);
void update_my_phone(void);
uint8_t Read_Latest_update_info_(update_flag_info_t *update_flag_info);

#endif
