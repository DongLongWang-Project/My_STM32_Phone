/*--------------------------------------------------------------------------------↓
	@项目	: 文本文件的处理
	@日期	:  2026-1-25
	@备注	:
↑--------------------------------------------------------------------------------*/


#include "file_txt.h"


static uint8_t UTF8_Check_Truncation(const char *buf, uint32_t len);


lv_fs_res_t read_file_data(char*read_buf,uint16_t read_len,uint32_t seek_index);
static void event_close_file_txt_cb(lv_event_t*e);
static void event_fle_txt_scroll(lv_event_t*e);
static void get_every_page_len(void);
txt_reader_t  txt_reader;
char txt_reader_static_buf[READ_BUF_MAX_SIZE+12];
/*--------------------------------------------------------------------------------↓
	@函数	  : 操作文本文件
	@参数	  : 父对象 路径
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
void operation_file_txt(lv_obj_t*parent,const char*path)
{
    uint16_t start_three_page_len;
    
    lv_obj_t*obj=lv_obj_create(parent);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(obj, 0, 0);
    
    lv_obj_set_scroll_dir(obj, LV_DIR_VER);         // 只允许垂直滚动
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO); // 自动显示滚动条

    lv_fs_res_t res = lv_fs_open(&txt_reader.file, path, LV_FS_MODE_RD); /*打开文件的状态*/
    if(res== LV_FS_RES_OK)
    {
        lv_fs_seek(&txt_reader.file, 0, LV_FS_SEEK_END);
        lv_fs_tell(&txt_reader.file,&txt_reader.file_size);
        lv_fs_seek(&txt_reader.file, 0, LV_FS_SEEK_SET);  
        get_every_page_len();
    }
    txt_reader.txt_label=lv_label_create(obj);/*创建textarea*/
    lv_obj_set_width(txt_reader.txt_label,lv_pct(100));
    lv_obj_set_style_pad_all(txt_reader.txt_label, 0, 0);
    lv_label_set_long_mode(txt_reader.txt_label,LV_LABEL_LONG_WRAP);
    lv_obj_add_flag(txt_reader.txt_label,LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    /*0~3页长度*/
    if(txt_reader.total_page_count>=MAX_PAGE_NUM)
    {
        start_three_page_len=txt_reader.page_offsets[2] - txt_reader.page_offsets[0];
    }
    else
    {
         start_three_page_len=txt_reader.page_offsets[2] - txt_reader.page_offsets[0];
    }
    
        if(read_file_data(txt_reader_static_buf,start_three_page_len,0)==LV_FS_RES_OK)
        {
            txt_reader.cur_page_index=0;
             lv_label_set_text_static(txt_reader.txt_label,txt_reader_static_buf);  
        }
    lv_obj_add_event_cb(obj,event_close_file_txt_cb,LV_EVENT_DELETE,NULL);
    lv_obj_add_event_cb(obj,event_fle_txt_scroll,LV_EVENT_SCROLL_END,NULL);
}


static void get_every_page_len(void)
{
        char temp[4];
        uint32_t read_cnt;
        uint32_t current_pos = 0; // 当前探测到的绝对位置
        txt_reader.total_page_count = 0;
        txt_reader.page_offsets[0] = 0; // 第一页永远从 0 开始

        while (current_pos < txt_reader.file_size) {
        // 1. 拟定下一个跳跃点（往后跳 1024）
        uint32_t target_pos = current_pos + PAGE_SIZE;

        // 2. 边界检查：如果跳过头了，说明到最后一页了
        if (target_pos >= txt_reader.file_size) {
            current_pos = txt_reader.file_size; 
        } else {
            // 3. 核心探测：在 target_pos 处往回读 4 字节
            // 注意：这里要 seek 到 target_pos - 3，确保覆盖掉 target_pos 这个点
            lv_fs_seek(&txt_reader.file, target_pos - 3, LV_FS_SEEK_SET);
            lv_fs_read(&txt_reader.file, temp, 4, &read_cnt);

            // 使用你的 UTF8 检查函数算出需要回退几个字节
            uint8_t backstep = UTF8_Check_Truncation(temp, 4);
            
            // 得到修正后的、安全的下一页起点
            current_pos = target_pos - backstep;
        }
         printf("第%d页开始读的地址:%d\r\n",txt_reader.total_page_count,txt_reader.page_offsets[txt_reader.total_page_count]);
         
        // 4. 记录这一页的起点（绝对坐标）
        txt_reader.total_page_count++;
        if (txt_reader.total_page_count < MAX_PAGES) {
            txt_reader.page_offsets[txt_reader.total_page_count] = current_pos;
        } else {
            break; // 防止页数过多撑爆数组
        }
       
}

       
}
/**
 * @brief 检查 UTF-8 缓冲区末尾是否有截断的字符
 * @return 返回需要从末尾回退的字节数。0 表示末尾是完整字符。
 */
static uint8_t UTF8_Check_Truncation(const char *buf, uint32_t len) 
{
    if (len == 0) return 0;

    const unsigned char *p = (const unsigned char *)buf;

    for (uint8_t i = 1; i <= 4 && i <= len; i++)
    {
        unsigned char b = p[len - i];

        if (i == 1 && (b & 0x80) == 0) return 0;


        if ((b & 0xC0) == 0xC0) 
        {
            uint8_t expect;
            if      ((b & 0xE0) == 0xC0) expect = 2; // 2字节字符
            else if ((b & 0xF0) == 0xE0) expect = 3; // 3字节字符
            else if ((b & 0xF8) == 0xF0) expect = 4; // 4字节字符
            else return i; // 非法开头，建议回退
            return (i < expect) ? i : 0;
        }
    }
    return (len > 4) ? 4 : (uint8_t)len;
}
static void event_close_file_txt_cb(lv_event_t*e)
{
    if(txt_reader.file.drv!=NULL)
    {
        printf("关闭打开的文本文件\r\n");
        lv_fs_close(&txt_reader.file);
       memset(&txt_reader,0,sizeof(txt_reader_t));
       txt_reader.file.drv=NULL;   
    }
}

lv_fs_res_t read_file_data(char*read_buf,uint16_t read_len,uint32_t seek_index)
{
   uint32_t cnt=0; 
   lv_fs_res_t res;
   
    res=lv_fs_seek(&txt_reader.file,seek_index,LV_FS_SEEK_SET) ;
    if(res != LV_FS_RES_OK) return res;
    res= lv_fs_read(&txt_reader.file, read_buf, read_len, &cnt); /*读取文件*/
    if(res==LV_FS_RES_OK && cnt!=0)
    {
        read_buf[cnt]= '\0'; 
    }
    return res;
}

static void event_fle_txt_scroll(lv_event_t* e)
{
    static bool is_loading = false;
    if(is_loading) return;

    lv_obj_t* file_txt = lv_event_get_target(e);
    
    // 关键优化 1：如果总页数太少，根本不需要执行复杂的滑动窗口加载
    if(txt_reader.total_page_count <= 3) return;

    lv_coord_t top = lv_obj_get_scroll_top(file_txt);
    lv_coord_t bottom = lv_obj_get_scroll_bottom(file_txt);

    // --- 1. 向下滚动 ---
    if(bottom < 50) 
    {
        // 确保后面还有内容可以加载
        if(txt_reader.cur_page_index < txt_reader.total_page_count - 3) 
        {
            is_loading = true;

            lv_coord_t old_y = lv_obj_get_scroll_y(file_txt);
            lv_obj_t* label = txt_reader.txt_label;
            
            // 关键优化 2：精准估算。如果当前不到 3 页，按实际页数除
            uint16_t current_visible_pages = (txt_reader.cur_page_index + 3 <= txt_reader.total_page_count) ? 3 : (txt_reader.total_page_count - txt_reader.cur_page_index);
            lv_coord_t page_h_estimate = lv_obj_get_self_height(label) / current_visible_pages;

            txt_reader.cur_page_index++; 

            uint32_t start_pos = txt_reader.page_offsets[txt_reader.cur_page_index];
            uint32_t end_pos = txt_reader.page_offsets[txt_reader.cur_page_index + 3]; // 这里有刻度表保护
            uint32_t load_len = end_pos - start_pos;

            if(load_len < READ_BUF_SIZE && read_file_data(txt_reader_static_buf, load_len, start_pos) == LV_FS_RES_OK)
            {
                lv_label_set_text_static(label, txt_reader_static_buf);
                
                // 补偿并限位
                lv_coord_t target_y = old_y - page_h_estimate;
                lv_obj_scroll_to_y(file_txt, (target_y < 0) ? 0 : target_y, LV_ANIM_OFF);
            }
            is_loading = false;
        }
    }
    // --- 2. 向上滚动 ---
    else if(top < 50)
    {
        if(txt_reader.cur_page_index > 0)
        {
            is_loading = true;

            lv_coord_t old_y = lv_obj_get_scroll_y(file_txt);
            lv_obj_t* label = txt_reader.txt_label;
            lv_coord_t page_h_estimate = lv_obj_get_self_height(label) / 3;

            txt_reader.cur_page_index--;

            uint32_t start_pos = txt_reader.page_offsets[txt_reader.cur_page_index];
            uint32_t load_len = txt_reader.page_offsets[txt_reader.cur_page_index + 3] - start_pos;

            if(load_len < READ_BUF_SIZE && read_file_data(txt_reader_static_buf, load_len, start_pos) == LV_FS_RES_OK)
            {
                lv_label_set_text_static(label, txt_reader_static_buf);
                
                // 向上补偿：确保不会越过底部
                lv_obj_scroll_to_y(file_txt, old_y + page_h_estimate, LV_ANIM_OFF);
            }
            is_loading = false;
        }
    }
}


