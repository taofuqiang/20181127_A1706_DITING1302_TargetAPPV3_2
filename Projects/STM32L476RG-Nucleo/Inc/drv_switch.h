/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			drv_switch.h
*\Description   
*\Note          
*\Log           2016.07.08    Ver 1.0   Job
*               创建文件。
***************************************************************************************************/
#ifndef _DRV_SWITCH_H
#define _DRV_SWITCH_H
#include "public_type.h"

typedef struct switch_type_st
{
    GPIO_TypeDef* gpio;
    u16 pin;
    bool valid;//有效电平
    /*上面需初始化赋值*/
}switch_type;

void stm32_switch_on(switch_type* sw);
void stm32_switch_off(switch_type* sw);
void stm32_switch_evert(switch_type* sw);
void stm32_switch_init(switch_type* sw);
#endif /*_DRV_SWITCH_H*/
