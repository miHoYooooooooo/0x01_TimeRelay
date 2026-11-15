/**
  *FileName:    Mcu_State.h
  *Date:        2026-07-09
  *Author:      Author
  *Version:     1.0
  *Description: 状态处理
 */

#ifndef __MCU_STATE_H__
#define __MCU_STATE_H__

/****************************************************
                        头文件
*****************************************************/
#include "Mcu_Type.h"

/****************************************************
                        宏定义
*****************************************************/

/****************************************************
                      结构体定义
*****************************************************/
typedef struct _STATE_OBJECT_
{
    BOOL bEdit;             /* 编辑模式 */
    BOOL bRun;              /* 运行状态 */
} STATE_OBJECT;

/****************************************************
                       全局变量
*****************************************************/

/****************************************************
                       函数声明
*****************************************************/
/**
  * @brief 获取编辑模式状态
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
BOOL State_GetEdit(void);

/**
  * @brief 设置编辑模式
  * @param[in]   bState     编辑模式状态
  * @param[out]  void
  * @return void
*/
void State_SetEdit(BOOL bState);

/**
  * @brief 获取运行状态
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
BOOL State_GetRun(void);

/**
  * @brief 设置运行状态
  * @param[in]   bRun       运行状态
  * @param[out]  void
  * @return void
*/
void State_SetRun(BOOL bRun);

/**
  * @brief 状态轮询函数
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
void State_Loop(void);

/****************************************************
                       函数定义
*****************************************************/

#endif
