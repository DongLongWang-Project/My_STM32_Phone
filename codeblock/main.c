#include <stdlib.h>
#include <unistd.h>

#include "lvgl/lvgl.h"
#include "lv_drivers/win32drv/win32drv.h"
#include <windows.h>

 #include "../my_gui/ui_main.h"

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow){

    system("chcp 65001");
    lv_init();

    lv_win32_init(hInstance, SW_SHOWNORMAL, 240, 320, NULL);

    LV_LOG_USER("LVGL initialization completed!");
    ui_init();
    while(!lv_win32_quit_signal) {
        lv_task_handler();
        usleep(10000);       /*Just to let the system breath*/
    }
    return 0;
}

