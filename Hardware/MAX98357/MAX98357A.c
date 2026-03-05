#include "MAX98357A.h"
#include "stdio.h"

// ---------------------- 配置 ----------------------
#define PLAY_BUF_HALF_SIZE   4096    // 内部播放缓冲半区大小（字节数）
#define PLAY_BUF_FULL_SIZE   (PLAY_BUF_HALF_SIZE*2)
#define SRAM_BUF_SIZE        (128*1024)  // 外部SRAM总大小
uint8_t play_buf[PLAY_BUF_FULL_SIZE]__attribute__((aligned(4))); // 内部DMA播放缓冲
uint8_t sram_ring_buf[SRAM_BUF_SIZE]__attribute__((section(".EXT_SRAM"))) __attribute__((aligned(4)));  // 外部SRAM大缓冲

volatile uint32_t sram_ring_buf_index = 0; // SRAM读取索引

typedef enum {
    SRAM_FILL_NONE = 0,
    SRAM_FILL_FIRST_HALF,
    SRAM_FILL_SECOND_HALF
} SRAM_FillHalf_t;

volatile SRAM_FillHalf_t sram_need_fill_half = SRAM_FILL_NONE;
volatile SRAM_FillHalf_t sram_need_sd_fill   = SRAM_FILL_NONE;

// FreeRTOS 信号量
volatile SemaphoreHandle_t Audio_BinSemaphore;

//ffmpeg -i hualian.wav -acodec pcm_s16le -ar 44100 -ac 2 hualian1.wav

void MAX98357_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S); 
    RCC_PLLI2SConfig(271, 2);
    RCC_PLLI2SCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);    
  
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 ;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin =GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
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
    I2S_InitStruct.I2S_CPOL = I2S_CPOL_Low;
    
    I2S_Init(SPI2, &I2S_InitStruct);
    
    DMA_DeInit(DMA1_Stream4);
    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE); // 等待复位完成
     
    DMA_InitTypeDef DMA_InitStruct;
    DMA_InitStruct.DMA_Channel = DMA_Channel_0;          // 通道0对应SPI2_TX
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR; // 外设地址：SPI2数据寄存器
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)play_buf;   // 内存缓冲区地址
    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;  // 内存→外设
    DMA_InitStruct.DMA_BufferSize = PLAY_BUF_HALF_SIZE/2;                // 缓冲区大小（采样点数）
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;  // 内存地址递增
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16位
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;       // 16位
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;          // 循环模式（关键！）
    DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;      // 高优先级
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
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

    I2S_Cmd(SPI2, ENABLE);
    DMA_Cmd(DMA1_Stream4, ENABLE); 
    Audio_BinSemaphore=xSemaphoreCreateBinary();
    if(Audio_BinSemaphore!=NULL)
    {
      xSemaphoreGive(Audio_BinSemaphore);
    }
    
}

// ------------------- DMA中断 ----------------------
void DMA1_Stream4_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_HTIF4) == SET)
    {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_HTIF4);
        sram_need_fill_half = SRAM_FILL_FIRST_HALF;
        xSemaphoreGiveFromISR(Audio_BinSemaphore, &xHigherPriorityTaskWoken);
    }

    if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET)
    {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
        sram_need_fill_half = SRAM_FILL_SECOND_HALF;
        xSemaphoreGiveFromISR(Audio_BinSemaphore, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// ------------------- 单任务处理播放 + SRAM补充 ----------------------
void AudioTask(void)
{
    uint32_t remain, num;
    while(1)
    {
        if(xSemaphoreTake(Audio_BinSemaphore, portMAX_DELAY) == pdTRUE)
        {
            switch(sram_need_fill_half)
            {
                case SRAM_FILL_FIRST_HALF:
                    // 内部DMA缓冲前半区
                    remain = music_win.wav_data.DataSize - sram_ring_buf_index;
                    if(remain < PLAY_BUF_HALF_SIZE)
                    {
                        memcpy(play_buf, sram_ring_buf + sram_ring_buf_index, remain);
                        memset(play_buf + remain, 0, PLAY_BUF_HALF_SIZE - remain);
                        sram_ring_buf_index = music_win.wav_data.DataSize;
                    }
                    else
                    {
                        memcpy(play_buf, sram_ring_buf + sram_ring_buf_index, PLAY_BUF_HALF_SIZE);
                        sram_ring_buf_index += PLAY_BUF_HALF_SIZE;
                    }

                    // 检查 SRAM 前半区是否需要SD补充
                    if(sram_ring_buf_index == SRAM_BUF_SIZE/2)
                        sram_need_sd_fill = SRAM_FILL_FIRST_HALF;

                    break;

                case SRAM_FILL_SECOND_HALF:
                    // 内部DMA缓冲后半区
                    remain = music_win.wav_data.DataSize - sram_ring_buf_index;
                    if(remain < PLAY_BUF_HALF_SIZE)
                    {
                        memcpy(play_buf + PLAY_BUF_HALF_SIZE, sram_ring_buf + sram_ring_buf_index, remain);
                        memset(play_buf + PLAY_BUF_HALF_SIZE + remain, 0, PLAY_BUF_HALF_SIZE - remain);
                        sram_ring_buf_index = music_win.wav_data.DataSize;
                    }
                    else
                    {
                        memcpy(play_buf + PLAY_BUF_HALF_SIZE, sram_ring_buf + sram_ring_buf_index, PLAY_BUF_HALF_SIZE);
                        sram_ring_buf_index += PLAY_BUF_HALF_SIZE;
                    }

                    // 检查 SRAM 后半区是否需要SD补充
                    if(sram_ring_buf_index >= SRAM_BUF_SIZE)
                    {
                        sram_ring_buf_index = 0;
                        sram_need_sd_fill = SRAM_FILL_SECOND_HALF;
                    }

                    break;

                default:
                    break;
            }

            sram_need_fill_half = SRAM_FILL_NONE;

            // ---------------- SD读取补充 ----------------
            if(sram_need_sd_fill == SRAM_FILL_FIRST_HALF)
            {
                lv_fs_read(&music_win.file, sram_ring_buf, SRAM_BUF_SIZE/2, &num);
                sram_need_sd_fill = SRAM_FILL_NONE;
            }
            else if(sram_need_sd_fill == SRAM_FILL_SECOND_HALF)
            {
                lv_fs_read(&music_win.file, sram_ring_buf + SRAM_BUF_SIZE/2, SRAM_BUF_SIZE/2, &num);
                sram_need_sd_fill = SRAM_FILL_NONE;
            }
        }
    }
}
