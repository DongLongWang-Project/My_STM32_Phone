#ifndef __XPT2046_H
#define __XPT2046_H

#include "stm32f4xx.h"
#include "TFTLCD.h"
#include "Delay.h"
#include  "stdlib.h"
#include "W25Qxx.h"
#include "USART1.h"

#define uint8_t u8
#define uint16_t u16

#define  XPT2046_Port1_CLK  RCC_AHB1Periph_GPIOB
#define  XPT2046_Port1    GPIOB
#define  XPT2046_CLK    GPIO_Pin_0
#define  XPT2046_MISO   GPIO_Pin_2
#define  XPT2046_PEN    GPIO_Pin_1

#define  XPT2046_Port2_CLK  RCC_AHB1Periph_GPIOC
#define  XPT2046_Port2    GPIOC
#define  XPT2046_CS     GPIO_Pin_13 
  
#define  XPT2046_Port3_CLK  RCC_AHB1Periph_GPIOF
#define  XPT2046_Port3    GPIOF
#define  XPT2046_MOSI   GPIO_Pin_11


// 触摸芯片 SPI 接口
#define XPT2046_CS_LOW()    GPIO_ResetBits(XPT2046_Port2, XPT2046_CS)
#define XPT2046_CS_HIGH()   GPIO_SetBits(XPT2046_Port2, XPT2046_CS)

#define XPT2046_CLK_LOW()   GPIO_ResetBits(XPT2046_Port1, XPT2046_CLK)
#define XPT2046_CLK_HIGH()  GPIO_SetBits(XPT2046_Port1, XPT2046_CLK)

#define XPT2046_DIN_LOW()   GPIO_ResetBits(XPT2046_Port3, XPT2046_MOSI)
#define XPT2046_DIN_HIGH()  GPIO_SetBits(XPT2046_Port3, XPT2046_MOSI)

#define XPT2046_DOUT()      (GPIO_ReadInputDataBit(XPT2046_Port1, XPT2046_MISO))
#define XPT2046_Pen()       (GPIO_ReadInputDataBit(XPT2046_Port1, XPT2046_PEN))


// ADC控制字
//#define XPT2046_CMD_X 0xD0  // 1010 0000  (12位模式)
//#define XPT2046_CMD_Y 0x90

#define XPT2046_CMD_Z1 0xB0
#define XPT2046_CMD_Z2 0xC0



#define TP_PRES_DOWN 0x80  //触屏被按下	  
#define TP_CATH_PRES 0x40  //有按键按下了 

#define OTT_MAX_TOUCH  5    		//电容屏支持的点数,固定为5点


 
//触摸屏控制器
typedef struct
{
	u8 (*init)(void);			//初始化触摸屏控制器
	u8 (*scan)(u8);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	void (*adjust)(void);		//触摸屏校准 
	u16 x[OTT_MAX_TOUCH]; 		//当前坐标
	u16 y[OTT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								//x[4],y[4]存储第一次按下时的坐标. 
	u8  sta;					//笔的状态 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
/////////////////////触摸屏校准参数(电容屏不需要校准)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	u8 touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;	 	//触屏控制器在touch.c里面定义
uint8_t XPT2046_SPI_Init(void);
u8 TP_Scan(u8 tp);
void TP_Adjust(void);
void rtp_test(void);
uint8_t TP_Get_Adjdata(uint32_t Address);
uint8_t XPT2046_Get_Adjdata(void);

#endif
