/**
 *FileName:    Bsp_ST7789.c
 *Date:        2026-07-03
 *Author:      Author
 *Version:     1.0
 *Description: ST7789驱动文件
 */

/****************************************************
                        头文件
*****************************************************/
#include "Bsp_ST7789.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
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
/* DMA通道号 */
static UINT32 dmaChan = 0;

/****************************************************
                       函数声明
*****************************************************/
/* Dma中断回调 */
extern void Irq_DmaHandler(void);
void ST7789_Reset(void);

/****************************************************
                       函数定义
*****************************************************/
/**
 * @brief 硬件初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
static void hardware_init()
{
    dma_channel_config dmaConfig = {0};

    /* 初始化控制脚 */
    gpio_init(BSP_ST7789_DC_PIN);
    gpio_init(BSP_ST7789_RES_PIN);
    gpio_init(BSP_ST7789_BLK_PIN);
    gpio_set_dir(BSP_ST7789_DC_PIN,  GPIO_OUT);
    gpio_set_dir(BSP_ST7789_RES_PIN, GPIO_OUT);
    gpio_set_dir(BSP_ST7789_BLK_PIN, GPIO_OUT);
    /* 默认状态 */
    gpio_put(BSP_ST7789_DC_PIN,  TRUE);         /* 数据模式 */
    gpio_put(BSP_ST7789_RES_PIN, TRUE);         /* 非复位   */
    gpio_put(BSP_ST7789_BLK_PIN, FALSE);        /* 关闭背光 */

    /* 初始化SPI */
    spi_init(BSP_ST7789_HANDLER, BSP_ST7789_BAUDRATE_MHZ * 1000 * 1000);
    spi_set_format(BSP_ST7789_HANDLER, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    /* 初始化SPI引脚 */
    gpio_set_function(BSP_ST7789_SCK_PIN,  GPIO_FUNC_SPI);
    gpio_set_function(BSP_ST7789_MOSI_PIN, GPIO_FUNC_SPI);

    /* 声明DMA通道 */
    dmaChan = dma_claim_unused_channel(TRUE);

    /* 初始化Tx DMA */
    dmaConfig = dma_channel_get_default_config(dmaChan);
    channel_config_set_transfer_data_size(&dmaConfig, DMA_SIZE_8);      /* 数据大小 */
    channel_config_set_read_increment (&dmaConfig, TRUE);               /* 自增设置 */
    channel_config_set_write_increment(&dmaConfig, FALSE);
    channel_config_set_dreq(&dmaConfig, spi_get_index(BSP_ST7789_HANDLER) ? DREQ_SPI1_TX : DREQ_SPI0_TX);        /* 传输请求 */
    dma_channel_configure(
        dmaChan,
        &dmaConfig,
        &spi_get_hw(BSP_ST7789_HANDLER)->dr,                            /* 写地址 */
        NULL,                                                           /* 读地址 */
        0,
        FALSE);

    /* 使能DMA通道中断 */
    dma_channel_set_irq0_enabled(dmaChan, true);
    /* 设置DMA中断处理函数 */
    irq_set_exclusive_handler(DMA_IRQ_0, Irq_DmaHandler);
    /* 使能DMA中断 */
    irq_set_enabled(DMA_IRQ_0, true);
}

/**
  * @brief 发送命令
  * @param[in]   cmd    命令
  * @param[out]  void
  * @return void
*/
static void st7789_write_cmd(UINT8 cmd)
{
    /* 命令 */
    gpio_put(BSP_ST7789_DC_PIN, FALSE);
    spi_write_blocking(BSP_ST7789_HANDLER, &cmd, 1);
}

/**
  * @brief 发送数据
  * @param[in]   pData  数据
  * @param[in]   len    长度
  * @param[out]  void
  * @return void
*/
static void st7789_write_data(UINT8 *pData, UINT32 len)
{
    /* 数据 */
    gpio_put(BSP_ST7789_DC_PIN, TRUE);
    spi_write_blocking(BSP_ST7789_HANDLER, pData, len);
}

/**
  * @brief 发送单条数据
  * @param[in]   data   数据
  * @param[out]  void
  * @return void
*/
static void st7789_write_data_byte(UINT8 data)
{
    st7789_write_data(&data, 1);
}

/**
 * @brief 软件初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
static void software_init(void)
{
    /* 复位 */
    ST7789_Reset();

    /* 方向 */
    /*  0x00	竖屏，起点左上	默认
        0x70	横屏，起点右上	旋转 90°
        0xC0	横屏，起点左下	旋转 180°
        0xA0	横屏，起点右下	旋转 270°
    */
    st7789_write_cmd(0x36);
    st7789_write_data_byte(0x00);

    /* RGB565 */
    st7789_write_cmd(0x3A);
    st7789_write_data_byte(0x05);

    // /* 调节屏幕扫描的时序参数 */
    // st7789_write_cmd(0xB2);
    // st7789_write_data_byte(0x0C);
    // st7789_write_data_byte(0x0C);
    // st7789_write_data_byte(0x00);
    // st7789_write_data_byte(0x33);
    // st7789_write_data_byte(0x33);

    // /* 设置Gate(行驱动)控制 */
    // st7789_write_cmd(0xB7);
    // st7789_write_data_byte(0x35);
    // /* 设置VCOM电压(液晶共模电压) */
    // st7789_write_cmd(0xBB);
    // st7789_write_data_byte(0x19);
    // /* 设置内部电压转换器输出 */
    // st7789_write_cmd(0xC0);
    // st7789_write_data_byte(0x2C);
    // /* 启用VDV(电压增益)和VRH(参考电压) */
    // st7789_write_cmd(0xC2);
    // st7789_write_data_byte(0x01);
    // /* 设置VRH(电压参考级别) */
    // st7789_write_cmd(0xC3);
    // st7789_write_data_byte(0x12);
    // /* 设置VDV(电压偏移量) */
    // st7789_write_cmd(0xC4);
    // st7789_write_data_byte(0x20);
    // /* 设置帧率(60Hz) */
    // st7789_write_cmd(0xC6);
    // st7789_write_data_byte(0x0F);
    // /* 设置第二组电源控制参数 */
    // st7789_write_cmd(0xD0);
    // st7789_write_data_byte(0xA4);
    // st7789_write_data_byte(0xA1);
    // /* 设置Gamma曲线 */
    // st7789_write_cmd(0xE0);
    // st7789_write_data_byte(0xD0);
    // st7789_write_data_byte(0x04);
    // st7789_write_data_byte(0x0D);
    // st7789_write_data_byte(0x11);
    // st7789_write_data_byte(0x13);
    // st7789_write_data_byte(0x2B);
    // st7789_write_data_byte(0x3F);
    // st7789_write_data_byte(0x54);
    // st7789_write_data_byte(0x4C);
    // st7789_write_data_byte(0x18);
    // st7789_write_data_byte(0x0D);
    // st7789_write_data_byte(0x0B);
    // st7789_write_data_byte(0x1F);
    // st7789_write_data_byte(0x23);
    // /* 设置Gamma曲线 */
    // st7789_write_cmd(0xE1);
    // st7789_write_data_byte(0xD0);
    // st7789_write_data_byte(0x04);
    // st7789_write_data_byte(0x0C);
    // st7789_write_data_byte(0x11);
    // st7789_write_data_byte(0x13);
    // st7789_write_data_byte(0x2C);
    // st7789_write_data_byte(0x3F);
    // st7789_write_data_byte(0x44);
    // st7789_write_data_byte(0x51);
    // st7789_write_data_byte(0x2F);
    // st7789_write_data_byte(0x1F);
    // st7789_write_data_byte(0x1F);
    // st7789_write_data_byte(0x20);
    // st7789_write_data_byte(0x23);

    /* 开启显示颜色反转 */
    st7789_write_cmd(0x21);
    /* 退出睡眠模式 */
    st7789_write_cmd(0x11);
    DelayMs(120);
    /* 使能显示 */
    st7789_write_cmd(0x29);
    DelayMs(20);
}

/**
  * @brief ST7789获取DMA通道号
  * @param[in]  void
  * @param[out] void
  * @return UINT32  DMA通道号
*/
UINT32 ST7789_GetDmaChan(void)
{
    return dmaChan;
}

/**
 * @brief ST7789设置地址
 * @param[in]   xStart      起始X坐标
 * @param[in]   yStart      起始Y坐标
 * @param[in]   xEnd        结束X坐标
 * @param[in]   yEnd        结束Y坐标
 * @param[out]  void
 * @return void
 */
void ST7789_SetAddress(UINT16 xStart, UINT16 yStart, UINT16 xEnd, UINT16 yEnd)
{
    /* 防止越界 */
    if (BSP_DISPLAY_X_WIDTH <= xEnd)
    {
        xEnd = BSP_DISPLAY_X_WIDTH - 1;
    }
    if (BSP_DISPLAY_Y_WIDTH <= yEnd)
    {
        yEnd = BSP_DISPLAY_Y_WIDTH - 1;
    }

    /* 设置X */
    st7789_write_cmd(0x2A);
    st7789_write_data_byte(xStart >> 8);
    st7789_write_data_byte(xStart & 0xFF);
    st7789_write_data_byte(xEnd >> 8);
    st7789_write_data_byte(xEnd & 0xFF);

    /* 设置Y */
    st7789_write_cmd(0x2B);
    st7789_write_data_byte(yStart >> 8);
    st7789_write_data_byte(yStart & 0xFF);
    st7789_write_data_byte(yEnd >> 8);
    st7789_write_data_byte(yEnd & 0xFF);

    /* 开始写入 */
    st7789_write_cmd(0x2C);
    /* 写数据 */
    gpio_put(BSP_ST7789_DC_PIN, TRUE);
}

/**
  * @brief ST7789发送数据
  * @param[in]  pBuff  数据
  * @param[in]  len    长度
  * @param[out] void
  * @return void
*/
void ST7789_SendData(UINT8 *pBuff, UINT32 len)
{
    if ((NULL == pBuff) || (0 == len))
    {
        return;
    }

    /* 等待上次传输完成 */
    while(0 != dma_channel_hw_addr(dmaChan)->transfer_count);

    /* 设置传输长度 */
    dma_channel_set_trans_count(dmaChan, len, FALSE);
    /* 通过设置读地址开启DMA内存到外设传输 */
    dma_channel_set_read_addr(dmaChan, pBuff, TRUE);
}

/**
 * @brief ST7789复位
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void ST7789_Reset(void)
{
    /* 复位 */
    gpio_put(BSP_ST7789_RES_PIN, FALSE);
    DelayMs(50);
    gpio_put(BSP_ST7789_RES_PIN, TRUE);
    DelayMs(120);
}

/**
 * @brief ST7789使能背光
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void ST7789_EnableBacklight(void)
{
    /* 开启背光 */
    gpio_put(BSP_ST7789_BLK_PIN, TRUE);
}

/**
 * @brief ST7789失能背光
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void ST7789_DisableBacklight(void)
{
    /* 开启背光 */
    gpio_put(BSP_ST7789_BLK_PIN, FALSE);
}

/**
 * @brief ST7789初始化
 * @param[in]   void
 * @param[out]  void
 * @return void
 */
void ST7789_Init(void)
{
    /* 硬件初始化 */
    hardware_init();

    /* 软件初始化 */
    software_init();
}
