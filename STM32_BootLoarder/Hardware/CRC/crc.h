#ifndef	__CRC_H
#define	__CRC_H
#include "stm32f4xx.h"
extern uint8_t crc_buf[0x10000];
void crc_init(void);
uint32_t Hardware_Continue_CRC32(uint8_t* data, uint32_t len, uint8_t first_chunk);
uint32_t Software_Continue_CRC32(uint32_t last_crc, uint8_t* data, uint32_t len); 
uint32_t Continue_CRC32(uint32_t last_crc, uint8_t* data, uint32_t len) ;
#endif
