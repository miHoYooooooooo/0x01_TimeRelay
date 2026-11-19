/**
 *FileName:    Bsp_SysTick.c
 *Date:        2026-06-30
 *Author:      Author
 *Version:     1.0
 *Description: SysTick驱动文件
 */

/****************************************************
                        头文件
*****************************************************/
#include "Bsp_SysTick.h"
#include "hardware/clocks.h"
#include "hardware/structs/systick.h"

/****************************************************
                        宏定义
*****************************************************/

/****************************************************
                      结构体定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/

/****************************************************
                       函数声明
*****************************************************/
volatile UINT32 sysTick_ms = 0;

/****************************************************
                       函数定义
*****************************************************/
/**
 * @brief 硬件初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
static void hardware_init(void)
{
    UINT32 arr = 0;

    /* 时钟源: 系统时钟 */
    systick_hw->csr |= (1u << 2);

    /* 计算ARR */
    arr = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS) - 1;

    /* 设置ARR */
    systick_hw->rvr =  arr;

    /* 设置当前值 */
    systick_hw->cvr = arr;

    /* 开启中断 */
    systick_hw->csr |= (1u << 1);

    /* 启动SysTick */
    systick_hw->csr |= (1u << 0);
}

/**
 * @brief SysTick初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void SysTick_Init(void)
{
    /* 硬件初始化 */
    hardware_init();
}
