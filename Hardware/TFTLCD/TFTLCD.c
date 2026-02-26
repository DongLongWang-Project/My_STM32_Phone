#include "TFTLCD.h"
#include "TFTLCD_Data.h"

/*--------------------------------------------------------------------------------↓
	@项目	:TFTLCD的底层驱动
	@日期	: 2025-11-28
	@备注	:1.屏幕芯片为:ILI9341
         2.屏幕驱动方式:FMSC驱动
         3.触摸芯片:XPT2046
         4.触摸芯片驱动方式:硬件SPI
↑--------------------------------------------------------------------------------*/

uint16_t Background=WHITE;
uint16_t LCD_Height=320;
uint16_t LCD_Length=240;
LCD_Direction Cur_scr_dir=Dir_Rev_Ver;
const Font_Size Font_Size_6x8  = {6, 8, 6}; // ASCII
const Font_Size Font_Size_8x16 = {8, 16, 16}; // ASCII
const Font_Size Font_Size_12x24 = {12, 24, 36}; // ASCII

const Font_Size Font_Size_16x16 = {16, 16, 0,32,0x00000000+0X4680}; // 中文
const Font_Size Font_Size_24x24 = {24, 24, 0,72,0x00045080+0X9EA0}; // 中文
const Font_Size Font_Size_32x32 = {32, 32, 0,128,0x000E05A0+0X11A00}; // 中文

/*--------------------------------------------------------------------------------↓
                              FMCS初始化
↑--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------↓
	@函数	  :
	@参数	  :
	@返回值 :
	@备注	  :
↑--------------------------------------------------------------------------------*/
void FSMC_Init(void)
{
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);//使能PD,PE,PF,PG时钟  
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);//使能FSMC时钟  
     
    GPIO_InitTypeDef GPIO_InitStruct;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

//    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;//PB15 推挽输出,控制背光
//    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
//    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
//    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//    GPIO_Init(GPIOB, &GPIO_InitStruct);//初始化 //PB15 推挽输出,控制背光

    GPIO_InitStruct.GPIO_Pin = (3<<0)|(3<<4)|(7<<8)|(3<<14);//PD0,1,4,5,8,9,10,14,15 AF OUT
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//复用输出
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOD, &GPIO_InitStruct);//初始化  

    GPIO_InitStruct.GPIO_Pin = (0X1FF<<7);//PE7~15,AF OUT
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//复用输出
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOE, &GPIO_InitStruct);//初始化  

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;//PF12,FSMC_A6
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//复用输出 
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOF, &GPIO_InitStruct);//初始化  

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;//PF12,FSMC_A6
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//复用输出
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOG, &GPIO_InitStruct);//初始化 

    GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);//PD0,AF12
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);//PD1,AF12
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);//PD15,AF12


    
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);//PE7,AF12
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12

    GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);//PF12,AF12
    GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);

    readWriteTiming.FSMC_AddressSetupTime = 4;	 //地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns	
    readWriteTiming.FSMC_AddressHoldTime = 0;	 //地址保持时间（ADDHLD）模式A未用到	
    readWriteTiming.FSMC_DataSetupTime = 12;			//数据保存时间为25个HCLK	=6*25=150ns
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 


    writeTiming.FSMC_AddressSetupTime =4;	      //地址建立时间（ADDSET）为8个HCLK =48ns 
    writeTiming.FSMC_AddressHoldTime = 0;	 //地址保持时间（A		
    writeTiming.FSMC_DataSetupTime = 12;		 //数据保存时间为6ns*9个HCLK=54ns
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 


    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // 使能BANK1 
}
/*--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------↓
		        			
↑--------------------------------------------------------------------------------*/

 void TFTLCD_W_Command(uint16_t Cmd)
{
    LCD->LCD_Reg=Cmd;
}

void LCD_Write_Data(uint16_t Data)
{
    LCD->LCD_RAM=Data;
}

uint16_t LCD_Read_Data(void)
{
    return LCD->LCD_RAM;
}

void LCD_Set_Direction(LCD_Direction Dir)
{
    Cur_scr_dir=Dir;
    TFTLCD_W_Command(0x36); /*存储器访问控制*/    
    LCD_Write_Data(Dir); 
    if (Dir==Dir_Ver || Dir==Dir_Rev_Ver)
    {
      LCD_Height=320;
      LCD_Length=240;  
    }
    else if (Dir==Dir_Hor ||Dir==Dir_Rev_Hor)
    {
      LCD_Height=240;
      LCD_Length=320;      
    }

}
void LCD_ReadID(void)
{
    uint16_t a,b,c,d;
    
    TFTLCD_W_Command(0XD3);
    a=LCD_Read_Data();
    b=LCD_Read_Data();
    c=LCD_Read_Data();
    d=LCD_Read_Data();
    
    printf("%x%x%x%x",a,b,c,d);
}



void TFTLCD_Set_Coordinate(uint16_t X1,uint16_t X2,uint16_t Y1,uint16_t Y2)
{
    TFTLCD_W_Command(0X2A);
    LCD_Write_Data(X1>>8);
    LCD_Write_Data(X1);
    LCD_Write_Data(X2>>8);
    LCD_Write_Data(X2);
    
    
    TFTLCD_W_Command(0X2B);
    LCD_Write_Data(Y1>>8);
    LCD_Write_Data(Y1);
    LCD_Write_Data(Y2>>8);
    LCD_Write_Data(Y2);
    
    TFTLCD_W_Command(0X2C);
}
void TFTLCD_ClearAreas(uint16_t X1, uint16_t X2, uint16_t Y1, uint16_t Y2, uint16_t Color)
{
    if (X1 > X2) { uint16_t t = X1; X1 = X2; X2 = t; }
    if (Y1 > Y2) { uint16_t t = Y1; Y1 = Y2; Y2 = t; }

    TFTLCD_Set_Coordinate(X1, X2, Y1, Y2);   // 内部已发 0x2C

    uint32_t w = (uint32_t)(X2 - X1 + 1);
    uint32_t h = (uint32_t)(Y2 - Y1 + 1);
    uint32_t count = w * h;

    while (count--) {
        LCD_Write_Data(Color);       
    }
}

void LCD_Init(uint16_t Color)
{
        TIM12_Init();
        FSMC_Init();
                
        Delay_ms(50);
        TFTLCD_W_Command(0xCF);  /*功耗控制*/
        LCD_Write_Data(0x00);
        LCD_Write_Data(0xC1); 
        LCD_Write_Data(0X30); 
    
        TFTLCD_W_Command(0xED);  /*电源序列控制*/
        LCD_Write_Data(0x64); 
        LCD_Write_Data(0x03); 
        LCD_Write_Data(0X12); 
        LCD_Write_Data(0X81); 
    
        TFTLCD_W_Command(0xE8);  /*驱动时许控制A*/
        LCD_Write_Data(0x85); 
        LCD_Write_Data(0x10); 
        LCD_Write_Data(0x7A); 
    
        TFTLCD_W_Command(0xCB);  /*功耗控制A*/
        LCD_Write_Data(0x39); 
        LCD_Write_Data(0x2C); 
        LCD_Write_Data(0x00); 
        LCD_Write_Data(0x34); 
        LCD_Write_Data(0x02); 
        
        TFTLCD_W_Command(0xF7);  /*泵比控制*/
        LCD_Write_Data(0x20);
        
        TFTLCD_W_Command(0xEA);  /*驱动时序控制B BBB（EAH EAHEAHEAH）*/
        LCD_Write_Data(0x00); 
        LCD_Write_Data(0x00);
        
        TFTLCD_W_Command(0xC0);    /*功耗控制1*/
        LCD_Write_Data(0x1B);  
        
        TFTLCD_W_Command(0xC1);   /*功耗控制2*/ 
        LCD_Write_Data(0x01);    
        
        TFTLCD_W_Command(0xC5);  /*VCOM控制1*/   
        LCD_Write_Data(0x30); 	 
        LCD_Write_Data(0x30);
        
        TFTLCD_W_Command(0xC7);    /*VCOM控制2*/
        LCD_Write_Data(0XB7);
        
        TFTLCD_W_Command(0x36); /*存储器访问控制*/    
        LCD_Write_Data(Direction); 
        
        TFTLCD_W_Command(0x3A);  /*像素格式设置*/ 
        LCD_Write_Data(0x55); 
        
        TFTLCD_W_Command(0xB1);   /*帧速率控制*/
        LCD_Write_Data(0x00);   
        LCD_Write_Data(0x1B                                                                                                                                                                                                                                                                                                                                                                            ); 
        
        TFTLCD_W_Command(0xB6);  /*显示功能控制*/  
        LCD_Write_Data(0x0A); 
        LCD_Write_Data(0xA2); 
        
        TFTLCD_W_Command(0xF2);   /*使能3伽马控制*/
        LCD_Write_Data(0x00); 
        
        TFTLCD_W_Command(0x26); /*伽马设置*/  
        LCD_Write_Data(0x01); 
        
        TFTLCD_W_Command(0xE0);    /*正极伽马校准*/
        LCD_Write_Data(0x0F); 
        LCD_Write_Data(0x2A); 
        LCD_Write_Data(0x28); 
        LCD_Write_Data(0x08); 
        LCD_Write_Data(0x0E); 
        LCD_Write_Data(0x08); 
        LCD_Write_Data(0x54); 
        LCD_Write_Data(0XA9); 
        LCD_Write_Data(0x43); 
        LCD_Write_Data(0x0A); 
        LCD_Write_Data(0x0F); 
        LCD_Write_Data(0x00); 
        LCD_Write_Data(0x00); 
        LCD_Write_Data(0x00); 
        LCD_Write_Data(0x00); 	
        
        TFTLCD_W_Command(0XE1);   /*负极伽马校准*/ 
        LCD_Write_Data(0x15); 
        LCD_Write_Data(0x17); 
        LCD_Write_Data(0x07); 
        LCD_Write_Data(0x11); 
        LCD_Write_Data(0x06); 
        LCD_Write_Data(0x2B); 
        LCD_Write_Data(0x56); 
        LCD_Write_Data(0x3C); 
        LCD_Write_Data(0x05); 
        LCD_Write_Data(0x10); 
        LCD_Write_Data(0x0F); 
        LCD_Write_Data(0x3F); 
        LCD_Write_Data(0x3F); 
        LCD_Write_Data(0x0F); 
                
        TFTLCD_W_Command(0x11); /*退出睡眠模式（*/
        Delay_ms(120);
        TFTLCD_W_Command(0x29);/*开显示*/ 
//        LCD_ReadID();
//         GPIO_SetBits(GPIOB,GPIO_Pin_15);


//        TIM12->CCR2=display_cfg.Brightness;
        TFTLCD_ClearAreas(0,LCD_Length-1,0,LCD_Height-1,Color); 
}
/*--------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------↓
		        			
↑--------------------------------------------------------------------------------*/

void TFTLCD_DrawPoint(uint16_t X,uint16_t Y,uint16_t Color)
{
    TFTLCD_ClearAreas(X,X,Y,Y,Color);
}

void  TFTLCD_DrawLine(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t Color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;

	if (abs(dx) > abs(dy)) // 横向为主
	{
		if (x0 > x1) { uint16_t t = x0; x0 = x1; x1 = t; t = y0; y0 = y1; y1 = t; } // 交换起点终点

		float k = (float)(y1 - y0) / (x1 - x0);
		for (uint16_t x = x0; x <= x1; x++) {
			uint16_t y = y0 + (uint16_t)((x - x0) * k);
			TFTLCD_DrawPoint(x, y, Color);
		}
	}
	else // 纵向为主
	{
		if (y0 > y1) { uint16_t t = x0; x0 = x1; x1 = t; t = y0; y0 = y1; y1 = t; }

		float k = (float)(x1 - x0) / (y1 - y0);
		for (uint16_t y = y0; y <= y1; y++) {
			uint16_t x = x0 + (uint16_t)((y - y0) * k);
			TFTLCD_DrawPoint(x, y, Color);
		}
	}  
}

void TFTLCD_ShowCircle(int16_t x0, int16_t y0, int16_t Radius , uint16_t Color_Pencil)
{
	for (int16_t y = y0 - Radius; y <= y0 + Radius; y++)
	{
		for (int16_t x = x0 - Radius; x <= x0 + Radius; x++)
		{
			int32_t d = (x - x0)*(x - x0) + (y - y0)*(y - y0);
			if (abs(d - Radius * Radius) <= Radius)  // 宽度为1的环
			{
				TFTLCD_DrawPoint(x, y, Color_Pencil);
			}
		}
	}	  
}

void TFTLCD_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	TFTLCD_DrawLine(x-12,y,x+13,y,color);//横线
	TFTLCD_DrawLine(x,y-12,x,y+13,color);//竖线
	TFTLCD_DrawPoint(x+1,y+1,color);
	TFTLCD_DrawPoint(x-1,y+1,color);
	TFTLCD_DrawPoint(x+1,y-1,color);
	TFTLCD_DrawPoint(x-1,y-1,color);
	TFTLCD_ShowCircle(x,y,6,color);//画中心圈
}

void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	TFTLCD_DrawPoint(x,y,color);//中心点 
	TFTLCD_DrawPoint(x+1,y,color);
	TFTLCD_DrawPoint(x,y+1,color);
	TFTLCD_DrawPoint(x+1,y+1,color);	 	  	
}
void TFTLCD_ShowChar(uint16_t X,uint16_t Y,char Char,Font_Size *FontSize,uint16_t Color_Penceil )
{
  TFTLCD_ClearAreas(X,X+FontSize->Font_Width-1,Y,Y+FontSize->Font_Height,Background);
	uint8_t Font_Index= Char - ' ';
  	if(FontSize==&Font_Size_6x8)
	{
    		const uint8_t *Font_Ptr = Font_6x8[Font_Index];
        for(uint8_t i=0;i<8;i++)
		{
			for(uint8_t j=0;j<6;j++)
			{
      
        LCD_Write_Data((Font_Ptr[j] & (1<<i)) ? Color_Penceil :Background) ;
      }
     }
  }
	if(FontSize==&Font_Size_8x16)
	{
		const uint8_t *Font_Ptr = Font_8x16[Font_Index];
		for(uint8_t i=0;i<16;i++)
		{
			uint8_t Bites = Font_Ptr[i];
			for(uint8_t j=0;j<8;j++)
			{
				LCD_Write_Data((Bites & (1<<j)) ? Color_Penceil :Background);
			}
		}
	}
	
	if(FontSize==&Font_Size_12x24)
	{
		const uint8_t *Font_Ptr = Font_12x24[Font_Index];
		for (uint8_t block = 0; block < 3; block++)          
		{
				for (uint8_t i = 0; i < 8; i++)           
				{
						for (uint8_t j = 0; j < 12; j++)       
						{
								uint8_t data = Font_Ptr[block * 12 + j];
								LCD_Write_Data((data & (1 << i)) ? Color_Penceil : Background);
						}
				}
		}
	}
}

void TFTLCD_ShowString(uint16_t X, uint16_t Y,  Font_Size *FontSize, char *String, uint16_t Color_Pencil)
{
    uint16_t i = 0;
    char SingleChar[5];
    uint8_t CharLength = 0;
    Font_Size *CurFontSize;
    while (String[i] != '\0') // 遍历字符串
    {
        CurFontSize = FontSize; // 重置为传入字体大小
        // GB2312编码提取单字符
        if ((String[i] & 0x80) == 0x00)           // 1字节字符
        {
            CharLength = 1;
            SingleChar[0] = String[i++];
            SingleChar[1] = '\0';
        }
        else                                      // 2字节字符
        {
            CharLength = 2;
            SingleChar[0] = String[i++];
            if (String[i] == '\0') break;
            SingleChar[1] = String[i++];
            SingleChar[2] = '\0';
        }
        // 根据字符长度和字体切换处理
        if (CharLength == 1) // 单字节字符（ASCII）
        {
         TFTLCD_ShowChar(X, Y, SingleChar[0], CurFontSize, Color_Pencil);
         X += CurFontSize->Font_Width;
        }
        else
        {
            if (CurFontSize == &Font_Size_8x16) CurFontSize = (Font_Size *)&Font_Size_16x16;
            else if (CurFontSize == &Font_Size_12x24) CurFontSize = (Font_Size *)&Font_Size_24x24;

						uint16_t pIndex;
            if (CurFontSize == &Font_Size_16x16)
            {
                for (pIndex = 0; strcmp(Font_CN_16x16[pIndex].Index, "") != 0; pIndex++)
                {
                    if (strcmp(Font_CN_16x16[pIndex].Index, SingleChar) == 0) break;
                }
                TFTLCD_ClearAreas(X, X + 16 - 1,Y,  Y + 16 - 1, Background);

                for (uint8_t j = 0; j < 32; j++)
                {
                    uint8_t Bites = Font_CN_16x16[pIndex].Data_16x16[j];
                    for (uint8_t i_bit = 0; i_bit < 8; i_bit++)
                    {
                        LCD_Write_Data((Bites & (1 << i_bit)) ? Color_Pencil : Background);
                    }
                }
                X += 16;
              }
            else if (CurFontSize == &Font_Size_24x24)
            {
                for (pIndex = 0; strcmp(Font_CN_24x24[pIndex].Index, "") != 0; pIndex++)
                {
                    if (strcmp(Font_CN_24x24[pIndex].Index, SingleChar) == 0) break;
                }
                TFTLCD_ClearAreas(X, X + 24 - 1,Y,  Y + 24 - 1, Background);

                for (uint8_t j = 0; j < 72; j++)
                {
                    uint8_t Bites = Font_CN_24x24[pIndex].Data_24x24[j];
                    for (uint8_t i_bit = 0; i_bit < 8; i_bit++)
                    {
                        LCD_Write_Data( (Bites & (1 << i_bit)) ? Color_Pencil : Background);
                    }
                }
                X += 24;
            }
            else if (CurFontSize == &Font_Size_32x32)
            {
                for (pIndex = 0; strcmp(Font_CN_32x32[pIndex].Index, "") != 0; pIndex++)
                {
                    if (strcmp(Font_CN_32x32[pIndex].Index, SingleChar) == 0) break;
                }
                TFTLCD_ClearAreas(X, X + 32 - 1,Y,  Y + 32 - 1, Background);

                for (uint8_t j = 0; j < 128; j++)
                {
                    uint8_t Bites = Font_CN_32x32[pIndex].Data_32x32[j];
                    for (uint8_t i_bit = 0; i_bit < 8; i_bit++)
                    {
                        LCD_Write_Data( (Bites & (1 << i_bit)) ? Color_Pencil : Background);
                    }
                }
                X += 32;
            }
            else
            {
                // 字体大小不匹配，显示问号
                TFTLCD_ShowChar(X, Y, '?', CurFontSize, Color_Pencil);
                X += 6;
            }                       
         }
     } 
}

void TFTLCD_Printf(uint16_t X, uint16_t Y, Font_Size *FontSize,uint16_t Color_Penceil,char *format, ...)
{
	char String[256];						//定义字符数组
	va_list arg;							//定义可变参数列表数据类型的变量arg
	va_start(arg, format);					//从format开始，接收参数列表到arg变量
	vsnprintf(String,sizeof(String),format, arg);			//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);							//结束变量arg
	TFTLCD_ShowString(X, Y, FontSize,String, Color_Penceil);//OLED显示字符数组（字符串）
}

void TFTLCD_ShowImages(uint16_t X, uint16_t Y, uint16_t width, uint16_t height, const uint8_t *Images)
{
    TFTLCD_Set_Coordinate(X, X + width - 1,Y,  Y + height - 1);
    for (uint16_t i = 0; i < height*width-1; i++)
    {
       LCD_Write_Data(Images[i]); 
    }
}
/*--------------------------------------------------------------------------------*/
  
/*--------------------------------------------------------------------------------↓
	DMA2初始化	        			
↑--------------------------------------------------------------------------------*/

volatile SemaphoreHandle_t TFTLCD_BinSemaphore;

 void LCD_DMA_Init(void)
 {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);

    DMA_DeInit(DMA2_Stream6);
    while(DMA_GetCmdStatus(DMA2_Stream6)!=DISABLE);
    DMA_InitTypeDef DMA_InitStruct;

    DMA_InitStruct.DMA_PeripheralBaseAddr=0;
    DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_INC4;
    DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Enable;

    DMA_InitStruct.DMA_Memory0BaseAddr=(uint32_t)&LCD->LCD_RAM;
    DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_INC4;
    DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Disable;
    
    
    DMA_InitStruct.DMA_BufferSize=0;
    DMA_InitStruct.DMA_Channel=DMA_Channel_0;
    DMA_InitStruct.DMA_DIR=DMA_DIR_MemoryToMemory;
    DMA_InitStruct.DMA_FIFOMode=DMA_FIFOMode_Enable;
    DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_Full;
    DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority=DMA_Priority_VeryHigh;

    DMA_Init(DMA2_Stream6,&DMA_InitStruct);

    DMA_ITConfig(DMA2_Stream6,DMA_IT_TC,ENABLE);
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel=DMA2_Stream6_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=6;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
    NVIC_Init(&NVIC_InitStruct);  
    
    TFTLCD_BinSemaphore=xSemaphoreCreateBinary();
    if(TFTLCD_BinSemaphore!=NULL)
    {
      xSemaphoreGive(TFTLCD_BinSemaphore);
    }
 } 
 
volatile uint32_t dma_remaining_bytes;
void LCD_DMA_Transform(uint32_t Addr, uint32_t total_count)
{
          // 初始化：剩余字节数 = 总长度
     dma_remaining_bytes = total_count;
     
    uint16_t *cur_dst = (uint16_t *)Addr;/*要以2个字节为步长递增,不然会出现重叠现象*/
    while(dma_remaining_bytes > 0)
    {
          if(xSemaphoreTake(TFTLCD_BinSemaphore,portMAX_DELAY)==pdTRUE)
          {
            uint16_t batch = (dma_remaining_bytes > 65535) ? 65535 : dma_remaining_bytes;
            
            DMA_Cmd(DMA2_Stream6, DISABLE);
            while(DMA_GetCmdStatus(DMA2_Stream6) != DISABLE);
            
            DMA2_Stream6->PAR  = (uint32_t)cur_dst;
            DMA2_Stream6->NDTR = batch;
            // 启动DMA前，更新剩余字节数（本次要传的batch）
            dma_remaining_bytes -= batch;
            cur_dst += batch;
            
            DMA_Cmd(DMA2_Stream6, ENABLE);
        }
  } 
}


/**
 * @brief  LCD DMA传输函数（自动分批，适配任意长度，单次≤65535）
 * @param  SrcAddr: 数据源地址（视频数据缓冲区）
 * @param  DstAddr: 显存目标地址（后台显存）
 * @param  total_count: 总传输字节数（可超过65535）
 * @retval 无
 */
 

/*--------------------------------------------------------------------------------*/

