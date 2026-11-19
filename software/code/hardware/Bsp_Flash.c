/**
 *FileName:    Bsp_Flash.c
 *Date:        2026-07-15
 *Author:      Author
 *Version:     1.0
 *Description: Flash驱动
 */

/****************************************************
                        头文件
*****************************************************/
#include "Bsp_Flash.h"
#include <string.h>
#include "pico/flash.h"
#include "hardware/flash.h"

/****************************************************
                        宏定义
*****************************************************/

/****************************************************
                      结构体定义
*****************************************************/
/* Flash参数结构体 */
typedef struct _FLASH_PARAM_
{
    UINT32  addr;           /* 地址 */
    UINT8  *pData;          /* 数据 */
    UINT32  len;            /* 长度 */
} FLASH_PARAM;

/****************************************************
                       全局变量
*****************************************************/

/****************************************************
                       函数声明
*****************************************************/

/****************************************************
                       函数定义
*****************************************************/
// This function will be called when it's safe to call flash_range_erase
static void call_flash_range_erase(void *param)
{
    FLASH_PARAM *pParam = (FLASH_PARAM *)param;

    flash_range_erase(pParam->addr, pParam->len);
}

// This function will be called when it's safe to call flash_range_program
static void call_flash_range_program(void *param)
{
    FLASH_PARAM *pParam = (FLASH_PARAM *)param;

    flash_range_program(pParam->addr, pParam->pData, pParam->len);
}

/**
  * @brief Flash擦
  * @param[in]   addr   地址
  * @param[in]   len    长度
  * @param[out]  void
  * @attention   addr必须4096-Byte对齐，len必须是4096的倍数
  * @return 执行结果
*/
INT32 Flash_Erase(UINT32 addr, UINT32 len)
{
    FLASH_PARAM param = {0};

    /* 校验地址 */
    if (0 != (addr % FLASH_SECTOR_SIZE))
    {
        return ERROR;
    }

    /* 校验长度 */
    if (0 != (len % FLASH_SECTOR_SIZE))
    {
        return ERROR;
    }

    param.addr  = addr;
    param.pData = NULL;
    param.len   = len;

    flash_safe_execute(call_flash_range_erase, (void *)&param, UINT32_MAX);

    return OK;
}

/**
  * @brief Flash写
  * @param[in]   addr   地址
  * @param[in]   pData  数据
  * @param[in]   len    长度
  * @param[out]  void
  * @attention   addr必须256-Byte对齐
  * @return 执行结果
*/
INT32 Flash_Write(UINT32 addr, UINT8 *pData, UINT32 len)
{
    FLASH_PARAM param = {0};

    if ((NULL == pData)
     || (   0 == len  ))
    {
        return ERROR;
    }

    /* 校验地址 */
    if (0 != (addr % FLASH_PAGE_SIZE))
    {
        return ERROR;
    }

    param.addr  = addr;
    param.pData = pData;
    param.len   = len;

    /* 写入Flash */
    flash_safe_execute(call_flash_range_program, (void *)&param, UINT32_MAX);

    return OK;
}

/**
  * @brief Flash读
  * @param[in]   addr   地址
  * @param[in]   len    长度
  * @param[out]  pData  数据
  * @return 执行结果
*/
INT32 Flash_Read(UINT32 addr, UINT8 *pData, UINT32 len)
{
    if ((XIP_NOALLOC_BASE <= addr )
     || (            NULL == pData)
     || (               0 == len  ))
    {
        return ERROR;
    }

    memcpy(pData, (void *)(XIP_BASE + addr), len);

    return OK;
}
