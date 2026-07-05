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
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "gg_utils.h"
#include "custom.h"
#include "Bsp_Clock.h"
#include "Bsp_Led.h"
#include "Bsp_SysTick.h"

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

/****************************************************
                       函数定义
*****************************************************/
int main()
{
    gg_ui_t guider_ui;

    /* 时钟初始化 */
    Clock_Init();
    /* SysTick初始化 */
    SysTick_Init();

    stdio_init_all();

    /* LED初始化 */
    Led_Init();

    /* 打印时钟信息 */
    Clock_PrintFreq();

    /* lvgl初始化 */
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    setup_ui(&guider_ui);
    custom_init(&guider_ui);

    while (TRUE)
    {
        lv_timer_handler();
    }
}
