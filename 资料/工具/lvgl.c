#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>  // 增加bool类型头文件
#ifdef _WIN32
#include <windows.h>
#else
#include <locale.h>   // Linux/macOS locale头文件
#endif
#include "lvgl.h"

// 3. 全局变量：存储计算后的数组长度
uint32_t glyph_dsc_len = 0;
uint32_t glyph_bitmap_len = 0;

// 4. 空实现LVGL函数（仅为编译通过，工具无需实际逻辑）
bool lv_font_get_glyph_dsc_fmt_txt(void)
{
    return true;
}

const uint8_t * lv_font_get_bitmap_fmt_txt(void)
{
    return NULL;
}

// 5. 计算字库数组长度（核心函数，替代sizeof）
void calc_font_array_len(void)
{
    // 计算glyph_dsc长度：遍历到终止符（bitmap_index=0xFFFFFFFF）
    glyph_dsc_len = 0;
    while (1) {
        // 防死循环：最大支持65536个字符
        if (glyph_dsc_len >= 65536) break;
        // LVGL字库终止符：bitmap_index全1（20位）
        if (glyph_dsc[glyph_dsc_len].bitmap_index == 0xFFFFF) break;
        glyph_dsc_len++;
    }

    // 计算glyph_bitmap长度：最大索引 + 最后一个字符的点阵字节数
    if (glyph_dsc_len > 0) {
        uint32_t max_index = 0;
        for (uint32_t i = 0; i < glyph_dsc_len; i++) {
            if (glyph_dsc[i].bitmap_index > max_index) {
                max_index = glyph_dsc[i].bitmap_index;
            }
        }
        // 最后一个字符的点阵字节数 = (宽*高 +7)/8（向上取整）
        lv_font_fmt_txt_glyph_dsc_t last_glyph = glyph_dsc[glyph_dsc_len - 1];
        uint32_t last_bitmap_len = (last_glyph.box_w * last_glyph.box_h + 7) / 8;
        glyph_bitmap_len = max_index + last_bitmap_len;
    }

    // 兜底：不超过1MB（LVGL 20位地址限制）
    if (glyph_bitmap_len > 1024 * 1024) {
        glyph_bitmap_len = 1024 * 1024;
    }
}

// 6. 生成BIN文件名（修复字符串越界问题）
void Get_bin_filename(const char *c_file, const char* suffix, char *bin_file, uint32_t bin_file_len)
{
    // 安全拷贝文件名（去掉.c后缀）
    const char *dot = strrchr(c_file, '.');
    if (dot != NULL && strcmp(dot, ".c") == 0) {
        // 复制.c前的部分
        uint32_t base_len = dot - c_file;
        if (base_len < bin_file_len - 1) {
            strncpy(bin_file, c_file, base_len);
            bin_file[base_len] = '\0';
        } else {
            strncpy(bin_file, c_file, bin_file_len - 1);
            bin_file[bin_file_len - 1] = '\0';
        }
    } else {
        // 无.c后缀，直接用原文件名
        strncpy(bin_file, c_file, bin_file_len - 1);
        bin_file[bin_file_len - 1] = '\0';
    }
    // 拼接后缀
    snprintf(bin_file + strlen(bin_file), bin_file_len - strlen(bin_file), "_%s.bin", suffix);
}

int main(int argc, char *argv[])
{
    // 设置UTF-8编码（解决中文乱码）
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    #else
    setlocale(LC_ALL, "zh_CN.UTF-8");
    #endif

    // 检查命令行参数
    if (argc != 2) {
        printf("LVGL字库转BIN工具使用方法：\r\n");
        printf("  %s <字库C文件路径>\r\n", argv[0]);
        printf("  示例：%s lv_font_16px.c\r\n", argv[0]);
        return 1;
    }

    const char *c_file = argv[1];
    printf("开始处理字库文件：%s\n", c_file);
    printf("提示：该工具仅处理小字体字库，大字体请使用font_tool_large！\n");

    // 计算字库数组长度
    calc_font_array_len();

    // 检查字库长度
    if (glyph_dsc_len == 0) {
        printf("错误：未检测到有效字库数据！\n");
        return 1;
    }

    // 打印字库信息
    if (glyph_dsc_len < 1024 * 1024) {
        printf("小文件 - 字符总数：%u，点阵长度：%u 字节\n", glyph_dsc_len, glyph_bitmap_len);
    } else {
        printf("大文件 - 字符总数：%u，点阵长度：%u 字节\n", glyph_dsc_len, glyph_bitmap_len);
    }

    // 生成并写入点阵BIN文件
    char bmp_bin[256] = {0};
    Get_bin_filename(c_file, "bitmap", bmp_bin, sizeof(bmp_bin));
    FILE *fp_bmp = fopen(bmp_bin, "wb");
    if (!fp_bmp) {
        printf("错误：打开点阵文件 %s 失败！\n", bmp_bin);
        return 1;
    } else {
        fwrite(glyph_bitmap, sizeof(uint8_t), glyph_bitmap_len, fp_bmp);
        fclose(fp_bmp);
        printf("成功：点阵文件 %s 写入完毕（大小：%u 字节）\n", bmp_bin, glyph_bitmap_len);
    }

    // 生成并写入索引BIN文件
    char dsc_bin[256] = {0};
    Get_bin_filename(c_file, "dsc", dsc_bin, sizeof(dsc_bin));
    FILE *fp_dsc = fopen(dsc_bin, "wb");
    if (!fp_dsc) {
        printf("错误：打开索引文件 %s 失败！\n", dsc_bin);
        return 1;
    } else {
        fwrite(glyph_dsc, sizeof(lv_font_fmt_txt_glyph_dsc_t), glyph_dsc_len, fp_dsc);
        fclose(fp_dsc);
        printf("成功：索引文件 %s 写入完毕（大小：%u 字节）\n", dsc_bin, glyph_dsc_len * sizeof(lv_font_fmt_txt_glyph_dsc_t));
    }

    printf("\n处理完成！按任意键退出...\n");
    #ifdef _WIN32
    system("pause"); // Windows暂停窗口
    #else
    getchar();       // Linux/macOS暂停
    #endif

    return 0;
}