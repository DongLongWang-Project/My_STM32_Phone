/*
	@项目	: 软件spi驱动触摸芯片XPT2046
	@日期	: 2025-11-28
	@备注	: T_SCK -> PB0
          T_PEN -> PB1
          T_MISO-> PB2
          T_MOSI-> PF11
          T_CS  -> PC13
*/

#include "XPT2046.h"

uint8_t XPT2046_CMD_X =0xD0;  // 1010 0000  (12位模式)
uint8_t XPT2046_CMD_Y =0x90 ;

_m_tp_dev tp_dev=
{
	XPT2046_SPI_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};

void TP_Save_Adjdata(uint32_t Address);
void XPT2046_Save_Adjdata(void);
uint8_t XPT2046_Get_Adjdata(void);

/*
↓----------------------------------------2.SPI的初始化----------------------------------------↓
*/
 /*
	@函数	  : SPI的初始化
	@参数	  : 无
	@返回值 : 无
	@备注	  : SPI的模式0:sck空闲为低电平,第一个边沿采样第二个边沿输出数据
*/
uint8_t  XPT2046_SPI_Init(void)
{
    /* 开启GPIOB,GPIOC,GPIOF时钟 */
    RCC_AHB1PeriphClockCmd(XPT2046_Port1_CLK | XPT2046_Port2_CLK | XPT2046_Port3_CLK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    /* PB0 = SCK 推挽输出 */
    GPIO_InitStructure.GPIO_Pin   = XPT2046_CLK;//PB0
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//上拉
    GPIO_Init(XPT2046_Port1, &GPIO_InitStructure);

    /* PB2 = MISO 输入，启用上拉 */
    GPIO_InitStructure.GPIO_Pin   = XPT2046_MISO;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(XPT2046_Port1, &GPIO_InitStructure);

    /* PB1 = PENIRQ 输入，启用上拉（未按下为高） */
    GPIO_InitStructure.GPIO_Pin   = XPT2046_PEN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(XPT2046_Port1, &GPIO_InitStructure);

    /* PF11 = MOSI 推挽输出 */
    GPIO_InitStructure.GPIO_Pin   = XPT2046_MOSI;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(XPT2046_Port3, &GPIO_InitStructure);

    /* PC13 = CS 推挽输出 */
    GPIO_InitStructure.GPIO_Pin   = XPT2046_CS;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(XPT2046_Port2, &GPIO_InitStructure);

    /* 默认电平 */
    XPT2046_CS_HIGH();   /* 拉高cs空闲 */
    XPT2046_CLK_LOW();  /* 时钟低电平为空闲状态(模式0) */
    XPT2046_DIN_LOW();
    W25Qxx_SPI_Init();
//    W25Qxx_SectorErase(SAVE_ADDR_BASE);
    if(XPT2046_Get_Adjdata()){
    
    print("已经校准%x\r\n",Cur_scr_dir);
    return 0;//已经校准
    }
    else
    {
    print("校准%x\r\n",Cur_scr_dir);    
     TP_Adjust();  	//屏幕校准                                                                                                                                             
//     TP_Get_Adjdata();
    }

//    TP_Get_Adjdata();

    return 1;
}


/*
	@函数	  :SPI通讯开启
	@参数	  :无
	@返回值 :无
	@备注	  :拉低CS,开启SPI
*/
void MySPI_Start(void)
{
    /* 拉低 CS */
    XPT2046_CS_LOW();
    Delay_us(1);
}

/*
	@函数	  :SPI通讯结束
	@参数	  :无
	@返回值 :无
	@备注	  :拉高CS,关闭SPI
*/
void MySPI_Stop(void)
{
    /* 拉高 CS */
    XPT2046_CS_HIGH();
    Delay_us(1);
}

/*
	@函数	  :SPI交换字节
	@参数	  :ByteSend :要交换的字节
	@返回值 :交换到的字节
	@备注	  :高位在前,spi模式0
*/
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
    uint8_t ByteReceive = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        /* 输出当前位（MSB first）*/
        if (ByteSend & 0x80)  XPT2046_DIN_HIGH();       
        else                   XPT2046_DIN_LOW();      
        XPT2046_CLK_HIGH();/* 上升沿采样 MISO（MODE0：数据在上升沿后稳定 */
        Delay_us(1); /* 短延时，确保总线稳定（根据速度再调） */
        ByteReceive <<= 1;
        
        if (XPT2046_DOUT()) //读取数据,如果读到高电平
            ByteReceive |= 0x01;//就置为1

        XPT2046_CLK_LOW();//下降输出数据,高位在前
        Delay_us(1);

        ByteSend <<= 1;
    }

    return ByteReceive;
}


/*
↑----------------------------------------2.SPI的初始化----------------------------------------↑
*/


/*
↓----------------------------------------3.XPT2046读写----------------------------------------↓
*/

/*
	@函数	  :XPT2046读取数据
	@参数	  : Cmd:命令
	@返回值 : 读取到的数据
	@备注	  : 读到的是16位数据,然后转换位12位并返回
*/
uint16_t XPT2046_ReadData(uint8_t Cmd)
{
    MySPI_Start();

    /* 发送命令 */
    MySPI_SwapByte(Cmd);

    /* 根据一些驱动，需要一个小延时，确保 ADC 准备好 */
    Delay_us(1);

    /* 读两个字节（16 bit），把 12bit 有效数据右对齐 */
    uint8_t MSB = MySPI_SwapByte(0x00);
    uint8_t LSB = MySPI_SwapByte(0x00);

    MySPI_Stop();

    uint16_t Value = ((uint16_t)MSB << 8) | LSB;
    Value = (Value >> 4) & 0x0FFF; /* 12-bit */

    return Value;
}

#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=XPT2046_ReadData(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
}

u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(XPT2046_CMD_X);
	ytemp=TP_Read_XOY(XPT2046_CMD_Y);	  												   
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}

#define ERR_RANGE 50 //误差范围 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}

u8 TP_Scan(u8 tp)
{			   
	if(XPT2046_Pen()==0)//有按键按下
	{
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//读取物理坐标
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//读取屏幕坐标
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//将结果转换为屏幕坐标
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下  
			tp_dev.x[4]=tp_dev.x[0];//记录第一次按下时的坐标
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);//标记按键松开	
		}else//之前就没有被按下
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}	


uint16_t  XPT2046_Touch_Distance(uint16_t x0,uint16_t x1,uint16_t y0,uint16_t y1)
{
  uint16_t X_Distance=0,Y_Distance=0,Distance=0;

  X_Distance=abs(x0-x1);
  Y_Distance=abs(y0-y1);
  
  Distance=sqrt((X_Distance*X_Distance)+(Y_Distance*Y_Distance));
  return  Distance;
}

void TP_Save_Adjdata(uint32_t Address)
{
    int32_t temp=0;  
//    W25Qxx_SectorErase(Address);
    // 保存 x/y 校正因子（定点）
    temp = tp_dev.xfac * 1000000;
    W25Qxx_Write(Address, temp, 4);
//    print("1 %d\r\n",temp);

    temp = tp_dev.yfac * 1000000;
    W25Qxx_Write(Address+4, temp, 4);
//    print("2 %d\r\n",temp);

    // 保存 x/y 偏移
    W25Qxx_Write(Address+8, tp_dev.xoff, 2);
    W25Qxx_Write(Address+10, tp_dev.yoff, 2);
//    print("3 %d\r\n",tp_dev.xoff);
//    print("4 %d\r\n",tp_dev.yoff);
    
    // 保存触屏类型
    W25Qxx_Write(Address+12, tp_dev.touchtype, 1);
//        print("5 %d\r\n",tp_dev.touchtype);

    // 保存校准标志
    W25Qxx_Write(Address+13, 0x0A, 1);
    print("保存%x\r\n",Cur_scr_dir);

 }

void XPT2046_Save_Adjdata(void)
{
  uint32_t Addr=0;
  switch(Cur_scr_dir)
  {
    case Dir_Ver:Addr=TOUCH_SAVE_VER_Addr;break;
    case Dir_Rev_Ver:Addr=TOUCH_SAVE_REV_VER_Addr;break;
    case Dir_Hor:Addr=TOUCH_SAVE_HOR_Addr;break;
    case Dir_Rev_Hor:Addr=TOUCH_SAVE_REV_HOR_Addr;break;
  }
  print("保存位置%x\r\n",Addr);
  TP_Save_Adjdata(Addr);
}
uint8_t XPT2046_Get_Adjdata(void)
{
uint32_t Addr=0;
  switch(Cur_scr_dir)
  {
    case Dir_Ver:       Addr=TOUCH_SAVE_VER_Addr;break;
    case Dir_Rev_Ver:   Addr=TOUCH_SAVE_REV_VER_Addr;break;
    case Dir_Hor:        Addr=TOUCH_SAVE_HOR_Addr;break;
    case Dir_Rev_Hor:    Addr=TOUCH_SAVE_REV_HOR_Addr;break;
  }
    print("获取位置%x\r\n",Addr);
   return TP_Get_Adjdata(Addr);
}

uint8_t TP_Get_Adjdata(uint32_t Address)
{
    uint8_t buf[14];

    // 必须读 14 个字节，包含校准标志
    W25Qxx_ReadData(Address, buf, 14);

    if(buf[13] != 0x0A) 
    {
      print("%x位置未获取到数据\r\n",Cur_scr_dir);    
      return 0; // 未校准
    }
    tp_dev.xfac = (float)(((int32_t)buf[0]) | ((int32_t)buf[1]<<8) | ((int32_t)buf[2]<<16) | ((int32_t)buf[3]<<24)) / 1000000;
    tp_dev.yfac = (float)(((int32_t)buf[4]) | ((int32_t)buf[5]<<8) | ((int32_t)buf[6]<<16) | ((int32_t)buf[7]<<24)) / 1000000;

    tp_dev.xoff = (buf[8] | (buf[9]<<8));
    tp_dev.yoff = (buf[10] | (buf[11]<<8));

    tp_dev.touchtype = buf[12];

    if(tp_dev.touchtype)
    {
        XPT2046_CMD_X = 0x90;
        XPT2046_CMD_Y = 0xD0;
    }
    else
    {
        XPT2046_CMD_X = 0xD0;
        XPT2046_CMD_Y = 0x90;
    }
//    print("1 %f\r\n",tp_dev.xfac);
//    print("2 %f\r\n",tp_dev.yfac);
//    print("3 %d\r\n",tp_dev.xoff);
//    print("4 %d\r\n",tp_dev.yoff);
//    print("5 %d\r\n",tp_dev.touchtype);
    print("获取%x\r\n",Cur_scr_dir);

    return 1; // 校准数据有效
}

//触摸屏校准代码
//得到四个校准参数
void TP_Adjust(void)
{								 
	u16 pos_temp[4][2];//坐标缓存值
	u8  cnt=0;	
	u16 d1,d2;
	double fac; 	
	u16 outtime=0;
 	cnt=0;				
  TFTLCD_ClearAreas(0,LCD_Length-1,0,LCD_Height-1,WHITE); 

	TFTLCD_Drow_Touch_Point(20,20,RED);//画点1 
	tp_dev.sta=0;//消除触发信号 
	tp_dev.xfac=0;//xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)//如果连续10秒钟没有按下,则自动退出
	{
		tp_dev.scan(1);//扫描物理坐标
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//按键按下了一次(此时按键松开了.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//标记按键已经被处理过了.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TFTLCD_Drow_Touch_Point(20,20,WHITE);				//清除点1 
					TFTLCD_Drow_Touch_Point(LCD_Length-20,20,RED);	//画点2
					break;
				case 2:
 					TFTLCD_Drow_Touch_Point(LCD_Length-20,20,WHITE);	//清除点2
					TFTLCD_Drow_Touch_Point(20,LCD_Height-20,RED);	//画点3
					break;
				case 3:
 					TFTLCD_Drow_Touch_Point(20,LCD_Height-20,WHITE);			//清除点3
 					TFTLCD_Drow_Touch_Point(LCD_Length-20,LCD_Height-20,RED);	//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		    //对边相等

					d1=XPT2046_Touch_Distance(pos_temp[0][0],pos_temp[1][0],pos_temp[0][1],pos_temp[1][1]);
          d2=XPT2046_Touch_Distance(pos_temp[2][0],pos_temp[3][0],pos_temp[2][1],pos_temp[3][1]);
					fac=(float)d1/d2;
          
					if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
					{
						cnt=0;
 				    	TFTLCD_Drow_Touch_Point(LCD_Length-20,LCD_Height-20,WHITE);	//清除点4
   	 					TFTLCD_Drow_Touch_Point(20,20,RED);								//画点1
//             print("x0:%d  y0:%d  x1:%d  y1:%d  \r\n",pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1]);
//             print("x2:%d  y2:%d  x3%d  y3:%d  \r\n",pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1]);
//             print("fac:%f  \r\n",fac*100);
             
 						continue;
					}


					d1=XPT2046_Touch_Distance(pos_temp[0][0],pos_temp[2][0],pos_temp[0][1],pos_temp[2][1]);        
          d2=XPT2046_Touch_Distance(pos_temp[1][0],pos_temp[3][0],pos_temp[1][1],pos_temp[3][1]);
          
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	TFTLCD_Drow_Touch_Point(LCD_Length-20,LCD_Height-20,WHITE);	//清除点4
   	 					TFTLCD_Drow_Touch_Point(20,20,RED);								//画点1
              
//             print("x0:%d  y0:%d  x1:%d  y1:%d  \r\n",pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1]);
//             print("x2:%d  y2:%d  x3%d  y3:%d  \r\n",pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1]);
//             print("fac:%f  \r\n",fac*100);
  
						continue;
					}//正确了
								   
					//对角线相等
					d1=XPT2046_Touch_Distance(pos_temp[1][0],pos_temp[2][0],pos_temp[1][1],pos_temp[2][1]);
          d2=XPT2046_Touch_Distance(pos_temp[0][0],pos_temp[3][0],pos_temp[0][1],pos_temp[3][1]);
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//不合格
					{
						cnt=0;
 				    	TFTLCD_Drow_Touch_Point(LCD_Length-20,LCD_Height-20,WHITE);	//清除点4
   	 					TFTLCD_Drow_Touch_Point(20,20,RED);								//画点1
//             print("x0:%d  y0:%d  x1:%d  y1:%d  \r\n",pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1]);
//             print("x2:%d  y2:%d  x3%d  y3:%d  \r\n",pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1]);
//             print("fac:%f  \r\n",fac*100);
						continue;
					}//正确了
					//计算结果
					tp_dev.xfac=(float)(LCD_Length-40)/(pos_temp[1][0]-pos_temp[0][0]);//得到xfac		 
					tp_dev.xoff=(LCD_Length-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					tp_dev.yfac=(float)(LCD_Height-40)/(pos_temp[2][1]-pos_temp[0][1]);//得到yfac
					tp_dev.yoff=(LCD_Height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff
          
					if(fabs(tp_dev.xfac)>2||fabs(tp_dev.yfac)>2)//触屏和预设的相反了.
					{
						cnt=0;
 				    	TFTLCD_Drow_Touch_Point(LCD_Length-20,LCD_Height-20,WHITE);	//清除点4
   	 					TFTLCD_Drow_Touch_Point(20,20,RED);								//画点1
//						LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
						tp_dev.touchtype=!tp_dev.touchtype;//修改触屏类型.
						if(tp_dev.touchtype)//X,Y方向与屏幕相反
						{
							XPT2046_CMD_X=0X90;
							XPT2046_CMD_Y=0XD0;	 
						}else				   //X,Y方向与屏幕相同
						{
							XPT2046_CMD_X=0XD0;
							XPT2046_CMD_Y=0X90;	 
						}			    
						continue;
					}		
//             print("x0:%d  y0:%d  x1:%d  y1:%d  \r\n",pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1]);
//             print("x2:%d  y2:%d  x3%d  y3:%d  \r\n",pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1]);
//             print("fac:%f  \r\n",fac*100);
               print("xfac:%f xoff:%d yfac:%f yoff:%d \r\n",tp_dev.xfac,tp_dev.xoff,tp_dev.yfac,tp_dev.yoff);
               print("Touch Screen Adjust OK!\r\n");    
				Delay_ms(1000);
				XPT2046_Save_Adjdata();
        TFTLCD_ClearAreas(0,LCD_Length-1,0,LCD_Height-1,WHITE);           
				return;//校正完成				 
			}
		}
		Delay_ms(10);
		outtime++;
		if(outtime>1000)
		{
			XPT2046_Get_Adjdata();
			break;
	 	} 
 	}
}

void rtp_test(void)
{
	while(1)
	{
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{	
		 	if(tp_dev.x[0]<LCD_Length&&tp_dev.y[0]<LCD_Height)
			{	
				if(tp_dev.x[0]>(LCD_Length-24)&&tp_dev.y[0]<16)        TFTLCD_ClearAreas(0,LCD_Length-1,0,LCD_Height-1,WHITE); 
				else TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//画图	  			   
			}
		}
    else Delay_ms(10);	//没有按键按下的时候 	    
	}
}
/*
↑----------------------------------------3.XPT2046读写----------------------------------------↑
*/
