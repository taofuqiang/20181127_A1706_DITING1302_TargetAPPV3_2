/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_uart.h
*\Description   
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#ifndef _STM32_UART_H
#define _STM32_UART_H
#include "stm32l4xx.h"
#include "public_type.h"
#include "usdl_buf.h"

/*uart类型*/
typedef struct UartBufType_st
{
    USART_TypeDef* uart;
    GpioType*   TxGpio; //uart发送脚
    GpioType*   RxGpio; //uart接收脚
    GpioType*   Rs485Ctrl;//如果是485，则有控制脚，否则为RT_NULL
    u32         baudrate;//波特率，发送时会根据波特率计算等待时间
    u8*         RxBuf_addr;
    u32         RxBuf_size;
    /*上面需初始化赋值*/
    RingBufType   rxbuf;  //uart接收缓冲区
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

