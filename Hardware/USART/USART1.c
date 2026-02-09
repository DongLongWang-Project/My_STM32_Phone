#include "USART1.h"

/*
函数:     串口初始化配置
参数:     无
返回值:   无
配置:    PA9  ->USART1_TX
         PA10 ->USART1_RX
*/

volatile SemaphoreHandle_t USART_DMA_Printf_Semaphore;
volatile SemaphoreHandle_t USART_DMA_Printf_MutexSemaphore;
void Serial_Init(void)
{ 
	/*时钟使能*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
    
    /*GPIO初始化*/
    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
    
    /*PA9为推挽输出模式,速度100MHZ*/
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;    
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOA,&GPIO_InitStruct);  
    /*
    初始化串口1
    */
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate=115200;
    USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode= USART_Mode_Tx;
    USART_InitStruct.USART_Parity=USART_Parity_No;
    USART_InitStruct.USART_StopBits=USART_StopBits_1;
    USART_InitStruct.USART_WordLength=USART_WordLength_8b;
    USART_Init(USART1,&USART_InitStruct);
    
    USART_Cmd(USART1,ENABLE);
    
    DMA_InitTypeDef USART1_DMA_TX_Struct; /*配置DMA发送*/
    USART1_DMA_TX_Struct.DMA_DIR=DMA_DIR_MemoryToPeripheral;/*内存->外设(AT队列取出命令发往串口数据寄存器)*/
    
    USART1_DMA_TX_Struct.DMA_Memory0BaseAddr=0;   /*内存地址(单独设置)*/
    USART1_DMA_TX_Struct.DMA_MemoryBurst=DMA_MemoryBurst_Single; /*不突发*/
    USART1_DMA_TX_Struct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;/*1字节*/
    USART1_DMA_TX_Struct.DMA_MemoryInc=DMA_MemoryInc_Enable; /*内存地址自增*/
    
    USART1_DMA_TX_Struct.DMA_PeripheralBaseAddr=(uint32_t)&USART1->DR; /*外设地址*/
    USART1_DMA_TX_Struct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;  /*不突发*/
    USART1_DMA_TX_Struct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte; /*1字节*/
    USART1_DMA_TX_Struct.DMA_PeripheralInc=DMA_PeripheralInc_Disable; /*不自增*/
    
    USART1_DMA_TX_Struct.DMA_BufferSize=0;  /*发送数据大小(单独设置)*/
    USART1_DMA_TX_Struct.DMA_Channel=DMA_Channel_4;/*通道4*/
    USART1_DMA_TX_Struct.DMA_Mode=DMA_Mode_Normal; /*正常模式*/
    USART1_DMA_TX_Struct.DMA_Priority=DMA_Priority_High;/*优先级高*/
    
    USART1_DMA_TX_Struct.DMA_FIFOMode=DMA_FIFOMode_Disable; /*关闭FIFO*/
    USART1_DMA_TX_Struct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;
    
    DMA_Init(DMA2_Stream7,&USART1_DMA_TX_Struct);
    
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
    
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
    
    USART_DMA_Printf_Semaphore=xSemaphoreCreateBinary();
    if(USART_DMA_Printf_Semaphore!=NULL)
    {
      xSemaphoreGive(USART_DMA_Printf_Semaphore);
    }
    USART_DMA_Printf_MutexSemaphore=xSemaphoreCreateMutex();
}

void Serial_SendByte(uint8_t Byte)
{
    USART_SendData(USART1,Byte);
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);  
}

void Serial_SendString(char *String)
{
	for (uint8_t i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		Serial_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
	return ch;
}

volatile uint8_t busy=0;
void USART_DMA_SendString(char *String)
{
//  busy=1;
  DMA_Cmd(DMA2_Stream7,DISABLE);
  DMA2_Stream7->M0AR=(uint32_t)String;
  DMA2_Stream7->NDTR=strlen(String);
  DMA_Cmd(DMA2_Stream7,ENABLE);
}

void DMA2_Stream7_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7)==SET)
  {
    DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    xSemaphoreGiveFromISR(USART_DMA_Printf_Semaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
 void print(char *format, ...)
{
    /*加static 防止栈溢出*/
    static char String[USART_Tx_BUF_SIZE];				//定义字符数组
    if(xTaskGetSchedulerState()==taskSCHEDULER_NOT_STARTED)
    {
        va_list arg;					//定义可变参数列表数据类型的变量arg
        va_start(arg, format);			//从format开始，接收参数列表到arg变量
        vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
        va_end(arg);					//结束变量arg
//        printf("use usart\r\n");
        Serial_SendString(String);
    }
    else
    {                        
       if(xSemaphoreTake(USART_DMA_Printf_MutexSemaphore,pdMS_TO_TICKS(1000))==pdTRUE)
        {
            va_list arg;					//定义可变参数列表数据类型的变量arg
            va_start(arg, format);			//从format开始，接收参数列表到arg变量
            vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
            va_end(arg);					//结束变量arg    
          if(xSemaphoreTake(USART_DMA_Printf_Semaphore,pdMS_TO_TICKS(200))==pdTRUE)
          {
      //     printf("use dma usart\r\n");
             USART_DMA_SendString(String);		//串口发送字符数组（字符串）
             if(xSemaphoreTake(USART_DMA_Printf_Semaphore,pdMS_TO_TICKS(500))==pdTRUE)
             {
                xSemaphoreGive(USART_DMA_Printf_Semaphore);
             }
          }
          else
          {
            Serial_SendString("DMA send is error\r\n now use normal usart send\r\n");
            Serial_SendString(String);
          }
          xSemaphoreGive(USART_DMA_Printf_MutexSemaphore); 
      }
      else
      {
          Serial_SendString("USART_DMA_Printf_MutexSemaphore is error\r\n");
          Serial_SendString(format); 
      }

    }
}
