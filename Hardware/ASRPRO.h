#ifndef	__ASRPRO_H
#define	__ASRPRO_H
#include "stm32f4xx.h"
#include "stdio.h"
#define Ringtone_NUM_MAX 10
#define Ringtone_LEN_MAX 32

char Ringtone[Ringtone_NUM_MAX][Ringtone_LEN_MAX];


typedef enum 
{
 GET_Ringtone_LIST_CMD=0x01,
 OPEN_ALARM_CMD,
 CLOSE_ALARM_CMD,
}ASRPRO_CMD_ENUM;


#endif
