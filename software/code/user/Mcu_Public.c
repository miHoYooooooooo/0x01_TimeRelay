
/**
 *FileName:    Mcu_Public.c
 *Date:        2026-06-30
 *Version:     1.0
 *Description: 公共文件
 */

 /****************************************************
                        头文件
*****************************************************/
#include <Mcu_Public.h>
#include <Bsp_SysTick.h>

/****************************************************
                        宏定义
*****************************************************/

/****************************************************
                      结构体定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/
extern volatile UINT32 sysTick_ms;

/* 全局打印等级 */
UINT8 printfLevel = PRT_ALL;

/****************************************************
                       函数声明
*****************************************************/

/****************************************************
                       函数定义
*****************************************************/
/**
  * @brief 获取当前Tick
  * @param[in]   void
  * @param[out]  void
  * @return 当前Tick
*/
UINT32 GetSysTick(void)
{
    return sysTick_ms;
}

/**
  * @brief 等待指定时间
  * @param[in]   wait       等待时间
  * @param[in]   start      开始时间
  * @param[in]   end        结束时间
  * @param[out]  void
  * @return 是否等待到
*/
BOOL WaitForTime(UINT32 wait, UINT32 start, UINT32 end)
{
    UINT32 diff = 0;

    diff = DIFF(start, end);

    if (wait <= diff)
    {
        return TRUE;
    }

    return FALSE;
}

/**
 * @brief   ms延时函数
 * @param[in]   ms          毫秒
 * @param[out]  void
 * @return  void
 */
void DelayMs(UINT32 ms)
{
    UINT32 tick = GetSysTick();

    while(1)
    {
        if (TRUE == WaitForTime(ms, tick, GetSysTick()))
        {
            break;
        }
    }
}

/**
  * @brief U32校验和
  * @param[in]   pData      数据
  * @param[in]   len        长度
  * @param[out]  void
  * @return 校验和
*/
UINT32 CheckSumU32(UINT8 *pData, UINT32 len)
{
    UINT32 i     = 0;
    UINT32 check = 0;

    for (i = 0; i < len; ++i)
    {
        check += pData[i];
    }

    return check;
}
