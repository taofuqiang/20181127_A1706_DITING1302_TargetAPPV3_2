/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          watchdog.c
*\Description   
*\Note          
*\Log           2015.05.30    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#include "drv_watchdog.h"
#include "stm32l4xx.h"
#include "stdio.h"
#include "stm32l4xx_hal.h"
#include "stm32_Inrtc.h"

#define WATCHDOG_TIMER_INTERVAL     (1000)//ι�����1000ms
#define AUTORESET_TIMER_INTERVAL    (60 * 30)//30����

uint32_t WatchdogTimer = 0;
static IWDG_HandleTypeDef IwdgHandle;
static TIME_SubSec LastTim = {INIT_TIME, 0};
/***************************************************************************************************
*\Function      watchdog_init
*\Description   ���Ź���ʼ��
*\Parameter     8s��λ
*\Return        void
*\Note          
*\Log           2015.05.30    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void  watchdog_init(void)
{
    RCC_OscInitTypeDef oscinit = {0};

    /* Enable LSI Oscillator */
    oscinit.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    oscinit.LSIState = RCC_LSI_ON;
    oscinit.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&oscinit)!= HAL_OK)
    {
        printf("LSI init fail!\n");
    }

    /*##-3- Configure the IWDG peripheral ######################################*/
    //LSIԼ����32000
    /* Set counter reload value to obtain 2000ms IWDG TimeOut.
    IWDG counter clock Frequency = LsiFreq / 32,
    Counter Reload Value = 2000ms / IWDG counter clock period
    = 2s / (1/LsiFreq/32)
    = LsiFreq / (16)
    = LsiFreq / 16
    ע��reloadֵ���ܴ���0xFFF*/
    IwdgHandle.Instance = IWDG;

    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_128;//IWDG_PRESCALER_256;//IWDG_PRESCALER_64;          //�����Ź�����Ϊ16s
    IwdgHandle.Init.Reload    = 32000 / 256 * 32;//32000 / 128 * 16;//16S   32000 / 64 * 8;//8S
    IwdgHandle.Init.Window    = IWDG_WINDOW_DISABLE;

    if (HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
    {
        /* Initialization Error */
        printf("watchdog init fail!\n");
    }
}

/***************************************************************************************************
*\Function      Watchdog_Refresh
*\Description   ι������
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.23    Ver 1.0    Job               
				����������
***************************************************************************************************/
void Watchdog_Refresh(void)
{
    HAL_IWDG_Refresh(&IwdgHandle);
}
/***************************************************************************************************
*\Function      Watchdog_Periodic_Handle
*\Description   ι������
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.05.30    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void Watchdog_Periodic_Handle(uint32_t localtime)
{
    if (localtime - WatchdogTimer >= WATCHDOG_TIMER_INTERVAL)
    {
        WatchdogTimer = localtime;
        //��ʱι��   1Sһ�� 
        HAL_IWDG_Refresh(&IwdgHandle);
    }
}

/***************************************************************************************************
*\Function      AutoReset_Periodic_Handle
*\Description   �Զ���������
*\Parameter     localtime
*\Return        void
*\Note          30��������һ��
*\Log           2015.05.30    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
extern void system_reset(void);
void AutoReset_Periodic_Handle(void)
{
    TIME_SubSec cur;
    TIME_SubSec initT = {INIT_TIME, 0};

    stm32_Inrtc_getTime(&cur);

    if (memcmp(&LastTim, &initT, sizeof(TIME_SubSec)) == 0)
    {
        LastTim = cur;
        return;
    }
    if (TimeDECSubTime(cur.T, LastTim.T) > AUTORESET_TIMER_INTERVAL)
    {
        printf("System will reset Every %d Min!\n", AUTORESET_TIMER_INTERVAL / (60));
        HAL_Delay(1000);
        //        NVIC_SystemReset();
        system_reset();
    }
}

u8 SysRstFlag = 0;//��λ��־��¼
//�õ�ϵͳ��λ�ķ�ʽ,���Ź�&���&����
void GetRestFlag(void)
{
    //FireWall��λ��־
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_FWRST) != RESET)
    {
        SysRstFlag = 1;
        //���縴λ
        printf("FireWall RST!\n\r");
    }
    //���Ź�
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
    {
        SysRstFlag = 2;
        //���Ź�
        printf("WatchDog RST!\n\r");
    }
    //�����λ��־
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
    {
        SysRstFlag = 3;
        //software��λ
        printf("SoftWare RST!\n\r");
    }	  	
    //NRST
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
        SysRstFlag = 4;
        //NRst��λ
        printf("PINRST RST!\n\r");
    }
    //WWDGRST
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)
    {
        SysRstFlag = 5;
        //WWDGRST��λ
        printf("WWDGRST RST!\n\r");
    }
    //LPWRRST
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET)
    {
        SysRstFlag = 6;
        //LPWRRST��λ
        printf("Low Power RST!\n\r");
    }
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) != RESET)
    {
        SysRstFlag = 7;
        //Option Byte Loader��λ
        printf("Option Byte Loader RST!\n\r");
    }
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET)
    {
        SysRstFlag = 8;
        //BORRST��λ
        printf("BOR RST!\n\r");
    }
    else
    {
        SysRstFlag = 9;
        printf("Other RST!\n\r");
    }
    __HAL_RCC_CLEAR_RESET_FLAGS();//�����λ��־
}
