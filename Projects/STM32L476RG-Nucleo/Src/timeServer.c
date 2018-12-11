/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          timeServer.c
*\Description   Ϊ�˶�ʱ�����ṩʱ����Ӳ����ʱ����1tick=1ms��������Ҫ����ת��
*\Note          
*\Log           2018.05.08    Ver 1.0    Job
*               �����ļ���
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
#define TIMER_MIN_TIMEOUT    (6)//����CMP�жϵ��ж���Ч����Сʱ�� ��С�ᵼ��ʱ����
/*!
* ���������ʱ�䰴�մ�С���󱣴棬��TimeCountΪԭʼ�ļ�ʱʱ�䣬����time�ж��еݼ�
����һ��100 200, 300��ʱ����
*/
static TimerEvent_t *TimerListHead = NULL;
static TimerTime_t TimerStampLast;//���һ�ο�ʼ��ʱ��ʱ���

/***************************************************************************************************
*\Function      GetTimerElapsedTime
*\Description   ��ȡ�Դ��ϴζ�ʱ�����Ѿ����ŵ�ʱ��
*\Parameter     
*\Return        u16
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
����������
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
				����������
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
*\Description   ���ó�ʱʱ�䣬�������ӹ���
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
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

        //ȷ�����óɹ�
        if (stm32_lptim_cmp_en(timeout))
        {
            break;
        }
    }    
}

/***************************************************************************************************
*\Function      TimerInsertTimer
*\Description   ����ʱ�䵽����
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
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
				����������
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
*\Description   �жϴ�����
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
***************************************************************************************************/
void TimerIrqHandler( void )
{
    TimerEvent_t* cur;

    u16 DeltaContext = stm32_lptim_cnt_read() - TimerStampLast; //intentionnal wrap around

    TimerStampLast = stm32_lptim_cnt_read();

    //Ϊ�˷�ֹ���ж�����룬���Ի��ǿ�TimeCount�Ƿ�Ϊ0���ж϶�ʱ�Ƿ�ʱ
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
*\Description   ��ʼ��
*\Parameter     obj
*\Parameter     
*\Parameter     
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
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
*\Description   ���ö�ʱֵ
*\Parameter     obj
*\Parameter     value
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
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
*\Description   ����timer������ǰ�������TimerSetValue����ʱ��
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
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
*\Description   ֹͣ��ʱ
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
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
*\Description   ��λ
*\Parameter     obj
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
***************************************************************************************************/
void TimerReset(TimerEvent_t *obj )
{
    TimerStop(obj);
    TimerStart(obj);
}

/***************************************************************************************************
*\Function      TimerGetCurrentTime
*\Description   ��ǰ��ʱ��ֵ
*\Parameter     
*\Return        TimerTime_t
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
***************************************************************************************************/
TimerTime_t TimerGetCurrentTime( void )
{
    return  stm32_lptim_cnt_read();
}
