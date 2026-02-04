#include "key.h"


#define KEY_PRESSED				1
#define KEY_UNPRESSED			0

#define KEY_TIME_DOUBLE			0
#define KEY_TIME_LONG			  600
#define KEY_TIME_REPEAT			80

uint8_t Key_Flag[KEY_COUNT];

void Key_Init(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode= GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8 |GPIO_Pin_9;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
  GPIO_Init(GPIOB,&GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Mode= GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
  GPIO_Init(GPIOA,&GPIO_InitStruct);
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_CounterMode= TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period=1000-1;
  TIM_TimeBaseInitStruct.TIM_Prescaler=84-1;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
  TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
  
  TIM_Cmd(TIM4,ENABLE);

  TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);  
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
  NVIC_InitTypeDef NVIC_InitStruct;
  NVIC_InitStruct.NVIC_IRQChannel=TIM4_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
  NVIC_Init(&NVIC_InitStruct);
}


uint8_t Key_GetState(uint8_t n)
{
	if (n == KEY_1)
	{
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) == 0)
		{
			return KEY_PRESSED;
		}
	}
	else if (n == KEY_2)
	{
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 0)
		{
			return KEY_PRESSED;
		}
	}
	else if (n == KEY_3)
	{
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
		{
			return KEY_PRESSED;
		}
	}
	return KEY_UNPRESSED;
}

uint8_t Key_Check(uint8_t n, uint8_t Flag)
{
	if (Key_Flag[n] & Flag)
	{
		if (Flag != KEY_HOLD)
		{
			Key_Flag[n] &= ~Flag;
		}
		return 1;
	}
	return 0;
}

void Key_Tick(void)
{
	static uint8_t Count, i;
	static uint8_t CurrState[KEY_COUNT], PrevState[KEY_COUNT];
	static uint8_t S[KEY_COUNT];
	static uint16_t Time[KEY_COUNT];
	
	for (i = 0; i < KEY_COUNT; i ++)
	{
		if (Time[i] > 0)
		{
			Time[i] --;
		}
	}
	
	Count ++;
	if (Count >= 20)
	{
		Count = 0;
		
		for (i = 0; i < KEY_COUNT; i ++)
		{
			PrevState[i] = CurrState[i];
			CurrState[i] = Key_GetState(i);
			
			if (CurrState[i] == KEY_PRESSED)
			{
				Key_Flag[i] |= KEY_HOLD;
			}
			else
			{
				Key_Flag[i] &= ~KEY_HOLD;
			}
			
			if (CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED)
			{
				Key_Flag[i] |= KEY_DOWN;
			}
			
			if (CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED)
			{
				Key_Flag[i] |= KEY_UP;
			}
			
			if (S[i] == 0)
			{
				if (CurrState[i] == KEY_PRESSED)
				{
					Time[i] = KEY_TIME_LONG;
					S[i] = 1;
				}
			}
			else if (S[i] == 1)
			{
				if (CurrState[i] == KEY_UNPRESSED)
				{
					Time[i] = KEY_TIME_DOUBLE;
					S[i] = 2;
				}
				else if (Time[i] == 0)
				{
					Time[i] = KEY_TIME_REPEAT;
					Key_Flag[i] |= KEY_LONG;
					S[i] = 4;
				}
			}
			else if (S[i] == 2)
			{
				if (CurrState[i] == KEY_PRESSED)
				{
					Key_Flag[i] |= KEY_DOUBLE;
					S[i] = 3;
				}
				else if (Time[i] == 0)
				{
					Key_Flag[i] |= KEY_SINGLE;
					S[i] = 0;
				}
			}
			else if (S[i] == 3)
			{
				if (CurrState[i] == KEY_UNPRESSED)
				{
					S[i] = 0;
				}
			}
			else if (S[i] == 4)
			{
				if (CurrState[i] == KEY_UNPRESSED)
				{
					S[i] = 0;
				}
				else if (Time[i] == 0)
				{
					Time[i] = KEY_TIME_REPEAT;
					Key_Flag[i] |= KEY_REPEAT;
					S[i] = 4;
				}
			}
		}
	}
}


void TIM4_IRQHandler(void)
{

  if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)
  {
    Key_Tick();
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
  }
}
