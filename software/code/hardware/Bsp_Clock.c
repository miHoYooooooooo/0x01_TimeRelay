/**
 *FileName:    Bsp_Clock.c
 *Date:        2026-06-30
 *Author:      Author
 *Version:     1.0
 *Description: Clk驱动文件
 */

/****************************************************
                        头文件
*****************************************************/
#include "Bsp_Clock.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "Mcu_Public.h"

/****************************************************
                        宏定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/

/****************************************************
                      结构体定义
*****************************************************/

/****************************************************
                       函数声明
*****************************************************/

/****************************************************
                       函数定义
*****************************************************/
/**
 * @brief 打印所有时钟频率
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void Clock_PrintFreq(void)
{
    UINT32 f_pll_sys  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    UINT32 f_pll_usb  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    UINT32 f_rosc     = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    UINT32 f_clk_sys  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    UINT32 f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    UINT32 f_clk_usb  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    UINT32 f_clk_adc  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    UINT32 f_clk_rtc  = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);

    PRT(PRT_INFO, " ------------------------ \r\n");
    PRT(PRT_INFO, "| pll_sys  = %7lu KHz | \r\n", f_pll_sys);
    PRT(PRT_INFO, "| pll_usb  = %7lu KHz | \r\n", f_pll_usb);
    PRT(PRT_INFO, "| rosc     = %7lu KHz | \r\n", f_rosc);
    PRT(PRT_INFO, "| clk_sys  = %7lu KHz | \r\n", f_clk_sys);
    PRT(PRT_INFO, "| clk_peri = %7lu KHz | \r\n", f_clk_peri);
    PRT(PRT_INFO, "| clk_usb  = %7lu KHz | \r\n", f_clk_usb);
    PRT(PRT_INFO, "| clk_adc  = %7lu KHz | \r\n", f_clk_adc);
    PRT(PRT_INFO, "| clk_rtc  = %7lu KHz | \r\n", f_clk_rtc);
    PRT(PRT_INFO, " ------------------------ \r\n");
}

/**
 * @brief 硬件初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
static void hardware_init(void)
{
    /* 先将系统时钟切换到USB时钟来避免宕机 */
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                    48 * MHZ,
                    48 * MHZ);

    /* 重新设置PLL */
    pll_deinit(pll_sys);
    pll_init(pll_sys, 1, 1500 * MHZ, 3, 2);

    /* 修改系统和外设时钟 */
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                    250 * MHZ,
                    250 * MHZ);

    clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    250 * MHZ,
                    250 * MHZ);
}

/**
 * @brief 时钟初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void Clock_Init(void)
{
    /* 硬件初始化 */
    hardware_init();
}
