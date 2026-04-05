#include "stm32f4xx.h"
#include "flash.h"
#include "USART1.h"
#include "w25Qxx.h"
#include "TFTLCD.h"
#include "Bootloarder.h"
#include "SD.h"
#include "Delay.h"
#include "crc.h"

update_flag_info_t update_flag_;
int main(void)
{

//     load_app(APP_Addr);


    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    delay_init(168);
    Serial_Init();
    W25Qxx_SPI_Init();
//    LCD_Init(BLACK);


//     Calculate_Sectors_From_KB(1024*100);

//    if(W25Qxx_SectorErase(UPDATE_INFO_Addr,W25Qxx_SECTOR_ERASE_4KB)==0)
//    {
//      printf("擦除成功\r\n");
//    }
//    else
//    printf("擦除失败\r\n");

//      uint8_t state=Read_Latest_update_info_(&update_flag_);
//      printf("state:0x%08X\r\n",state);



//    W25Qxx_ReadData(UPDATE_INFO_Addr, (uint8_t*)&update_flag_, sizeof(update_flag_info_t));
//        printf("file_crc:0X%08X\r\n",update_flag_.file_crc);
//        printf("file_size:%u    \r\n",update_flag_.file_size);
//        printf("file_version:%u    \r\n",update_flag_.file_version);
//        printf("update_flag:0X%08X\r\n",update_flag_.update_flag);

	while(1)
	{
         update_my_phone();
	}
}
//    if(W25Qxx_SectorErase(UPDATE_INFO_Addr,W25Qxx_SECTOR_ERASE_4KB)==0)
//    {
//      printf("擦除成功\r\n");
//    }
//    else
//    printf("擦除失败\r\n");
