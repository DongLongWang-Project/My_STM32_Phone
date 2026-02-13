/*
	@项目	:  硬件spi驱动W25q128
	@日期	:  2025-11-27 
	@备注	:  CS  ->PB14
           CLK ->PB3
           MISO->PB4
           MOSI->PB5
           W25q128是128/8=16MB空间
           规定:
                  1页=256字节(默认)
                  1扇区=4096字节(16页)
                  1块=16扇区
*/

#include "W25Qxx.h"

W25Qxx_ID W25Qxx;
const uint8_t w25q_dummy_byte=W25Qxx_DUMMY_BYTE;

volatile SemaphoreHandle_t W25Qxx_Read_BinSemaphore;
volatile SemaphoreHandle_t W25Qxx_Read_MutexSemaphore;

void W25Qxx_Read_DMA_config(void);
/*
	@函数	  : 设置spi的片选引脚cs
	@参数	  : State->状态
	@返回值 : 无
	@备注	  :
*/

void SPI_Set_CS(uint8_t State)
{
  GPIO_WriteBit(GPIOB,GPIO_Pin_14,(BitAction) State);
}

/*
	@函数	  : spi开始通信
	@参数	  : 无
	@返回值 : 无
	@备注	  : 拉低cs开启spi通信
*/

void SPI_Start(void)
{
  SPI_Set_CS(0);
}

/*
	@函数	  : 停止spi
	@参数	  : 无
	@返回值 : 无
	@备注	  : 拉高spi停止spi通信
*/

void SPI_Stop(void)
{
  SPI_Set_CS(1);
}

/*
	@函数	  : spi和相应的GPIO的初始化
	@参数	  : 无
	@返回值 : 无
	@备注	  :
*/

void W25Qxx_SPI_Init(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//开启GPIOB时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//开始SPI1时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOG时钟

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;//复用模式
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;//推挽输出
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;//上拉
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;//速度
  GPIO_Init(GPIOB,&GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;//软件控制片选 推挽输出
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_14;
  GPIO_Init(GPIOB,&GPIO_InitStruct);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);//PB3开启复用
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);//PB4开启复用
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);//PB5开启复用
  
  SPI_InitTypeDef SPI_InitStructure;
  SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;//二分频 
  SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;//数据在第二个边沿被采样
  SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;//clk空闲时为低电平
  SPI_InitStructure.SPI_CRCPolynomial=7;//CRC校验,暂时不用
  SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;//8位数据
  SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;//全双工
  SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;//数据从高位开始
  SPI_InitStructure.SPI_Mode=SPI_Mode_Master;//作为主机
  SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;//软件控制片选引脚
  SPI_Init(SPI1,&SPI_InitStructure);
  
  SPI_Cmd(SPI1,ENABLE);//使能SPI1
  GPIO_SetBits(GPIOG,GPIO_Pin_7);//PG7输出1,防止NRF干扰SPI FLASH的通信 

  SPI_Set_CS(1);//拉高CS,空闲著状态
  
  W25Qxx_Read_DMA_config();
}

/*
	@函数	  : SPI的数据交换
  @参数	  : ByteSend:要交换的数据
	@返回值 : 交换到的数据
	@备注	  : 每发送一个数据位也会得到一个数据位
*/

uint8_t SPI_SwapByte(uint8_t ByteSend)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET);	//等待发送数据寄存器空
	
	SPI_I2S_SendData(SPI1, ByteSend);								//写入数据到发送数据寄存器，开始产生时序
	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);	//等待接收数据寄存器非空
	
	return SPI_I2S_ReceiveData(SPI1);								//读取接收到的数据并返回
}


/*
	@函数	  :  读取W25Qxx的ID
	@参数	  :  W25Qxx_ID结构体
	@返回值 :  无
	@备注	  :  调用 ID可以获得相应的结构体成员
*/

void W25Qxx_ReadID(W25Qxx_ID * ID)
{
	SPI_Start();								//SPI起始
	SPI_SwapByte(W25Qxx_JEDEC_ID);			//交换发送读取ID的指令
	ID->MID = SPI_SwapByte(W25Qxx_DUMMY_BYTE);	//交换接收MID，通过输出参数返回
	ID->DID = SPI_SwapByte(W25Qxx_DUMMY_BYTE);	//交换接收DID高8位
	ID->DID <<= 8;									//高8位移到高位
	ID->DID |= SPI_SwapByte(W25Qxx_DUMMY_BYTE);	//或上交换接收DID的低8位，通过输出参数返回
	SPI_Stop();								//SPI终止
}

/*
	@函数	  :  W25Qxx写使能
	@参数	  :  无
	@返回值 :  无
	@备注	  :
*/

void W25Qxx_WriteEnable(void)
{
	SPI_Start();								//SPI起始
	SPI_SwapByte(W25Qxx_WRITE_ENABLE);		//交换发送写使能的指令
	SPI_Stop();								//SPI终止
}


/*
	@函数	  : W25Qxx忙等待
	@参数	  : 无
	@返回值 : 是否忙完了(1没忙完,0忙完了)
	@备注	  : 
*/

uint8_t W25Qxx_WaitBusy(void)
{
	uint32_t Timeout=1000000;//给定超时计数时间
  uint8_t result=0;
	SPI_Start();								//SPI起始
	SPI_SwapByte(W25Qxx_READ_STATUS_REGISTER_1);				//交换发送读状态寄存器1的指令
	while ((SPI_SwapByte(W25Qxx_DUMMY_BYTE) & 0x01) == 0x01)	//循环等待忙标志位
	{
		Timeout --;								//等待时，计数值自减
		if (Timeout == 0)						//自减到0后，等待超时
		{
			result=1;
			break;								//跳出等待，不等了
		}
	}
	SPI_Stop();								//SPI终止
  return result;
}


/*
	@函数	  : W25Qxx的页写入
	@参数	  : 1.Address :要写入的首地址:0X000000~0XEFFFFF(W25Q128)                  
            2.DataArray:要写入的数据
            3.Count:写入数据的数量(字节/单位)
	@返回值 : 是否忙完了(1没忙完,0忙完了)
	@备注	  : 1024*1024*型号/8-1=地址最大范围,每次写入只能为1页=256字节,不能跨页写
*/
        
uint8_t W25Qxx_PageProgram(uint32_t Address, const uint8_t *DataArray, uint16_t Count)
{
	uint16_t i;
  
	W25Qxx_WriteEnable();						//写使能
	
	SPI_Start();								//SPI起始
	SPI_SwapByte(W25Qxx_PAGE_PROGRAM);		//交换发送页编程的指令
	SPI_SwapByte(Address >> 16);				//交换发送地址23~16位
	SPI_SwapByte(Address >> 8);				//交换发送地址15~8位
	SPI_SwapByte(Address);					//交换发送地址7~0位
	for (i = 0; i < Count; i ++)				//循环Count次
	{
		SPI_SwapByte(DataArray[i]);			//依次在起始地址后写入数据
	}
	SPI_Stop();								//SPI终止
	
	return W25Qxx_WaitBusy();							//等待忙
}

void W25Qxx_WriteBuffer(uint32_t addr,const uint8_t *buf, uint32_t len)                      
{
    uint32_t page_remain;

    while(len > 0)
    {
        page_remain = 256 - (addr % 256);

        if(len <= page_remain)
        {
            W25Qxx_PageProgram(addr, buf, len);
            break;
        }
        else
        {
            W25Qxx_PageProgram(addr, buf, page_remain);

            addr += page_remain;
            buf  += page_remain;
            len  -= page_remain;
        }
    }
    
}

void W25Qxx_Write(uint32_t Address, uint32_t Data,uint8_t len)
{
    if(len > 4) len = 4;   // 安全限制
    uint8_t buf[len];
     for(uint8_t i=0;i<len;i++)
     {
      buf[i] = (uint8_t)((Data>>(i*8)) & 0xFF);
     }
    W25Qxx_PageProgram(Address, buf, len);    // 一次写入 4 字节
}

/*
	@函数	  :W25Qxx的擦除
	@参数	  :要擦除的首地址:0X000000~0XEFFFFF(W25Q128)
	@返回值 :是否忙完了(1没忙完,0忙完了)
	@备注	  :最小擦除4kb(1个扇区)
*/

uint8_t W25Qxx_SectorErase(uint32_t Address)
{

	W25Qxx_WriteEnable();						//写使能
	
	SPI_Start();								//SPI起始
	SPI_SwapByte(W25Qxx_SECTOR_ERASE_4KB);	//交换发送扇区擦除的指令
	SPI_SwapByte(Address >> 16);				//交换发送地址23~16位
	SPI_SwapByte(Address >> 8);				//交换发送地址15~8位
	SPI_SwapByte(Address);					//交换发送地址7~0位
	SPI_Stop();								//SPI终止
	
	return W25Qxx_WaitBusy();							//等待忙
}


/*
	@函数	  :从W25Qxx中读取数据
	@参数	  :1.Address:读取的首地址0X000000~0XEFFFFF(W25Q128)
           2.DataArray:要存入的数组
           3.Count:要读取的字节数(字节/单位)
	@返回值 :无
	@备注	  :
*/

void W25Qxx_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	SPI_Start();								//SPI起始
	SPI_SwapByte(W25Qxx_READ_DATA);			//交换发送读取数据的指令
	SPI_SwapByte(Address >> 16);				//交换发送地址23~16位
	SPI_SwapByte(Address >> 8);				//交换发送地址15~8位
	SPI_SwapByte(Address);					//交换发送地址7~0位
	for (uint32_t i = 0; i < Count; i ++)				//循环Count次
	{
		DataArray[i] = SPI_SwapByte(W25Qxx_DUMMY_BYTE);	//依次在起始地址后读取数据
	}
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	SPI_Stop();								//SPI终止
}


/*
	@函数	  : W25Qxx写入扇区
	@参数	  : 1.SectorNo:扇区号0~4096
            2.DataArray:要写入的数据
            3.NumSector:写入的扇区数
	@返回值 : 无
	@备注	  : 写入以扇区为单位
*/

uint8_t W25Qxx_Write_Sector(uint32_t SectorNo, const uint8_t *DataArray, uint32_t NumSector)
{
    uint32_t StartAddr = SectorNo * 4096; //转换为首地址
    uint32_t offset = 0;//偏移量

    for (uint32_t sec = 0; sec < NumSector; sec++)
    {
        W25Qxx_SectorErase(StartAddr + sec * 4096); //每次擦4KB=1扇区

        for (uint8_t page = 0; page < 16; page++) //每次写256个字节,要写16次
        {
            if (W25Qxx_PageProgram(StartAddr + offset, DataArray + offset, 256))
                return 1;   // 失败返回1
            offset += 256;
        }
    }
    return 0;  // 成功
}
  
/*--------------------------------------------------------------------------------↓
	@函数	  :
	@参数	  :
	@返回值 :
	@备注	  :
↑--------------------------------------------------------------------------------*/

void W25Qxx_Read_DMA_config(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
  
  SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Rx,ENABLE);
  SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);  

  
  DMA_InitTypeDef W25Qxx_Send_DMA_Initstruct;
  DMA_InitTypeDef W25Qxx_Read_DMA_Initstruct;
  
  W25Qxx_Send_DMA_Initstruct.DMA_Memory0BaseAddr=(uint32_t)&w25q_dummy_byte;
  W25Qxx_Send_DMA_Initstruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;
  W25Qxx_Send_DMA_Initstruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
  W25Qxx_Send_DMA_Initstruct.DMA_MemoryInc=DMA_MemoryInc_Disable;
  W25Qxx_Send_DMA_Initstruct.DMA_PeripheralBaseAddr=(uint32_t)&SPI1->DR;
  W25Qxx_Send_DMA_Initstruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;
  W25Qxx_Send_DMA_Initstruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
  W25Qxx_Send_DMA_Initstruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
  
  W25Qxx_Send_DMA_Initstruct.DMA_Mode=DMA_Mode_Normal;
  W25Qxx_Send_DMA_Initstruct.DMA_Priority=DMA_Priority_High;

  W25Qxx_Send_DMA_Initstruct.DMA_Channel=DMA_Channel_3;
  W25Qxx_Send_DMA_Initstruct.DMA_DIR=DMA_DIR_MemoryToPeripheral;
  W25Qxx_Send_DMA_Initstruct.DMA_BufferSize=0;
   
  W25Qxx_Send_DMA_Initstruct.DMA_FIFOMode=DMA_FIFOMode_Disable;
  W25Qxx_Send_DMA_Initstruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;
  
   

  W25Qxx_Read_DMA_Initstruct.DMA_Memory0BaseAddr=0;
  W25Qxx_Read_DMA_Initstruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;
  W25Qxx_Read_DMA_Initstruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
  W25Qxx_Read_DMA_Initstruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
  W25Qxx_Read_DMA_Initstruct.DMA_PeripheralBaseAddr=(uint32_t)&SPI1->DR;
  W25Qxx_Read_DMA_Initstruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;
  W25Qxx_Read_DMA_Initstruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
  W25Qxx_Read_DMA_Initstruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
  
  W25Qxx_Read_DMA_Initstruct.DMA_Mode=DMA_Mode_Normal;
  W25Qxx_Read_DMA_Initstruct.DMA_Priority=DMA_Priority_VeryHigh;

  W25Qxx_Read_DMA_Initstruct.DMA_Channel=DMA_Channel_3;
  W25Qxx_Read_DMA_Initstruct.DMA_DIR=DMA_DIR_PeripheralToMemory;
  W25Qxx_Read_DMA_Initstruct.DMA_BufferSize=0;
   
  W25Qxx_Read_DMA_Initstruct.DMA_FIFOMode=DMA_FIFOMode_Disable;
  W25Qxx_Read_DMA_Initstruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;
  
  DMA_Init(DMA2_Stream5,&W25Qxx_Send_DMA_Initstruct);
  DMA_Init(DMA2_Stream0,&W25Qxx_Read_DMA_Initstruct);
  
  DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE); 
  
  NVIC_InitTypeDef NVIC_W25Qxx_Read_DMA_Initstruct;
  NVIC_W25Qxx_Read_DMA_Initstruct.NVIC_IRQChannel=DMA2_Stream0_IRQn;
  NVIC_W25Qxx_Read_DMA_Initstruct.NVIC_IRQChannelCmd=ENABLE;
  NVIC_W25Qxx_Read_DMA_Initstruct.NVIC_IRQChannelPreemptionPriority=7;
  NVIC_W25Qxx_Read_DMA_Initstruct.NVIC_IRQChannelSubPriority=0;
  NVIC_Init(&NVIC_W25Qxx_Read_DMA_Initstruct);
  
  W25Qxx_Read_BinSemaphore=xSemaphoreCreateBinary();
  W25Qxx_Read_MutexSemaphore=xSemaphoreCreateMutex();
  
}

void DMA2_Stream0_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0)==SET)
  {
     DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);
     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     xSemaphoreGiveFromISR(W25Qxx_Read_BinSemaphore, &xHigherPriorityTaskWoken);

     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

uint8_t W25Qxx_DMA_ReadData(uint32_t W25Qxx_Addr,uint32_t Target_addr,uint16_t byte_num)
{   
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) 
    {
       uint32_t timeout=0xFFFFFFF;
       
       SPI_Start();								//SPI起始
       SPI_SwapByte(W25Qxx_READ_DATA);			//交换发送读取数据的指令
       SPI_SwapByte(W25Qxx_Addr >> 16);				//交换发送地址23~16位
       SPI_SwapByte(W25Qxx_Addr >> 8);				//交换发送地址15~8位
       SPI_SwapByte(W25Qxx_Addr);					//交换发送地址7~0位
       
       DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
       DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5); 
       
       DMA_Cmd(DMA2_Stream5,DISABLE);
       while(DMA_GetCmdStatus(DMA2_Stream5) != DISABLE); // 确保 DMA 已停止
       DMA2_Stream5->NDTR=byte_num;

       DMA_Cmd(DMA2_Stream0,DISABLE);
       while(DMA_GetCmdStatus(DMA2_Stream0) != DISABLE); // 确保 DMA 已停止
       DMA2_Stream0->M0AR=Target_addr;
       DMA2_Stream0->NDTR=byte_num;
       
       DMA_Cmd(DMA2_Stream0,ENABLE);
       DMA_Cmd(DMA2_Stream5,ENABLE);
    
        // 【开机搬运阶段】：使用阻塞查询，不依赖信号量
        // CPU 在这里等待 DMA 完成，保证下一行代码执行时数据已就绪
        while (DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0) == RESET)
        {
          timeout--;
          if(timeout==0)
          {
            SPI_Stop();
            print("开机DMA读取超时, 地址: 0x%X\r\n", W25Qxx_Addr);
            return 0;
          }
        }
        SPI_Stop();
        return 1;
    }
    else 
    {
        if(xSemaphoreTake(W25Qxx_Read_MutexSemaphore,pdMS_TO_TICKS(10000))==pdTRUE)
      {
         SPI_Start();								//SPI起始
         SPI_SwapByte(W25Qxx_READ_DATA);			//交换发送读取数据的指令
         SPI_SwapByte(W25Qxx_Addr >> 16);				//交换发送地址23~16位
         SPI_SwapByte(W25Qxx_Addr >> 8);				//交换发送地址15~8位
         SPI_SwapByte(W25Qxx_Addr);					//交换发送地址7~0位
         
         DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
         DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5); 
         
         DMA_Cmd(DMA2_Stream5,DISABLE);
         while(DMA_GetCmdStatus(DMA2_Stream5) != DISABLE); // 确保 DMA 已停止
         DMA2_Stream5->NDTR=byte_num;

         DMA_Cmd(DMA2_Stream0,DISABLE);
         while(DMA_GetCmdStatus(DMA2_Stream0) != DISABLE); // 确保 DMA 已停止
         DMA2_Stream0->M0AR=Target_addr;
         DMA2_Stream0->NDTR=byte_num;
         
         DMA_Cmd(DMA2_Stream0,ENABLE);
         DMA_Cmd(DMA2_Stream5,ENABLE);
      
        // 【系统运行阶段】：使用信号量，不占用 CPU
        if(xSemaphoreTake(W25Qxx_Read_BinSemaphore, pdMS_TO_TICKS(10000)) == pdTRUE) 
        {
            SPI_Stop(); 
            xSemaphoreGive(W25Qxx_Read_MutexSemaphore);
            return 1; 
        }
        else
        {
           SPI_Stop();
           print("DMA异常/获取不到互斥信号量,索引表/点阵数据搬运错误\r\n");
           xSemaphoreGive(W25Qxx_Read_MutexSemaphore);
           return 0;
        }

      }
      else
        return 0; 
    }
       
}
