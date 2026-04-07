/*--------------------------------------------------------------------------------↓
	@项目	: DX_WF25模块代码(ESP32-C2的wifi模块)
	@日期	: 2026-1-21
	@备注	:
↑--------------------------------------------------------------------------------*/

#include "DX_WF25.h"
#include "setting_about.h"
#include "fifo.h"


volatile QueueHandle_t DX_WF25_CMD_Queue; /*AT指令队列*/
//volatile SemaphoreHandle_t Timer_Send_AT_BinSemaphore;
volatile SemaphoreHandle_t DX_WF25_Send_AT_BUSY_BinSemaphore;
volatile SemaphoreHandle_t DX_WF25_Rev_AT_RESP_CountSemaphore; 

volatile uint16_t Pre_deal_index,Total_Len_resp;/*上一次复制数据的位置,总长度*/


char WIFI_DMA_REV_BUF[USART3_RX_BUF_SIZE];/*接收缓冲区*/
char DEAL_BUF[DEAL_BUF_SIZE];/*总数据处理缓冲区*/
uint8_t WF25_Rev_BUF[FIFO_BUF_SIZE]__attribute__((section(".CCMRAM")));
wifi_context_t wifi_scan_list;/*扫描当前环境wifi的结构体*/
wifi_connect_t connected_wifi;/*已连接wifi结构体*/
wifi_save_t wifi_save_list;   /*已保存在w25q的wifi结构体*/
_fifo_t WF25_Rev_fifo;

Hotspot_data_t  hotspot_data=  /*热点的初始化配置*/
{
    .hotspot_name="donglongwang", /*热点名称*/
    .hotspot_pass="wdl147258",    /*热点密码*/
    .hotspot_channel=1,           /*热点信道*/  
    .hotspot_method=3,            /*热点*/
    .hotspot_users=2,             /*热点可连接用户数*/
    .hotspot_ip="192.168.4.1",    /*默认热点分配ip*/
    .hotspot_name_allow=true,    /*默认热点名称正确*/
    .hotspot_pass_allow=true,    /*默认热点密码正确*/
    .hotspot_ip_allow=true,      /*默认热点分配ip可用*/
};

AT_CMD_WIFI_ENUM cur_cmd=AT_CMD_NONE;/*当前正在处理的命令*/

static void DMA_Receive_Data(uint16_t Cur_index);


static void Handle_AT_CMD_CWLAP(const char*buf);
static void Handle_AT_CMD_ATE1(const char*buf);
static void Handle_AT_CMD_CWJAP(const char*buf);
static void Handle_AT_CMD_CWJAP_USER(const char*buf);
static void Handle_AT_CMD_CIFSR(const char*buf);
static void Handle_AT_CMD_CWSAP(const char*buf);
static void Handle_AT_CMD_CIPSEND(const char*buf);
static void Handle_AT_GET_CLOCK_WEATHER(const char*buf);
static void Handle_AT_GET_NTP_TIME(const char*buf);
static void Handle_Get_GitHub_MyPhone_file_head(const char*buf);
static void Handle_Get_GitHub_MyPhone_file(const char*buf);
/* AT命令结构体配置 */
const wifi_cmd_t wifi_cmd_table[AT_CMD_NONE] = 
{
                      /* 枚举索引           指令字符串                   超时(ms)   预期响应 */
    [AT_CMD_AT]     ={AT_CMD_AT,         "AT\r\n",                   500,      "\r\nOK"},      /* 基础测试 */
    [AT_CMD_RST]    ={AT_CMD_RST,        "AT+RST\r\n",               5000,     "ready"},       /* 复位需等待硬件就绪标志 */
    [AT_CMD_ATE1]   ={AT_CMD_ATE1,       "ATE1\r\n",                 500,      "\r\nOK",Handle_AT_CMD_ATE1},      /* 开启回显 */
    
    [AT_MODE_OFF]   ={AT_MODE_OFF,       "AT+CWMODE=0\r\n",          1000,     "\r\nOK"}, 
    [AT_MODE_STA]   ={AT_MODE_STA,       "AT+CWMODE=1\r\n",          1000,     "\r\nOK"}, 
    [AT_MODE_AP]    ={AT_MODE_AP,        "AT+CWMODE=2\r\n",          1000,     "\r\nOK"}, 
    [AT_MODE_STA_AP]={AT_MODE_STA_AP,    "AT+CWMODE=3\r\n",          1000,     "\r\nOK"},
    
    [AT_CMD_CIPMUX_ONE]={AT_CMD_CIPMUX_ONE,  "AT+CIPMUX=0\r\n",      500,      "\r\nOK"},
    [AT_CMD_CIPMUX_MANY]={AT_CMD_CIPMUX_MANY, "AT+CIPMUX=1\r\n",     500,      "\r\nOK"},
    
    [AT_CMD_CIPSERVERE]={AT_CMD_CIPSERVERE, "AT+CIPSERVER=1,8080\r\n",1000,     "\r\nOK"},
    
    [AT_CMD_CIFSR]={AT_CMD_CIFSR,      "AT+CIFSR\r\n",             1000,     "\r\nOK",Handle_AT_CMD_CIFSR},      /* 查询IP */
    [AT_CMD_CIPAP]= {AT_CMD_CIPAP,      "",                        1000,     "\r\nOK"},      /* 查询AP IP */
    [AT_CMD_CWSAP]={AT_CMD_CWSAP,      "",                         1000,     "\r\nOK",Handle_AT_CMD_CWSAP},      /* 查询热点配置 */
    
    [AT_CMD_CWLAP]={AT_CMD_CWLAP,      "AT+CWLAP\r\n",             8000,     "\r\nOK",Handle_AT_CMD_CWLAP},      /* 扫描热点极慢，给8秒 */
    [AT_CMD_CWJAP]={AT_CMD_CWJAP,      "",                         15000,    "WIFI GOT IP",Handle_AT_CMD_CWJAP},    /* 连路由最慢，给15秒 */
    [AT_CMD_CWJAP_USER]={AT_CMD_CWJAP_USER,     "AT+CWJAP?\r\n",            1000,     "\r\nOK",Handle_AT_CMD_CWJAP_USER},
    
    [AT_CMD_CIPMODE_0]={AT_CMD_CIPMODE_0,    "AT+CIPMODE=0\r\n",        500,        "\r\nOK" },    /*普通模式*/
    [AT_CMD_CIPMODE_1]={AT_CMD_CIPMODE_1,    "AT+CIPMODE=1\r\n",        500,        "\r\nOK" },    /*普通模式*/
    
    [AT_CMD_CIPSTART]={AT_CMD_CIPSTART,   "AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n",500, "CONNECT"},
    [AT_CMD_CIPSEND]={AT_CMD_CIPSEND,         "",           2000,        ">",Handle_AT_CMD_CIPSEND},
    [AT_GET_CLOCK_WEATHER]={AT_GET_CLOCK_WEATHER,  "",           5000,       "CLOSED\r\n",Handle_AT_GET_CLOCK_WEATHER},
    
    [AT_CMD_CIPSNTPCFG]={AT_CMD_CIPSNTPCFG,"AT+CIPSNTPCFG=1,8,\"ntp.aliyun.com\",\"cn.ntp.org.cn\"\r\n",2000,"OK\r\n"} ,
    [AT_GET_NTP_TIME]={AT_GET_NTP_TIME,"AT+CIPSNTPTIME?\r\n",2000,"OK\r\n",Handle_AT_GET_NTP_TIME},
    
    [Connect_GitHubUser]={Connect_GitHubUser,"AT+CIPSTART=\"SSL\",\"raw.githubusercontent.com\",443\r\n",4000,"CONNECT"}, 
    [Get_GitHub_MyPhone_file_head]={Get_GitHub_MyPhone_file_head,  "", 8000,"CLOSED",Handle_Get_GitHub_MyPhone_file_head},
    [Get_GitHub_MyPhone_file]={Get_GitHub_MyPhone_file,"",8000,"CLOSED",Handle_Get_GitHub_MyPhone_file},
};

//const char*get_time_weather_str="GET /v3/weather/now.json?key=S_fPwMVZxdqUzfG_i&location=beijing&language=zh-Hans&unit=c HTTP/1.1\r\nHost: api.seniverse.com\r\nConnection: close\r\n\r\n";            
const char*weather_api_key_str="S_fPwMVZxdqUzfG_i";
const char*weather_api_str="api.seniverse.com";
//GET /v3/weather/now.json?key=S_fPwMVZxdqUzfG_i&location=beijing&language=zh-Hans&unit=c HTTP/1.1
//Host: api.seniverse.com
//Connection: close

const char*get_update_head_str="GET /DongLongWang-Project/My_STM32_Phone/main/SD/bin/myPhone.bin HTTP/1.1\r\nHost: raw.githubusercontent.com\r\nRange: bytes=0-511\r\nConnection: close\r\n\r\n";
const char*get_update_file_str="GET /DongLongWang-Project/My_STM32_Phone/main/SD/bin/myPhone.bin HTTP/1.1\r\nHost: raw.githubusercontent.com\r\nConnection: close\r\n\r\n";

void DX_WF25_Init(void)
{

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);/*开启串口3时钟*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); /*开启GPIOB时钟*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);  /*开启DMA1时钟*/
  
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;  /*复用模式*/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;/*推挽*/
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10; /*PB10*/
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
  GPIO_Init(GPIOB,&GPIO_InitStruct);

  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP; /*上拉*/
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;  /*PB11*/
  GPIO_Init(GPIOB,&GPIO_InitStruct);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); /*对应开启串口*/
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
  
  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate=115200;  /*波特率*/
  USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None; /*无控制位*/
  USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx; /*开启发送和接收*/
  USART_InitStruct.USART_Parity=USART_Parity_No; /*无奇偶校验*/
  USART_InitStruct.USART_StopBits=USART_StopBits_1; /*1停止位*/
  USART_InitStruct.USART_WordLength=USART_WordLength_8b;/*8位数据*/
  USART_Init(USART3,&USART_InitStruct);
  
  USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);   /*开启串口空闲中断*/
  
  NVIC_InitTypeDef USART3_NVICInitStruct;
  USART3_NVICInitStruct.NVIC_IRQChannel=USART3_IRQn;
  USART3_NVICInitStruct.NVIC_IRQChannelCmd=ENABLE;   /*使能串口中断*/ 
  USART3_NVICInitStruct.NVIC_IRQChannelPreemptionPriority=6; /*抢占优先级(接收)*/
  USART3_NVICInitStruct.NVIC_IRQChannelSubPriority=0;   /*响应优先级*/
  NVIC_Init(&USART3_NVICInitStruct);
  
  USART_Cmd(USART3,ENABLE);   /*开启串口3*/
   
  DMA_InitTypeDef USART3_DMA_RX_Struct;  /*配置DMA来串口接收*/

  USART3_DMA_RX_Struct.DMA_DIR=DMA_DIR_PeripheralToMemory; /*外设发往内存(串口的数据寄存器->数据缓冲区)*/
  
  USART3_DMA_RX_Struct.DMA_PeripheralBaseAddr=(uint32_t)&USART3->DR; /*外设地址(串口3的数据寄存器)*/
  USART3_DMA_RX_Struct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single; /*不应对突发*/
  USART3_DMA_RX_Struct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte; /*1字节*/
  USART3_DMA_RX_Struct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;  /*外设地址不递增*/
  
  USART3_DMA_RX_Struct.DMA_Memory0BaseAddr=(uint32_t)WIFI_DMA_REV_BUF; /*内存地址(数据缓冲区)*/
  USART3_DMA_RX_Struct.DMA_MemoryBurst=DMA_MemoryBurst_Single;   /*不应对突发*/
  USART3_DMA_RX_Struct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;/*1字节*/
  USART3_DMA_RX_Struct.DMA_MemoryInc=DMA_MemoryInc_Enable; /*内存地址自增*/
  
  USART3_DMA_RX_Struct.DMA_BufferSize=USART3_RX_BUF_SIZE;  /*缓冲区大小*/
  USART3_DMA_RX_Struct.DMA_Channel=DMA_Channel_4;           /*DMA通道4(串口3的接收)*/
  USART3_DMA_RX_Struct.DMA_Mode=DMA_Mode_Circular;          /*环形DMA*/
  USART3_DMA_RX_Struct.DMA_Priority=DMA_Priority_VeryHigh; /*最高DMA优先级*/
  
  USART3_DMA_RX_Struct.DMA_FIFOMode=DMA_FIFOMode_Disable;//串口dma不定长接收一定要关了,不然会出现接收数据尾部不正常的情况
  USART3_DMA_RX_Struct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;
  
  DMA_Init(DMA1_Stream1,&USART3_DMA_RX_Struct);/*DMA1的数据流1通道4*/
  
  DMA_ITConfig(DMA1_Stream1,DMA_IT_HT,ENABLE); /*DMA缓冲区半满中断*/
  DMA_ITConfig(DMA1_Stream1,DMA_IT_TC,ENABLE); /*DMA缓冲区满中断*/
  
  NVIC_InitTypeDef DMA_RX_NVICInitStruct;
  DMA_RX_NVICInitStruct.NVIC_IRQChannel=DMA1_Stream1_IRQn;
  DMA_RX_NVICInitStruct.NVIC_IRQChannelCmd=ENABLE;
  DMA_RX_NVICInitStruct.NVIC_IRQChannelPreemptionPriority=5;/*抢占优先级*/
  DMA_RX_NVICInitStruct.NVIC_IRQChannelSubPriority=0; /*响应优先级*/
  NVIC_Init(&DMA_RX_NVICInitStruct);
  
  DMA_InitTypeDef USART3_DMA_TX_Struct; /*配置DMA发送*/
  USART3_DMA_TX_Struct.DMA_DIR=DMA_DIR_MemoryToPeripheral;/*内存->外设(AT队列取出命令发往串口数据寄存器)*/
  
  USART3_DMA_TX_Struct.DMA_Memory0BaseAddr=0;   /*内存地址(单独设置)*/
  USART3_DMA_TX_Struct.DMA_MemoryBurst=DMA_MemoryBurst_Single; /*不突发*/
  USART3_DMA_TX_Struct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;/*1字节*/
  USART3_DMA_TX_Struct.DMA_MemoryInc=DMA_MemoryInc_Enable; /*内存地址自增*/
  
  USART3_DMA_TX_Struct.DMA_PeripheralBaseAddr=(uint32_t)&USART3->DR; /*外设地址*/
  USART3_DMA_TX_Struct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;  /*不突发*/
  USART3_DMA_TX_Struct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte; /*1字节*/
  USART3_DMA_TX_Struct.DMA_PeripheralInc=DMA_PeripheralInc_Disable; /*不自增*/
  
  USART3_DMA_TX_Struct.DMA_BufferSize=0;  /*发送数据大小(单独设置)*/
  USART3_DMA_TX_Struct.DMA_Channel=DMA_Channel_4;/*通道4*/
  USART3_DMA_TX_Struct.DMA_Mode=DMA_Mode_Normal; /*正常模式*/
  USART3_DMA_TX_Struct.DMA_Priority=DMA_Priority_High;/*优先级高*/
  
  USART3_DMA_TX_Struct.DMA_FIFOMode=DMA_FIFOMode_Disable; /*关闭FIFO*/
  USART3_DMA_TX_Struct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;
  
  DMA_Init(DMA1_Stream3,&USART3_DMA_TX_Struct);
  
  USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
  USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
  
  DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE); /*DMA1的数据流3*/
  
  NVIC_InitTypeDef NVIC_DMA_TXStruct; /*配置DMA发送中断*/
  NVIC_DMA_TXStruct.NVIC_IRQChannel= DMA1_Stream3_IRQn;
  NVIC_DMA_TXStruct.NVIC_IRQChannelCmd=ENABLE;
  NVIC_DMA_TXStruct.NVIC_IRQChannelPreemptionPriority=6;
  NVIC_DMA_TXStruct.NVIC_IRQChannelSubPriority=0;
  NVIC_Init(&NVIC_DMA_TXStruct);
  
  DMA_Cmd(DMA1_Stream1,ENABLE);  /*开启DMA接收(当串口3的接收数据寄存器有数据就自动接收)*/


   DX_WF25_CMD_Queue=xQueueCreate(DX_WF25_Queue_MAX_LEN,sizeof(wifi_cmd_t));

//    Timer_Send_AT_BinSemaphore=xSemaphoreCreateBinary();
//    if(Timer_Send_AT_BinSemaphore!=NULL)
//    {
//      xSemaphoreGive(Timer_Send_AT_BinSemaphore);
//    }
    
    DX_WF25_Send_AT_BUSY_BinSemaphore=xSemaphoreCreateBinary();
    if(DX_WF25_Send_AT_BUSY_BinSemaphore!=NULL)
    {
      xSemaphoreGive(DX_WF25_Send_AT_BUSY_BinSemaphore);
    } 
    
    DX_WF25_Rev_AT_RESP_CountSemaphore=xSemaphoreCreateCounting(15,0);
    
    fifo_register(&WF25_Rev_fifo,WF25_Rev_BUF,sizeof(WF25_Rev_BUF),My_FIFO_Lock,My_FIFO_Unlock);
    
    
     DX_WF25_Send_Static(AT_CMD_ATE1);
     DX_WF25_Send_Static(AT_CMD_CIPMUX_ONE); 
     DX_WF25_Send_Static(AT_CMD_RST);
      
}

void USART3_DMA_SendData(const char *Data)
{
  DMA_Cmd(DMA1_Stream3,DISABLE);
  DMA1_Stream3->M0AR=(uint32_t)Data;
  DMA1_Stream3->NDTR=strlen(Data);
  DMA_Cmd(DMA1_Stream3,ENABLE);
}

void DMA1_Stream3_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)==SET)
  {
    DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TCIF3);
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(DX_WF25_Send_AT_BUSY_BinSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    
  }
}

//void TIM2_IRQHandler(void)
//{
//    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
//    {
//       static uint32_t time=0;
//       time++;
//       if(time>=1800)
//       {  
//          if(wifi_scan_list.connected==1)
//          {
//               BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//               xSemaphoreGiveFromISR(Timer_Send_AT_BinSemaphore, &xHigherPriorityTaskWoken);

//               time=0;
//               portYIELD_FROM_ISR(xHigherPriorityTaskWoken); 
//          }
//          else
//          {
//            time-=60;
//          }
//       }
//        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//    }
//}

static void DMA_Receive_Data(uint16_t Cur_index)
{
    uint16_t start = Pre_deal_index;
    uint16_t len = 0;

    // 如果传入的 Cur_index 是 USART3_RX_BUF_SIZE (TC触发)，
    // 下一次执行时 start 也会变成这个，所以我们要让它自动回绕。
    if (start >= USART3_RX_BUF_SIZE) start = 0; 
    if (Cur_index == start) return;

    if (Cur_index > start) 
    {
        len = Cur_index - start;
        fifo_write(&WF25_Rev_fifo, (uint8_t*)&WIFI_DMA_REV_BUF[start], len);
    }
    else // 处理回绕 (Cur_index < start)
    {
        uint16_t len_to_end = USART3_RX_BUF_SIZE - start;
        fifo_write(&WF25_Rev_fifo, (uint8_t*)&WIFI_DMA_REV_BUF[start], len_to_end);
        fifo_write(&WF25_Rev_fifo, (uint8_t*)&WIFI_DMA_REV_BUF[0], Cur_index);
    }

    // 更新索引：如果到了末尾，自动回 0
    Pre_deal_index = (Cur_index >= USART3_RX_BUF_SIZE) ? 0 : Cur_index;
}

void DMA1_Stream1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_HTIF1) == SET)
    {
        DMA_Receive_Data(USART3_RX_BUF_SIZE / 2);
        DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_HTIF1);
    }
    else if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1) == SET)
    {
        // 关键点：TC触发时，DMA写指针实际上已经回到了起始位置
        // 但我们要强制处理到末尾，然后再将 Pre_deal_index 归零
        DMA_Receive_Data(USART3_RX_BUF_SIZE);
        DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
    }
}
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_IDLE) == SET)
    {
        // 清除标志
        USART3->SR;
        USART3->DR;

        uint16_t current_pos = USART3_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Stream1);

        DMA_Receive_Data(current_pos);
        
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(DX_WF25_Rev_AT_RESP_CountSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                 
    }
}

static void get_wifi_scan(const char *resp)
{
uint8_t count=0;
const char *p=resp;
while((p=strstr(p,"+CWLAP:"))!=NULL)
{
  if(count>=WIFI_SCAN_MAX_NUM)break;
  else
    {
      memset(&wifi_scan_list.scan_list[count], 0, sizeof(wifi_cwlap_info_t));
      sscanf(p,"+CWLAP:(%hhd,\"%31[^\"]\",%hhd,\"%17[^\"]\",%19[^\r\n])",\
                                  &wifi_scan_list.scan_list[count].authmode,\
                                  wifi_scan_list.scan_list[count].ssid,\
                                  &wifi_scan_list.scan_list[count].rssi,\
                                  wifi_scan_list.scan_list[count].bssid,\
                                  wifi_scan_list.scan_list[count].other);
      
     if(wifi_scan_list.scan_list[count].ssid[0]!='\0')count++;
      p=strchr(p,'\n');
      if(!p)break;
    }
}
  wifi_scan_list.scan_count=count; 
  print("scan_wifi=%d\r\n",wifi_scan_list.scan_count);   
}

void deal_connected_wifi(const char *resp)
{
  memset(&connected_wifi,0,sizeof(wifi_connect_t));
//  print("%s",resp);
  const char *p = strstr(resp, "+CWJAP:");
  if (p == NULL) {
      print("No CWJAP found\n");
      return;
  }
  uint8_t ret=sscanf( p,"+CWJAP:\"%31[^\"]\",\"%17[^\"]",     
                              connected_wifi.ssid,\
                              connected_wifi.bssid);

     if(ret)wifi_scan_list.connected=1;
     
     print("连接状态%d\r\n",wifi_scan_list.connected);     
}

void deal_wifi_hotspot_ip_data(const char *resp)
{
   const char *p = strstr(resp, "STAIP,");
    if (p == NULL) {
          print("No STAIP found\n");
          
          }
    else
    {
      uint8_t ret=sscanf( p,"STAIP,\"%15[^\"]",connected_wifi.ip);
      if(ret==1)print("ip地址%s\r\n",connected_wifi.ip); 
      else print("解析wifi_ip失败\r\n" );
    }

     p = strstr(resp, "APIP,");
      if (p == NULL) {
            print("No APIP found\n");
            }
      else
      {
        uint8_t ret=sscanf( p,"APIP,\"%15[^\"]",hotspot_data.hotspot_ip);
        if(ret==1)print("ap_ip地址%s\r\n",hotspot_data.hotspot_ip); 
        else print("解析ap_ip失败\r\n" );
      }  
    
}

bool check_ip_conflict(const char *sta_ip,const char *ap_ip)
{
  int sta[4],ap[4];
  
  if(sscanf(sta_ip,"%d.%d.%d.%d",&sta[0],&sta[1],&sta[2],&sta[3])!=4) return false;
  if(sscanf(ap_ip,"%d.%d.%d.%d",&ap[0],&ap[1],&ap[2],&ap[3])!=4) return false;
  if(sta[0]==ap[0] && sta[1]==ap[1] && sta[2]==ap[2])return false;
  
  return true;
}

/* 通用发送：发送静态命令（只需传入枚举 ID） */
void DX_WF25_Send_Static(AT_CMD_WIFI_ENUM cmd_id) 
{
    BaseType_t err=xQueueSend(DX_WF25_CMD_Queue, &wifi_cmd_table[cmd_id], 0);
   if(err!=pdTRUE)print("DX_WF25_CMD_Queue is FULL!!\r\n");
}

#define BUF_NUM  4    // 缓冲区数量
#define BUF_SIZE 256  // 每个缓冲区的大小

static char Dynamic_AT_CMD_BUF[BUF_NUM][BUF_SIZE];
static uint8_t write_idx = 0; // 写指针

void DX_WF25_Send_Dynamic(AT_CMD_WIFI_ENUM cmd_id, const char* format, ...) 
{
    // 1. 获取当前可用的“块”
    char* target_ptr = Dynamic_AT_CMD_BUF[write_idx];

    // 2. 使用变长参数拼装（比 snprintf 更灵活）
    va_list args;
    va_start(args, format);
    vsnprintf(target_ptr, BUF_SIZE, format, args);
    va_end(args);

    // 3. 发送地址到队列
    wifi_cmd_t msg = {
        .cmd = cmd_id,
        .cmd_str = target_ptr, // 传的是这块“砖”的固定地址
    };
    
//    print("拼装后:%s\r\n",target_ptr);
//    print("长度:%d\r\n",strlen(target_ptr));
    xQueueSend(DX_WF25_CMD_Queue, &msg, 0);

    // 4. 环形移动指针：0 -> 1 -> 2 -> 3 -> 0
    write_idx = (write_idx + 1) % BUF_NUM;
}

static uint8_t DX_WF25_Rev_queue(void)
{
    wifi_cmd_t q_msg;
    if(xQueueReceive(DX_WF25_CMD_Queue, &q_msg, 0) != pdTRUE) return 0;

    
    // 指针二选一：优先用消息里的，没有就用表里的
    const char* final_ptr = q_msg.cmd_str;
    if (final_ptr) {
    
        memset(DEAL_BUF, 0, Total_Len_resp);
        Total_Len_resp=0;
        
        cur_cmd = q_msg.cmd;
        // 如果是天气长命令，这里加个 CIPSEND 的特殊判断逻辑...
        USART3_DMA_SendData((char*)final_ptr);
        return 1;
    }
    return 0;
}

/*--------------------------------------------------------------------------------↓
	@函数	  : wifi空闲任务的数据处理函数
	@参数	  :  无
	@返回值 :  无
	@备注	  :  上电自动连接到wifi的处理
↑--------------------------------------------------------------------------------*/
void wifi_ide_deal(void)
{
    if(xSemaphoreTake(DX_WF25_Rev_AT_RESP_CountSemaphore, 10) == pdTRUE)
    {
        uint16_t Total_Len=fifo_get_occupy_size(&WF25_Rev_fifo);
      if(Total_Len)
      {
         fifo_read(&WF25_Rev_fifo,(uint8_t *)(DEAL_BUF+Total_Len_resp),Total_Len);
         DEAL_BUF[Total_Len+Total_Len_resp]='\0'; 
        if(strstr(DEAL_BUF+Total_Len_resp, "WIFI GOT IP") != NULL)
        {
            if(display_cfg.wifi_switch_state == false) 
            {
                DX_WF25_Send_Static(AT_CMD_CWJAP_USER); 
                display_cfg.wifi_switch_state = true;
                DX_WF25_Send_Static(AT_CMD_CIFSR);
                print("WiFi Event: IP Obtained, auto-updating info...\r\n");
            }
            DX_WF25_Send_Static(AT_CMD_CIPSNTPCFG);
            DX_WF25_Send_Static(AT_GET_NTP_TIME); 
          Get_Weather_data(weather_api_str,weather_api_key_str,"ip");
        }
        if(strstr(DEAL_BUF+Total_Len_resp, "WIFI DISCONNECT") != NULL)
        {
            display_cfg.wifi_switch_state = false;
        }
        
      }
    }
}

void Get_Weather_data(const char*api_str,const char*api_key_str,const char*place_str)
{
    char get_time_weather_str[256];
//    snprintf(get_time_weather_str,sizeof(get_time_weather_str),"GET /v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",api_key_str,place_str,api_str);
    snprintf(get_time_weather_str,sizeof(get_time_weather_str),"GET /v3/weather/daily.json?key=%s&location=%s&language=zh-Hans&unit=c&start=0&days=3 HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",api_key_str,place_str,api_str);
  
    DX_WF25_Send_Static(AT_CMD_CIPMODE_0);
    DX_WF25_Send_Static(AT_CMD_CIPSTART);
    DX_WF25_Send_Dynamic(AT_CMD_CIPSEND,"AT+CIPSEND=%d\r\n",strlen(get_time_weather_str)); 
    DX_WF25_Send_Dynamic(AT_GET_CLOCK_WEATHER,"%s",get_time_weather_str); 
}

void Get_GitHub_MyPhone_Update_file(AT_CMD_WIFI_ENUM Github_cmd,const char*str)
{
    DX_WF25_Send_Static(Connect_GitHubUser);
    DX_WF25_Send_Dynamic(AT_CMD_CIPSEND,"AT+CIPSEND=%d\r\n",strlen(str));
    DX_WF25_Send_Dynamic(Github_cmd,"%s",str);   
}
/*--------------------------------------------------------------------------------↓
	@函数	  :wifi收发状态机
	@参数	  : 无
	@返回值 :无
	@备注	  :包含命令的发送以及接收数据的处理
↑--------------------------------------------------------------------------------*/
void wifi_cmd_stateMACHINE(void)
{
  static uint8_t S;
  static TickType_t at_start_tick;

  if(S==0)
  {
      wifi_ide_deal();
    if(DX_WF25_Rev_queue())
    {
      S=1;
    }
  }
  else if(S==1)
  {
      if(xSemaphoreTake(DX_WF25_Send_AT_BUSY_BinSemaphore,pdMS_TO_TICKS(500))==pdTRUE)
      {
        S=2;
        at_start_tick=xTaskGetTickCount();
      }
      else
      {
        S=0;
        print("CRITICAL: DMA TX Timeout! Force Resetting Semaphore...\r\n");
        xSemaphoreGive(DX_WF25_Send_AT_BUSY_BinSemaphore);
      }
  }
  else if(S==2)
  {  
      
      if((xTaskGetTickCount() - at_start_tick) >= pdMS_TO_TICKS(wifi_cmd_table[cur_cmd].Delay_Tick))
        {
            S = 0; 
            Total_Len_resp=0;
            print("cmd超时:%d\r\n",cur_cmd);
        }
     else if(xSemaphoreTake(DX_WF25_Rev_AT_RESP_CountSemaphore,pdMS_TO_TICKS(50))==pdTRUE)
      {
            
            uint16_t Total_Len=fifo_get_occupy_size(&WF25_Rev_fifo);        
          if(Total_Len)
          {
//            printf("fifo存的长度:%d\r\n",Total_Len);
            fifo_read(&WF25_Rev_fifo,(uint8_t *)(DEAL_BUF+Total_Len_resp),Total_Len);
            
            DEAL_BUF[Total_Len_resp+Total_Len]='\0';
//            printf("当前%s",DEAL_BUF+len);
            if(strstr(DEAL_BUF+Total_Len_resp, wifi_cmd_table[cur_cmd].cmd_resp) != NULL)
              {
                  S = 3; 
                print("命令:%d通道1进入处理阶段\r\n",cur_cmd);
              }
              if(cur_cmd==Get_GitHub_MyPhone_file_head)
              {
               if(strstr(DEAL_BUF+Total_Len_resp, "+IPD,512:") != NULL)
                {
                    S = 3;
                    print("命令:%d通道2进入处理阶段\r\n",cur_cmd);  
                }
              }
              if(cur_cmd==Get_GitHub_MyPhone_file)
              {
                 if(strstr(DEAL_BUF+Total_Len_resp, "Recv")!= NULL)
                  {
                    S = 3;
                    print("命令:%d通道3进入处理阶段\r\n",cur_cmd);                        
                  }  
                
              }

              Total_Len_resp+= Total_Len; 
              Total_Len_resp%=DEAL_BUF_SIZE;
          }
      }
  }
  else if(S==3)
  {

  if(wifi_cmd_table[cur_cmd].handler!=NULL)
  { 
     wifi_cmd_table[cur_cmd].handler(DEAL_BUF); 
  }
      S=0;
      cur_cmd=AT_CMD_NONE;
  }
}

void WIFI_SAVE(void)
{
    uint8_t found = 0;  // 0 = 未找到，1 = 已找到
    static uint8_t write_index=0;
    for(uint8_t i=0;i<wifi_save_list.save_count;i++)
    {
       if((strcmp(wifi_save_list.save_list[i].ssid,connected_wifi.ssid)==0))  
       {
          found = 1;
          print("这个系统中存在,不需要保存\r\n");
          break;                          
       }
    }
   if(found==0)  
   {
      if(wifi_save_list.save_count<WIFI_SCAN_MAX_NUM)
      {
        wifi_save_list.save_list[wifi_save_list.save_count]=connected_wifi;
        wifi_save_list.save_count++;
      }
      else
      {
        write_index%=WIFI_SCAN_MAX_NUM;
        wifi_save_list.save_list[write_index]=connected_wifi;
        write_index++;
      }

      taskENTER_CRITICAL();           /* 进入临界区 */
      W25Qxx_SectorErase(WIFI_SAVE_Addr,W25Qxx_SECTOR_ERASE_4KB);       
      W25Qxx_WriteBuffer(WIFI_SAVE_Addr,(uint8_t*)&wifi_save_list,sizeof(wifi_save_t));
      taskEXIT_CRITICAL();            /* 退出临界区 */
   } 
}
 /*--------------------------------------------------------------------------------↓
	AT命令的回调函数	        			
↑--------------------------------------------------------------------------------*/

 static void Handle_AT_CMD_CWLAP(const char*buf)
{
     get_wifi_scan(buf);
     lv_async_call(wifi_scan_done_cb,NULL);
}

static void Handle_AT_CMD_ATE1(const char*buf)
{
    print("回显已打开\r\n");
}

static void Handle_AT_CMD_CWJAP(const char*buf)
{
    if(strcmp(wifi_scan_list.ready_wifi_ssid,connected_wifi.ssid)==0)
    {
      print("手动:%s\r\n",wifi_scan_list.ready_wifi_ssid);
    }
}

static void Handle_AT_CMD_CWJAP_USER(const char*buf)
{
    deal_connected_wifi(buf);
    DX_WF25_Send_Static(AT_CMD_CWLAP);
    print("自动连接到:%s\r\n",connected_wifi.ssid);
    lv_async_call(wifi_icon_show,NULL);
    
    if(strlen(wifi_scan_list.ready_wifi_password)!=0)
    {
     strcpy(connected_wifi.password,wifi_scan_list.ready_wifi_password);    
     print("保存的密码%s\r\n",wifi_scan_list.ready_wifi_password);
     WIFI_SAVE();
    }
}

static void Handle_AT_CMD_CIFSR(const char*buf)
{
   deal_wifi_hotspot_ip_data(buf);
}
static void Handle_AT_CMD_CWSAP(const char*buf)
{
  DX_WF25_Send_Static(AT_CMD_CIFSR);
}

static void Handle_AT_CMD_CIPSEND(const char*buf)
{
// DX_WF25_Send_Dynamic(AT_GET_CLOCK_WEATHER,"%s",get_time_weather_str); 
//   USART3_DMA_SendData(get_time_weather_str);
//   USART3_DMA_SendData(get_time_weather_str);
//     SendString(get_time_weather_str);
}
//static void Handle_AT_GET_CLOCK_WEATHER(const char* buf)
//{
//    char* p;
//    p = strstr(buf, "\"name\":\""); 
//    if (p != NULL) {
//        p += strlen("\"name\":\""); // 此时 p 指向 驻 字的起始地址
//        
//        // 寻找下一个双引号作为结束符
//        char *end = strchr(p, '\"');
//        if (end != NULL) {
//            int len = end - p;
//            if (len < sizeof(CITY_STR_MAX_LEN)) {
//                memcpy(weather_widget.place_str, p, len); // 拷贝这部分原始编码
//                weather_widget.place_str[len] = '\0';     // 必须手动封口
//            }
//        }
//    }

//    // 1. 提取温度 (使用你最喜欢的 strlen 方案)
//    p = strstr(buf, "\"temperature\":");
//    if (p != NULL) {
//        // +strlen("\"temperature\":") 会直接跳到冒号后面
//        // 如果数据是 "temperature":"2"，跳过后指向 "2"
//        p += strlen("\"temperature\":") + 1; 
//        Cur_Time.temperature = (int8_t)atoi(p);
//    }

//    // 2. 提取天气代码
//    p = strstr(buf, "\"code\":");
//    if (p != NULL) {
//        p += strlen("\"code\":") + 1; 
//        Cur_Time.weather_code = (uint8_t)atoi(p);
//    }

//    // 3. 提取时间 (注意下划线 _)
//    p = strstr(buf, "\"last_update\":");
//    if (p != NULL) {
//        // 跳过 "last_update":" 这 15 个字节
//        p += 15; 
//        // 格式：%hu 是 uint16_t, %hhu 是 uint8_t
//        // 注意：有些编译器 sscanf 不支持 %hhu，如果报错请改用临时 int 中转
//        sscanf(p, "%hu-%hhu-%hhuT", 
//               &Cur_Time.year, &Cur_Time.month, &Cur_Time.day);
//    }
//    
////    print("天气:%d\r\n温度:%d\r\n时间:%d-%d-%d %d:%d:%d",Cur_Time.weather_code,Cur_Time.temperature, 
////    Cur_Time.year,Cur_Time.month,Cur_Time.day,Cur_Time.hour,Cur_Time.min,Cur_Time.sec
////    );
//}
char* parse_value(const char *src, const char *key, char *out_buf) {
    char *p = strstr(src, key);
    if (p) {
        p += strlen(key);
        int i = 0;
        while (p[i] != '\"' && p[i] != '\0') {
            out_buf[i] = p[i];
            i++;
        }
        out_buf[i] = '\0';
        return (char*)p+i;
    }
    return (char *)src;
}

static void Handle_AT_GET_CLOCK_WEATHER(const char* buf)
{
//    printf("\r\n天气数据%s\r\n",buf);
    
    char  temp_buf[32];
    const char *p=buf;
    p=parse_value(p,"\"name\":\"",Cur_Time.place_str);
    for(uint8_t i=0;i<3;i++)
    {
      p=strstr(p,"\"date\":\"");
      if (p == NULL) break; // 如果找不到了，直接跳出循环防止死机

      if (p != NULL)
      {
          // 跳过 "date":" 这 8 个字节
          p += 8; 
          sscanf(p, "%hu-%hhu-%hhuT", 
                 &Cur_Time.three_day_data[i].year, &Cur_Time.three_day_data[i].month, &Cur_Time.three_day_data[i].day);
      }
      p=parse_value(p,"\"text_day\":\"",Cur_Time.three_day_data[i].weather_str);
      p=parse_value(p,"\"code_day\":\"",temp_buf);
      Cur_Time.three_day_data[i].weather_code_day=(uint8_t)atoi(temp_buf);
      p=parse_value(p,"\"code_night\":\"",temp_buf);
      Cur_Time.three_day_data[i].weather_code_night=(uint8_t)atoi(temp_buf); 
      
      p=parse_value(p,"\"high\":\"",temp_buf);
      Cur_Time.three_day_data[i].high_temperature=(int8_t)atoi(temp_buf);
      p=parse_value(p,"\"low\":\"",temp_buf);
      Cur_Time.three_day_data[i].low_temperature=(int8_t)atoi(temp_buf);    
      if(i==0)
      {
              p=parse_value(p,"\"wind_direction\":\"",Cur_Time.wind_dir_str);
              p=parse_value(p,"\"humidity\":\"",temp_buf);
              Cur_Time.humidity=(uint8_t)atoi(temp_buf);             
      }
    }
    print("\r\n%d-%d-%d %d:%d:%d\r\n high:%d low:%d,w_day:%d,w_night:%d\r\nplace:%s,wind_dir:%s,hum:%d\r\n",Cur_Time.three_day_data[0].year,Cur_Time.three_day_data[0].month,Cur_Time.three_day_data[0].day,Cur_Time.hour,Cur_Time.min,Cur_Time.sec,
    Cur_Time.three_day_data[0].high_temperature,Cur_Time.three_day_data[0].low_temperature,Cur_Time.three_day_data[0].weather_code_day,
    Cur_Time.three_day_data[0].weather_code_night,
    Cur_Time.place_str,Cur_Time.wind_dir_str,Cur_Time.humidity);
    print("\r\n%d-%d-%d %d:%d:%d\r\n high:%d low:%d,w_day:%d,w_night:%d\r\nplace:%s,wind_dir:%s,hum:%d\r\n",Cur_Time.three_day_data[1].year,Cur_Time.three_day_data[1].month,Cur_Time.three_day_data[1].day,Cur_Time.hour,Cur_Time.min,Cur_Time.sec,
    Cur_Time.three_day_data[1].high_temperature,Cur_Time.three_day_data[1].low_temperature,Cur_Time.three_day_data[1].weather_code_day,
    Cur_Time.three_day_data[1].weather_code_night,
    Cur_Time.place_str,Cur_Time.wind_dir_str,Cur_Time.humidity); 

    print("\r\n%d-%d-%d %d:%d:%d\r\n high:%d low:%d,w_day:%d,w_night:%d\r\nplace:%s,wind_dir:%s,hum:%d\r\n",Cur_Time.three_day_data[2].year,Cur_Time.three_day_data[2].month,Cur_Time.three_day_data[2].day,Cur_Time.hour,Cur_Time.min,Cur_Time.sec,
    Cur_Time.three_day_data[2].high_temperature,Cur_Time.three_day_data[2].low_temperature,Cur_Time.three_day_data[2].weather_code_day,
    Cur_Time.three_day_data[2].weather_code_night,
    Cur_Time.place_str,Cur_Time.wind_dir_str,Cur_Time.humidity);

   
}
static void Handle_AT_GET_NTP_TIME(const char*buf)
{
   char *p = strstr(buf, "+CIPSNTPTIME:");
if(p != NULL) {
        // 注意：NTP 格式非常固定，但要数准空格
        // 用 sscanf 匹配这种带月份缩写的格式
        // 格式说明：%*s 代表跳过一个字符串（比如跳过 Tue 和 Feb）
      sscanf(p, "+CIPSNTPTIME:%*s %*s %*hhu %hhu:%hhu:%hhu %*hu", 
             &Cur_Time.hour, 
             &Cur_Time.min, 
             &Cur_Time.sec 
);
    }
}

void new_version_cb(void *user_data)
{
  if(lv_obj_is_valid(ui_setting_update.update_obj.new_version_label))
  {
     lv_label_set_text(ui_setting_update.update_obj.new_version_label,"发现新版本,点击下载"); 
  }
}

static void Handle_Get_GitHub_MyPhone_file_head(const char*buf){
    // 1. 查找 +IPD,512:
    
     
    char *p = strstr(buf, "+IPD,512:");
    
    if(p == NULL) {print("没有找到:+IPD,512:");return;} // 找不到直接退出

    // 2. 跳过头部，定位到真正的 BIN 数据开始
    uint8_t *bin_start = (uint8_t*)p + strlen("+IPD,512:");
    printf("%s",bin_start);
    // 3. 复制 512 字节到你的结构体
    memcpy(&ui_setting_update.head[HEAD_GitHUB], bin_start, sizeof(head_t));
    
    printf("\r\ncrc32:0X%08X",ui_setting_update.head[HEAD_GitHUB].crc32);
    printf("\r\nname:%s",ui_setting_update.head[HEAD_GitHUB].name);
    printf("\r\nreserved:%s",ui_setting_update.head[HEAD_GitHUB].reserved);
    printf("\r\nversion:%u\r\n",ui_setting_update.head[HEAD_GitHUB].version); 
  if(ui_setting_update.head[HEAD_GitHUB].version>ui_setting_update.head[HEAD_SD].version)
  {
    printf("GitHub有新版本\r\n");
    update_is_ready=has_git_new;
    lv_async_call(new_version_cb,NULL);
  }
}
// 输入参数：p_buf 是你 DEAL_BUF 的地址
// 输出参数：save_len 用来存解析出的 1371
// 返回值：指向冒号后面第一个字节的指针（即真数据的开头）
//uint8_t* parse_ipd_info_with_filter(const char *p_buf, uint32_t *save_len) 
//{
//    char *p_ipd = NULL;
//    char *p_colon = NULL;

//    // 1. 定位 +IPD 标志
//    p_ipd = strstr(p_buf, "+IPD,");
//    if (p_ipd == NULL) return NULL;

//    // 2. 寻找冒号，解析长度
//    char *p_num = p_ipd + 5; 
//    p_colon = strchr(p_num, ':');
//    if (p_colon == NULL) return NULL;

//    *save_len = (uint32_t)atoi(p_num);
//    uint8_t* data_start = (uint8_t*)(p_colon + 1);

//    // --- 核心改动：识别并过滤非数据包 ---
//    
//    // A. 过滤 HTTP Header (通常包含 "HTTP/" 字符串)
//    if (strstr((const char*)data_start, "HTTP/1.1") != NULL) {
//        printf("检测到 HTTP Header，跳过本次写入\r\n");
//        *save_len = 0; // 告诉主循环，这包有效数据长度为 0
//        return data_start; 
//    }

//    // B. 过滤关闭连接的提示 (某些模块会把 CLOSED 拼在 IPD 后面)
//    if (strstr((const char*)data_start, "CLOSED") != NULL) {
//        printf("检测到连接关闭标志\r\n");
//        *save_len = 0;
//        return data_start;
//    }

//    return data_start;
//}

///*--------------------------------------------------------------------------------*/
//      

//static void Handle_Get_GitHub_MyPhone_file(const char*buf)
//{
//  FRESULT res;
//  static FIL f;
//  
//  res=f_open(&f,"0:/SD/bin/myPhone.bin",FA_CREATE_ALWAYS|FA_WRITE);
//  if(res!=FR_OK)
//  {
//    printf("创建新更新文件失败res:%d\r\n",res);
//    return;
//  }
//  const char*p=buf;
//  uint8_t* data_ptr;
//  uint32_t ipd_data_len;
//  uint16_t len;
//  uint32_t remain;
//  static uint32_t total_received_file_size = 0;
//  static uint32_t total_received_file_size_pre = 0;
//  uint32_t current_pkg_rem = 0; // 记录当前包还没写完的剩余长度

//    while(1) {
//      UINT  write_len=0;
//        // --- 1. 先消化存量 ---
//        if (current_pkg_rem > 0) {
//            
//            uint32_t can_write = (Total_Len_resp > current_pkg_rem) ? current_pkg_rem : Total_Len_resp;
//            f_write(&f, DEAL_BUF, can_write, &write_len);
//            total_received_file_size += write_len;
//            uint32_t extra = Total_Len_resp - can_write;
//            if(extra > 0) memmove(DEAL_BUF, DEAL_BUF + can_write, extra);
//            Total_Len_resp = extra;
//            current_pkg_rem -= can_write;
//        } 
//        else {
//            // 尝试解析新包头
//            data_ptr = parse_ipd_info_with_filter(p, &ipd_data_len);
//            if(data_ptr != NULL) {
//                len = data_ptr - (uint8_t*)p;
//                remain = Total_Len_resp - len;
//                
//                if(remain >= ipd_data_len) 
//                {
//                    // 整包都在，写完平移
//                    f_write(&f, data_ptr, ipd_data_len, &write_len); 
//                    total_received_file_size += write_len;
//                    remain -= write_len;
//                    memmove(DEAL_BUF, data_ptr +ipd_data_len , remain);
//                    Total_Len_resp = remain;
//                } else 
//                {
//                    f_write(&f, data_ptr, remain, &write_len);
//                    total_received_file_size += write_len;
//                    current_pkg_rem = ipd_data_len - write_len;
//                    Total_Len_resp = 0;
//                }
////if(remain >= ipd_data_len) {
////    f_write(&f, data_ptr, ipd_data_len, &write_len); 
////    total_received_file_size += write_len;
////    
////    // 关键修正：计算真正属于“下一包”的起始偏移量
////    uint32_t pkg_end_offset = (data_ptr - (uint8_t*)DEAL_BUF) + ipd_data_len;
////    uint32_t next_pkg_len = Total_Len_resp - pkg_end_offset;
////    
////    // 关键修正：只移动真正属于下一包的内容
////    memmove(DEAL_BUF, DEAL_BUF + pkg_end_offset, next_pkg_len);
////    Total_Len_resp = next_pkg_len; 
////}
////} else {
////    // 缓冲区里这一包没收全
////    f_write(&f, data_ptr, remain, &write_len);
////    total_received_file_size += write_len;
////    current_pkg_rem = ipd_data_len - write_len;
////    
////    // 这里也要清空，因为这一包已经“吃”完了目前缓冲区的所有东西
////    Total_Len_resp = 0; 
////}
//                
//            }
//        }
//        // --- 2. 后补充增量 ---
////        if(xSemaphoreTake(DX_WF25_Rev_AT_RESP_CountSemaphore, pdMS_TO_TICKS(1)) == pdTRUE) 
////{      
//            uint16_t save_len = fifo_get_occupy_size(&WF25_Rev_fifo); 
//            if(save_len > 0) {
//                // 计算缓冲区还能装多少，防止爆掉
//                uint16_t space = DEAL_BUF_SIZE - Total_Len_resp - 1;
//                uint16_t read_len = (save_len > space) ? space : save_len;
//                if(read_len > 2048) read_len = 2048;

//                fifo_read(&WF25_Rev_fifo, (uint8_t *)(DEAL_BUF + Total_Len_resp), read_len);
//                Total_Len_resp += read_len;
//                DEAL_BUF[Total_Len_resp] = '\0';
//            }
////        }
//        if(total_received_file_size_pre!=total_received_file_size)
//          print("file_size:%u\r\n",total_received_file_size);
//        
//        total_received_file_size_pre=total_received_file_size;
//        
//        if (total_received_file_size >= (ui_setting_update.head[HEAD_GitHUB].file_size+sizeof(head_t)) ) 
//        {

//            f_close(&f);
//            update_is_ready=has_no_new;
//            printf("下载完毕\r\n");
//            return; // 下载完成
//        }
////        f_sync(&f);
//        vTaskDelay(1); 
//        
//    }
//}

/**
 * @brief  解析IPD报头并过滤HTTP响应头
 * @param  p_buf: 缓冲区起点 (DEAL_BUF)
 * @param  save_len: [传出] 真正需要写入SD卡的数据长度
 * @param  raw_pkg_len: [传出] ESP8266声明的原始数据长度 (AT指令里的那个数字)
 * @return uint8_t*: 指向真正有效数据（BIN开头）的起始地址
 */
//uint8_t* parse_ipd_info_with_filter(const char *p_buf, uint32_t *save_len, uint32_t *raw_pkg_len) 
//{
//    char *p_ipd = strstr(p_buf, "+IPD,");
//    if (p_ipd == NULL) return NULL;

//    // 1. 提取原始长度 (例如 895 或 1460)
//    char *p_colon = strchr(p_ipd, ':');
//    if (p_colon == NULL) return NULL;
//    
//    // atoi(p_ipd + 5) 只有在单路连接下才准，这里建议用更稳妥的办法找到数字
//    *raw_pkg_len = (uint32_t)atoi(p_ipd + 5); 
//    
//    uint8_t* data_start = (uint8_t*)(p_colon + 1); // 真正的原始数据起点
//    *save_len = *raw_pkg_len;                      // 默认存整包

//    // 2. 过滤 HTTP 报头（仅针对含有响应头的包）
//    if (strstr((const char*)data_start, "HTTP/1.1") != NULL) {
//        char *p_body = strstr((const char*)data_start, "\r\n\r\n");
//        if (p_body != NULL) {
//            p_body += 4; // 跨过这 4 个字节的分界线
//            uint32_t header_len = (uint8_t*)p_body - data_start;
//            
//            // 剩下的才是真肉 (BIN数据)
//            if (*raw_pkg_len > header_len) {
//                *save_len = *raw_pkg_len - header_len;
//            } else {
//                *save_len = 0; 
//            }
//            return (uint8_t*)p_body; 
//        } else {
//            // 还没收到完整的空行，这一包先不写，等下一波凑齐
//            *save_len = 0;
//            return data_start;
//        }
//    }
//    return data_start;
//}

//static void Handle_Get_GitHub_MyPhone_file(const char* buf)
//{
//    FRESULT res;
//    static FIL f;
//    static uint32_t total_received_file_size = 0;
//    uint32_t current_pkg_rem = 0; // 上一包还没写完的剩余字节
//    uint32_t raw_pkg_len = 0;    

//    res = f_open(&f, "0:/SD/bin/os.bin", FA_CREATE_ALWAYS | FA_WRITE);
//    if(res != FR_OK) {
//        printf("Create file failed: %d\r\n", res);
//        return;
//    }

//    while(1) {
//        UINT write_len = 0;

//        // --- A. 处理跨包残余 (断点续传) ---
//        if (current_pkg_rem > 0) {
//            // 能写多少取决于缓冲区里现有的数据量
//            uint32_t can_write = (Total_Len_resp > current_pkg_rem) ? current_pkg_rem : Total_Len_resp;
//            
//            if (can_write > 0) {
//                f_write(&f, DEAL_BUF, can_write, &write_len);
//                total_received_file_size += write_len;

//                // 物理平移：吃掉多少挪走多少
//                uint32_t next_len = Total_Len_resp - write_len;
//                if(next_len > 0) {
//                    memmove(DEAL_BUF, DEAL_BUF + write_len, next_len);
//                }
//                Total_Len_resp = next_len;
//                current_pkg_rem -= write_len;
//            }
//            // 如果这一包还没补齐，跳出循环去读 FIFO
//            if (current_pkg_rem > 0) goto READ_FIFO;
//        } 

//        // --- B. 解析新包 ---
//        char *p_ipd = strstr((char*)DEAL_BUF, "+IPD,");
//        if (p_ipd != NULL) {
//            uint32_t ipd_save_len = 0;
//            uint8_t* data_ptr = parse_ipd_info_with_filter((const char*)DEAL_BUF, &ipd_save_len, &raw_pkg_len);
//            
//            if (data_ptr != NULL) {
//                char *p_colon = strchr(p_ipd, ':');
//                // 核心对齐公式：跳过长度 = (+IPD到冒号距离) + 1冒号 + 原始总长度
//                uint32_t pkg_physical_size = (p_colon - p_ipd) + 1 + raw_pkg_len;
//                uint32_t total_skip = ( (uint8_t*)p_ipd - (uint8_t*)DEAL_BUF ) + pkg_physical_size;

//                if (Total_Len_resp >= total_skip) {
//                    // 整包已到：写有效数据，平移整个物理块
//                    if (ipd_save_len > 0) {
//                        f_write(&f, data_ptr, ipd_save_len, &write_len);
//                        total_received_file_size += write_len;
//                    }
//                    uint32_t next_len = Total_Len_resp - total_skip;
//                    memmove(DEAL_BUF, DEAL_BUF + total_skip, next_len);
//                    Total_Len_resp = next_len;
//                } else {
//                    // 包没收全：把当前缓冲区里属于这一包的数据先写了
//                    uint32_t data_in_buf = Total_Len_resp - (data_ptr - (uint8_t*)DEAL_BUF);
//                    if (data_in_buf > 0 && ipd_save_len > 0) {
//                        uint32_t can_w = (data_in_buf > ipd_save_len) ? ipd_save_len : data_in_buf;
//                        f_write(&f, data_ptr, can_w, &write_len);
//                        total_received_file_size += write_len;
//                        current_pkg_rem = ipd_save_len - write_len;
//                    }
//                    // 这里绝对不能 Total_Len_resp = 0，要精准平移掉已处理的
//                    uint32_t consumed = Total_Len_resp; 
//                    Total_Len_resp = 0; 
//                }
//            }
//        }

//    READ_FIFO:
//    {
//            // --- C. 补充新数据 (从 FIFO 读) ---
//            uint16_t fifo_size = fifo_get_occupy_size(&WF25_Rev_fifo);
//            if(fifo_size > 0) {
//                uint16_t space = DEAL_BUF_SIZE - Total_Len_resp - 1;
//                uint16_t read_len = (fifo_size > space) ? space : fifo_size;
//                fifo_read(&WF25_Rev_fifo, (uint8_t *)(DEAL_BUF + Total_Len_resp), read_len);
//                Total_Len_resp += read_len;
//                DEAL_BUF[Total_Len_resp] = '\0';
//            }
//    }


//        // 检查下载是否结束
//        if (total_received_file_size >= (ui_setting_update.head[HEAD_GitHUB].file_size + sizeof(head_t))) {
//            f_close(&f);
//            return;
//        }
//        vTaskDelay(1);
//    }
//}

//typedef enum {
//    IPD_FIND_HEAD,
//    IPD_PARSE_LEN,
//    IPD_READ_DATA
//} ipd_state_t;

//typedef struct {
//    ipd_state_t state;
//    uint32_t data_len;     // 当前 +IPD 声明的总长度
//    uint32_t data_cnt;     // 当前已收到的字节计数
//    
//    uint8_t header_done;   // 是否跨过了 HTTP 报头
//    uint8_t crlf_step;     // 用于严格匹配 \r\n\r\n (0->\r, 1->\n, 2->\r, 3->\n)
//    uint8_t match_idx;     // 用于匹配 "+IPD,"

//    uint8_t cache[512];    // SD卡扇区缓存
//    uint16_t cache_ptr;    // 缓存当前指针
//    uint32_t total_saved;  // ✨ 新增：记录总共写进 SD 卡的有效字节数
//    FIL *file_handle;      // 外部传入文件句柄
//} ipd_ctx_t;


//void ipd_stream_process(ipd_ctx_t *ctx, uint8_t *buf, uint32_t len)
//{
//    for(uint32_t i = 0; i < len; i++)
//    {
//        uint8_t ch = buf[i];

//        switch(ctx->state)
//        {
//            case IPD_FIND_HEAD:
//                if(ch == "+IPD,"[ctx->match_idx]) {
//                    ctx->match_idx++;
//                    if(ctx->match_idx == 5) {
//                        ctx->match_idx = 0;
//                        ctx->state = IPD_PARSE_LEN;
//                        ctx->data_len = 0;
//                    }
//                } else {
//                    ctx->match_idx = 0;
//                }
//                break;

//            case IPD_PARSE_LEN:
//                if(ch >= '0' && ch <= '9') {
//                    ctx->data_len = ctx->data_len * 10 + (ch - '0');
//                } else if(ch == ':') {
//                    ctx->data_cnt = 0;
//                    ctx->state = IPD_READ_DATA;
//                }
//                break;

//            case IPD_READ_DATA:
//                if(!ctx->header_done) {
//                    // 严格匹配 \r\n\r\n
//                    if      (ctx->crlf_step == 0 && ch == 'H') ctx->crlf_step = 1;
//                    else if (ctx->crlf_step == 1 && ch == 'T') ctx->crlf_step = 2;
//                    else if (ctx->crlf_step == 2 && ch == 'T') ctx->crlf_step = 3;
//                    else if (ctx->crlf_step == 3 && ch == 'P') {
//                        ctx->header_done = 1; 
//                        ctx->state = IPD_FIND_HEAD;
//                        printf("\r\n[OK] HTTP Header 过滤完成，开始写入 BIN 数据...\r\n");
//                    } else {
//                        ctx->crlf_step = (ch == 'H') ? 1 : 0; 
//                    }
//                    // 注意：这里没有 ctx->cache[ptr] = ch，所以报头字符被全部丢弃
//                } 
//                else {
//                    // 只有 header_done 为 1 后，数据才进入缓存和计数
//                    ctx->cache[ctx->cache_ptr++] = ch;
//                    ctx->total_saved++; 

//                    if(ctx->cache_ptr >= 512) {
//                        UINT bw;
//                        f_write(ctx->file_handle, ctx->cache, ctx->cache_ptr, &bw);
//                        ctx->cache_ptr = 0;
//                    }
//                }

//                ctx->data_cnt++;
//                if(ctx->data_cnt >= ctx->data_len) {
//                    ctx->state = IPD_FIND_HEAD;
//                    ctx->match_idx = 0;
//                }
//                break;
//        }
//    }
//}

//ipd_ctx_t ipd_ctx; // 建议放在全局

//static void Handle_Get_GitHub_MyPhone_file(const char* buf)
//{
//    FRESULT res;
//    static FIL f; 
//    uint32_t target_size = ui_setting_update.head[HEAD_GitHUB].file_size + sizeof(head_t);
//    uint32_t last_print_time = 0;

//    // 1. 初始化
//    memset(&ipd_ctx, 0, sizeof(ipd_ctx));
//    ipd_ctx.file_handle = &f; 

//    // 2. 打开文件
//    res = f_open(&f, "0:/SD/bin/os.bin", FA_CREATE_ALWAYS | FA_WRITE);
//    if(res != FR_OK) {
//        printf("[Error] SD卡文件创建失败: %d\r\n", res);
//        return;
//    }

//    printf("\r\n==========================================\r\n");
//    printf("  OTA 开始下载: os.bin\r\n");
//    printf("  目标大小: %u 字节\r\n", target_size);
//    printf("==========================================\r\n");
//    ipd_stream_process(&ipd_ctx,  (uint8_t*)DEAL_BUF, Total_Len_resp);
//    
//    while(1)
//    {
//        uint16_t len = fifo_get_occupy_size(&WF25_Rev_fifo);
//        if(len > 0)
//        {
//            uint16_t read_len = (len > sizeof(DEAL_BUF)) ? sizeof(DEAL_BUF) : len;
//            fifo_read(&WF25_Rev_fifo, (uint8_t*)DEAL_BUF, read_len);
//            
//            ipd_stream_process(&ipd_ctx,  (uint8_t*)DEAL_BUF, read_len);

//            // 每隔 1000ms 打印一次下载进度 (使用 HAL_GetTick 或其他系统时钟)
//            if (xTaskGetTickCount() - last_print_time > 1000) {
//                last_print_time = xTaskGetTickCount();
//                printf("下载进度: %u / %u (%u%%)\r\n", 
//                        ipd_ctx.total_saved, 
//                        target_size, 
//                        (ipd_ctx.total_saved * 100) / target_size);
//            }

//            if (ipd_ctx.total_saved >= target_size) {
//                printf("\r\n[Success] 已达到目标大小: %u\r\n", ipd_ctx.total_saved);
//                break; 
//            }
//        }
//        
//        // 增加一个简单的超时处理 (如果超过 10 秒没数据，认为网络断开)
//        // 这里需要你自己根据系统增加计时逻辑
//        
//        vTaskDelay(1);
//    }

//    // 3. 收尾
//    if(ipd_ctx.cache_ptr > 0) {
//        UINT bw;
//        f_write(&f, ipd_ctx.cache, ipd_ctx.cache_ptr, &bw);
//        printf("[System] 刷入最后余量数据: %d 字节\r\n", ipd_ctx.cache_ptr);
//        ipd_ctx.cache_ptr = 0;
//    }

//    f_close(&f);
//    printf("==========================================\r\n");
//    printf("  下载完成！文件已保存至 SD 卡。\r\n");
//    printf("  总写入量: %u 字节\r\n", ipd_ctx.total_saved);
//    printf("==========================================\r\n");
//}

static void Handle_Get_GitHub_MyPhone_file(const char* buf)
{
    FRESULT res;
    static FIL f;
    static uint32_t total_raw_saved = 0;
    static uint32_t total_raw_saved_pre = 0;
    uint32_t last_data_time = xTaskGetTickCount(); // 记录最后一次抓到数据的时间
    UINT bw;
    
    res = f_open(&f, "0:/SD/bin/raw_dump.bin", FA_CREATE_ALWAYS | FA_WRITE);
    if(res != FR_OK) {
        printf("[Error] SD卡打开失败: %d\r\n", res);
        return;
    }
    f_write(&f, DEAL_BUF, Total_Len_resp, &bw);
    Total_Len_resp=0;
    printf("\r\n[System] 暴力模式启动！监测到 5 秒无数据将自动退出...\r\n");

    while(1)
    {
        uint16_t len = fifo_get_occupy_size(&WF25_Rev_fifo);
        
        if(len > 0)
        {
            // 1. 只要有数据，就更新时间戳
            last_data_time = xTaskGetTickCount(); 

            uint16_t read_len = (len > sizeof(DEAL_BUF)) ? sizeof(DEAL_BUF) : len;
            fifo_read(&WF25_Rev_fifo, (uint8_t*)DEAL_BUF, read_len);
            
            
            f_write(&f, DEAL_BUF, read_len, &bw);
            total_raw_saved += bw;
            total_raw_saved_pre=total_raw_saved;
            // 顺便打个点，表示正在工作
        }
        else 
        {
            // 2. 没数据时，检查是否超时
            if ((xTaskGetTickCount() - last_data_time) > pdMS_TO_TICKS(5000)) 
            {
                printf("\r\n[Timeout] 持续 5 秒无数据，判定下载结束。\r\n");
                break; 
            }
        }
        if(total_raw_saved!=total_raw_saved_pre)
        {
          print("当前进度:%u\r\n",total_raw_saved);
        }
        
        vTaskDelay(5); // 稍微加长一点，给 FIFO 留出堆积的空间，提高写入效率
    }

    f_close(&f);
    printf("[Success] 原始数据已固化至 SD 卡，共 %u 字节。\r\n", total_raw_saved);
}