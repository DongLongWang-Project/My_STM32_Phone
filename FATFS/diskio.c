                                                                                                        #include "diskio.h"		/* Declarations FatFs MAI */

//宏定义
#define SD                   0     // SD卡
#define SPI_FLASH           1     // 预留外部SPI Flash使用
#define SD_Block_Size     512     // SD卡块大小

/*
	@函数	  : 获取磁盘状态
	@参数	  : pdrv:磁盘号
	@返回值 : 磁盘状态 
	@备注	  : (默认:STA_NOINIT驱动器未初始化)
            (成功:相反的状态)
            W25Qxx:判断id号是否正确
*/

DSTATUS disk_status (BYTE pdrv)
{
  DSTATUS status = STA_NOINIT;//默认状态
  switch (pdrv)
  {   
    case SD:              status &= ~STA_NOINIT; break; //置status为相反状态
    case SPI_FLASH:      W25Qxx_ReadID(&W25Qxx);//获取ID号,存在结构体里
                         if(W25Qxx.MID==W25Qxx_MID && W25Qxx.DID ==W25Qxx_DID ) //判断是否与设定的型号ID相同
                              status &= ~STA_NOINIT; break;                          //相同则将status置为相反状态
   }         
  return status;//返回状态
}


/*
	@函数	  :初始化磁盘
	@参数	  :pdrv 对应的磁盘号
	@返回值 :初始化状态  
	@备注	  :1.默认状态STA_NOINIT未初始化
           2.SD:SD_Init()初始化成功就置status为相反状态
             W25Qxx:先进性SPI初始化,然后进行ID判断
*/

DSTATUS disk_initialize (BYTE pdrv)
{
  DSTATUS status = STA_NOINIT;
      switch (pdrv) 
      {
        case SD:  if (SD_Init()==SD_OK)        status &= ~STA_NOINIT;        
                  else                           status = STA_NOINIT;   break;                
        case SPI_FLASH: W25Qxx_SPI_Init();    
                         W25Qxx_ReadID(&W25Qxx);
                         if(W25Qxx.MID==W25Qxx_MID && W25Qxx.DID ==W25Qxx_DID )
                              status &= ~STA_NOINIT; break;   
      }
    return status;
}


/*
@brief	: 磁盘读操作
@param	: 1.pdrv:磁盘号
          2.buff:存入缓存区
          3.sector:扇区号
          4.count :要读取的扇区数
@return	: 读取是否成功  
@note	  : SD: 
              1.sector<<9相当于扇区地址
              2.SD_ReadMultiBlocks  :读取多个扇区放入buff
              3.SD_WaitReadOperation:等待读操作
              4.SD_GetStatus        :获取当前状态
           W25Qxx:
               1. 读取数据,返回成功  
*/
DRESULT disk_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    switch(pdrv)     
    {
        case SD:  
        {   
            SD_Error ret = SD_ReadMultiBlocks(buff, sector << 9, SD_Block_Size, count); 
            if (ret == SD_OK) 
            {
                SD_WaitReadOperation();
                while(SD_GetStatus() != SD_TRANSFER_OK);
                return RES_OK;      
            }
            break;
        }
      case  SPI_FLASH :
      {
             W25Qxx_ReadData(sector*4096,  buff,  4096*count);
             return RES_OK;      

      }
    }
    return RES_ERROR;
}
/*
@brief	: 磁盘写操作
@param	: 1.pdrv:磁盘号
          2.buff:要写的内容
          3.sector:扇区号
          4.count :写入的字节数
@return	: 写入是否成功
@note	:   与读操作相同
*/
DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    switch(pdrv)
    {
      case SD:
        {
          SD_Error ret = SD_WriteMultiBlocks((uint8_t*)buff, sector << 9, SD_Block_Size, count);   
          if (ret == SD_OK) 
            {
              SD_WaitWriteOperation();
              while(SD_GetStatus() != SD_TRANSFER_OK);
              return RES_OK;
            }
        } break;
      case SPI_FLASH:
         {
          uint8_t state=W25Qxx_Write_Sector (sector, buff, count);
          if(!state)
            return RES_OK;
          }
      }
    return RES_ERROR;
}
/*
@brief	: 磁盘状态
@param	: 1.pdrv:磁盘号
          2.cmd :控制指令
          3.buff:写入或者读取数据地址指针
@return	:状态
@note	: 1.GET_SECTOR_SIZE :获取扇区大小
        2.GET_BLOCK_SIZE  :获取块大小
        3.GET_SECTOR_COUNT:获取扇区数
        4.CTRL_SYNC       :完成挂起的写入过程（FF_FS_READONLY == 0 时需要）
*/
DRESULT disk_ioctl (BYTE pdrv,BYTE cmd,void *buff )      
{
  DRESULT status = RES_PARERR;
  switch (pdrv)
  {
    case SD: 
        switch (cmd) 
          {
            case GET_SECTOR_SIZE :
                *(WORD * )buff = SD_Block_Size; status = RES_OK;
                break;    
            case GET_BLOCK_SIZE :
                *(DWORD * )buff = SDCardInfo.CardBlockSize;status = RES_OK;
                break;
            case GET_SECTOR_COUNT:
                *(DWORD*)buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize; status = RES_OK;
                break;
            case CTRL_SYNC :
                while (SD_GetStatus() != SD_TRANSFER_OK);   status = RES_OK;  // 等总线空闲
                break;
          }
        break;
      case SPI_FLASH:
              switch (cmd) 
            {
              case GET_SECTOR_SIZE   :  *(WORD * )buff   =   4096;      status = RES_OK;  break;
              case GET_BLOCK_SIZE    :  *(DWORD * )buff  =    16;       status = RES_OK;  break;
              case GET_SECTOR_COUNT  :  *(DWORD * )buff  =    4096;     status = RES_OK;  break;
              case CTRL_SYNC          :  if (W25Qxx_WaitBusy() == 0)    status = RES_OK;  break;          
            }break;         
  }       
  return status;
}
  
/*
@brief	: 获取时间
@param	: 无
@return	: 返回
@note	:
*/
DWORD get_fattime (void)
{
  return 0;
}
