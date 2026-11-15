/**
 *FileName:    Mcu_State.c
 *Date:        2026-07-09
 *Author:      Author
 *Version:     1.0
 *Description: 状态处理
 */

/****************************************************
                        头文件
*****************************************************/
#include "Mcu_State.h"
#include "Bsp_Key.h"

/****************************************************
                        宏定义
*****************************************************/

/****************************************************
                      结构体定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/
static STATE_OBJECT stateObj = {0};

/****************************************************
                       函数声明
*****************************************************/

/****************************************************
                       函数定义
*****************************************************/
/**
  * @brief 获取编辑模式状态
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
BOOL State_GetEdit(void)
{
    return stateObj.bEdit;
}

/**
  * @brief 设置编辑模式
  * @param[in]   bState     编辑模式状态
  * @param[out]  void
  * @return void
*/
void State_SetEdit(BOOL bState)
{
    stateObj.bEdit = bState;
}

/**
  * @brief 获取运行状态
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
BOOL State_GetRun(void)
{
    return stateObj.bRun;
}

/**
  * @brief 设置运行状态
  * @param[in]   bRun       运行状态
  * @param[out]  void
  * @return void
*/
void State_SetRun(BOOL bRun)
{
    stateObj.bRun = bRun;
}

/**
  * @brief 状态轮询函数
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
void State_Loop(void)
{
    if (KEY_TYPE_RUN == Key_GetType())
    {
        /* 运行状态切换 */
        State_SetRun(!State_GetRun());

        /* 清空按键，防止一直按住 */
        Key_ClearType();
    }
}
