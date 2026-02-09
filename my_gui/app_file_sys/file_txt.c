/*--------------------------------------------------------------------------------↓
	@项目	: 文本文件的处理
	@日期	:  2026-1-25
	@备注	:
↑--------------------------------------------------------------------------------*/


#include "file_txt.h"

#define FILE_TXT_OPERATION LV_FS_MODE_RD


/*--------------------------------------------------------------------------------↓
	@函数	  : 操作文本文件
	@参数	  : 父对象 路径
	@返回值 : 无
	@备注	  :
↑--------------------------------------------------------------------------------*/
void operation_file_txt(lv_obj_t*parent,const char*path)
{
    
    lv_fs_file_t file;/*文件*/
    lv_fs_res_t res = lv_fs_open(&file, path, FILE_TXT_OPERATION); /*打开文件的状态*/

    lv_obj_t *file_txt=lv_textarea_create(parent);/*创建textarea*/
    lv_obj_set_size(file_txt, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(file_txt, 0, 0);

    lv_obj_set_style_shadow_opa (file_txt, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(file_txt,0,LV_PART_MAIN);/**边框宽度0 **/
    lv_obj_add_event_cb(file_txt,event_textarea_cb,LV_EVENT_CLICKED,NULL); /*textarea的点击事件*/

    if(res == LV_FS_RES_OK)/*打开文件状态*/
    {

      char buf[256];
      uint32_t cnt=0;
      while(1)
      {
        memset(buf,0,sizeof(buf));
        res = lv_fs_read(&file, buf, sizeof(buf)-1, &cnt); /*读取文件*/
        if(res != LV_FS_RES_OK || cnt == 0) break;
        else
        {
          buf[cnt] = '\0';
          lv_textarea_add_text(file_txt, buf); /*将读到的内容添加到textarea*/
          if(cnt < sizeof(buf)-1) break;
        }
      }
    }
    lv_fs_close(&file); /*关闭文件*/   
}
