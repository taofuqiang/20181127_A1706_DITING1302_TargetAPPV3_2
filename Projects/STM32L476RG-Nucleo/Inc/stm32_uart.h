/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_uart.h
*\Description   
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#ifndef _STM32_UART_H
#define _STM32_UART_H
#include "stm32l4xx.h"
#include "public_type.h"
#include "usdl_buf.h"

/*uart����*/
typedef struct UartBufType_st
{
    USART_TypeDef* uart;
    GpioType*   TxGpio; //uart���ͽ�
    GpioType*   RxGpio; //uart���ս�
    GpioType*   Rs485Ctrl;//�����485�����п��ƽţ�����ΪRT_NULL
    u32         baudrate;//�����ʣ�����ʱ����ݲ����ʼ���ȴ�ʱ��
    u8*         RxBuf_addr;
    u32         RxBuf_size;
    /*�������ʼ����ֵ*/
    RingBufType   rxbuf;  //uart���ջ�����
}UartDevType;

void stm32_uart_isr(UartDevType* uart_dev);

void stm32_uart_init(UartDevType* uart_dev);
void stm32_uart_disinit(UartDevType* uart_dev);
u32 stm32_uart_send(UartDevType* uart_dev, u8* tx, u32 len);
u32 stm32_uart_recv(UartDevType* uart_dev, u8* rx, u32 len, bool clear);
u32 stm32_uart_len_get(UartDevType* uart_dev);
u32 stm32_uart_del(UartDevType* uart_dev, u32 len);
bool stm32_uart_clear(UartDevType* uart_dev);

#endif /*_STM32_UART_H*/

