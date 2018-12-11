/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_spi.h
*\Description   
*\Note          
*\Log           2018.05.05    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#ifndef _STM32_SPI_H
#define _STM32_SPI_H
#include "public_type.h"
#include "drv_switch.h"

/*spi类型*/
typedef struct SpiDevType_st
{
    SPI_TypeDef* spi;
    switch_type* SPI_CS;//片选
    GpioType*   SCLK;//时钟脚
    GpioType*   MOSI;//主输出
    GpioType*   MISO;//主输入
    u32         Freq;  //频率
    u32         DataBit;//数据位
    u32         CLKPolarity;         
    u32         CLKPhase;    
    u32         FirstBit;  
    /*上面需初始化赋值*/
    SPI_HandleTypeDef hspi;
}SpiDevType;

void stm32_spi_init(SpiDevType* spi_dev);
u16 stm32_spi_aByte(SpiDevType* spi_dev, u16 txData);
void stm32_spi_deinit(SpiDevType* spi_dev);
#endif /*_STM32_SPI_H*/

