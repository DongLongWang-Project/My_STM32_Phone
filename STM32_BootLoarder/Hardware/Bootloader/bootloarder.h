#ifndef	__BOOTLOARDER_H
#define	__BOOTLOARDER_H
#include "stm32f4xx.h"

#define APP_HEAD_Addr 0x08010000
#define APP_Addr 0x08010200

void load_app(u32 appxaddr);
#endif
