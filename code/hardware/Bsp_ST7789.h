/**
  *FileName:    Bsp_ST7789.h.
  *Date:        2026-07-03
  *Author:      Author
  *Version:     1.0
  *Description: ST7789驱动文件
 */

#ifndef __BSP_ST7789_H__
#define __BSP_ST7789_H__

/****************************************************
                        头文件
*****************************************************/
#include "Mcu_Type.h"

/****************************************************
                        宏定义
*****************************************************/
#define     BSP_ST7789_HANDLER              (spi0)              /* SPI句柄 */
#define     BSP_ST7789_BAUDRATE_MHZ         (50U)               /* SPI速率 */

#define     BSP_ST7789_SCK_PIN              ( 2U)               /* 时钟 */
#define     BSP_ST7789_MOSI_PIN             ( 3U)               /* 数据 */

#define     BSP_ST7789_DC_PIN               (13U)               /* 数据/命令 */
#define     BSP_ST7789_RES_PIN              (12U)               /* 复位 */
#define     BSP_ST7789_BLK_PIN              (14U)               /* 背光 */

#define     BSP_DISPLAY_X_WIDTH             (240U)              /* X轴宽度  */
#define     BSP_DISPLAY_Y_WIDTH             (240U)              /* Y轴宽度  */

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
 * @brief ST7789设置地址
 * @param[in]   xStart      起始X坐标
 * @param[in]   yStart      起始Y坐标
 * @param[in]   xEnd        结束X坐标
 * @param[in]   yEnd        结束Y坐标
 * @param[out]  void
 * @return void
 */
void ST7789_SetAddress(UINT16 xStart, UINT16 yStart, UINT16 xEnd, UINT16 yEnd);

/**
  * @brief ST7789发送数据
  * @param[in]  pBuff  缓冲区
  * @param[in]  len    长度
  * @param[out] void
  * @return void
*/
void ST7789_SendData(UINT8 *pBuff, UINT32 len);

/**
 * @brief ST7789复位
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void ST7789_Reset(void);

/**
 * @brief ST7789使能背光
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void ST7789_EnableBacklight(void);

/**
 * @brief ST7789失能背光
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void ST7789_DisableBacklight(void);

/**
 * @brief ST7789初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void ST7789_Init(void);

/****************************************************
                       函数定义
*****************************************************/

#endif
