#include "main.h"

extern  void fs_init(void);
void my_phone_config(void);

USB_OTG_CORE_HANDLE  USB_OTG_dev;
 void read_music(void);
int main(void)
{  

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
//     read_music();
    while (1)
     {

     }
}

void my_phone_config(void)
{
//     my_mem_init(SRAMIN);
         
//   memset(&wifi_save_list, 0, sizeof(wifi_save_t));
//   W25Qxx_SectorErase(WIFI_SAVE_Addr);  
     W25Qxx_ReadData(WIFI_SAVE_Addr,(uint8_t*)&wifi_save_list,sizeof(wifi_save_t)); 
     if(wifi_save_list.save_count==255)
     {
      wifi_save_list.save_count=0;
     }
     #if keil
     Alarm_System_Init();
     #endif
     print("save_count:%d",wifi_save_list.save_count);
     
}

void read_music(void)
{
  FATFS f;
  FRESULT res;
  FIL fp;
  uint32_t read_len;
  uint8_t wav_head[128];
  res=f_mount(&f,"0:",1);
  if(res!=FR_OK)
  {
    printf("挂载失败\r\n");
    return ;
  }
  res=f_open(&fp,"0:/SD/music/output_16k.wav",FA_READ);
  if(res!=FR_OK)
  {
    printf("读取文件失败\r\n");
    return ;
  }
  
  res=f_read(&fp,wav_head,sizeof(wav_head),&read_len);
              if(res==FR_OK) 
           {
             printf("num:%d\r\n",read_len);
             music_win.wav_data.NumChannels=*(uint16_t*)(&wav_head[22]);  
             music_win.wav_data.SampleRate=*(uint32_t*)(&wav_head[24]);
             music_win.wav_data.BitsPerSample=*(uint16_t*)(&wav_head[34]);
             for(uint8_t i=34;i<128;i++)
             {
              if(wav_head[i] == 0x64 && wav_head[i+1] == 0x61 && wav_head[i+2] == 0x74 && wav_head[i+3] == 0x61)
                 {
                  music_win.wav_data.DataSize=*(uint32_t*)(&wav_head[i+4]); 
                  f_lseek(&fp,i+8);
                  
                   music_win.wav_data.DataSize=music_win.wav_data.DataSize-(i+8);
                  break;                  
                 }
             }
             
           } 
}
