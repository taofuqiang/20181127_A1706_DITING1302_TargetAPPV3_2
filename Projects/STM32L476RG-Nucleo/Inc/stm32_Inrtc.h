/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_Inrtc.h
*\Description   
*\Note          
*\Log           2018.05.07    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#ifndef _STM32_INRTC_H
#define _STM32_INRTC_H
#include "public_type.h"
#include "common_lib.h"

#pragma pack(1)
//带亚秒的时间类型
typedef struct TIME_SubSec_st
{
    TIME T;//日期 时间
    u16  SubSecs;//亚秒部分
}TIME_SubSec;
#pragma pack()

typedef enum AlarmType_st
{
    RTC_ALARMA,
    RTC_ALARMB,
}AlarmType;

extern RTC_HandleTypeDef RtcHandle;

void stm32_Inrtc_init(bool force);
bool stm32_Inrtc_setTime(TIME_SubSec SetT);
bool stm32_Inrtc_getTime(TIME_SubSec* getT);

bool stm32_Inrtc_EnWakeup(u16 ms);
bool stm32_Inrtc_DisWakeup(void);

bool stm32_Inrtc_DisAlarm(AlarmType alarm);
bool stm32_Inrtc_EnAlarm(AlarmType alarm, u32 timeout);

bool stm32_Inrtc_msShift(s16 nMs);
#endif /*_STM32_INRTC_H*/

