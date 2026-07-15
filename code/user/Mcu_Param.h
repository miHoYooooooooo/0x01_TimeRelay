/**
  *FileName:    Mcu_Param.h
  *Date:        2026-07-14
  *Author:      Author
  *Version:     1.0
  *Description: 参数
 */

#ifndef __MCU_PARAM_H__
#define __MCU_PARAM_H__

/****************************************************
                        头文件
*****************************************************/
#include "Mcu_Type.h"
#include "Bsp_Flash.h"
#include "Bsp_Relay.h"

/****************************************************
                        宏定义
*****************************************************/
#define     MCU_PARAM_ADDR                          (0x001F6000U)
#define     MCU_PARAM_LEN                           (BSP_FLASH_SECTOR_SIZE)     /* 0x1000 = 4096-Byte */

/* 继电器 */
#define     MCU_PARAM_RELAY_ON_TIME_DEFAULT         (  0U)
#define     MCU_PARAM_RELAY_ON_UNIT_DEFAULT         (RELAY_UNIT_S)
#define     MCU_PARAM_RELAY_OFF_TIME_DEFAULT        (  0U)
#define     MCU_PARAM_RELAY_OFF_UNIT_DEFAULT        (RELAY_UNIT_S)
/****************************************************
                      结构体定义
*****************************************************/
/* 参数结构体 */
typedef struct _PARAM_OBJECT_
{
    RELAY_OBJECT relayOnObj;            /* 合闸参数 */
    RELAY_OBJECT relayOffObj;           /* 分闸参数 */

    UINT32       checkSum;              /* 校验和 */
} PARAM_OBJECT;

/****************************************************
                       全局变量
*****************************************************/

/****************************************************
                       函数声明
*****************************************************/
/**
  * @brief 获取参数
  * @param[in]   void
  * @param[out]  void
  * @return 参数指针
*/
PARAM_OBJECT *Param_GetObj(void);

/**
  * @brief 更新参数
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
void Param_Update(void);

/**
  * @brief 参数初始化
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
void Param_Init(void);

/****************************************************
                       函数定义
*****************************************************/

#endif
