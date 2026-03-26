#include "crc.h"

#include "stdio.h"
#include <stdint.h>
#include "W25Qxx.h"


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
    uint8_t reserved[256-32];     
}head_t;
typedef enum
{
   HEAD_FLASH=0,
   HEAD_W25Q,
   HEAD_NUM
}head_enum;
head_t head[HEAD_NUM];

uint32_t Continue_CRC32(uint32_t last_crc, uint8_t* data, uint32_t len) {
    uint32_t crc = last_crc; // 接力上次的结果
    uint32_t calc_len = len;
    uint8_t remainder = len % 4;
    if (remainder != 0) 
    {

        uint8_t padding_count = 4 - remainder; // 需要补几个 0xFF (1, 2, 或 3)
        
        for (uint8_t i = 0; i < padding_count; i++) {
            data[len + i] = 0xFF; // 在末尾填充 0xFF
        }
        
        calc_len = len + padding_count; // 更新计算用的长度
       }
       
    for (uint32_t i = 0; i < calc_len; i++) {
        crc ^= ((uint32_t)data[i] << 24);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80000000) crc = (crc << 1) ^ 0x04C11DB7;
            else crc = (crc << 1);
        }
    }
    return crc;
}
