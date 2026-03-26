#include "stm32f4xx.h"
#include "flash.h"
#include "USART1.h"
#include "w25Qxx.h"
#include "TFTLCD.h"
#include "Bootloarder.h"

int main(void)
{
  load_app(APP_Addr);
//    Serial_Init();
//    W25Qxx_SPI_Init();
//    w25q_DMA_readdata(0x00000000,read_buf,100);
//    LCD_Init(WHITE);
//    TFTLCD_Printf(0,0,&Font_Size_6x8,RED,"1234");
   
	while(1)
	{
//    if(read_ok_flag==1)
//    {
//      read_ok_flag=0;
//    for(uint8_t i=0;i<100;i++)
//    {
//      printf("0X%02X ",read_buf[i]);
//    }
//    }
	}
}
