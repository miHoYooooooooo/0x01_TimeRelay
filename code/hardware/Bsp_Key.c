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
#include <string.h>
#include "Mcu_Public.h"

/****************************************************
                        宏定义
*****************************************************/

/****************************************************
                      结构体定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/
static KEY_OBJECT keyObj = {0};

/****************************************************
                       函数声明
*****************************************************/
extern void Irq_GpioHandler(uint gpio, uint32_t events);

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
    gpio_init(BSP_KEY_ENTER_PIN);
    /* 设置外部中断 */
    gpio_set_irq_enabled_with_callback(BSP_KEY_UP_PIN,    GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &Irq_GpioHandler);
    gpio_set_irq_enabled_with_callback(BSP_KEY_DOWN_PIN,  GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &Irq_GpioHandler);
    gpio_set_irq_enabled_with_callback(BSP_KEY_ENTER_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &Irq_GpioHandler);
}

/**
  * @brief 按键按下
  * @param[in]   type   按键类型
  * @param[out]  void
  * @return void
*/
void Key_Press(KEY_TYPE type)
{
    memset(&keyObj, 0, sizeof(KEY_OBJECT));

    keyObj.pressingType = type;
    keyObj.lastTick     = GetSysTick();
}

/**
  * @brief 按键释放
  * @param[in]   type   按键类型
  * @param[out]  void
  * @return void
*/
void Key_Release(KEY_TYPE type)
{
    if (keyObj.pressingType == type)
    {
        /* 计算按下的时间 */
        keyObj.pressedTick = DIFF(keyObj.lastTick, GetSysTick());

        /* 更新类型 */
        keyObj.pressingType = KEY_TYPE_NULL;
        keyObj.pressedType  = type;
    }
    else
    {
        memset(&keyObj, 0, sizeof(KEY_OBJECT));
    }
}

/**
  * @brief 立刻获取按键类型
  * @param[in]   void
  * @param[out]  void
  * @return 按键类型
*/
KEY_TYPE Key_GetTypeImmediately(void)
{
    /* 直接判断端口 */
    if (0 == gpio_get(BSP_KEY_UP_PIN))
    {
        return KEY_TYPE_UP;
    }
    else if (0 == gpio_get(BSP_KEY_DOWN_PIN))
    {
        return KEY_TYPE_DOWN;
    }
    else if (0 == gpio_get(BSP_KEY_ENTER_PIN))
    {
        return KEY_TYPE_ENTER;
    }

    return KEY_TYPE_NULL;
}

/**
  * @brief 获取按键类型
  * @param[in]   void
  * @param[out]  void
  * @return 按键类型
*/
KEY_TYPE Key_GetType(void)
{
    KEY_TYPE type = KEY_TYPE_NULL;

    /* 按住 */
    if (KEY_TYPE_NULL != keyObj.pressingType)
    {
        type = keyObj.pressingType;
    }
    /* 按下 */
    else if (KEY_TYPE_NULL != keyObj.pressedType)
    {
        type = keyObj.pressedType;

        keyObj.pressedType = KEY_TYPE_NULL;
    }
    else
    {
        type = KEY_TYPE_NULL;
    }

    return type;
}

/**
  * @brief 获取按下的持续时间
  * @param[in]   void
  * @param[out]  void
  * @return 时间
*/
UINT32 Key_GetPressedTime(void)
{
    /* 有按键在按住or按下 */
    if ((KEY_TYPE_NULL != keyObj.pressingType)
     || (KEY_TYPE_NULL != keyObj.pressedType))
    {
        keyObj.pressedTick = DIFF(keyObj.lastTick, GetSysTick());

        return keyObj.pressedTick;
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
