/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_lptim.c
*\Description   0~65535ms，最大计时为65秒
*\Note          使用stm32 LPTIM1，此tim可在stop2模式下继续运行 且可以唤醒
*\Note          分频使用1tick 1ms，CC1作为触发中断 相当于闹钟功能
*\Log           2018.05.09    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "stm32_lptim.h"

static LPTIM_HandleTypeDef LptimHandle;

Callback LpTimCmpHandle = NULL;

void LPTIM1_IRQHandler(void)
{
    /* LPTIM in time Base mode */
    HAL_LPTIM_IRQHandler(&LptimHandle); 
}
/***************************************************************************************************
*\Function      HAL_LPTIM_CompareMatchCallback
*\Description   匹配中断处理函数
*\Parameter     hlptim
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim)
{    
    if (LpTimCmpHandle)
    {
        LpTimCmpHandle();
    }
}
/***************************************************************************************************
*\Function      HAL_LPTIM_MspInit
*\Description   硬件资源初始化
*\Parameter     hlptim
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef *hlptim)
{
    /* ## - 1 - Enable LPTIM clock ############################################ */
    __HAL_RCC_LPTIM1_CLK_ENABLE();

    /* ## - 2 - Force & Release the LPTIM Periheral Clock Reset ############### */  
    /* Force the LPTIM Peripheral Clock Reset */
    __HAL_RCC_LPTIM1_FORCE_RESET();

    /* Release the LPTIM Peripheral Clock Reset */
    __HAL_RCC_LPTIM1_RELEASE_RESET();

    /* ## - 4 - Enable and set LPTIM Interrupt to the high priority ######## */
    HAL_NVIC_SetPriority(LPTIM1_IRQn, 8, 0);
    HAL_NVIC_EnableIRQ(LPTIM1_IRQn);
}

/***************************************************************************************************
*\Function      LSI_ClockEnable
*\Description   LSI时钟是否使能
*\Parameter     
*\Return        HAL_StatusTypeDef
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static HAL_StatusTypeDef LSI_ClockEnable(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    /* Enable LSI clock */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    return (HAL_RCC_OscConfig(&RCC_OscInitStruct));
}
/***************************************************************************************************
*\Function      stm32_lptim_init
*\Description   时钟使用LSI 32kHz
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
void stm32_lptim_init(void)
{
    /* Clocks structure declaration */
    RCC_PeriphCLKInitTypeDef        RCC_PeriphCLKInitStruct;


    //先检查LSI时钟
    if(LSI_ClockEnable() != HAL_OK)
    {
        printf("LSI is Error\n\r");
        return;
    }

    /* ### - 1 - Re-target the LSE to Clock the LPTIM Counter ################# */
    /* Select the LSE clock as LPTIM peripheral clock */
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPTIM1;
    RCC_PeriphCLKInitStruct.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_LSI;  
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);


    /* ### - 2 - Initialize the LPTIM peripheral ############################## */
    /*
    *  Instance        = LPTIM1
    *  Clock Source    = APB or LowPowerOSCillator (in this example LSI is
    *                    already selected from the RCC stage)
    *  Counter source  = Internal event.
    *  Clock prescaler = 32 
    *  Counter Trigger = Software trigger
    *  Output Polarity = High
    *  Update mode     = Immediate (Registers are immediately updated after any
    *                    write access) 
    */

    LptimHandle.Instance = LPTIM1;

    LptimHandle.Init.Clock.Source    = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    LptimHandle.Init.CounterSource   = LPTIM_COUNTERSOURCE_INTERNAL;
    LptimHandle.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV32;  
    LptimHandle.Init.Trigger.Source  = LPTIM_TRIGSOURCE_SOFTWARE; 
    LptimHandle.Init.OutputPolarity  = LPTIM_OUTPUTPOLARITY_HIGH;
    LptimHandle.Init.UpdateMode      = LPTIM_UPDATE_IMMEDIATE;
    LptimHandle.Init.Input1Source    = LPTIM_INPUT1SOURCE_GPIO;
    LptimHandle.Init.Input2Source    = LPTIM_INPUT2SOURCE_GPIO;

    HAL_LPTIM_DeInit(&LptimHandle);

    /* Initialize LPTIM peripheral according to the passed parameters */
    if (HAL_LPTIM_Init(&LptimHandle) != HAL_OK)
    {
        printf("LPTIM init error\n\r");
        return;
    }

    /* ### - 3 - Start counting in interrupt mode ############################# */
    /*
    *  Period = 0xFFFF,至此 lptim已经启动，从0~0xFFFF一直运行 1tick=1ms
    */
    if (HAL_LPTIM_Counter_Start(&LptimHandle, 0xFFFF) != HAL_OK)
    {
        printf("lptim count error\n\r");
        return;
    }
}

/***************************************************************************************************
*\Function      stm32_lptim_cnt_read
*\Description   返回当前CNT值
*\Parameter     
*\Return        u16
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
u16 stm32_lptim_cnt_read(void)
{
    u16 read_reg = 0;


    read_reg = HAL_LPTIM_ReadCounter(&LptimHandle);
    while (HAL_LPTIM_ReadCounter(&LptimHandle) != read_reg)
    {
        read_reg = HAL_LPTIM_ReadCounter(&LptimHandle);
    }
    return read_reg;
}

/***************************************************************************************************
*\Function      stm32_lptim_cmp_en
*\Description   设置比较值并启动中断
*\Parameter     cmp
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
bool stm32_lptim_cmp_en(u16 cmp)
{
    u32 delay = 0;

    __HAL_LPTIM_CLEAR_FLAG(&LptimHandle, LPTIM_FLAG_CMPOK);
    __HAL_LPTIM_DISABLE_IT(&LptimHandle, LPTIM_IT_CMPM);
    __HAL_LPTIM_COMPARE_SET(&LptimHandle, cmp);
    //必须等待置位才会生效 否则触发中断还是原来的CMP 等待时间大约3ms左右
    while (__HAL_LPTIM_GET_FLAG(&LptimHandle, LPTIM_FLAG_CMPOK) == RESET && delay++ < 0x3FFFF)
    {
    }
    if (delay >= 0x3FFFF)
    {
        printf("set cmp Tout\r\n");
        return FALSE;
    }
    //清中断标志 然后使能
    __HAL_LPTIM_CLEAR_FLAG(&LptimHandle, LPTIM_FLAG_CMPM);
    __HAL_LPTIM_ENABLE_IT(&LptimHandle, LPTIM_IT_CMPM);

    return TRUE;
}

/***************************************************************************************************
*\Function      stm32_lptim_cmp_dis
*\Description   禁能CMP中断
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void stm32_lptim_cmp_dis(void)
{
    __HAL_LPTIM_DISABLE_IT(&LptimHandle, LPTIM_IT_CMPM);
}

/***************************************************************************************************
*\Function      stm32_lptim_cmpfunc_set
*\Description   设置回调函数
*\Parameter     func
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void stm32_lptim_cmpfunc_set(Callback func)
{
    LpTimCmpHandle = func;
}
