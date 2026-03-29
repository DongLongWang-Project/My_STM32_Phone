#include "Bootloarder.h"
#include "TFTLCD.h"
#include "crc.h"

head_t head[HEAD_NUM];
void display_update(void);

iapfun jump2app;


void Clear_All_Interrupts(void) {
    // 1. 禁用所有中断总开关


    // 2. 清除所有 NVIC 挂起的中断标志
    for (uint8_t i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF; // 禁用所有中断通道
        NVIC->ICPR[i] = 0xFFFFFFFF; // 清除所有挂起的中断位
    }

    // 3. 关闭 SysTick 定时器（它是 HAL_Delay 的核心，必须关掉）
    SysTick->CTRL = 0;
    SysTick->VAL  = 0;
    SysTick->LOAD = 0;
}

void DeInit_Everything(void) {
    // 1. 关闭 SysTick 定时器 (至关重要！)
    SysTick->CTRL = 0;
    SysTick->VAL  = 0;
    SysTick->LOAD = 0;

    // 2. 强制复位所有外设总线 (以 STM32F4 为例)
    // 这会让所有 GPIO、SPI、DMA 寄存器立刻恢复默认值
    RCC->AHB1RSTR = 0xFFFFFFFF; RCC->AHB1RSTR = 0x00;
    RCC->AHB2RSTR = 0xFFFFFFFF; RCC->AHB2RSTR = 0x00;
    RCC->AHB3RSTR = 0xFFFFFFFF; RCC->AHB3RSTR = 0x00; // FSMC 在这里
    RCC->APB1RSTR = 0xFFFFFFFF; RCC->APB1RSTR = 0x00;
    RCC->APB2RSTR = 0xFFFFFFFF; RCC->APB2RSTR = 0x00;

    // 3. 清除所有 NVIC 挂起的中断，并禁用它们
    for (uint8_t i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
}

void load_app(u32 appxaddr)
{
    // 1. 检查栈顶地址是否合法 (RAM范围)
    if(((*(vu32*)appxaddr) & 0x2FF00000) == 0x20000000)
    {
        // 2. 关闭全局中断，防止跳转瞬间触发中断
           __disable_irq();
        DeInit_Everything();
        RCC->AHB3RSTR = 0x01;  // 开启复位
        __DSB();               // 确保指令执行完
        RCC->AHB3RSTR = 0x00;  // 停止复位
            
            // 3. 禁用 FSMC 时钟 (就是你加的那句)
        RCC->AHB3ENR &= ~(0x01);
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

void SD_get_update_file_head(const char*update_file_path)
{
   uint32_t num;
   f_unmount("0");
   FRESULT res=f_mount(&update_file.fs,"0",1);
   if(res!=FR_OK)
   {
    printf("sd卡挂载失败\r\n");
    return;
   }
   else
   {  
      res=f_open(&update_file.fp,update_file_path,FA_READ);
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
         }
         else
         {
            printf("读取app头部失败\r\n");
         }
         
      }
//      f_unmount("0");
   }

}
uint8_t get_update_file_head(head_enum head_)
{

        switch(head_)
        {
          case HEAD_SD:         SD_get_update_file_head(UPDATE_FILE_PATH);                                 break;
          case HEAD_FLASH:      myFLASH_ReadData(APP_HEAD_Addr,&head[HEAD_FLASH],sizeof(head_t));         break;
          case HEAD_W25Q_Cur:   w25q_DMA_readdata(Application_Addr_1,&head[HEAD_W25Q_Cur],sizeof(head_t)); break;
          case HEAD_W25Q_Pre:   w25q_DMA_readdata(Application_Addr_2,&head[HEAD_W25Q_Pre],sizeof(head_t)); break;
          default:break;
        }
       uint32_t buf_size = head[head_].file_size;
       printf("version:%u\r\n",head[head_].version);
       if (buf_size == 0 || buf_size == 0xFFFFFFFF) return 0; 
       if(update_is_valid(head_)) return 1;
       
       return 0;   
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
    if(head_==HEAD_SD )  f_lseek(&update_file.fp,512);
    while (remain > 0)
    {
        // 计算本次读取长度：取 buf_size 和 剩余长度 的最小值
        read_len = (remain > buf_size) ? buf_size : remain;

        // 从 Flash 读取当前分块
        switch(head_)
        {
          case HEAD_SD:       f_read(&update_file.fp,crc_buf,read_len,&num);break;
          case HEAD_FLASH:    myFLASH_ReadData(APP_Addr + offset, crc_buf, read_len);num=read_len;break;
          case HEAD_W25Q_Cur: w25q_DMA_readdata(Application_Addr_1+ offset+sizeof(head_t),crc_buf,read_len);num=read_len;break;
          case HEAD_W25Q_Pre: w25q_DMA_readdata(Application_Addr_1+ offset+sizeof(head_t),crc_buf,read_len);num=read_len;break;
          default:break;
        }
        current_crc=Continue_CRC32(current_crc,crc_buf,num);
        printf("%02X %02X %02X %02X num:%d current_crc:0X%08X\r\n",crc_buf[num-4],crc_buf[num-3],crc_buf[num-2],crc_buf[num-1],num,current_crc);
 
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

typedef struct
{
  uint32_t update_flag;
  uint32_t file_version;
  uint32_t file_crc;
  uint32_t file_size;
}update_flag_info_t;

update_flag_info_t update_flag_info;

uint8_t Get_Latest_update_info_Index(uint32_t Address)
{
    uint32_t current_magic;
    uint8_t info_size = sizeof(update_flag_info_t);
    uint8_t max_slots = 4096 / info_size;

    for(uint8_t i = 0; i < max_slots; i++)
    {
        // 建议读取 4 字节的 Magic 字段，而不是 1 字节
        W25Qxx_ReadData(Address + (i * info_size), (uint8_t*)&current_magic, 4);
        
        // 如果是 0xFFFFFFFF，说明这格是彻底没写过的空位
        if(current_magic == 0xFFFFFFFF) 
        { 
            if(i == 0) return 0xFF; // 第 0 个就是空的
            return i - 1;           // 找到了空位，返回上一个有效的
        }
    }
    // 如果循环走完都没发现 0xFF，说明 4KB 扇区全部写满了
    return max_slots - 1;
}

void update_flag(update_flag_info_t *update_flag_info)
{
    uint8_t info_size=sizeof(update_flag_info_t);
    uint8_t LastIndex=Get_Latest_update_info_Index(UPDATE_INFO_Addr);
    uint8_t Target_index=0;
    if(LastIndex==0XFF)
    {
      /*直接在第一个位置写就行*/
      Target_index=0;
      printf("直接在第一个位置写就行\r\n");
    }
    else if(LastIndex==(4096/info_size)-1)
    {
      /*说明写满了,擦除扇区从头写*/
      W25Qxx_SectorErase(UPDATE_INFO_Addr,W25Qxx_SECTOR_ERASE_4KB);
      Target_index=0;
      printf("说明写满了,擦除扇区从头写\r\n");
    }
    else
    {
      Target_index=LastIndex+1;
      printf("还有空间可以直接写\r\n");
    }

    W25Qxx_WriteBuffer(UPDATE_INFO_Addr+Target_index*info_size,(const uint8_t*)update_flag_info,info_size); 
}

uint8_t Read_Latest_update_info_(update_flag_info_t *update_flag_info)
{
    uint8_t info_size = sizeof(update_flag_info_t);
    uint8_t LastIndex = Get_Latest_update_info_Index(UPDATE_INFO_Addr);
    
    if(LastIndex == 0xFF) return 0; // 彻底没记录

    // 只读最新的一条
    W25Qxx_ReadData(UPDATE_INFO_Addr + (LastIndex * info_size), (uint8_t*)update_flag_info, info_size);
    
    // 只有这两种情况是合法的
    if (update_flag_info->update_flag == 0x5A5A1234 || 
        update_flag_info->update_flag == 0x00000000) 
    {
        return 1; // 找到了明确的当前状态
    }
    
    // 如果最新的一条是乱码（掉电损坏），此时才考虑紧急往前翻一条“保命”
    // 或者直接判定为无效，让系统报错，这比误更新要安全
    printf("最新记录损坏，系统不确定是否需要更新\r\n");
    return 0; 
}
typedef enum {
    SYS_STATE_CHECK_LOCAL,      // 1. 检查本地 Flash 数据 (CRC校验)
    SYS_STATE_CHECK_FLAG,       // 2. 检查 W25Q 更新标志位
    SYS_STATE_FIND_SOURCE,      // 3. 匹配更新源 (SD卡/W25Q备份区)
    SYS_STATE_VERIFY_SOURCE,    // 4. 预校验源文件 (正式擦除前的保命检查)
    SYS_STATE_EXECUTING_UPDATE, // 5. 执行擦除与烧录
    SYS_STATE_UPDATE_SUCCESS,   // 6. 更新成功，清理标志位
    SYS_STATE_JUMP_TO_APP,      // 7. 跳转至 App
    SYS_STATE_ERROR             // 8. 错误状态 (没系统且更新失败)
} BootState_t;

BootState_t BootState=SYS_STATE_CHECK_LOCAL;

/**
  * @brief  计算需要擦除的扇区数量
  * @param  file_size_kb: 文件大小 (Byte >> 10)
  * @retval 需要擦除的扇区个数 (NbOfSectors)
  */
uint32_t Calculate_Sectors_From_KB(uint32_t file_size_kb)
{
    const uint32_t sector_sizes[] = {64, 128, 128, 128, 128, 128, 128, 128};
    const uint32_t sector[] = {FLASH_Sector_4, FLASH_Sector_5, FLASH_Sector_6, FLASH_Sector_7, 
                               FLASH_Sector_8, FLASH_Sector_9, FLASH_Sector_10, FLASH_Sector_11};
    
    uint32_t accumulated_kb = 0;
    uint32_t nb_sectors = 0;

    // 1. 进入循环前解锁一次即可
    FLASH_Unlock();
    // 清除所有 Flash 错误标志位 (防止因为之前的操作报错导致现在擦不动)
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    for (uint8_t i = 0; i < 8; i++) 
    {
        printf("正在擦除扇区 %d... \r\n", i + 4);
        
        // 2. 执行擦除
        if (FLASH_EraseSector(sector[i], VoltageRange_3) != FLASH_COMPLETE)
        {
            printf("扇区 %d 擦除失败！\r\n", i + 4);
            FLASH_Lock();
            return 0; // 报错退出
        }
        
        accumulated_kb += sector_sizes[i];
        nb_sectors++;

        if (accumulated_kb >= file_size_kb) break;
    }

    FLASH_Lock(); // 3. 循环结束上锁
    return nb_sectors;
}

uint8_t Moveing_file_to_flash(head_enum head_)
{
    uint32_t buf_size = sizeof(crc_buf);
    uint32_t offset = 0;
    uint32_t remain = head[head_].file_size+sizeof(head_t);
    uint32_t read_len;
    uint32_t current_crc = 0XFFFFFFFF;
    uint32_t num;
    // 基本合法性检查
    if (remain == 0 || remain == 0xFFFFFFFF) return 0;

    Calculate_Sectors_From_KB(head[head_].file_size>>10);
    SD_get_update_file_head(UPDATE_FILE_PATH);
    if(head_==HEAD_SD ) f_lseek(&update_file.fp,0);
    while (remain > 0)
    {
        // 计算本次读取长度：取 buf_size 和 剩余长度 的最小值
        read_len = (remain > buf_size) ? buf_size : remain;

        // 从 Flash 读取当前分块
        switch(head_)
        {
          case HEAD_SD:       f_read(&update_file.fp,crc_buf,read_len,&num);break;
          case HEAD_W25Q_Cur: w25q_DMA_readdata(Application_Addr_1+ offset,crc_buf,read_len); num=read_len;break;
          case HEAD_W25Q_Pre: w25q_DMA_readdata(Application_Addr_1+ offset,crc_buf,read_len);num=read_len;break;
          default:break;
        }
        if (num == 0) break;
        
        myFLASH_WriteData(APP_HEAD_Addr+offset,crc_buf,num);
//        printf("写入:%u\r\n",offset);
        offset += num;
        remain -= num;
    }
      printf("一共写入:%u\r\n",offset);
    // 最终校验
    if(get_update_file_head(HEAD_FLASH)) return 1;
    

    return 0; 
}
void update_my_phone(void)
{
 
      if(Read_Latest_update_info_(&update_flag_info))
      {
          printf("发现更新标志\r\n");
        if(get_update_file_head(HEAD_SD))
        {
          if(update_flag_info.file_version==head[HEAD_SD].version)
             {
                printf("更新信息版本号与sd的版本号一致,准备搬运\r\n");
             }
             else
             {
               printf("更新信息版本号与sd的版本号不同,请将确认安装包数据正确,并存放在sd卡的相关目录\r\n"); 
               if( get_update_file_head(HEAD_FLASH))
               {
                load_app(APP_Addr);  
               }
               
             }
        }
      }
      else
      {
         printf("未发现更新标志\r\n");
//          
//         
        if(get_update_file_head(HEAD_SD))
        {
          //准备搬运
          if(Moveing_file_to_flash(HEAD_SD))
          {
            printf("搬运成功\r\n");
            load_app(APP_Addr);
          }
          else
          {
            printf("搬运失败\r\n");
          }
        }
         
      }
      //查询w25q中的更新标志信息
      //如果有信息
      //查询sd的固件信息和w25的信息一致不,一致了就进行校验
}