#include "crc.h"

uint32_t Software_Continue_CRC32(uint32_t last_crc, uint8_t* data, uint32_t len) {
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

void crc_init(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
}

uint32_t Hardware_Continue_CRC32(uint8_t* data, uint32_t len, uint8_t first_chunk)
{
    uint32_t word_len = len / 4;
    uint32_t remainder = len % 4;
    uint32_t *p32 = (uint32_t *)data;

    if (first_chunk) {
        CRC_ResetDR(); 
    }

    for (uint32_t i = 0; i < word_len; i++) {
        // 如果和软件算的不一样，试试加上 __REV(p32[i])
        CRC->DR = __REV(p32[i]); 
    }

    if (remainder != 0) {
        // 关键：初始值必须与软件补齐逻辑同步
        uint32_t last_word = 0xFFFFFFFF; 
        for (uint32_t i = 0; i < remainder; i++) {
            ((uint8_t*)&last_word)[i] = data[len - remainder + i];
        }
        CRC->DR = last_word;
    }

    return CRC->DR; 
}

uint32_t Continue_CRC32(uint32_t last_crc, uint8_t* data, uint32_t len) {
    uint32_t crc = last_crc; // 接力上次的结果
    uint32_t calc_len = len;   
    for (uint32_t i = 0; i < calc_len; i++) {
        crc ^= ((uint32_t)data[i] << 24);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80000000) crc = (crc << 1) ^ 0x04C11DB7;
            else crc = (crc << 1);
        }
    }
    return crc;
}
