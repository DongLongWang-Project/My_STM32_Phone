#include "ASRPRO.h"

char USART_Rev_buf[128];
void ASRPRO_Init(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
  
  GPIO_InitTypeDef USART2_GPIO_InitStruct;
  USART2_GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
  USART2_GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  USART2_GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;
  USART2_GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  USART2_GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
  
  GPIO_Init(GPIOA,&USART2_GPIO_InitStruct);
  
  GPIO_InitTypeDef EXTI_GPIO_InitStruct;
  EXTI_GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
  EXTI_GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;
  EXTI_GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;
  EXTI_GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
  GPIO_Init(GPIOA,&EXTI_GPIO_InitStruct);
  
  GPIO_InitTypeDef Control_GPIO_InitStruct;
  Control_GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
  Control_GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  Control_GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;
  Control_GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
  GPIO_Init(GPIOA,&Control_GPIO_InitStruct);
  
  
  EXTI_InitTypeDef EXTI_InitStruct;
  EXTI_InitStruct.EXTI_Line=EXTI_Line1;
  EXTI_InitStruct.EXTI_LineCmd=ENABLE;
  EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
  EXTI_Init(&EXTI_InitStruct);
  
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,GPIO_PinSource1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

 
  USART_InitTypeDef USART2_InitStruct;
  USART2_InitStruct.USART_BaudRate=115200;  /*波特率*/
  USART2_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None; /*无控制位*/
  USART2_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx; /*开启发送和接收*/
  USART2_InitStruct.USART_Parity=USART_Parity_No; /*无奇偶校验*/
  USART2_InitStruct.USART_StopBits=USART_StopBits_1; /*1停止位*/
  USART2_InitStruct.USART_WordLength=USART_WordLength_8b;/*8位数据*/
  USART_Init(USART2,&USART2_InitStruct);
  
  USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);   /*开启串口空闲中断*/
  USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);   /*开启串口接收中断*/

  
  NVIC_InitTypeDef USART2_NVIC_InitStruct;
  USART2_NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;
  USART2_NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
  USART2_NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=8;
  USART2_NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
  NVIC_Init(&USART2_NVIC_InitStruct);
  
  NVIC_InitTypeDef EXTI_NVIC_InitStruct;
  EXTI_NVIC_InitStruct.NVIC_IRQChannel=EXTI1_IRQn;
  EXTI_NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
  EXTI_NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=8;
  EXTI_NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
  NVIC_Init(&EXTI_NVIC_InitStruct);
  
  USART_Cmd(USART2,ENABLE);  
}


void USART2_SendByte(uint8_t Byte)
{
    USART_SendData(USART2,Byte);
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
}
void USART2_SendString(char *String)
{
	for (uint8_t i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		USART2_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

void Send_CMD_To_ASPPRO(ASRPRO_CMD_ENUM Cmd,uint8_t Ringtone_data)
{
   char cmd[4];
   sprintf(cmd,"%d%d%d%d",0x55,Cmd,Ringtone_data,0XAA);
   USART2_SendString(cmd);   
}

void Ringtone_State(void)
{
    uint8_t ALARM_State=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);
    uint8_t Self_State=GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_4);
   if(Self_State)
    {
       if(ALARM_State)
       {
       /*铃声正在运行*/ 
       }
       else
       {
         /*铃声已结束*/
         GPIO_ResetBits(GPIOA,GPIO_Pin_4);
       }
    }
  else
  {
         if(ALARM_State)
       {
         /*铃声正在运行*/
        /*串口发送停止命令*/       
       }
     
  }
}
void USART2_IRQHandler(void)
{
  static uint8_t Total_len=0;
  if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET)
  {
    USART_Rev_buf[Total_len++]=USART_ReceiveData(USART2);
    USART_ClearITPendingBit(USART2,USART_IT_RXNE);
  }
  if(USART_GetITStatus(USART2,USART_IT_IDLE)==SET)
  { 
    USART2->SR;
    USART2->DR;
    USART_Rev_buf[Total_len]='\0';
    Total_len=0;
  } 
}

void  EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1)==SET)
  {
    Ringtone_State();
    EXTI_ClearITPendingBit(EXTI_Line1);
  }
}
