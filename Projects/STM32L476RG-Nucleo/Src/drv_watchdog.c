/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          watchdog.c
*\Description   
*\Note          
*\Log           2015.05.30    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#include "drv_watchdog.h"
#include "stm32l4xx.h"
#include "stdio.h"
#include "stm32l4xx_hal.h"
#include "stm32_Inrtc.h"

#define WATCHDOG_TIMER_INTERVAL     (1000)//喂狗间隔1000ms
#define AUTORESET_TIMER_INTERVAL    (60 * 30)//30分钟

uint32_t WatchdogTimer = 0;
static IWDG_HandleTypeDef IwdgHandle;
static TIME_SubSec LastTim = {INIT_TIME, 0};
/***************************************************************************************************
*\Function      watchdog_init
*\Description   看门狗初始化
*\Parameter     8s复位
*\Return        void
*\Note          
*\Log           2015.05.30    Ver 1.0    张波
*               创建函数。
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
    //LSI约等于32000
    /* Set counter reload value to obtain 2000ms IWDG TimeOut.
    IWDG counter clock Frequency = LsiFreq / 32,
    Counter Reload Value = 2000ms / IWDG counter clock period
    = 2s / (1/LsiFreq/32)
    = LsiFreq / (16)
    = LsiFreq / 16
    注意reload值不能大于0xFFF*/
    IwdgHandle.Instance = IWDG;

    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_128;//IWDG_PRESCALER_256;//IWDG_PRESCALER_64;          //将看门狗更改为16s
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
*\Description   喂狗动作
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.23    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void Watchdog_Refresh(void)
{
    HAL_IWDG_Refresh(&IwdgHandle);
}
/***************************************************************************************************
*\Function      Watchdog_Periodic_Handle
*\Description   喂狗任务
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.05.30    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void Watchdog_Periodic_Handle(uint32_t localtime)
{
    if (localtime - WatchdogTimer >= WATCHDOG_TIMER_INTERVAL)
    {
        WatchdogTimer = localtime;
        //定时喂狗   1S一次 
        HAL_IWDG_Refresh(&IwdgHandle);
    }
}

/***************************************************************************************************
*\Function      AutoReset_Periodic_Handle
*\Description   自动重启任务
*\Parameter     localtime
*\Return        void
*\Note          30分钟重启一次
*\Log           2015.05.30    Ver 1.0    张波
*               创建函数。
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

u8 SysRstFlag = 0;//复位标志记录
//得到系统复位的方式,看门狗&软件&掉电
void GetRestFlag(void)
{
    //FireWall复位标志
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_FWRST) != RESET)
    {
        SysRstFlag = 1;
        //掉电复位
        printf("FireWall RST!\n\r");
    }
    //看门狗
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
    {
        SysRstFlag = 2;
        //看门狗
        printf("WatchDog RST!\n\r");
    }
    //软件复位标志
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
    {
        SysRstFlag = 3;
        //software复位
        printf("SoftWare RST!\n\r");
    }	  	
    //NRST
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
        SysRstFlag = 4;
        //NRst复位
        printf("PINRST RST!\n\r");
    }
    //WWDGRST
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)
    {
        SysRstFlag = 5;
        //WWDGRST复位
        printf("WWDGRST RST!\n\r");
    }
    //LPWRRST
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET)
    {
        SysRstFlag = 6;
        //LPWRRST复位
        printf("Low Power RST!\n\r");
    }
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) != RESET)
    {
        SysRstFlag = 7;
        //Option Byte Loader复位
        printf("Option Byte Loader RST!\n\r");
    }
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET)
    {
        SysRstFlag = 8;
        //BORRST复位
        printf("BOR RST!\n\r");
    }
    else
    {
        SysRstFlag = 9;
        printf("Other RST!\n\r");
    }
    __HAL_RCC_CLEAR_RESET_FLAGS();//清楚复位标志
}
