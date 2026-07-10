/**
  *FileName:    Bsp_Key.h
  *Date:        2026-07-05
  *Author:      Author
  *Version:     1.0
  *Description: Key驱动文件
 */

#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

/****************************************************
                        头文件
*****************************************************/
#include "Mcu_Type.h"

/****************************************************
                        宏定义
*****************************************************/
#define     BSP_KEY_UP_PIN              (15U)
#define     BSP_KEY_DOWN_PIN            (16U)
#define     BSP_KEY_ENTER_PIN           (17U)

/****************************************************
                      结构体定义
*****************************************************/
/* 按键类型枚举 */
typedef enum _KEY_TYPE_
{
    KEY_TYPE_NULL   = 0,

    KEY_TYPE_UP     = 1,
    KEY_TYPE_DOWN   = 2,
    KEY_TYPE_ENTER  = 3,

    KEY_TYPE_MAX_   = 0xFFFFFFFF
} KEY_TYPE;

/* 按键结构体 */
typedef struct _KEY_OBJECT_
{
    UINT32   lastTick;              /* 时间戳 */
    KEY_TYPE lastType;              /* 类型   */

    KEY_TYPE pressingType;          /* 按住的类型 */
    BOOL     bPassing;              /* 是否正在按住 */

    UINT32   pressedTick;           /* 按下的时间 */
    KEY_TYPE pressedType;           /* 按下的类型 */
} KEY_OBJECT;


/****************************************************
                       全局变量
*****************************************************/

/****************************************************
                       函数声明
*****************************************************/
/**
  * @brief 按键按下
  * @param[in]   type   按键类型
  * @param[out]  void
  * @return void
*/
void Key_Press(KEY_TYPE type);

/**
  * @brief 按键释放
  * @param[in]   type   按键类型
  * @param[out]  void
  * @return void
*/
void Key_Release(KEY_TYPE type);

/**
  * @brief 立刻获取按键类型
  * @param[in]   void
  * @param[out]  void
  * @return 按键类型
*/
KEY_TYPE Key_GetTypeImmediately(void);

/**
  * @brief 获取按键类型
  * @param[in]   void
  * @param[out]  void
  * @return 按键类型
*/
KEY_TYPE Key_GetType(void);

/**
  * @brief 获取按下的持续时间
  * @param[in]   void
  * @param[out]  void
  * @return 时间
*/
UINT32 Key_GetPressedTime(void);

/**
  * @brief 清空按下的持续时间
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
void Key_ClearPressedTime(void);

/**
  * @brief 获取是否正在按住
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
BOOL Key_IsPressing(void);

/**
 * @brief Key初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void Key_Init(void);

/****************************************************
                       函数定义
*****************************************************/


#endif
