/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_spi.h
*\Description   
*\Note          
*\Log           2018.05.05    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#ifndef _STM32_SPI_H
#define _STM32_SPI_H
#include "public_type.h"
#include "drv_switch.h"

/*spi����*/
typedef struct SpiDevType_st
{
    SPI_TypeDef* spi;
    switch_type* SPI_CS;//Ƭѡ
    GpioType*   SCLK;//ʱ�ӽ�
    GpioType*   MOSI;//�����
    GpioType*   MISO;//������
    u32         Freq;  //Ƶ��
    u32         DataBit;//����λ
    u32         CLKPolarity;         
    u32         CLKPhase;    
    u32         FirstBit;  
    /*�������ʼ����ֵ*/
    SPI_HandleTypeDef hspi;
}SpiDevType;

void stm32_spi_init(SpiDevType* spi_dev);
u16 stm32_spi_aByte(SpiDevType* spi_dev, u16 txData);
void stm32_spi_deinit(SpiDevType* spi_dev);
#endif /*_STM32_SPI_H*/

