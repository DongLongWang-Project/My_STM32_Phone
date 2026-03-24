#include "USART1.h"                                                                                                                    


/*
函数:     串口初始化配置
参数:     无
返回值:   无
配置:    PA9  ->USART1_TX
         PA10 ->USART1_RX
*/
void Serial_Init(void)
{ 
	/*时钟使能*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    
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
    
}

void Serial_SendByte(uint8_t Byte)
{
    USART_SendData(USART1,Byte);
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);  
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
	return ch;
}
