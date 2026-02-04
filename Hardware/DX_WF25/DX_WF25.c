/*--------------------------------------------------------------------------------↓
	@项目	: DX_WF25模块代码(ESP32-C2的wifi模块)
	@日期	: 2026-1-21
	@备注	:
↑--------------------------------------------------------------------------------*/

#include "DX_WF25.h"



volatile SemaphoreHandle_t Timer_Send_AT_BinSemaphore;
volatile SemaphoreHandle_t DX_WF25_Send_AT_BUSY_BinSemaphore;
volatile SemaphoreHandle_t DX_WF25_Rev_AT_RESP_CountSemaphore; 

volatile uint8_t  Rev_IDLE_flag=0;/*空闲中断接收完成标志*/
volatile uint8_t  DMA_Send_Busy; /*DMA发送中标志,由串口DMA发送函数置位,由DMA发送完成中断清标志*/

volatile uint16_t Pre_deal_index,Total_Len;/*上一次复制数据的位置,总长度*/

char WIFI_DMA_REV_BUF[USART3_RX_BUF_SIZE];/*接收缓冲区*/
char DEAL_BUF[DEAL_BUF_SIZE];/*总数据处理缓冲区*/

wifi_context_t wifi_scan_list;/*扫描当前环境wifi的结构体*/
wifi_connect_t connected_wifi;/*已连接wifi结构体*/
wifi_save_t wifi_save_list;   /*已保存在w25q的wifi结构体*/

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

ui_weather_time_t Cur_Time=
{
    .year=2026,
    .month=2,
    .day=3,
    .hour=10,
    .min=2,
    .sec=30,
    
    .temperature=2,
    .weather_code=1,
    
    .timer_hour=0,
    .timer_min=0,
    .timer_sec=0,
};

QueueHandle_t DX_WF25_CMD_Queue; /*AT指令队列*/


AT_CMD_WIFI_ENUM cur_cmd=AT_CMD_NUM;/*当前正在处理的命令*/

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
/* AT命令结构体配置 */
const wifi_cmd_t wifi_cmd_table[AT_CMD_NUM] = 
{
                      /* 枚举索引           指令字符串                   超时(ms)   预期响应 */
    [AT_CMD_AT]     ={AT_CMD_AT,         "AT\r\n",                   500,      "\r\nOK"},      /* 基础测试 */
    [AT_CMD_RST]    ={AT_CMD_RST,        "AT+RST\r\n",               3000,     "ready"},       /* 复位需等待硬件就绪标志 */
    [AT_CMD_ATE1]   ={AT_CMD_ATE1,       "ATE1\r\n",                 500,      "\r\nOK",Handle_AT_CMD_ATE1},      /* 开启回显 */
    
    [AT_MODE_OFF]   ={AT_MODE_OFF,       "AT+CWMODE=0\r\n",          1000,     "\r\nOK"}, 
    [AT_MODE_STA]   ={AT_MODE_STA,       "AT+CWMODE=1\r\n",          1000,     "\r\nOK"}, 
    [AT_MODE_AP]    ={AT_MODE_AP,        "AT+CWMODE=2\r\n",          1000,     "\r\nOK"}, 
    [AT_MODE_STA_AP]={AT_MODE_STA_AP,    "AT+CWMODE=3\r\n",          1000,     "\r\nOK"},
    
    [AT_CMD_CIPMUX_ONE]={AT_CMD_CIPMUX_ONE,  "AT+CIPMUX=0\r\n",         500,      "\r\nOK"},
    [AT_CMD_CIPMUX_MANY]={AT_CMD_CIPMUX_MANY, "AT+CIPMUX=1\r\n",         500,      "\r\nOK"},
    
    [AT_CMD_CIPSERVERE]={AT_CMD_CIPSERVERE, "AT+CIPSERVER=1,8080\r\n",  1000,     "\r\nOK"},
    
    [AT_CMD_CIFSR]={AT_CMD_CIFSR,      "AT+CIFSR\r\n",             1000,     "\r\nOK",Handle_AT_CMD_CIFSR},      /* 查询IP */
    [AT_CMD_CIPAP]= {AT_CMD_CIPAP,      "",                         1000,     "\r\nOK"},      /* 查询AP IP */
    [AT_CMD_CWSAP]={AT_CMD_CWSAP,      "",                         1000,     "\r\nOK",Handle_AT_CMD_CWSAP},      /* 查询热点配置 */
    
    [AT_CMD_CWLAP]={AT_CMD_CWLAP,      "AT+CWLAP\r\n",             8000,     "\r\nOK",Handle_AT_CMD_CWLAP},      /* 扫描热点极慢，给8秒 */
    [AT_CMD_CWJAP]={AT_CMD_CWJAP,      "",                         15000,    "WIFI GOT IP",Handle_AT_CMD_CWJAP},    /* 连路由最慢，给15秒 */
    [AT_CMD_CWJAP_USER]={AT_CMD_CWJAP_USER,     "AT+CWJAP?\r\n",            1000,     "\r\nOK",Handle_AT_CMD_CWJAP_USER},
    
    [AT_CMD_CIPMODE_0]={AT_CMD_CIPMODE_0,    "AT+CIPMODE=0\r\n",        500,        "\r\nOK" },    /*普通模式*/
    [AT_CMD_CIPMODE_1]={AT_CMD_CIPMODE_1,    "AT+CIPMODE=1\r\n",        500,        "\r\nOK" },    /*普通模式*/
    
    [AT_CMD_CIPSTART]={AT_CMD_CIPSTART,   "AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n",500, "CONNECT\r\n\r\nOK"},
    [AT_CMD_CIPSEND]={AT_CMD_CIPSEND,         "",           500,        ">",Handle_AT_CMD_CIPSEND},
    [AT_GET_CLOCK_WEATHER]={AT_GET_CLOCK_WEATHER,  "",           1000,       "CLOSED\r\n",Handle_AT_GET_CLOCK_WEATHER},
    
    [AT_CMD_CIPSNTPCFG]={AT_CMD_CIPSNTPCFG,"AT+CIPSNTPCFG=1,8,\"ntp.aliyun.com\",\"cn.ntp.org.cn\"\r\n",2000,"OK\r\n+TIME_UPDATED"} ,
    [AT_GET_NTP_TIME]={AT_GET_NTP_TIME,"AT+CIPSNTPTIME?\r\n",1000,"OK\r\n",Handle_AT_GET_NTP_TIME},
};

//const char*get_time_weather_str="GET /v3/weather/now.json?key=S_fPwMVZxdqUzfG_i&location=beijing&language=zh-Hans&unit=c HTTP/1.1\r\nHost: api.seniverse.com\r\nConnection: close\r\n\r\n";            
const char*get_time_weather_str="GET /v3/weather/now.json?key=S_fPwMVZxdqUzfG_i&location=beijing&language=zh-Hans&unit=c HTTP/1.1\r\nHost: api.seniverse.com\r\nConnection: close\r\n\r\n";            

//GET /v3/weather/now.json?key=S_fPwMVZxdqUzfG_i&location=beijing&language=zh-Hans&unit=c HTTP/1.1
//Host: api.seniverse.com
//Connection: close



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

    Timer_Send_AT_BinSemaphore=xSemaphoreCreateBinary();
    if(Timer_Send_AT_BinSemaphore!=NULL)
    {
      xSemaphoreGive(Timer_Send_AT_BinSemaphore);
    }
    
    DX_WF25_Send_AT_BUSY_BinSemaphore=xSemaphoreCreateBinary();
    if(DX_WF25_Send_AT_BUSY_BinSemaphore!=NULL)
    {
      xSemaphoreGive(DX_WF25_Send_AT_BUSY_BinSemaphore);
    } 
    
    DX_WF25_Rev_AT_RESP_CountSemaphore=xSemaphoreCreateCounting(5,0);
}

void SendByte(uint8_t Byte)
{
    USART_SendData(USART3,Byte);
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);  
}

void SendString(const char *String)
{
	for (uint8_t i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
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

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
       static uint32_t time=0;
       time++;
       if(time>=1800)
       {  
          if(wifi_scan_list.connected==1)
          {
               BaseType_t xHigherPriorityTaskWoken = pdFALSE;
               xSemaphoreGiveFromISR(Timer_Send_AT_BinSemaphore, &xHigherPriorityTaskWoken);

               time=0;
               portYIELD_FROM_ISR(xHigherPriorityTaskWoken); 
          }
          else
          {
            time-=60;
          }
       }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

static void DMA_Receive_Data(uint16_t Cur_index)
{
    uint16_t start = Pre_deal_index;
    uint16_t len = 0;

    if (Cur_index == start) return;

    // 情况1：没有回绕 (正常顺序)
    if (Cur_index > start) 
    {
        len = Cur_index - start;
        if (Total_Len + len < DEAL_BUF_SIZE) 
        {
            memcpy(&DEAL_BUF[Total_Len], &WIFI_DMA_REV_BUF[start], len);
            Total_Len += len;
        }
    }
    // 情况2：发生了回绕 (新指针跳回了头部)
    else 
    {
        // 第一段：拷贝 [旧指针 -> 缓冲区物理末尾]
        uint16_t len_to_end = USART3_RX_BUF_SIZE - start;
        // 第二段：拷贝 [缓冲区物理开头 -> 新指针]
        uint16_t len_from_start = Cur_index;

        if (Total_Len + len_to_end + len_from_start < DEAL_BUF_SIZE) 
        {
            // 必须分两步拷贝，找回丢失的“末尾字节”
            memcpy(&DEAL_BUF[Total_Len], &WIFI_DMA_REV_BUF[start], len_to_end);
            memcpy(&DEAL_BUF[Total_Len + len_to_end], &WIFI_DMA_REV_BUF[0], len_from_start);
            
            Total_Len += (len_to_end + len_from_start);
        }
    }

    // 更新索引
    Pre_deal_index = Cur_index;
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
        Pre_deal_index = 0; 
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
#define BUF_SIZE 512  // 每个缓冲区的大小

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
    
    print("拼装后:%s\r\n",target_ptr);
    print("长度:%d\r\n",strlen(target_ptr));
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
    if(xSemaphoreTake(DX_WF25_Rev_AT_RESP_CountSemaphore, 0) == pdTRUE)
    {
        if(strstr(DEAL_BUF, "WIFI GOT IP") != NULL)
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
            
            DX_WF25_Send_Static(AT_CMD_CIPMODE_0);
            DX_WF25_Send_Static(AT_CMD_CIPSTART);
            DX_WF25_Send_Dynamic(AT_CMD_CIPSEND,"AT+CIPSEND=%d\r\n",strlen(get_time_weather_str)); 
            DX_WF25_Send_Dynamic(AT_GET_CLOCK_WEATHER,"%s",get_time_weather_str); 
        }
        else if(strstr(DEAL_BUF, "WIFI DISCONNECT") != NULL)
        {
            display_cfg.wifi_switch_state = false;
        }
    }
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
      Total_Len=0;     
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
            Total_Len = 0;
            memset(DEAL_BUF, 0, sizeof(DEAL_BUF));
        }
     else if(xSemaphoreTake(DX_WF25_Rev_AT_RESP_CountSemaphore,pdMS_TO_TICKS(100))==pdTRUE)
      {
          if(strstr(DEAL_BUF, wifi_cmd_table[cur_cmd].cmd_resp) != NULL)
            {
                S = 3; 
                DEAL_BUF[Total_Len]='\0';
                print("%s",DEAL_BUF);
            }
      }
  }
  else if(S==3)
  {
  if(wifi_cmd_table[cur_cmd].handler!=NULL)
  {
     wifi_cmd_table[cur_cmd].handler(DEAL_BUF);
  }
  else
  {
    printf("没来得及\r\n");
  }
     
      S=0;
      Total_Len = 0;
      memset(DEAL_BUF,0,sizeof(DEAL_BUF));
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
      W25Qxx_SectorErase(WIFI_SAVE_Addr);       
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
   deal_wifi_hotspot_ip_data(DEAL_BUF);
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
static void Handle_AT_GET_CLOCK_WEATHER(const char* buf)
{
    char* p;

    // 1. 提取温度 (使用你最喜欢的 strlen 方案)
    p = strstr(buf, "\"temperature\":");
    if (p != NULL) {
        // +strlen("\"temperature\":") 会直接跳到冒号后面
        // 如果数据是 "temperature":"2"，跳过后指向 "2"
        p += strlen("\"temperature\":") + 1; 
        Cur_Time.temperature = (int8_t)atoi(p);
    }

    // 2. 提取天气代码
    p = strstr(buf, "\"code\":");
    if (p != NULL) {
        p += strlen("\"code\":") + 1; 
        Cur_Time.weather_code = (uint8_t)atoi(p);
    }

    // 3. 提取时间 (注意下划线 _)
    p = strstr(buf, "\"last_update\":");
    if (p != NULL) {
        // 跳过 "last_update":" 这 15 个字节
        p += 15; 
        // 格式：%hu 是 uint16_t, %hhu 是 uint8_t
        // 注意：有些编译器 sscanf 不支持 %hhu，如果报错请改用临时 int 中转
        sscanf(p, "%hu-%hhu-%hhuT", 
               &Cur_Time.year, &Cur_Time.month, &Cur_Time.day);
    }
    
    print("天气:%d\r\n温度:%d\r\n时间:%d-%d-%d %d:%d:%d",Cur_Time.weather_code,Cur_Time.temperature, 
    Cur_Time.year,Cur_Time.month,Cur_Time.day,Cur_Time.hour,Cur_Time.min,Cur_Time.sec
    );
}
static void Handle_AT_GET_NTP_TIME(const char*buf)
{
   char *p = strstr(buf, "+CIPSNTPTIME:");
if(p != NULL) {
        // 注意：NTP 格式非常固定，但要数准空格
        // 用 sscanf 匹配这种带月份缩写的格式
        // 格式说明：%*s 代表跳过一个字符串（比如跳过 Tue 和 Feb）
      sscanf(p, "+CIPSNTPTIME:%*s %*s %hhu %hhu:%hhu:%hhu %hu", 
             &Cur_Time.day, 
             &Cur_Time.hour, 
             &Cur_Time.min, 
             &Cur_Time.sec, 
             &Cur_Time.year);
    }
}
/*--------------------------------------------------------------------------------*/

