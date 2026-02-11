/**
 * @file lv_fs_fatfs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_FS_FATFS
#include "ff.h"
#include "diskio.h"
/*********************
 *      DEFINES
 *********************/

#if LV_FS_FATFS_LETTER == '\0'
    #error "LV_FS_FATFS_LETTER must be an upper case ASCII letter"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_fatfs_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    /*Add a simple drive to open images*/
    static lv_fs_drv_t fs_drv; /*A driver descriptor*/
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.letter = LV_FS_FATFS_LETTER;
    fs_drv.cache_size = LV_FS_FATFS_CACHE_SIZE;

    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your Storage device and File system.*/
static void fs_init(void)
{
    static FATFS fs;
    FRESULT res;

    // 1. 先强行卸载（不管之前有没有挂载成功）
    f_mount(NULL, "0:", 0);
    
    // 2. 彻底复位底层 SDIO 硬件和 DMA
    SD_LowLevel_DeInit(); 
    Delay_ms(100);       // 给电容放电时间
    
    // 3. 重新初始化硬件
    if(SD_Init() != SD_OK) {
        printf("SD Card Hardware Init Failed!\r\n");
        return;
    }

    // 4. 重新挂载文件系统
    res = f_mount(&fs, "0:", 1);
    
    if(res != FR_OK) {
        printf("LVGL fatfs mount failed: %d\r\n", res);
    } else {
        printf("LVGL fatfs mount succeed!\r\n");  
    }
}

/**
 * Open a file
 * @param drv pointer to a driver where this function belongs
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return pointer to FIL struct or NULL in case of fail
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    uint8_t flags = 0;

    if(mode == LV_FS_MODE_WR) flags = FA_WRITE | FA_OPEN_ALWAYS;
    else if(mode == LV_FS_MODE_RD) flags = FA_READ;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;

//     char *real_path=lv_mem_alloc(256);
//     if(real_path==NULL) return NULL;
//     lv_snprintf(real_path,256,LV_FS_FATFS_PATH"%s",path);
    
    FIL * f = lv_mem_alloc(sizeof(FIL));
    if(f == NULL) return NULL;

    FRESULT res = f_open(f, path, flags);
    if(res == FR_OK) {
        return f;
    }
    else {
        lv_mem_free(f);
        return NULL;
    }
}

/**
 * Close an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FIL variable. (opened with fs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    f_close(file_p);
    lv_mem_free(file_p);
    return LV_FS_RES_OK;
}

#if 0
// 在文件开头定义一个 512 字节的静态中转缓冲区
// 必须确保它不在 CCM RAM 中（默认的全局变量通常在 SRAM）
#define SD_DMA_BRIDGE_SIZE  1024
static uint8_t g_sd_bridge[SD_DMA_BRIDGE_SIZE] __attribute__((aligned(4))); 
#include "USART1.h"

static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    FRESULT res = FR_OK;
    *br = 0; // 初始化已读取字节数
    print("buf地址:0x%x\r\n",(uint32_t)buf);
    print("g_sd_bridge地址:0x%x\r\n",(uint32_t)g_sd_bridge);
    // 关键判断：目标缓冲区 buf 是否落在 CCM RAM 区域 (0x10000000 开头)
    if (((uint32_t)buf & 0xF0000000) == 0x10000000) 
    {
        uint32_t bytes_to_read = btr;
        uint32_t total_read = 0;
        UINT current_br = 0;
        print("btr:%d\r\n",btr);
        while (bytes_to_read > 0) 
        {
            // 每次最多读取 512 字节（即中转站的大小）
            uint32_t chunk = (bytes_to_read > SD_DMA_BRIDGE_SIZE) ? SD_DMA_BRIDGE_SIZE : bytes_to_read;
            
             taskENTER_CRITICAL();           /* 进入临界区 */
               
            res = f_read((FIL*)file_p, g_sd_bridge, chunk, &current_br);
            
            taskEXIT_CRITICAL();            /* 退出临界区 */
            
             print("res:%d\r\n",res);
             print("current_br:%d\r\n",current_br);
            if (res != FR_OK) return LV_FS_RES_UNKNOWN;
            if (current_br == 0) break; // 读取结束（到达文件末尾）

            // CPU 搬运：从 SRAM 中转站拷贝到 CCM 目标地址
            memcpy((uint8_t*)buf + total_read, g_sd_bridge, current_br);
            
            total_read += current_br;
            bytes_to_read -= current_br;
        }
        *br = total_read;
        return LV_FS_RES_OK;
    }
    else 
    {
        // 【原生路径】
        // 如果地址在普通 SRAM (0x20000000)，直接调用 FatFs 走 DMA 极速读取
        
        res = f_read((FIL*)file_p, buf, btr, (UINT *)br);
        
        if(res == FR_OK) return LV_FS_RES_OK;
        else return LV_FS_RES_UNKNOWN;
    }
}

static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    FRESULT res = FR_OK;
    *bw = 0;

    // 检查源数据 buf 是否在 CCM RAM (0x10000000)
    if (((uint32_t)buf & 0xF0000000) == 0x10000000) 
    {
        uint32_t bytes_to_write = btw;
        uint32_t total_written = 0;
        UINT current_bw = 0;

        // 【分段写入逻辑】
        while (bytes_to_write > 0) 
        {
            uint32_t chunk = (bytes_to_write > SD_DMA_BRIDGE_SIZE) ? SD_DMA_BRIDGE_SIZE : bytes_to_write;

            // 1. CPU 搬运：从 CCM 搬到 SRAM 中转站
            memcpy(g_sd_bridge, (uint8_t*)buf + total_written, chunk);

            taskENTER_CRITICAL();           /* 进入临界区 */
            // 2. 让 FatFs 从 SRAM 中转站写入 SD 卡 (DMA 可正常访问)
            res = f_write((FIL*)file_p, g_sd_bridge, chunk, &current_bw);

            taskEXIT_CRITICAL();            /* 退出临界区 */
            
            if (res != FR_OK) return LV_FS_RES_UNKNOWN;
            if (current_bw == 0) break; // 磁盘可能已满

            total_written += current_bw;
            bytes_to_write -= current_bw;
        }
        *bw = total_written;
        return LV_FS_RES_OK;
    }
    else 
    {
        // 原生路径：buf 已经在 SRAM，直接调用 FatFs 写入
        res = f_write((FIL*)file_p, buf, btw, (UINT *)bw);
        
        if(res == FR_OK) return LV_FS_RES_OK;
        else return LV_FS_RES_UNKNOWN;
    }
}

#else
/**
 * Read data from an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FIL variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    FRESULT res = f_read(file_p, buf, btr, (UINT *)br);
    if(res == FR_OK) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

/**
 * Write into a file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FIL variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btw Bytes To Write
 * @param bw the number of real written bytes (Bytes Written). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    FRESULT res = f_write(file_p, buf, btw, (UINT *)bw);
    if(res == FR_OK) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

#endif




/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FIL variable. (opened with fs_open )
 * @param pos the new position of read write pointer
 * @param whence only LV_SEEK_SET is supported
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    switch(whence) {
        case LV_FS_SEEK_SET:
            f_lseek(file_p, pos);
            break;
        case LV_FS_SEEK_CUR:
            f_lseek(file_p, f_tell((FIL *)file_p) + pos);
            break;
        case LV_FS_SEEK_END:
            f_lseek(file_p, f_size((FIL *)file_p) + pos);
            break;
        default:
            break;
    }
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FIL variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);
    *pos_p = f_tell((FIL *)file_p);
    return LV_FS_RES_OK;
}

/**
 * Initialize a 'DIR' variable for directory reading
 * @param drv pointer to a driver where this function belongs
 * @param path path to a directory
 * @return pointer to an initialized 'DIR' variable
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    LV_UNUSED(drv);
    DIR * d = lv_mem_alloc(sizeof(DIR));
    if(d == NULL) return NULL;

//     char *real_path=lv_mem_alloc(256);
//     if(real_path==NULL) return NULL;
//     lv_snprintf(real_path,256,LV_FS_FATFS_PATH"%s",path);

    FRESULT res = f_opendir(d, path);
    if(res != FR_OK) {
        lv_mem_free(d);
        d = NULL;
    }
    return d;
}

/**
 * Read the next filename from a directory.
 * The name of the directories will begin with '/'
 * @param drv pointer to a driver where this function belongs
 * @param dir_p pointer to an initialized 'DIR' variable
 * @param fn pointer to a buffer to store the filename
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn)
{
    LV_UNUSED(drv);
    FRESULT res;
    FILINFO fno;
    fn[0] = '\0';

    do {
        res = f_readdir(dir_p, &fno);
        if(res != FR_OK) return LV_FS_RES_UNKNOWN;

        if(fno.fattrib & AM_DIR) {
            fn[0] = '/';
            strcpy(&fn[1], fno.fname);
        }
        else strcpy(fn, fno.fname);

    } while(strcmp(fn, "/.") == 0 || strcmp(fn, "/..") == 0);

    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param drv pointer to a driver where this function belongs
 * @param dir_p pointer to an initialized 'DIR' variable
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p)
{
    LV_UNUSED(drv);
    f_closedir(dir_p);
    lv_mem_free(dir_p);
    return LV_FS_RES_OK;
}

#else /*LV_USE_FS_FATFS == 0*/

#if defined(LV_FS_FATFS_LETTER) && LV_FS_FATFS_LETTER != '\0'
    #warning "LV_USE_FS_FATFS is not enabled but LV_FS_FATFS_LETTER is set"
#endif

#endif /*LV_USE_FS_POSIX*/

