/**
 *FileName:    Bsp_Key.c
 *Date:        2026-07-05
 *Author:      Author
 *Version:     1.0
 *Description: Key驱动文件
 */

/****************************************************
                        头文件
*****************************************************/
#include "Bsp_Key.h"
#include "hardware/gpio.h"

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
/**
 * @brief 硬件初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
static void hardware_init(void)
{
    /* 初始化引脚 */
    gpio_init(BSP_KEY_UP_PIN);
    gpio_init(BSP_KEY_DOWN_PIN);
    gpio_init(BSP_KEY_LEFT_PIN);
    gpio_init(BSP_KEY_RIGHT_PIN);
}

/**
 * @brief Key初始化
 * @param[in]   void
 * @param[out]  void
 * @return UINT32   Key枚举
 */
UINT32 Key_Read(void)
{
    if (0 == gpio_get(BSP_KEY_UP_PIN))
    {
        return 3;
    }
    else if (0 == gpio_get(BSP_KEY_DOWN_PIN))
    {
        return 4;
    }
    else if (0 == gpio_get(BSP_KEY_LEFT_PIN))
    {
        return 5;
    }
    else if (0 == gpio_get(BSP_KEY_RIGHT_PIN))
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Key初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void Key_Init(void)
{
    /* 硬件初始化 */
    hardware_init();
}
