/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_Inrtc.c
*\Description   
*\Note          
*\Log           2018.05.07    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "stm32_Inrtc.h"

//#define RTC_CALIB_OUT_EN //У׼���ʹ��

//��һ���ϵ��� ��¼�ڱ�������
#define RTC_FIRST_FLAG_DR0 0x12345678

//RTCʱ�ӷ�Ƶ����
/* subsecond number of bits */
#define N_PREDIV_S                 10
/* Synchonuous prediv  */
#define PREDIV_S                  ((1<<N_PREDIV_S)-1)
/* Asynchonuous prediv   */
#define PREDIV_A                  (1<<(15-N_PREDIV_S))-1

//�Զ����ѵ�Ԫ ����ʱ��Ϊ1ms��32��
#define MIN_WU_TIME    1
#define MAX_WU_TIME    32000 /* 32 = 65536 / (32768 / 16) */

/* Convert miliseconds to wake up counter. */
#define mSEC_2_WU_CNT(ms)    (32768ul / 16 * (ms) / 1000) /* the MAX=32s */


RTC_HandleTypeDef RtcHandle;
static TIME_SubSec RTCAlarmA_Last;//�ϴ����ӵ�ʱ��
static TIME_SubSec RTCAlarmB_Last;//�ϴ����ӵ�ʱ��
/***************************************************************************************************
*\Function      stm32_Inrtc_init
*\Description    first,���ΪTRUE����ǿ�ƽ���һ�γ�ʼ��,��ʱ������Ϊ2010-1-1 0:0:0
*\Parameter     force
*\Return        void
*\Note          1��ϵͳ�ڵ�һ���ϵ�ʱ���ʼ��
*\Log           2018.05.07    Ver 1.0    Job               
				����������
***************************************************************************************************/
void stm32_Inrtc_init(bool force)
{
    TIME_SubSec time = {INIT_TIME, 0};
    RCC_OscInitTypeDef        RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

    /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
    /* To change the source clock of the RTC feature (LSE, LSI), You have to:
    - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
    - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
    configure the RTC clock source (to be done once after reset).
    - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
    __HAL_RCC_BACKUPRESET_RELEASE().
    - Configure the needed RTc clock source */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    //�ȳ�ʼ���ṹ�壬�����������ʱ��ʹ��
     /*##-1- Configure the RTC peripheral #######################################*/
    /* Configure RTC prescaler and RTC data registers */
    /* RTC configured as follows:
    - Hour Format    = Format 24
    - Asynch Prediv  = Value according to source clock
    - Synch Prediv   = Value according to source clock
    - OutPut         = Output Disable
    - OutPutPolarity = High Polarity
    - OutPutType     = Open Drain */ 
    RtcHandle.Instance = RTC; 
    RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RtcHandle.Init.AsynchPrediv = PREDIV_A;
    RtcHandle.Init.SynchPrediv = PREDIV_S;
    RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (force || HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR0) != RTC_FIRST_FLAG_DR0)
    {
        HAL_RTC_DeInit(&RtcHandle);

        /*##-2- Configure LSE as RTC clock source ###################################*/
        RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
        RCC_OscInitStruct.LSEState = RCC_LSE_ON;
        if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        { 
            printf("RCC LSE init fail!\n\r");
            while (1)
            {
            }
        }

        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
        if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        { 
            printf("RTC LSE error!\n\r");
            while (1)
            {
            }
        }

        /*##-3- Enable RTC peripheral Clocks #######################################*/
        /* Enable RTC Clock */
        __HAL_RCC_RTC_ENABLE();

        if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
        {
            /* Initialization Error */
            printf("RTC init error!\n");
        }
//        stm32_Inrtc_setTime(time);

        /*Enable Direct Read of the calendar registers (not through Shadow) */
        HAL_RTCEx_EnableBypassShadow(&RtcHandle);

        HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, RTC_FIRST_FLAG_DR0);
        printf("RTC init Firstly...\n\r");
    }
    else
    {
         printf("RTC init...\n\r");
    }

    HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle);
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RtcHandle, RTC_FLAG_WUTF);

    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);
    /* Clear RTC Alarm Flag */
    __HAL_RTC_ALARM_CLEAR_FLAG( &RtcHandle, RTC_FLAG_ALRAF);    

    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);
    /* Clear RTC Alarm Flag */
    __HAL_RTC_ALARM_CLEAR_FLAG( &RtcHandle, RTC_FLAG_ALRBF);

    /*##-3- Configure the NVIC for RTC Alarm ###################################*/
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x0, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);

#ifdef RTC_CALIB_OUT_EN
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();

        HAL_RTCEx_SetCalibrationOutPut(&RtcHandle, RTC_CALIBOUTPUT_1HZ);
    }
#endif
}

/***************************************************************************************************
*\Function      stm32_Inrtc_setTime
*\Description   ����DECʱ��
*\Parameter     SetT
*\Return        bool
*\Note          1����Ϊ������ֻ���ģ��������� 
*               2������ʱ��ʱ ������Զ����� ����ʹ��ƫ���������벿��
*\Log           2018.05.07    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_Inrtc_setTime(TIME_SubSec SetT)
{
    RTC_TimeTypeDef times;
    RTC_DateTypeDef date;
    TIME time = SetT.T;

    if (!TimeDECCheck(&time))
    {
        return FALSE;
    }
    /*ʱ���ʽ*/
    times.Seconds   = time.Second;
    times.Minutes   = time.Minute;
    times.Hours     = time.Hour;
    times.TimeFormat = RTC_HOURFORMAT_24;
    times.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
    times.StoreOperation = RTC_STOREOPERATION_RESET;

    /*��ȡ���ڼ�*/
    date.WeekDay    = TimeDECWeekDay(time) + 1;
    date.Date       = time.Day;
    date.Month      = time.Month;
    date.Year       = time.Year;

    if (HAL_RTC_SetDate(&RtcHandle, &date, RTC_FORMAT_BIN) != HAL_OK)
    {
        return FALSE;
    }

    if(HAL_RTC_SetTime(&RtcHandle, &times, RTC_FORMAT_BIN) != HAL_OK)
    {
        return FALSE;
    }

    if (stm32_Inrtc_msShift((s16)SetT.SubSecs) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      stm32_Inrtc_getTime
*\Description   
*\Parameter     getT
*\Return        bool
*\Note          
*\Log           2018.05.07    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_Inrtc_getTime(TIME_SubSec* getT)
{
    RTC_TimeTypeDef times;
    RTC_DateTypeDef date;
    u32 first_read;
#if (N_PREDIV_S != 10)
    #error "you must understand your set value!"
#else
    /* Get Time and Date*/
    HAL_RTC_GetTime(&RtcHandle, &times, RTC_FORMAT_BIN );

    //��������Ϊ�˱�֤ʱ��û�з������䣬�ô���ǰ����1��subsecond�����ڿ���ִ�����
    /* make sure it is correct due to asynchronus nature of RTC*/
    do
    {
        first_read = times.SubSeconds;
        HAL_RTC_GetDate( &RtcHandle, &date, RTC_FORMAT_BIN );
        HAL_RTC_GetTime( &RtcHandle, &times, RTC_FORMAT_BIN );
    } while (first_read != times.SubSeconds);
#endif
    getT->T.Second    = times.Seconds;
    getT->T.Hour      = times.Hours;
    getT->T.Minute    = times.Minutes;

    getT->T.Day       = date.Date;
    getT->T.Month     = date.Month;
    getT->T.Year      = date.Year;

    //�������ƽ�ƣ��˴�����ֵ���ܲ���ȷ
    getT->SubSecs = (u16)((times.SecondFraction - times.SubSeconds) * (uint32_t)1000 /(times.SecondFraction + 1));//���벿��
    /*���Ϸ���*/
    if (!TimeDECCheck(&getT->T))
    {
        return FALSE;
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      stm32_Inrtc_EnWakeup
*\Description   ʹ���Զ�����
*\Parameter     ms
*\Return        bool
*\Note          1ms-32�� ������ò�Ҫ��̫С�ļ�� ��Ҫ�ж�����������жϴ�����
*\Log           2018.05.08    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_Inrtc_EnWakeup(u16 ms)
{
    if (!(MIN_WU_TIME <= ms && ms <= MAX_WU_TIME))
    {
        return FALSE;
    }

    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RtcHandle, RTC_FLAG_WUTF);

    HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, mSEC_2_WU_CNT(ms), RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_Inrtc_DisWakeup
*\Description   �����Զ�����
*\Parameter     
*\Return        bool
*\Note          
*\Log           2018.05.08    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_Inrtc_DisWakeup(void)
{
    if (HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle) != HAL_OK)
    {
        return FALSE;
    }

    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RtcHandle, RTC_FLAG_WUTF);

    return TRUE;
}

/***************************************************************************************************
*\Function      stm32_Inrtc_DisAlarm
*\Description   
*\Parameter     alarm
*\Return        bool
*\Note          
*\Log           2018.05.08    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_Inrtc_DisAlarm(AlarmType alarm)
{
    if (alarm == RTC_ALARMA)
    {
        HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);
        /* Clear RTC Alarm Flag */
        __HAL_RTC_ALARM_CLEAR_FLAG( &RtcHandle, RTC_FLAG_ALRAF);    
    }
    else
    {
        HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);
        /* Clear RTC Alarm Flag */
        __HAL_RTC_ALARM_CLEAR_FLAG( &RtcHandle, RTC_FLAG_ALRBF);
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      stm32_Inrtc_EnAlarm
*\Description   
*\Parameter     alarm
*\Parameter     timeout����λms
*\Return        bool
*\Note          1��ʹ������ʱ����޸�ʱ�� ���ܻ���ʱ��
*\Log           2018.05.08    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_Inrtc_EnAlarm(AlarmType alarm, u32 timeout)
{
    RTC_AlarmTypeDef salarmstructure;
    u32 sub_sec = timeout % 1000;//���벿��
    u32 nsec = timeout / 1000;//���벿��
    u32 sub_sec_reg = 0;//�����Ӧ�ļ���tick
    TIME_SubSec* time;

    if (alarm == RTC_ALARMA)
    {
        salarmstructure.Alarm = RTC_ALARM_A;
        time = &RTCAlarmA_Last;
    }
    else
    {
        salarmstructure.Alarm = RTC_ALARM_B;
        time = &RTCAlarmB_Last;
    }

    if (timeout < 2)
    {
        timeout = 2;
    }

    salarmstructure.AlarmMask = RTC_ALARMMASK_NONE;//�������ʾ�ĸ���Ŀ����Ҫƥ�䣬������ڲ���Ҫ��ѡ�����ڣ��˴�������Ҫȫ��ƥ�� 
    salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;//����
    salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;//����Ҳ��Ҫȫ��ƥ��

    stm32_Inrtc_getTime(time);
    time->SubSecs += sub_sec;//���벿�����
    if (time->SubSecs > 1000)
    {
        nsec += 1;
        time->SubSecs %= 1000;
    }
    TimeDECAddSecond(&time->T, nsec);
    sub_sec_reg = PREDIV_S - PREDIV_S * time->SubSecs / 1000;

    salarmstructure.AlarmDateWeekDay = time->T.Day;
    salarmstructure.AlarmTime.Hours = time->T.Hour;
    salarmstructure.AlarmTime.Minutes = time->T.Minute;
    salarmstructure.AlarmTime.Seconds = time->T.Second;
    salarmstructure.AlarmTime.SubSeconds = sub_sec_reg%PREDIV_S;//��ֹ����

    if(HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure,RTC_FORMAT_BIN) != HAL_OK)
    {
        return FALSE;
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      stm32_Inrtc_msShift
*\Description   ����ƫ�� Add or Subtract x millisecond for RTC
*\Parameter     nMs   +100: positive shift 100ms; -100: negative shift 100ms.
*\Return        bool
*\Note          Assume the RTC=03:25:32.500,
*                   to 03:25:32.600: performs a positive shift of 100ms, call "rtc_SetMs(100)";
*                   to 03:25:32.400: performs a negative shift of 100ms, call "rtc_SetMs(-100)".
*\Log           2018.05.08    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_Inrtc_msShift(s16 nMs)
{
    //s16    nWaitCnt;
    u16    wShiftSubFS;

    if (Abs(nMs) >= 1000 || nMs == 0)
    {
        return FALSE;
    }

    if (0 < nMs) /*Add x ms for current RTC time*/
    {
        /* Convert millisecond to the value,which will be add SUBFS[14:0] in RTC_SHIFTRx register */
        wShiftSubFS = (u32)(1000 - nMs) * (PREDIV_S + 1) / 1000;
        if (HAL_OK != HAL_RTCEx_SetSynchroShift(&RtcHandle, RTC_SHIFTADD1S_SET, wShiftSubFS))
        {
            return FALSE; /* Set Shift failed */
        }
    }
    else /*Subtract x ms for current RTC time*/
    {
        /* Convert millisecond to the value,which will be add SUBFS[14:0] in RTC_SHIFTRx register */
        wShiftSubFS =  (u32)(-1 * nMs) * (PREDIV_S + 1) / 1000;
        if (HAL_OK != HAL_RTCEx_SetSynchroShift(&RtcHandle, RTC_SHIFTADD1S_RESET, wShiftSubFS))
        {
            return FALSE; /* Set Shift failed */
        }  
    }

    return TRUE;
    /* Waiting until the shift operation is finished. */
    //nWaitCnt = 0x7FFF;
    //while ((RtcHandle.Instance->ISR & RTC_FLAG_SHPF) && (0 < nWaitCnt))
    //{
    //    --nWaitCnt;
    //}

    //if (0 < nWaitCnt)
    //{
    //    return TRUE; /* OK */
    //}
    //else
    //{
    //    return FALSE; /* Shift operation have not completed */
    //}
}

