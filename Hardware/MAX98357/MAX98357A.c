#include "MAX98357A.h"
#include "stdio.h"

// ---------------------- 配置 ----------------------

uint8_t play_buf[PLAY_BUF_FULL_SIZE]__attribute__((aligned(4))); // 内部DMA播放缓冲
uint8_t sram_ring_buf[SRAM_BUF_SIZE]__attribute__((section(".EXT_SRAM"))) __attribute__((aligned(4)));  // 外部SRAM大缓冲

volatile uint32_t sram_ring_buf_index = 0; // SRAM读取索引

typedef enum {
    SRAM_FILL_NONE = 0,
    SRAM_FILL_FIRST_HALF,
    SRAM_FILL_SECOND_HALF
} SRAM_FillHalf_t;



// FreeRTOS 信号量
volatile SemaphoreHandle_t Audio_Fill_Buf_Queue;

//ffmpeg -i hualian.wav -acodec pcm_s16le -ar 44100 -ac 2 hualian1.wav

void MAX98357_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    RCC_PLLI2SConfig(271, 2);//44100
//    RCC_PLLI2SConfig(192, 3);//16
//    RCC_PLLI2SConfig(258, 3);//48

 
    RCC_PLLI2SCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);    
  
    RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S); 
      
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 ;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin =GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);
    
    SPI_I2S_DeInit(SPI2);

    I2S_InitTypeDef I2S_InitStruct;
    I2S_InitStruct.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitStruct.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStruct.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStruct.I2S_MCLKOutput = I2S_MCLKOutput_Disable; // MAX98357不用MCLK
    I2S_InitStruct.I2S_AudioFreq = I2S_AudioFreq_44k; // 44100Hz
//    I2S_InitStruct.I2S_AudioFreq = I2S_AudioFreq_16k; // 44100Hz
    
    I2S_InitStruct.I2S_CPOL = I2S_CPOL_Low;
                   
    I2S_Init(SPI2, &I2S_InitStruct);
    
//    SPI2->I2SPR = 0x013E;//16
    SPI2->I2SPR = 0x0130;;//44100
   
    DMA_DeInit(DMA1_Stream4);
    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE); // 等待复位完成
     
    DMA_InitTypeDef DMA_InitStruct;
    DMA_InitStruct.DMA_Channel = DMA_Channel_0;          // 通道0对应SPI2_TX
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR; // 外设地址：SPI2数据寄存器
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)play_buf;   // 内存缓冲区地址
    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;  // 内存→外设
    DMA_InitStruct.DMA_BufferSize = PLAY_BUF_HALF_SIZE;                // 缓冲区大小（采样点数）
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;  // 内存地址递增
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16位
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;       // 16位
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;          // 循环模式（关键！）
    DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;      // 高优先级
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    
    DMA_Init(DMA1_Stream4, &DMA_InitStruct);
    
    
    DMA_ITConfig(DMA1_Stream4, DMA_IT_TC , ENABLE);
    DMA_ITConfig(DMA1_Stream4, DMA_IT_HT , ENABLE);
    

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE); 
    
    memset(play_buf,0,sizeof(play_buf));
    memset(sram_ring_buf,0,sizeof(sram_ring_buf));
    
// --- 纯净方波测试块开始 ---
// 16位音频数据，0x2000 为正半周，0xE000 为负半周（带符号 16-bit）
//uint16_t *test_ptr = (uint16_t *)play_buf;
//for (uint32_t i = 0; i < (PLAY_BUF_FULL_SIZE / 2); i++)
//{
//    // 每 16 个采样点翻转一次电平，产生约 1kHz 的方波 (针对 16k 或 44.1k 采样率)
//    if ((i / 16) % 2 == 0) {
//        test_ptr[i] = 0x2000; // 一个明显的正电压
//    } else {
//        test_ptr[i] = 0xE000; // 一个明显的负电压
//    }
//}
// --- 纯净方波测试块结束 ---

    
    I2S_Cmd(SPI2, ENABLE);
     
    Audio_Fill_Buf_Queue=xQueueCreate(Audio_Fill_Buf_Queue_MAX_Len,sizeof(SRAM_FillHalf_t));

//    if(Audio_BinSemaphore!=NULL)
//    {
//      xSemaphoreGive(Audio_BinSemaphore);
//    }
    
}

// ------------------- DMA中断 ----------------------
void DMA1_Stream4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_HTIF4) == SET)
    {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_HTIF4);
        SRAM_FillHalf_t sram_need_fill_half = SRAM_FILL_FIRST_HALF;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(Audio_Fill_Buf_Queue,&sram_need_fill_half,&xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        
    }

    if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET)
    {
       DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
        SRAM_FillHalf_t sram_need_fill_half = SRAM_FILL_SECOND_HALF;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(Audio_Fill_Buf_Queue,&sram_need_fill_half,&xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);   
    }


}

// ------------------- 单任务处理播放 + SRAM补充 ----------------------
void AudioTask(void)
{
    uint32_t num;
    // 1. total_read 必须严格等于“已发送给 DMA 播放”的总量
     
    static uint8_t closing_cnt = 0;
    SRAM_FillHalf_t sram_need_fill_half;

        // 关键点：必须死等信号量！没收到 DMA 的 HT/TC 中断信号，坚决不动
        if(xQueueReceive(Audio_Fill_Buf_Queue,&sram_need_fill_half,portMAX_DELAY) == pdTRUE)
        {
               if (closing_cnt == 0) 
                {


            // --- 步骤 A：同步搬运 ---
            if(sram_need_fill_half == SRAM_FILL_FIRST_HALF)
            {
                memcpy(play_buf, sram_ring_buf + sram_ring_buf_index, PLAY_BUF_HALF_SIZE);
                sram_ring_buf_index += PLAY_BUF_HALF_SIZE;
                music_win.music_time.total_played += PLAY_BUF_HALF_SIZE; // 这里才是真实的进度
            }
            else if(sram_need_fill_half == SRAM_FILL_SECOND_HALF)
            {
                memcpy(play_buf + PLAY_BUF_HALF_SIZE, sram_ring_buf + sram_ring_buf_index, PLAY_BUF_HALF_SIZE);
                sram_ring_buf_index += PLAY_BUF_HALF_SIZE;
                music_win.music_time.total_played += PLAY_BUF_HALF_SIZE;
            }
               music_win.music_time.cur_time_ms=(uint32_t)((uint64_t)music_win.music_time.total_played*1000/music_win.music_time.byte_sec);
            // --- 步骤 B：检查 SRAM 环形缓冲区是否需要从 SD 卡“拉货” ---
            // 只要 sram_ring_buf_index 跑完了一半 (64KB)，就去填 SRAM
            if(sram_ring_buf_index == SRAM_BUF_SIZE / 2)
            {
                // 注意：这里是填充 SRAM 的 0~64KB 区域
                lv_fs_read(&music_win.file, sram_ring_buf, SRAM_BUF_SIZE / 2, &num);
            }
            else if(sram_ring_buf_index >= SRAM_BUF_SIZE)
            {
                sram_ring_buf_index = 0; // SRAM 索引复位
                // 这里是填充 SRAM 的 64~128KB 区域
                lv_fs_read(&music_win.file, sram_ring_buf + SRAM_BUF_SIZE / 2, SRAM_BUF_SIZE / 2, &num);
            }

            // --- 步骤 C：文件末尾判断 ---
            if(music_win.music_time.total_played >= music_win.wav_data.DataSize)
            {

                    closing_cnt=1;                  
            }
          }
          else
          {
              // 停止播放逻辑... 
              closing_cnt++;
//              printf("closing_cnt:%d\r\n",closing_cnt);
               if(closing_cnt>5)
               {
                    music_stop();
                    
                    closing_cnt=0; 
               }


          }
        }

}


void music_playing(void)
{
   DMA_Cmd(DMA1_Stream4,ENABLE);
   lv_timer_resume(music_win.music_timer);
   music_win.state=MUSIC_STATE_PLAYING;  
}
void music_pause(void)
{
   DMA_Cmd(DMA1_Stream4,DISABLE);
   while(DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);
   lv_timer_pause(music_win.music_timer);
   music_win.state=MUSIC_STATE_PAUSED; 
}
void music_stop(void)                     
{
   DMA_Cmd(DMA1_Stream4,DISABLE);
   while(DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);
   lv_timer_pause(music_win.music_timer);
   music_win.state=MUSIC_STATE_STOP; 
   
   if(music_win.file.drv!=NULL)
   {
    lv_fs_close(&music_win.file);
    music_win.file.drv=NULL;
   }
    music_win.music_time.total_played=0;
    sram_ring_buf_index=0;
    memset(play_buf,0,sizeof(play_buf));
    memset(sram_ring_buf,0,sizeof(sram_ring_buf)); 
}

void music_refresh(void)
{
  
}