#ifndef  __FILE_TXT_H
#define __FILE_TXT_H

#include "lvgl.h"
#include "../ui_widgets.h"

void operation_file_txt(lv_obj_t*parent,const char*path);

#define PAGE_SIZE 1024
#define MAX_PAGES 100
#define MAX_PAGE_NUM 3
#define READ_BUF_MAX_SIZE PAGE_SIZE*MAX_PAGE_NUM
#define READ_BUF_SIZE (PAGE_SIZE * MAX_PAGE_NUM + 12) // 多给 10 字节安全区

typedef struct 
{
    lv_fs_file_t file;
    lv_obj_t * txt_label;
    uint32_t page_offsets[MAX_PAGES]; // 绝对位置书签
    uint32_t total_page_count;        // 文件已读过的最大页数
    uint32_t total_read_len;        // 当前文本框显示的第一页索引
    uint32_t file_size;
    uint16_t cur_page_index;
} txt_reader_t;

#endif

