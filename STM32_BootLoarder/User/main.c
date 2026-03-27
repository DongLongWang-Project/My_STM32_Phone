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
    LCD_Init(BLACK);
    printf("开始挂载sd\r\n");

    get_update_file_head();
//  load_app(APP_Addr);
	while(1)
	{

	}
}
