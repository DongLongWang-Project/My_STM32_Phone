#ifndef 	__FATFS_H
#define   __FATFS_H	

#include "stm32f4xx.h"
#include "diskio.h"
#include "string.h"

FRESULT disk_format(const TCHAR* path);

#endif
