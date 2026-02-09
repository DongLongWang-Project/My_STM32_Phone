#ifndef 	__TFTLCD_H
#define	    __TFTLCD_H

#include "stm32f4xx.h"
#include "Delay.h"
#include "stdio.h"
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "SRAM.h"
#include "LVGL_Timer.h"
//#include "ui_app_setting.h"

//画笔颜色
#define WHITE         	  0xFFFF
#define BLACK         	  0x0000	  
#define BLUE         	    0x001F  
#define BRED              0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			      0XBC40 //棕色
#define BRRED 			      0XFC07 //棕红色
#define GRAY  			      0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


#define Direction 0xC8			//0X00 正常竖屏	//0XC0	竖屏翻转
														//0X60 横屏			//0XA0	横屏翻转			0X08 BGR顺序	


extern uint16_t LCD_Height;
extern uint16_t LCD_Length;
//#if (Direction == 0x08) || (Direction == 0xC8)
//// 竖屏或竖屏翻转
//#define LCD_Height   320
//#define LCD_Length  240

//#elif (Direction == 0x68) || (Direction == 0xA8)
//// 横屏或横屏翻转
//#define LCD_Height   240
//#define LCD_Length  320

//#endif

typedef enum
{
 Dir_Ver=0x08,
 Dir_Rev_Ver=0xC8,
 Dir_Hor=0x68,
 Dir_Rev_Hor=0xA8,
}LCD_Direction;
extern LCD_Direction Cur_scr_dir;

typedef struct{
    uint16_t LCD_Reg;
    uint16_t LCD_RAM;
}LCD_AddrTypeDef;

#define LCD_Addr_Base ((uint32_t)(0X6C000000 | 0X0000007E))
#define LCD             ((LCD_AddrTypeDef *)LCD_Addr_Base)

typedef struct {
    uint8_t  Font_Width;   // 字宽
    uint8_t  Font_Height;  // 字高
    uint8_t  Font_Num_ASCII;     // 点阵数据总字节数
		uint8_t	Font_Num_CN;
		uint32_t Font_Offset_CN;			 //字库偏移值
} Font_Size;
typedef struct
{
	char Index[4];
	uint8_t Data_16x16[32];
}CN_Font_16x16;

typedef struct
{
	char Index[4];
	uint8_t Data_24x24[72];
}CN_Font_24x24;


typedef struct
{
	char Index[4];
	uint8_t Data_32x32[128];
}CN_Font_32x32;

extern const Font_Size Font_Size_6x8; 
extern const Font_Size Font_Size_8x16;
extern const Font_Size Font_Size_12x24;

extern const Font_Size Font_Size_16x16;
extern const Font_Size Font_Size_24x24;
extern const Font_Size Font_Size_32x32;

extern 	const uint8_t Font_6x8[95][6];
extern 	const uint8_t Font_8x16[95][16];

extern	const CN_Font_16x16 Font_CN_16x16[];
extern	const CN_Font_24x24 Font_CN_24x24[];
extern	const CN_Font_32x32 Font_CN_32x32[];

void LCD_Init(uint16_t Color);
void TFTLCD_ClearAreas(uint16_t X1, uint16_t X2, uint16_t Y1, uint16_t Y2, uint16_t Color);
void TFTLCD_Set_Coordinate(uint16_t X1,uint16_t X2,uint16_t Y1,uint16_t Y2);
void LCD_Write_Data(uint16_t Data);

void TFTLCD_ShowChar(uint16_t X,uint16_t Y,char Char,Font_Size *FontSize,uint16_t Color_Penceil );
void TFTLCD_Printf(uint16_t X, uint16_t Y, Font_Size *FontSize,uint16_t Color_Penceil,char *format, ...);
void TFTLCD_ShowImages(uint16_t X, uint16_t Y, uint16_t width, uint16_t height, const uint8_t *Images) ;

void TFTLCD_DrawPoint(uint16_t X,uint16_t Y,uint16_t Color);
void  TFTLCD_DrawLine(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t Color);
void TFTLCD_ShowCircle(int16_t x0, int16_t y0, int16_t Radius , uint16_t Color_Pencil);
void TFTLCD_Drow_Touch_Point(u16 x,u16 y,u16 color);
void TP_Draw_Big_Point(u16 x,u16 y,u16 color);

void LCD_DMA_Init(void);
void LCD_DMA_Transform(uint32_t Addr,uint16_t count) ;


void LCD_Set_Direction(LCD_Direction Dir);

#endif
