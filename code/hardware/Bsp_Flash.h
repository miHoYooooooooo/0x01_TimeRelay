/**
  *FileName:    Bsp_Flash.h
  *Date:        2026-07-15
  *Author:      Author
  *Version:     1.0
  *Description: Flash驱动
 */

#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

/****************************************************
                        头文件
*****************************************************/
#include "Mcu_Type.h"

/****************************************************
                        宏定义
*****************************************************/
#define     BSP_FLASH_SECTOR_SIZE           (4096U)         /* 0x1000 = 4096-Byte */
#define     BSP_FLASH_PAGE_SIZE             ( 256U)         /* 0x0100 =  256-Byte */

/****************************************************
                      结构体定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/

/****************************************************
                       函数声明
*****************************************************/
/**
  * @brief Flash擦
  * @param[in]   addr   地址
  * @param[in]   len    长度
  * @param[out]  void
  * @attention   addr必须4096-Byte对齐，len必须是4096的倍数
  * @return 执行结果
*/
INT32 Flash_Erase(UINT32 addr, UINT32 len);

/**
  * @brief Flash写
  * @param[in]   addr   地址
  * @param[in]   pData  数据
  * @param[in]   len    长度
  * @param[out]  void
  * @attention   addr必须256-Byte对齐
  * @return 执行结果
*/
INT32 Flash_Write(UINT32 addr, UINT8 *pData, UINT32 len);

/**
  * @brief Flash读
  * @param[in]   addr   地址
  * @param[in]   len    长度
  * @param[out]  pData  指针
  * @return 执行结果
*/
INT32 Flash_Read(UINT32 addr, UINT8 *pData, UINT32 len);

/****************************************************
                       函数定义
*****************************************************/

#endif
