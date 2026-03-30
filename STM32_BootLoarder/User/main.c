#include "stm32f4xx.h"
#include "flash.h"
#include "USART1.h"
#include "w25Qxx.h"
#include "TFTLCD.h"
#include "Bootloarder.h"
#include "SD.h"
#include "Delay.h"
#include "crc.h"

int main(void)
{

//     load_app(APP_Addr);


    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    delay_init(168);
    Serial_Init();
    W25Qxx_SPI_Init();
    LCD_Init(BLACK);

    update_my_phone();


	while(1)
	{

	}
}
//    if(W25Qxx_SectorErase(TOUCH_SAVE_VER_Addr,W25Qxx_SECTOR_ERASE_4KB)==0)
//    {
//      printf("擦除成功\r\n");
//    }
//    else
//    printf("擦除失败\r\n");
