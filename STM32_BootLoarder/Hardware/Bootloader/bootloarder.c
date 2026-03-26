#include "Bootloarder.h"



typedef void (*iapfun)(void);
iapfun jump2app;

void load_app(u32 appxaddr)
{
    // 1. 检查栈顶地址是否合法 (RAM范围)
    if(((*(vu32*)appxaddr) & 0x2FF00000) == 0x20000000)
    {
        // 2. 关闭全局中断，防止跳转瞬间触发中断
        __disable_irq();

        // 3. 获取 APP 的复位地址 (偏移 4 字节)
        jump2app = (iapfun)*(vu32*)(appxaddr + 4);

        // 4. 设置主栈指针 (MSP)
        // 使用 CMSIS 标准函数，比手动写汇编更安全且具可移植性
        __set_MSP(*(vu32*)appxaddr);

        // 5. 跳转
        jump2app();
    }
}