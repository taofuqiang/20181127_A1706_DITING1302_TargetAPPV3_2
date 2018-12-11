/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_gpio_irq.h
*\Description   
*\Note          
*\Log           2018.05.11    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#ifndef _STM32_GPIO_IRQ_H
#define _STM32_GPIO_IRQ_H
#include "public_type.h"


bool stm32_gpio_irq_config(u16 gpio_pin, pHookHandle func, void* p);
#endif /*_STM32_GPIO_IRQ_H*/

