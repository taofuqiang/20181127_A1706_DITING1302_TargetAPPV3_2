/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          low_power.c
*\Description   
*\Note          
*\Log           2018.05.23    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "low_power.h"
#include "app_board.h"
#include "lora_pp.h"
#include "rtc.h"
static bool SysClockRecoverFlag = FALSE;//系统时钟恢复标志

/***************************************************************************************************
*\Function      SYSCLKConfigRecover
*\Description   系统时钟恢复
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.23    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static void SYSCLKConfigRecover(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    uint32_t pFLatency = 0;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Get the Oscillators configuration according to the internal RCC registers */
    HAL_RCC_GetOscConfig(&RCC_OscInitStruct);

    /* Enable PLL */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {

    }

    /* Get the Clocks configuration according to the internal RCC registers */
    HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);

    /* Select PLL as system clock source and keep HCLK, PCLK1 and PCLK2 clocks dividers as before */
    RCC_ClkInitStruct.ClockType     = RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInitStruct.SYSCLKSource  = RCC_SYSCLKSOURCE_PLLCLK;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency) != HAL_OK)
    {

    }
}

/***************************************************************************************************
*\Function      stm32_lowpower_enter
*\Description   进入低功耗
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.23    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void stm32_lowpower_enter(void)
{
    BACKUP_PRIMASK();
    DISABLE_IRQ( );
    //IO低功耗操作 todo
    stm32_spi_deinit(LoRa_Dev.SPI);
    SysClockRecoverFlag = FALSE;
    RESTORE_PRIMASK();
    ENABLE_IRQ();
    //进入休眠
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
}

/***************************************************************************************************
*\Function      stm32_lowpower_exit
*\Description   退出低功耗时执行
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.23    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void stm32_lowpower_exit(void)
{
    if (SysClockRecoverFlag)
    {
        return;
    }

    //系统时钟
    SYSCLKConfigRecover();
    //IO恢复 todo

    stm32_spi_init(LoRa_Dev.SPI);

    SysClockRecoverFlag = TRUE;

}
