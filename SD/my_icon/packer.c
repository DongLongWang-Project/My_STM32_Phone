#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>  // Windows 下遍历文件专用库

#pragma pack(1)
typedef struct {
    char magic[4];
    unsigned short width;
    unsigned short height;
    unsigned short fps;
    unsigned short loop;
    char reserve[4];
} MyHeader;
#pragma pack()

void process_file(char* filename) {
    char cmd[512];
    char out_name[256];
    int w = 0, h = 0;
    
    // 1. 获取文件名（去掉后缀）
    strcpy(out_name, filename);
    char *dot = strrchr(out_name, '.');
    if(dot) *dot = '\0';
    strcat(out_name, ".dat");

    // 2. 自动获取原图分辨率 (通过 ffprobe)
    sprintf(cmd, "ffprobe -v error -select_streams v:0 -show_entries stream=width,height -of csv=s=x:p=0 \"%s\"", filename);
    FILE *fp = _popen(cmd, "r");
    if (fp) {
        if (fscanf(fp, "%dx%d", &w, &h) != 2) { _pclose(fp); return; }
        _pclose(fp);
    }

    printf("--> 正在处理: [%s] 分辨率: %dx%d\n", filename, w, h);

    // 3. 调用 FFmpeg 转换为原始 RGB565le
    sprintf(cmd, "ffmpeg -i \"%s\" -f rawvideo -pix_fmt rgb565le -y \"temp.raw\" >nul 2>&1", filename);
    system(cmd);

    // 4. 封装文件头
    MyHeader head = {"RGB!", (unsigned short)w, (unsigned short)h, 0, 1, {0}};
    
    FILE *f_raw = fopen("temp.raw", "rb");
    FILE *f_out = fopen(out_name, "wb");
    if (f_raw && f_out) {
        fwrite(&head, 1, sizeof(MyHeader), f_out); // 写16字节头
        unsigned char buf[8192];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), f_raw)) > 0) fwrite(buf, 1, n, f_out);
        fclose(f_raw);
        fclose(f_out);
        printf("    [OK] 已生成: %s\n", out_name);
    }
    if(f_raw) fclose(f_raw);
    remove("temp.raw");
}

int main() {
    struct _finddata_t file_info;
    intptr_t handle;

    // 扫描所有 png 和 jpg
    char* targets[] = {"*.png", "*.jpg", "*.bmp"};
    for(int i=0; i<3; i++) {
        handle = _findfirst(targets[i], &file_info);
        if (handle != -1) {
            do {
                process_file(file_info.name);
            } while (_findnext(handle, &file_info) == 0);
            _findclose(handle);
        }
    }

    printf("\n所有文件处理完毕！按任意键退出...\n");
    getchar();
    return 0;
}