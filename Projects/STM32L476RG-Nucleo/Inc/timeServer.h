/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          timeServer.h
*\Description   
*\Note          
*\Log           2018.05.08    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#ifndef __TIMESERVER_H__
#define __TIMESERVER_H__
#include "public_type.h"

typedef u16  TimerTime_t;

typedef struct TimerEvent_st
{
    u16 TimeCount;         //! Expiring timer value in ms from TimerContext
    u16 TimeInterval;       //! Reload Value when Timer is restarted

    bool IsRunning;             //! Is the timer currently running
    void ( *Callback )( void* ); //! Timer IRQ callback function
    void* para;//回调函数参数
    struct TimerEvent_st *Next;  //! Pointer to the next Timer object.
} TimerEvent_t;

void TimerIrqHandler( void );
void TimerInit( TimerEvent_t *obj, void ( *callback )( void* ), void* para );
void TimerSetValue( TimerEvent_t *obj, u16 value );
void TimerStart( TimerEvent_t *obj );
void TimerStop( TimerEvent_t *obj );
void TimerReset( TimerEvent_t *obj );


TimerTime_t TimerGetCurrentTime( void );

#endif /* __TIMESERVER_H__*/
