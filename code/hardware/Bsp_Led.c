/**
 *FileName:    Bsp_Led.c
 *Date:        2026-06-30
 *Author:      Author
 *Version:     1.0
 *Description: LED驱动文件(PWM)
 */

/****************************************************
                        头文件
*****************************************************/
#include "Bsp_Led.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

/****************************************************
                        宏定义
*****************************************************/

/****************************************************
                      结构体定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/
/* PWM片 */
static UINT32 sliceNum = 0;

/****************************************************
                       函数声明
*****************************************************/
extern void Irq_PwmHandler(void);

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
    pwm_config config = {0};            /* PWM配置 */

    /**************** GPIO ****************/
    /* 初始化LED引脚 */
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);

    /**************** PWM  ****************/
    /* 获取引脚对应的PWM片 */
    sliceNum = pwm_gpio_to_slice_num(LED_PIN);

    /* 清空LED引脚对应PWM片的中断 */
    pwm_clear_irq(sliceNum);
    /* 使能PWM中断 */
    pwm_set_irq_enabled(sliceNum, TRUE);
    /* 绑定中断回调函数 */
    irq_set_exclusive_handler(PWM_IRQ_WRAP, Irq_PwmHandler);
    /* 使能中断 */
    irq_set_enabled(PWM_IRQ_WRAP, TRUE);

    /* 为PWM配置获取缺省值 */
    config = pwm_get_default_config();
    /* 设置最大值为 100 * 100 */
    pwm_config_set_wrap(&config, 100 * 100);
    /* 设置分频器 */
    pwm_config_set_clkdiv(&config, (float)frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS) / 1000.0f);

    /* 初始化PWM并运行 */
    pwm_init(sliceNum, &config, TRUE);
}

/**
 * @brief LED初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void Led_Init(void)
{
    /* 硬件初始化 */
    hardware_init();
}
