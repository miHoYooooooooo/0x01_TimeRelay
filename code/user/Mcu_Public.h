
/**
 *FileName:    Mcu_Public.h
 *Date:        2026-06-30
 *Author:      Author
 *Version:     1.0
 *Description: 公共文件
 */

#ifndef __MCU_PUBLIC_H__
#define __MCU_PUBLIC_H__

/****************************************************
                        头文件
*****************************************************/
#include "Mcu_Type.h"
#include <stdio.h>

/****************************************************
                        宏定义
*****************************************************/
/* 日志打印等级 */
#define     PRT_NONE                        0x00
#define     PRT_ERR                         0x01
#define     PRT_WARN                        0x02
#define     PRT_INFO                        0x03
#define     PRT_ALL                         0x04

/* 打印 */
#ifndef PRT
    #define PRT(_level_, format, ... ) \
        do { \
            if (_level_ <= printfLevel) \
            { \
                printf(format, ##__VA_ARGS__); \
            } \
        }while(0)
#endif

/****************************************************
                      结构体定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/
/* 全局打印等级 */
extern UINT8 printfLevel;

/****************************************************
                       函数声明
*****************************************************/
/**
  * @brief 获取当前Tick
  * @param[in]   void
  * @param[out]  void
  * @return 当前Tick
*/
UINT32 GetSysTick(void);

/**
  * @brief 等待指定时间
  * @param[in]   wait       等待时间
  * @param[in]   start      开始时间
  * @param[in]   end        结束时间
  * @param[out]  void
  * @return 是否等待到
*/
BOOL WaitForTime(UINT32 wait, UINT32 start, UINT32 end);

/**
 * @brief   ms延时函数
 * @param[in]   ms      毫秒
 * @param[out]  void
 * @return  void
 */
void DelayMs(UINT32 ms);

/****************************************************
                       函数定义
*****************************************************/

#endif
