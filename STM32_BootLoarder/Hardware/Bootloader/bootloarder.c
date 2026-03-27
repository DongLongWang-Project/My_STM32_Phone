#include "Bootloarder.h"
#include "TFTLCD.h"
#include "crc.h"

head_t head[HEAD_NUM];
void display_update(void);

iapfun jump2app;

void load_app(u32 appxaddr)
{
    // 1. 检查栈顶地址是否合法 (RAM范围)
    if(((*(vu32*)appxaddr) & 0x2FF00000) == 0x20000000)
    {
        // 2. 关闭全局中断，防止跳转瞬间触发中断
        __disable_irq();

        // 3. 获取 APP 的复位地址 (偏移 4 字节)
        jump2app = (iapfun)*(vu32*)(appxaddr + 4);

        // 4. 设置主栈指针 (MSP)
        // 使用 CMSIS 标准函数，比手动写汇编更安全且具可移植性
        __set_MSP(*(vu32*)appxaddr);

        // 5. 跳转
        jump2app();
    }
}
typedef struct
{
   FATFS fs;
   FIL fp;
}update_file_t;
update_file_t update_file;
void get_update_file_head(void)
{
   uint32_t num;

   FRESULT res=f_mount(&update_file.fs,"0",1);
   if(res!=FR_OK)
   {
    printf("sd卡挂载失败\r\n");
    return;
   }
   else
   {  
      res=f_open(&update_file.fp,"0:SD/bin/myPhone.bin",FA_READ);
      if(res!=FR_OK)
      {
        printf("打开app文件失败\r\n");
      }
      else
      {
         res=f_read(&update_file.fp,&head[HEAD_SD],sizeof(head_t),&num);
         if(res==FR_OK && num==sizeof(head_t))
         {
           printf("读取app头部成功\r\n");
//           printf("head[HEAD_SD].CRC32:0X%08X\r\n",head[HEAD_SD].CRC32);
//           printf("head[HEAD_SD].version:%u\r\n",head[HEAD_SD].version);
            update_is_valid(HEAD_SD);

         }
         else
         {
            printf("读取app头部失败\r\n");
         }
         
      }
//      f_unmount("0");
   }
//   for(uint8_t i=0;i<16;i++)
//   {
//     W25Qxx_SectorErase(Application_Addr_1+(i*64*1024),W25Qxx_BLOCK_ERASE_64KB);
////     W25Qxx_SectorErase(Application_Addr_2+(i*64*1024),W25Qxx_BLOCK_ERASE_64KB);
//   }
   
   myFLASH_ReadData(APP_HEAD_Addr,&head[HEAD_FLASH],sizeof(head_t));
   w25q_DMA_readdata(Application_Addr_1,&head[HEAD_W25Q_Cur],sizeof(head_t));
   w25q_DMA_readdata(Application_Addr_2,&head[HEAD_W25Q_Pre],sizeof(head_t)); 
//           printf("head[HEAD_SD].CRC32:0X%08X\r\n",head[HEAD_W25Q_Cur].CRC32);
//           printf("head[HEAD_SD].version:%u\r\n",head[HEAD_W25Q_Cur].version); 

//   display_update(); 
}

void display_update(void)
{
  if(head[HEAD_SD].version>head[HEAD_FLASH].version)
  {
     TFTLCD_Printf(0,30,&Font_Size_12x24,WHITE,"SD:%s",head[HEAD_SD].name);
     TFTLCD_Printf(0,60,&Font_Size_12x24,WHITE,"Cur:%s",head[HEAD_FLASH].name);
     TFTLCD_Printf(0,90,&Font_Size_12x24,WHITE,"W25_1:%s",head[HEAD_W25Q_Cur].name);
     TFTLCD_Printf(0,120,&Font_Size_12x24,WHITE,"W25_2:%s",head[HEAD_W25Q_Pre].name);
  }
  else
  {
     TFTLCD_Printf(0,60,&Font_Size_12x24,WHITE,"Cur:%s",head[HEAD_FLASH].name);
     TFTLCD_Printf(0,90,&Font_Size_12x24,WHITE,"W25_1:%s",head[HEAD_W25Q_Cur].name);
     TFTLCD_Printf(0,120,&Font_Size_12x24,WHITE,"W25_2:%s",head[HEAD_W25Q_Pre].name);
  }
}

uint8_t update_is_valid(head_enum head_)
{
    uint32_t buf_size = sizeof(crc_buf);
    uint32_t offset = 0;
    uint32_t remain = head[head_].file_size;
    uint32_t read_len;
    uint32_t current_crc = 0XFFFFFFFF;
    uint32_t num;
    // 基本合法性检查
    if (remain == 0 || remain == 0xFFFFFFFF) return 0;

    while (remain > 0)
    {
        // 计算本次读取长度：取 buf_size 和 剩余长度 的最小值
        read_len = (remain > buf_size) ? buf_size : remain;

        // 从 Flash 读取当前分块
        switch(head_)
        {
          case HEAD_SD:       f_read(&update_file.fp,crc_buf,read_len,&num);break;
          case HEAD_FLASH:    myFLASH_ReadData(APP_Addr + offset, crc_buf, read_len);break;
          case HEAD_W25Q_Cur: w25q_DMA_readdata(Application_Addr_1+ offset,crc_buf,read_len);break;
          case HEAD_W25Q_Pre: w25q_DMA_readdata(Application_Addr_1+ offset,crc_buf,read_len);break;
          default:break;
        }
        printf("%02X %02X %02X %02X num:%d\r\n",crc_buf[0],crc_buf[1],crc_buf[2],crc_buf[3],num);
        current_crc=Continue_CRC32(current_crc,crc_buf,num);
        printf("current_crc:0X%08X\r\n",current_crc);
        offset += num;
        remain -= num;
    }
    // 最终校验
    if (current_crc == head[head_].CRC32)
    {
        printf("Flash CRC OK: 0x%08X\r\n", current_crc);
        return 1;
    }
    
    printf("Flash CRC Error! Calc: 0x%08X, Target: 0x%08X\r\n", current_crc, head[head_].CRC32);
    return 0;
}