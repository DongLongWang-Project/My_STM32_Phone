#include "flash.h"


/**
  * 函    数：FLASH读取一个32位的字
  * 参    数：Address 要读取数据的字地址
  * 返 回 值：指定地址下的数据
  */
uint32_t MyFLASH_ReadWord(uint32_t Address)
{
	return *((__IO uint32_t *)(Address));	//使用指针访问指定地址下的数据并返回
}

/**
  * 函    数：FLASH读取一个16位的半字
  * 参    数：Address 要读取数据的半字地址
  * 返 回 值：指定地址下的数据
  */
uint16_t MyFLASH_ReadHalfWord(uint32_t Address)
{
	return *((__IO uint16_t *)(Address));	//使用指针访问指定地址下的数据并返回
}

/**
  * 函    数：FLASH读取一个8位的字节
  * 参    数：Address 要读取数据的字节地址
  * 返 回 值：指定地址下的数据
  */
uint8_t MyFLASH_ReadByte(uint32_t Address)
{
	return *((__IO uint8_t *)(Address));	//使用指针访问指定地址下的数据并返回
}


void myFLASH_ReadData(uint32_t Address, void* buf, uint32_t len)
{
    // 1. 在内部定义一个字节指针，方便进行偏移计算
    uint8_t* pDest = (uint8_t*)buf; 
    
    uint32_t i = 0;
    uint32_t word_limit = (len / 4) * 4;

    // 2. 4 字节对齐读取
    for (i = 0; i < word_limit; i += 4)
    {
        // 这里的转换逻辑：先取偏移后的地址，转为 uint32_t 指针，再解引用赋值
        *(uint32_t*)(pDest + i) = MyFLASH_ReadWord(Address + i);
    }

    // 3. 剩余字节读取
    for (; i < len; i++)
    {
        pDest[i] = MyFLASH_ReadByte(Address + i);
    }
}