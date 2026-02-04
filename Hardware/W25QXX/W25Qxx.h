#ifndef 	__W25Qxx_H
#define	  __W25Qxx_H

#include "stm32f4xx.h"
typedef struct
{
  uint8_t   MID;
  uint16_t  DID;
}W25Qxx_ID;
extern W25Qxx_ID W25Qxx;

#define W25Qxx_MID 0xEF
#define W25Qxx_DID 0X4018


#define WIFI_SAVE_Addr                    0x00FFE000
  

#define TOUCH_SAVE_VER_Addr               0x00FFF000
#define TOUCH_SAVE_REV_VER_Addr       TOUCH_SAVE_VER_Addr+14
#define TOUCH_SAVE_HOR_Addr            TOUCH_SAVE_REV_VER_Addr+14
#define TOUCH_SAVE_REV_HOR_Addr        TOUCH_SAVE_HOR_Addr+14

#define ALARM_CLOCK_SAVE_Addr                    0x00FFD000
#define ALARM_SAVE_BLOT_SIZE                     128                   

void W25Qxx_SPI_Init(void);
void W25Qxx_ReadID(W25Qxx_ID * ID);
uint8_t W25Qxx_PageProgram(uint32_t Address, const uint8_t *DataArray, uint16_t Count)  ;
uint8_t W25Qxx_SectorErase(uint32_t Address);
void W25Qxx_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count);
uint8_t W25Qxx_WaitBusy(void);
void W25Qxx_Write(uint32_t Address, uint32_t Data,uint8_t len);
void W25Qxx_WriteBuffer(uint32_t addr,const uint8_t *buf, uint32_t len);    
uint8_t W25Qxx_Write_Sector(uint32_t Sector, const uint8_t *DataArray, uint32_t Count) ;

  
#define W25Qxx_WRITE_ENABLE							0x06
#define W25Qxx_WRITE_DISABLE						0x04
#define W25Qxx_READ_STATUS_REGISTER_1				0x05
#define W25Qxx_READ_STATUS_REGISTER_2				0x35
#define W25Qxx_WRITE_STATUS_REGISTER				0x01
#define W25Qxx_PAGE_PROGRAM							0x02
#define W25Qxx_QUAD_PAGE_PROGRAM					0x32
#define W25Qxx_BLOCK_ERASE_64KB						0xD8
#define W25Qxx_BLOCK_ERASE_32KB						0x52
#define W25Qxx_SECTOR_ERASE_4KB						0x20
#define W25Qxx_CHIP_ERASE							0xC7
#define W25Qxx_ERASE_SUSPEND						0x75
#define W25Qxx_ERASE_RESUME							0x7A
#define W25Qxx_POWER_DOWN							0xB9
#define W25Qxx_HIGH_PERFORMANCE_MODE				0xA3
#define W25Qxx_CONTINUOUS_READ_MODE_RESET			0xFF
#define W25Qxx_RELEASE_POWER_DOWN_HPM_DEVICE_ID		0xAB
#define W25Qxx_MANUFACTURER_DEVICE_ID				0x90
#define W25Qxx_READ_UNIQUE_ID						0x4B
#define W25Qxx_JEDEC_ID								0x9F
#define W25Qxx_READ_DATA							0x03
#define W25Qxx_FAST_READ							0x0B
#define W25Qxx_FAST_READ_DUAL_OUTPUT				0x3B
#define W25Qxx_FAST_READ_DUAL_IO					0xBB
#define W25Qxx_FAST_READ_QUAD_OUTPUT				0x6B
#define W25Qxx_FAST_READ_QUAD_IO					0xEB
#define W25Qxx_OCTAL_WORD_READ_QUAD_IO				0xE3

#define W25Qxx_DUMMY_BYTE							0xFF


#endif
