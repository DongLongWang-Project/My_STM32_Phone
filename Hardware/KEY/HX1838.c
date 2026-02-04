#include "HX1838.h"

volatile uint32_t HX1838_RevData;
volatile uint8_t HX1838_RepeatFlag;
volatile uint8_t HX1838_RevFlag;
void HX1838_Init(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
  GPIO_Init(GPIOF,&GPIO_InitStruct);
  
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource6,GPIO_AF_TIM10);
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period=65536-1;
  TIM_TimeBaseInitStruct.TIM_Prescaler=168-1;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
  TIM_TimeBaseInit(TIM10,&TIM_TimeBaseInitStruct);
  
  TIM_ICInitTypeDef TIM_ICInitStruct;
  TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;
  TIM_ICInitStruct.TIM_ICFilter=0XF;
  TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Falling;
  TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1;
  TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_DirectTI;
  TIM_ICInit(TIM10,&TIM_ICInitStruct);
  
  TIM_ITConfig(TIM10,TIM_IT_CC1|TIM_IT_Update,ENABLE);
  
  NVIC_InitTypeDef NVIC_InitStruct;
  NVIC_InitStruct.NVIC_IRQChannel=TIM1_UP_TIM10_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority=4;
  NVIC_Init(&NVIC_InitStruct);
  TIM_Cmd(TIM10,ENABLE);
}

HX1828_KeyNum Get_HX1838_KeyNum(void)
{
  if(HX1838_RevFlag)
  { 
    HX1838_RevFlag=0;
    return (HX1828_KeyNum)((HX1838_RevData>>16)&0XFF);
  }
  else if(HX1838_RepeatFlag)
  {
    HX1838_RepeatFlag=0;
    return (HX1828_KeyNum)((HX1838_RevData>>16)&0XFF);
  }
  return HX1823_NULL;
}
void TIM1_UP_TIM10_IRQHandler(void)
{
    static uint8_t  state = 0;
    static uint8_t  bit_index = 0;
    static uint32_t last_ts = 0;
    static uint32_t overflow_cnt = 0;

    uint32_t now, delta;

    /* Update 中断：累计时间 */
    if (TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
        overflow_cnt++;
        /* 超时保护：20ms 无边沿 */
        if (state != 0)
        {
            now = overflow_cnt << 16;
            if ((now - last_ts) > 20000)
            {
                state = 0;
                bit_index = 0;
                overflow_cnt = 0;
                last_ts = 0;
            }
        }
    }

    /* CC1 捕获中断（下降沿） */
    if (TIM_GetITStatus(TIM10, TIM_IT_CC1) != RESET)
    {
        TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);

        now = (overflow_cnt << 16) | TIM_GetCapture1(TIM10);
        delta = now - last_ts;
        last_ts = now;
//        printf("%d\r\n",delta);
        switch (state)
        {
        /* 等待引导 */
        case 0:
            if (delta > 13000 && delta < 14000)   // 9ms + 4.5ms
            {
                state = 1;
                bit_index = 0;
                HX1838_RevData = 0;
                
            }
            else if (delta > 11000 && delta < 12000) // 重复码
            {
                HX1838_RepeatFlag = 1;
                overflow_cnt = 0;
                last_ts = 0;
//                printf("add:%x  %x\r\n",(HX1838_RevData)&0xff,~(HX1838_RevData)&0xff);
//                printf("radd:%x  %x\r\n",(HX1838_RevData>>8)&0xff,~(HX1838_RevData>>8)&0xff);
//                printf("Key:%x  %x\r\n",(HX1838_RevData>>16)&0xff,~(HX1838_RevData>>16)&0xff);
//                printf("rKey:%x  %x\r\n",(HX1838_RevData>>24)&0xff,~(HX1838_RevData>>24)&0xff);
            }
            break;

        /* 接收 32 位数据 */
        case 1:
            if (delta > 2000 && delta < 2500)     // 逻辑 1
            {
//                printf("%d:1\r\n",bit_index);
                HX1838_RevData |= (1UL << bit_index);
                bit_index++;
            }
            else if (delta > 1000 && delta < 1300) // 逻辑 0
            {
//                printf("%d:0\r\n",bit_index);
                bit_index++;
            }
            else
            {
                state = 0;
                bit_index = 0;
                break;
            }

            if (bit_index >= 32)
            {
                HX1838_RevFlag = 1;
                state = 0;
                bit_index = 0;
                overflow_cnt = 0;
                last_ts = 0;
//                printf("add:%x  %x\r\n",(HX1838_RevData)&0xff,~(HX1838_RevData)&0xff);
//                printf("radd:%x  %x\r\n",(HX1838_RevData>>8)&0xff,~(HX1838_RevData>>8)&0xff);
                printf("Key:%x  %x\r\n",(HX1838_RevData>>16)&0xff,~(HX1838_RevData>>16)&0xff);
//                printf("rKey:%x  %x\r\n",(HX1838_RevData>>24)&0xff,~(HX1838_RevData>>24)&0xff);
            }
            break;
        }
    }
}
