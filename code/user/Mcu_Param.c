/**
 *FileName:    Mcu_Param.c
 *Date:        2026-07-14
 *Author:      Author
 *Version:     1.0
 *Description: 参数
 */

/****************************************************
                        头文件
*****************************************************/
#include "Mcu_Param.h"
#include "Mcu_Public.h"

/****************************************************
                        宏定义
*****************************************************/

/****************************************************
                      结构体定义
*****************************************************/

/****************************************************
                       全局变量
*****************************************************/
/* 参数对象 */
static PARAM_OBJECT paramObj = {0};

/****************************************************
                       函数声明
*****************************************************/

/****************************************************
                       函数定义
*****************************************************/
/**
  * @brief 获取参数
  * @param[in]   void
  * @param[out]  void
  * @return 参数指针
*/
PARAM_OBJECT *Param_GetObj(void)
{
    return &paramObj;
}

/**
  * @brief 更新参数
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
void Param_Update(void)
{
    /* 更新校验和 */
    paramObj.checkSum = CheckSumU32((UINT8 *)&paramObj, sizeof(PARAM_OBJECT) - 4);

    /* 擦除Flash */
    Flash_Erase(MCU_PARAM_ADDR, MCU_PARAM_LEN);
    /* 写入Flash */
    Flash_Write(MCU_PARAM_ADDR, (UINT8 *)&paramObj, sizeof(PARAM_OBJECT));
}

/**
  * @brief 参数初始化
  * @param[in]   void
  * @param[out]  void
  * @return void
*/
void Param_Init(void)
{
    UINT32 checkSum = 0;

    /* 获取Flash中的参数 */
    Flash_Read(MCU_PARAM_ADDR, (UINT8 *)&paramObj, sizeof(PARAM_OBJECT));

    /* 重新计算Flash里的参数校验和 */
    checkSum = CheckSumU32((UINT8 *)&paramObj, sizeof(PARAM_OBJECT) - 4);

    /* 校验Flash里的参数是否合法 */
    if (checkSum != paramObj.checkSum)
    {
        /* 使用默认参数 */
        paramObj.relayOnObj.time  = MCU_PARAM_RELAY_ON_TIME_DEFAULT;
        paramObj.relayOnObj.unit  = MCU_PARAM_RELAY_ON_UNIT_DEFAULT;
        paramObj.relayOffObj.time = MCU_PARAM_RELAY_OFF_TIME_DEFAULT;
        paramObj.relayOffObj.unit = MCU_PARAM_RELAY_OFF_UNIT_DEFAULT;

        /* 写入 */
        Param_Update();
    }
}
