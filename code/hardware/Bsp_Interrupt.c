/**
 *FileName:    Bsp_Interrupt.c
 *Date:        2026-06-30
 *Author:      Author
 *Version:     1.0
 *Description: 中断处理文件
 */

/****************************************************
                        头文件
*****************************************************/
#include "hardware/dma.h"
#include "hardware/pwm.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "Bsp_Led.h"

/****************************************************
                        宏定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/
/* SysTick */
extern UINT32 sysTick_ms;
/* 获取DMA通道 */
extern UINT32 ST7789_GetDmaChan(void);

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
 * @brief 检查PWM中断状态
 * @param[in]   gpio    引脚号
 * @param[out]  void
 * @return bool 是否中断
 */
static BOOL pwm_check_irq(UINT32 gpio)
{
    /* 获取中断掩码后偏移对应GPIO的中断片 */
    return (BOOL)((pwm_get_irq_status_mask() >> pwm_gpio_to_slice_num(gpio)) & 0x00000001);
}

/**
 * @brief SysTick中断
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void isr_systick()
{
    sysTick_ms++;

    lv_tick_inc(1);
}

/**
 * @brief Dma中断回调
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void Irq_DmaHandler(void)
{
    /* LCD */
    if (dma_hw->ints0 & (1u << ST7789_GetDmaChan()))
    {
        /* 清除DMA通道中断标志位 */
        dma_hw->ints0 = 1u << ST7789_GetDmaChan();

        /* lvgl刷新完成 */
        lv_disp_ready();
    }
}

/**
 * @brief PWM中断回调
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void Irq_PwmHandler(void)
{
    UINT32 mask = pwm_get_irq_status_mask();    /* 掩码 */
    UINT32 sliceNum  = 0;                       /* PWM片 */
    static INT32 count = 0;                     /* 比较值 */
    static BOOL  bGoingUp  = TRUE;              /* 向上计数标志位 */

    /* LED */
    sliceNum = pwm_gpio_to_slice_num(LED_PIN);
    if (TRUE == (mask >> sliceNum))
    {
        /* 清除中断标志 */
        pwm_clear_irq(sliceNum);

        /* PWM更新需要先关闭 */
        // pwm_set_enabled(sliceNum, FALSE);

        /* 判断方向 */
        if (TRUE == bGoingUp)
        {
            ++count;

            if (100 <= count)
            {
                count = 100;
                bGoingUp = FALSE;
            }
        }
        else
        {
            --count;

            if (count <= 0)
            {
                count = 0;
                bGoingUp = TRUE;
            }
        }

        /* 将淡入淡出值平方以使LED亮度看起来更线性 */
        pwm_set_gpio_level(LED_PIN, count * count);

        /* PWM更新后再打开 */
        // pwm_set_enabled(sliceNum, TRUE);
    }
}
