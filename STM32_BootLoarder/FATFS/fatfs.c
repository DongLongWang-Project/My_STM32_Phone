#include "fatfs.h"

/*
	@函数	  :FATFS的磁盘格式化
	@参数	  :磁盘号
	@返回值 :初始化结果
	@备注	  :默认返回(FR_INVALID_PARAMETER:无效)
*/
FRESULT disk_format(const TCHAR* path) 
{
    static UINT format_buff[8192];//格式化缓冲区大小,要大于最大扇区4096
    FRESULT res=FR_INVALID_PARAMETER;//默认返回状态
  
    MKFS_PARM disk;//结构体
    disk.n_fat=1;  //指定 FAT/FAT32 卷上的 FAT 副本数量。此成员的有效值为 1 或 2
     disk.align=0; //指定卷数据区（文件分配池，通常是闪存介质的擦除块边界）的对齐方式，单位为扇区。
    disk.n_root=512;//指定 FAT 卷上的根目录项数量
  
  if( strcmp(path, "0:") == 0) //如果是SD卡,则
  {
    disk.fmt=FM_FAT32;//格式化为FAT32
    disk.au_size=0;    //指定簇（分配单元）的大小，单位为字节。如果给定零（默认值）或无效值，则该函数将使用取决于卷大小的默认簇大小
  }
  else if (strcmp(path, "1:") == 0)//如果是W25Qxx,则
  {
    disk.fmt=FM_FAT;    //格式化为FM_FAT
    disk.au_size=4096;   
  }
  res=f_mkfs(path,&disk,format_buff,sizeof(format_buff));
  return res;//返回状态
}
