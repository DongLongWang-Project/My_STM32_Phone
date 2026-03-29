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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    delay_init(168);
    Serial_Init();
    crc_init();
    W25Qxx_SPI_Init();
//    get_update_file_head(HEAD_FLASH);
////    LCD_Init(BLACK);
////    RCC->AHB3ENR &= ~(0x01); // 禁用 FSMC 时钟
    update_my_phone();
//     load_app(APP_Addr);
	while(1)
	{

	}
}
