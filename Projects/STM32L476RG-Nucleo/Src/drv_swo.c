/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			drv_swo.c
*\Description   ITM调试打印信息 重定向printf
*\Note          
*\Log           2016.07.15    Ver 1.0   Job
*               创建文件。
***************************************************************************************************/
#include "stm32l4xx.h"
#include "stdio.h"
#include "app_board.h"

#ifndef USING_USART_PRINTF//调试功能

int fputc(int ch, FILE* f)
{
    ITM_SendChar(ch);
    return ch;
}
#endif
