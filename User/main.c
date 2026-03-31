#include "main.h"


USB_OTG_CORE_HANDLE  USB_OTG_dev;
int main(void)
{  
    __enable_irq();
    NVIC_SetVectorTable(NVIC_VectTab_FLASH,APP_Addr);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    Delay_init(168);
   
    Serial_Init();

    TIM2_Int_Init(10000-1,8400-1);
    Key_Init();
    W25Qxx_SPI_Init();
    FSMC_SRAM_Init();
    MAX98357_Init();
    DX_WF25_Init();
    update_font();
    USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);


    my_phone_config();
     
    MyPhone_stm32_task();

    while (1)
     {

     }
}

void my_phone_config(void)
{
//   my_mem_init(SRAMIN);      
//   W25Qxx_SectorErase(WIFI_SAVE_Addr);  
     W25Qxx_ReadData(WIFI_SAVE_Addr,(uint8_t*)&wifi_save_list,sizeof(wifi_save_t)); 
     if(wifi_save_list.save_count==255)
     {
      wifi_save_list.save_count=0;
     }
     print("wifi_save_count:%d",wifi_save_list.save_count);
     #if keil
     Alarm_System_Init();
     #endif  
}
