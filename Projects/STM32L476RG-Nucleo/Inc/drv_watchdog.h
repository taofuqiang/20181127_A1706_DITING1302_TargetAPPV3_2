/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          watchdog.h
*\Description   
*\Note          
*\Log           2015.05.30    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#ifndef _DRV_WATCHDOG_H
#define _DRV_WATCHDOG_H
#include "public_type.h"

void watchdog_init(void);
void Watchdog_Refresh(void);

void Watchdog_Periodic_Handle(uint32_t localtime);
void AutoReset_Periodic_Handle(void);

extern u8 SysRstFlag;//��λ��־��¼
//�õ�ϵͳ��λ�ķ�ʽ,���Ź�&���&����
void GetRestFlag(void);
#endif /*_WATCHDOG_H*/

