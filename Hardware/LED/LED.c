#include "LED.h"

/*--------------------------------------------------------------------------------↓
	@函数	  :
	@参数	  :
	@返回值 :
	@备注	  :
↑--------------------------------------------------------------------------------*/
//void LED_init(void)
//{

//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
//   
//    GPIO_InitTypeDef GPIO_InitStruct;
//    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;    
//    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
//    GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
//    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
//    GPIO_Init(GPIOF,&GPIO_InitStruct);  
//    
//    GPIO_SetBits(GPIOF,GPIO_Pin_9);
//    GPIO_SetBits(GPIOF,GPIO_Pin_10);
//    
//}

//void LED1_Set(uint8_t  state)
//{
//    GPIO_WriteBit(GPIOF,GPIO_Pin_9,(BitAction)state);
//}

//void LED2_Set(uint8_t  state)
//{
//    GPIO_WriteBit(GPIOF,GPIO_Pin_10,(BitAction)state);
//}

void LED_init(void)
{

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
   
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13;    
    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStruct);  
    
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    GPIO_ResetBits(GPIOB,GPIO_Pin_13);
    
}

void LED1_Set(uint8_t  state)
{
    GPIO_WriteBit(GPIOB,GPIO_Pin_12,(BitAction)state);
}

void LED2_Set(uint8_t  state)
{
    GPIO_WriteBit(GPIOB,GPIO_Pin_13,(BitAction)state);
}
