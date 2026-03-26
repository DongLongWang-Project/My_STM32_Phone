#include "stdio.h"
#include <stdint.h>
//0x4C11DB7
uint8_t buf[1024*1024];
typedef enum
{
    update_none=0xFF,
    update_is_running=0xFE,
    update_is_ok=0xFC,
}update_state_t;
typedef struct 
{
    uint32_t version;
    uint32_t CRC32;
    uint32_t file_size;
    char name[16];
    update_state_t update_state;   
}head_t;
head_t head=
{
    .version=20260324,
    .name="myPhone_v1.0",
    .update_state=update_none,
};
uint32_t Continue_CRC32(uint32_t last_crc, uint8_t* data, uint32_t len) {
    uint32_t crc = last_crc; // 接力上次的结果
    for (uint32_t i = 0; i < len; i++) {
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

        // --- 修正 2: 读取数据 ---
        // 参数含义：读取到buf, 每个单元1字节, 总共读len个, 从fp读
        size_t count = fread(buf, 1, len, fp); 

        if (count == len) {
        uint32_t calc_len = count;
        uint8_t remainder = count % 4;

        if (remainder != 0) {

            uint8_t padding_count = 4 - remainder; // 需要补几个 0xFF (1, 2, 或 3)
            
            for (uint8_t i = 0; i < padding_count; i++) {
                buf[count + i] = 0xFF; // 在末尾填充 0xFF
            }
            
            calc_len = count + padding_count; // 更新计算用的长度
                    }
            // --- 修正 3: CRC计算 ---
            // 初始值给 0xFFFFFFFF
            head.CRC32 = Continue_CRC32(0xFFFFFFFF, buf, calc_len);
            head.file_size=count;
            printf("文件长度: %u 字节\r\n", len);
            printf("实际读取: %u 字节,补充满4字节后:%u\r\n", (uint32_t)count,calc_len);
            // 打印 CRC 建议用十六进制 %08X，方便对比
            printf("计算出的 CRC32: 0x%08X\r\n", head.CRC32);
        }
        else {
            printf("读取数据不完整！\r\n");
        }
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
                uint32_t write_app_len=fwrite(buf,1,count,fp_new);
                if(write_app_len==count)
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