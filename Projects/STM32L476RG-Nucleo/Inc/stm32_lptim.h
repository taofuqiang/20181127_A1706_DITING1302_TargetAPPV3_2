/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_lptim.h
*\Description   
*\Note          
*\Log           2018.05.09    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#ifndef _STM32_LPTIM_H
#define _STM32_LPTIM_H
#include "public_type.h"
#include "common_lib.h"

typedef void(*Callback)(void);

void stm32_lptim_init(void);
u16 stm32_lptim_cnt_read(void);
bool stm32_lptim_cmp_en(u16 cmp);
void stm32_lptim_cmp_dis(void);

void stm32_lptim_cmpfunc_set(Callback func);
#endif /*_STM32_LPTIM_H*/

