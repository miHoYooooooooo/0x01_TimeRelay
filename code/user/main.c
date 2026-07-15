/**
 *FileName:    main.c
 *Date:        2025-06-15
 *Author:      Author
 *Version:     1.0
 *Description: main函数
 */

/****************************************************
                        头文件
*****************************************************/
#include "pico/stdlib.h"
#include "pico/flash.h"
#include "pico/multicore.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "gg_utils.h"
#include "custom.h"
#include "Bsp_Clock.h"
#include "Bsp_Led.h"
#include "Bsp_SysTick.h"
#include "Mcu_Param.h"

/****************************************************
                        宏定义
*****************************************************/
#define     PICO_USE_MULTICORE              (1U)        /* 是否使用多核 */

/****************************************************
                      结构体定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/
gg_ui_t guider_ui;

/****************************************************
                       函数声明
*****************************************************/

/****************************************************
                       函数定义
*****************************************************/
#if  PICO_USE_MULTICORE
/**
  * @brief 核1入口函数
  * @param[in]   void
  * @param[out]  void
  * @return 参数指针
*/
void core1_entry(void)
{
    /* Flash多核操作安全初始化 */
    flash_safe_execute_core_init();

    while (TRUE)
    {
        lv_timer_handler();
    }
}
#endif

/**
  * @brief 主函数
  * @param[in]   void
  * @param[out]  void
  * @return 参数指针
*/
int main(void)
{
    /* 时钟初始化 */
    Clock_Init();
    /* SysTick初始化 */
    SysTick_Init();

    stdio_init_all();

    /* LED初始化 */
    Led_Init();

    /* 参数初始化 */
    Param_Init();

    /* 打印时钟信息 */
    Clock_PrintFreq();

    /* lvgl初始化 */
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    /* UI初始化 */
    setup_ui(&guider_ui);
    custom_init(&guider_ui);

#if  PICO_USE_MULTICORE
    /* Flash多核操作安全初始化 */
    flash_safe_execute_core_init();
    /* 启动多核 */
    multicore_launch_core1(core1_entry);
#endif

    while (TRUE)
    {
    #if  !PICO_USE_MULTICORE
        lv_timer_handler();
    #endif
    }
}
