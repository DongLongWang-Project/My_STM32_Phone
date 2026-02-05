#ifndef	__DX_WF25_H
#define	__DX_WF25_H
#include "stm32f4xx.h"
#include "string.h"
#include "stdio.h"
#include "W25Qxx.h"

#include "USART1.h"
#include "FreeRTOS.h"
#include "Queue.h"
#include "ui_app_setting.h"
#include "ui_app_clock_config.h"

#define USART3_RX_BUF_SIZE 512       //串口的缓冲区大小
#define DEAL_BUF_SIZE       2048
#define AT_CMD_MAX_LEN 256             //wifi的AT最长命令
#define DX_WF25_Queue_MAX_LEN  8     //队列长度
#define WIFI_SCAN_MAX_NUM       10   // 最多扫描到 10 个 wifi

#define WIFI_SSID_MAX_LEN       32    //wifi名字长度
#define WIFI_PASSWORD_MAX_LEN   63   //  密码最大长度
#define WIFI_BSSID_MAX_LEN      18   //路由的bssid长度
#define wifi_other_infor_MAX_LEN 20  //其他信息的长度
#define WIFI_IP_MAX_LEN           16




typedef struct 
{
    uint8_t authmode;                           // 加密方式
    char ssid[WIFI_SSID_MAX_LEN];               // SSID
    int8_t rssi;                                // 信号强度
    char bssid[WIFI_BSSID_MAX_LEN];             // BSSID
//    uint8_t channel;                            // 信道
//    int8_t offset1;                             // 模块返回附加信息（可选）
//    int8_t offset2;
//    uint8_t phy_mode;
//    uint8_t bandwidth;
//    uint8_t scan_type;
//    uint8_t wps;
  char other[wifi_other_infor_MAX_LEN];
} wifi_cwlap_info_t;

typedef struct {
    wifi_cwlap_info_t scan_list[WIFI_SCAN_MAX_NUM]; // 扫描到的 AP 列表
    uint8_t scan_count;                              // 扫描到的数量

    char ready_wifi_ssid[WIFI_SSID_MAX_LEN];         // 准备连接 SSID(自动输入)
    char ready_wifi_bssid[WIFI_BSSID_MAX_LEN];       // 准备连接 BSSID(自动复制)
    char ready_wifi_password[WIFI_PASSWORD_MAX_LEN];// 准备连接 PASS(手动输入)
    
    
    
    int8_t rssi;                                    // 当前已连接信号强度
    uint8_t connected;                              // 1=已连接，0=未连接
} wifi_context_t;

typedef struct
{
  char ssid[WIFI_SSID_MAX_LEN];
  char password[WIFI_PASSWORD_MAX_LEN];
  char bssid[WIFI_BSSID_MAX_LEN];
  char ip[WIFI_IP_MAX_LEN];
//  uint8_t channel;
//  int8_t rssi; 
//   char other[wifi_other_infor_MAX_LEN];
}wifi_connect_t;  //已连接的wifi信息

typedef struct
{
  uint8_t save_count;
  wifi_connect_t save_list[WIFI_SCAN_MAX_NUM];
}wifi_save_t;

typedef enum
{
    AT_CMD_AT = 0,          // AT
    AT_CMD_RST,             // AT+RST 
    AT_CMD_ATE1,            //打开回显
    
    AT_MODE_OFF,
    AT_MODE_STA,
    AT_MODE_AP,
    AT_MODE_STA_AP,
    
    AT_CMD_CIPMUX_ONE,
    AT_CMD_CIPMUX_MANY,
    
    AT_CMD_CIPSERVERE,
    
    AT_CMD_CIFSR,
    AT_CMD_CIPAP,
    AT_CMD_CWSAP,

    AT_CMD_CWLAP,           // AT+CWLAP
    AT_CMD_CWJAP,           //连接wifi
    AT_CMD_CWJAP_USER,
    
    AT_CMD_CIPMODE_0, 
    AT_CMD_CIPMODE_1,
    AT_CMD_CIPSTART,
    AT_CMD_CIPSEND,
    
    
    AT_GET_CLOCK_WEATHER,
    AT_CMD_CIPSNTPCFG,
    AT_GET_NTP_TIME,
    AT_CMD_NUM
} AT_CMD_WIFI_ENUM;

typedef void (*cmd_handler_t)(const char* rx_buf);
typedef struct {
 AT_CMD_WIFI_ENUM cmd;
 const char *cmd_str;
 uint16_t Delay_Tick;
 const char *cmd_resp;
 cmd_handler_t handler;
}wifi_cmd_t;

typedef struct {
  uint8_t     hotspot_channel;
  uint8_t     hotspot_method;
  uint8_t     hotspot_users;
  char         hotspot_name[32];
  char         hotspot_pass[64];
  bool         hotspot_name_allow;
  bool         hotspot_pass_allow;
  bool         hotspot_ip_allow;
  char         hotspot_ip[WIFI_IP_MAX_LEN]; 
}Hotspot_data_t;

extern  const wifi_cmd_t wifi_cmd_table[AT_CMD_NUM];
extern const char*get_time_weather_str;

extern QueueHandle_t DX_WF25_CMD_Queue;
extern volatile SemaphoreHandle_t Timer_Send_AT_BinSemaphore;

extern wifi_context_t wifi_scan_list;
extern wifi_connect_t connected_wifi;
extern wifi_save_t wifi_save_list;
extern Hotspot_data_t  hotspot_data; 


void DX_WF25_Init(void);
void USART3_DMA_SendData(const char *Data);
void wifi_cmd_stateMACHINE(void);

void DX_WF25_Send_Static(AT_CMD_WIFI_ENUM cmd_id);
void DX_WF25_Send_Dynamic(AT_CMD_WIFI_ENUM cmd_id, const char* format, ...);

void WIFI_SAVE(void);
void DX_WF25_CMD_EVENT(AT_CMD_WIFI_ENUM CMD_AT);

bool check_ip_conflict(const char *sta_ip,const char *ap_ip);
#endif
