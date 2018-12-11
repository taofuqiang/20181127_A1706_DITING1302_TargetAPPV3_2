/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          watchdog.h
*\Description   
*\Note          
*\Log           2015.05.30    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#ifndef _DRV_WATCHDOG_H
#define _DRV_WATCHDOG_H
#include "public_type.h"

void watchdog_init(void);
void Watchdog_Refresh(void);

void Watchdog_Periodic_Handle(uint32_t localtime);
void AutoReset_Periodic_Handle(void);

extern u8 SysRstFlag;//复位标志记录
//得到系统复位的方式,看门狗&软件&掉电
void GetRestFlag(void);
#endif /*_WATCHDOG_H*/

