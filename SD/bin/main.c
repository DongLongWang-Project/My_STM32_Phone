#include "stdio.h"
#include <stdint.h>
//0x4C11DB7
uint8_t buf[0x1000];
uint8_t buf_total[1024*1024];
typedef enum
{
    update_none=0xFF,
    update_is_running=0xFE,
    update_is_ok=0xFC,
}update_state_t;
typedef struct 
{
    uint32_t version;
    uint32_t crc32;
    uint32_t file_size;
    char name[16];
    update_state_t update_state;
    uint8_t reserved[512-32];   
}head_t;
head_t head=
{
    .version=20260406,
    .name="MyPhoneOS_v1.5",
    .update_state=update_none,
    .reserved="This is my Graduation Project Work",
};
    // uint8_t remainder = len % 4;
    // if (remainder != 0) 
    // {
    //     uint8_t padding_count = 4 - remainder; // 需要补几个 0xFF (1, 2, 或 3)
        
    //     for (uint8_t i = 0; i < padding_count; i++) {
    //         data[len + i] = 0xFF; // 在末尾填充 0xFF
    //     }
        
    //     calc_len = len + padding_count; // 更新计算用的长度
    //    }
uint32_t Continue_CRC32(uint32_t last_crc, uint8_t* data, uint32_t len) {
    uint32_t crc = last_crc; // 接力上次的结果
    uint32_t calc_len = len;


       
    for (uint32_t i = 0; i < calc_len; i++) {
        crc ^= ((uint32_t)data[i] << 24);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80000000) crc = (crc << 1) ^ 0x04C11DB7;
            else crc = (crc << 1);
        }
    }
    return crc;
}


int main(void)
{
    FILE *fp;
    FILE *fp_new;
    fp = fopen("project.bin", "rb");
    if (fp != NULL)
    {
        printf("打开文件成功\r\n");

        // --- 修正 1: 获取文件大小的正确姿势 ---
        fseek(fp, 0, SEEK_END);      // 先移到末尾
        uint32_t len = ftell(fp);    // 获取末尾位置（即大小）
        fseek(fp, 0, SEEK_SET);      // ***必须移回开头***，否则读不到数据
        uint32_t save_len=fread(buf_total, 1, len, fp);
        fseek(fp, 0, SEEK_SET);
        printf("save_len:%u\r\n",save_len);
        uint32_t file_size = len;
        uint32_t buf_size = sizeof(buf);
        uint32_t offset = 0;
        uint32_t remain = file_size;
        uint32_t read_len;
        uint32_t current_crc = 0XFFFFFFFF;
        uint32_t num;


        // --- 修正 2: 读取数据 ---
        // 参数含义：读取到buf, 每个单元1字节, 总共读len个, 从fp读
        // size_t count = fread(buf, 1, len, fp); 
    while (remain > 0)
    {
        // 计算本次读取长度：取 buf_size 和 剩余长度 的最小值
        read_len = (remain > buf_size) ? buf_size : remain;
        // 从 Flash 读取当前分块
        size_t count=fread(buf, 1, read_len, fp);
        // --- 修正 3: CRC计算 ---
        // 初始值给 0xFFFFFFFF
        //  Continue_CRC32(0xFFFFFFFF, buf, 0XFFFF*3);
        current_crc=Continue_CRC32(current_crc,buf,count);
        printf("%02X %02X %02X %02X count:%d current_crc:0X%08X\r\n",buf[count-4],buf[count-3],buf[count-2],buf[count-1],count,current_crc);

     
        // printf("文件长度: %u 字节\r\n", count); 
        // printf("read_len:%u current_crc:0x%08X\r\n",read_len,current_crc); 
        // 更新状态
        offset += count;
        remain -= count;
        
    }
        head.crc32=current_crc;
        head.file_size=offset;
        printf("计算出的 CRC32: 0x%08X\r\n", head.crc32);
        fclose(fp);    
        fp_new=fopen("myPhone.bin","wb");
        if(fp_new!=NULL)
        {
            printf("创建成功\r\n");
            uint32_t count_head;
            count_head=fwrite(&head,1,sizeof(head_t),fp_new);
            if(count_head==sizeof(head_t))
            {
                printf("写入头部成功\r\n");
                fseek(fp_new,0,SEEK_END);
                uint32_t write_app_len=fwrite(buf_total,1,head.file_size,fp_new);
                if(write_app_len==head.file_size)
                {
                  printf("写入app数据成功\r\n");  
                }
            }
            fclose(fp_new);     
        }
        
    }
    else
    {
        printf("打开文件失败，请检查文件名或路径\r\n"); 
    }

    while(1);
}