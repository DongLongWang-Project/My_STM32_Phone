#include "LVGL_Timer.h"

void TIM3_Int_Init(uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = arr;              // 自动重装值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;           // 预分频系数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 高优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM3, ENABLE);
}

void TIM2_Int_Init(uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8; // 比 TIM3 低
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

//void TIM2_IRQHandler(void)
//{
//    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
//    {
////        lv_task_handler();  // 刷新 LVGL 任务
//        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//    }
//}

//void TIM3_IRQHandler(void)
//{
//       /* 检测时间更新中断的标志位是否置位 */
//    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
//    {
//        lv_tick_inc(1); // 告诉LVGL：时间过了1ms
//        /* 清空标志位 */
//        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//    }
//}
void TIM12_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;//PB15 推挽输出,控制背光
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//普通输出模式
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStruct);//初始化 //PB15 推挽输出,控制背光
 
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_TIM12);//PB15
    
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_InternalClockConfig(TIM12);
    TIM_TimeBaseStructure.TIM_Period = 100;
    TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

    TIM_OCInitTypeDef TIM_OCInitStruct;
    TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse=100;
    TIM_OC2Init(TIM12,&TIM_OCInitStruct);
    
    TIM_OC2PreloadConfig(TIM12, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
   
    TIM_ARRPreloadConfig(TIM12,ENABLE);//ARPE使能 
    TIM_Cmd(TIM12, ENABLE);
}

