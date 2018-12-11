/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          timeServer.c
*\Description   为此定时服务提供时机的硬件计时必须1tick=1ms，否则需要进行转换
*\Note          
*\Log           2018.05.08    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "timeServer.h"
#include "app_board.h"

/*!
* safely execute call back
*/
#define exec_cb( _callback_ , para)     \
    do {                          \
    if( _callback_ == NULL )    \
      {                           \
      while(1);                 \
      }                           \
      else                        \
      {                           \
      _callback_(para );               \
      }                           \
    } while(0);                   


#define TIMER_SERVER_MAX_CNT 0x10000
#define TIMER_MIN_TIMEOUT    (6)//设置CMP中断到中断生效的最小时间 过小会导致时间错过
/*!
* 该链表保存的时间按照从小到大保存，且TimeCount为原始的计时时间，会在time中断中递减
例如一个100 200, 300计时链表
*/
static TimerEvent_t *TimerListHead = NULL;
static TimerTime_t TimerStampLast;//最后一次开始计时的时间戳

/***************************************************************************************************
*\Function      GetTimerElapsedTime
*\Description   获取自从上次定时以来已经流逝的时间
*\Parameter     
*\Return        u16
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
u16 TimerElapsedTime_Get(void)
{
    return (stm32_lptim_cnt_read() + TIMER_SERVER_MAX_CNT - TimerStampLast) % TIMER_SERVER_MAX_CNT;
}

/***************************************************************************************************
*\Function      TimerExists
*\Description   
*\Parameter     obj
*\Return        bool
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static bool TimerExists( TimerEvent_t *obj )
{
    TimerEvent_t* cur = TimerListHead;

    while( cur != NULL )
    {
        if( cur == obj )
        {
            return TRUE;
        }
        cur = cur->Next;
    }
    return FALSE;
}
/***************************************************************************************************
*\Function      TimerSetTimeout
*\Description   设置超时时间，类似闹钟功能
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static void TimerSetTimeout(TimerEvent_t *obj )
{
    u16 timeout = 0;
    u16 minTicks= TIMER_MIN_TIMEOUT;

    obj->IsRunning = TRUE; 

    while (1)
    {
        //in case deadline too soon
        if(obj->TimeCount  <  TimerElapsedTime_Get() + minTicks)
        {
            obj->TimeCount = TimerElapsedTime_Get() + minTicks;
        }

        timeout = TimerStampLast + obj->TimeCount;

        //确保设置成功
        if (stm32_lptim_cmp_en(timeout))
        {
            break;
        }
    }    
}

/***************************************************************************************************
*\Function      TimerInsertTimer
*\Description   插入时间到后面
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static void TimerInsertTimer(TimerEvent_t *obj)
{
    TimerEvent_t* cur = TimerListHead;
    TimerEvent_t* next = TimerListHead->Next;

    while (cur->Next != NULL )
    {  
        if( obj->TimeCount  > next->TimeCount)
        {
            cur = next;
            next = next->Next;
        }
        else
        {
            cur->Next = obj;
            obj->Next = next;
            return;

        }
    }
    cur->Next = obj;
    obj->Next = NULL;
}

/***************************************************************************************************
*\Function      TimerInsertNewHeadTimer
*\Description   
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static void TimerInsertNewHeadTimer(TimerEvent_t *obj)
{
    TimerEvent_t* cur = TimerListHead;

    if( cur != NULL )
    {
        cur->IsRunning = FALSE;
    }

    obj->Next = cur;
    TimerListHead = obj;
    TimerSetTimeout(TimerListHead);
}

/***************************************************************************************************
*\Function      TimerIrqHandler
*\Description   中断处理函数
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void TimerIrqHandler( void )
{
    TimerEvent_t* cur;

    u16 DeltaContext = stm32_lptim_cnt_read() - TimerStampLast; //intentionnal wrap around

    TimerStampLast = stm32_lptim_cnt_read();

    //为了防止此中断误进入，所以还是看TimeCount是否为0来判断定时是否超时
    cur = TimerListHead;
    while( cur != NULL )
    {
        if (cur->TimeCount > DeltaContext)
        {
            cur->TimeCount -= DeltaContext;
        }
        else
        {
            cur->TimeCount = 0;
        }
        cur = cur->Next;
    }

    /* execute imediately the alarm callback */
    if ( TimerListHead != NULL && TimerListHead->TimeCount == 0)
    {
        cur = TimerListHead;
        TimerListHead = TimerListHead->Next;
        exec_cb( cur->Callback, cur->para);
    }


    // remove all the expired object from the list
    while( ( TimerListHead != NULL ) && (TimerListHead->TimeCount < TimerElapsedTime_Get() ))
    {
        cur = TimerListHead;
        TimerListHead = TimerListHead->Next;
        exec_cb( cur->Callback, cur->para);
    }

    /* start the next TimerListHead if it exists AND NOT running */
    if((TimerListHead != NULL ) && (TimerListHead->IsRunning == FALSE))
    {
        TimerSetTimeout(TimerListHead);
    }
}


/***************************************************************************************************
*\Function      TimerInit
*\Description   初始化
*\Parameter     obj
*\Parameter     
*\Parameter     
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void TimerInit( TimerEvent_t *obj, void ( *callback )( void* ), void* para )
{
    obj->TimeCount = 0;
    obj->TimeInterval = 0;
    obj->IsRunning = FALSE;
    obj->Callback = callback;
    obj->para = para;
    obj->Next = NULL;
}

/***************************************************************************************************
*\Function      TimerSetValue
*\Description   设置定时值
*\Parameter     obj
*\Parameter     value
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void TimerSetValue(TimerEvent_t *obj, u16 value)
{
    TimerStop( obj );

    if( value < TIMER_MIN_TIMEOUT )
    {
        value = TIMER_MIN_TIMEOUT;
    }

    obj->TimeCount = value;
    obj->TimeInterval = value;
}
/***************************************************************************************************
*\Function      TimerStart
*\Description   启动timer，启动前必须调用TimerSetValue设置时间
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void TimerStart( TimerEvent_t *obj )
{
    u16 elapsedTime = 0;

    BACKUP_PRIMASK();

    DISABLE_IRQ( );

    if( ( obj == NULL ) || ( TimerExists( obj ) == TRUE ) )
    {
        RESTORE_PRIMASK();
        ENABLE_IRQ();
        return;
    }
    obj->TimeCount = obj->TimeInterval;
    obj->IsRunning = FALSE;

    if(TimerListHead == NULL)
    {
        TimerStampLast = stm32_lptim_cnt_read();
        stm32_lptim_cmpfunc_set(TimerIrqHandler);      
        TimerInsertNewHeadTimer(obj); // insert a timeout at now+obj->Timestamp
    }
    else 
    {
        elapsedTime = TimerElapsedTime_Get();
        obj->TimeCount += elapsedTime;

        if( obj->TimeCount < TimerListHead->TimeCount )
        {
            TimerInsertNewHeadTimer(obj);
        }
        else
        {
            TimerInsertTimer( obj);
        }
    }
    RESTORE_PRIMASK();
    ENABLE_IRQ();
}

/***************************************************************************************************
*\Function      TimerStop
*\Description   停止计时
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void TimerStop(TimerEvent_t *obj) 
{
    BACKUP_PRIMASK();

    DISABLE_IRQ( );

    TimerEvent_t* prev = TimerListHead;
    TimerEvent_t* cur = TimerListHead;

    // List is empty or the Obj to stop does not exist 
    if( ( TimerListHead == NULL ) || ( obj == NULL ) )
    {
        RESTORE_PRIMASK();
        ENABLE_IRQ();
        return;
    }

    if( TimerListHead == obj ) // Stop the Head                  
    {
        if(TimerListHead->IsRunning == TRUE) // The head is already running 
        {    
            if(TimerListHead->Next != NULL )
            {
                TimerListHead->IsRunning = FALSE;
                TimerListHead = TimerListHead->Next;
                TimerSetTimeout( TimerListHead );
            }
            else
            {
                stm32_lptim_cmp_dis( );
                TimerListHead = NULL;
            }
        }
        else // Stop the head before it is started
        {   
            if( TimerListHead->Next != NULL )   
            {
                TimerListHead = TimerListHead->Next;
            }
            else
            {
                TimerListHead = NULL;
            }
        }
    }
    else // Stop an object within the list
    {      
        while(cur != NULL )
        {
            if( cur == obj )
            {
                if( cur->Next != NULL )
                {
                    cur = cur->Next;
                    prev->Next = cur;
                }
                else
                {
                    cur = NULL;
                    prev->Next = cur;
                }
                break;
            }
            else
            {
                prev = cur;
                cur = cur->Next;
            }
        }   
    }

    RESTORE_PRIMASK( );
    ENABLE_IRQ();
}  

/***************************************************************************************************
*\Function      TimerReset
*\Description   复位
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void TimerReset(TimerEvent_t *obj )
{
    TimerStop(obj);
    TimerStart(obj);
}

/***************************************************************************************************
*\Function      TimerGetCurrentTime
*\Description   当前定时器值
*\Parameter     
*\Return        TimerTime_t
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
TimerTime_t TimerGetCurrentTime( void )
{
    return  stm32_lptim_cnt_read();
}
