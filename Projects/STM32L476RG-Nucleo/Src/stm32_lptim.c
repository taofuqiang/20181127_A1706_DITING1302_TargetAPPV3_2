/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_lptim.c
*\Description   0~65535ms������ʱΪ65��
*\Note          ʹ��stm32 LPTIM1����tim����stop2ģʽ�¼������� �ҿ��Ի���
*\Note          ��Ƶʹ��1tick 1ms��CC1��Ϊ�����ж� �൱�����ӹ���
*\Log           2018.05.09    Ver 1.0    Job
*               �����ļ���
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
*\Description   ƥ���жϴ�����
*\Parameter     hlptim
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
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
*\Description   Ӳ����Դ��ʼ��
*\Parameter     hlptim
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
����������
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
*\Description   LSIʱ���Ƿ�ʹ��
*\Parameter     
*\Return        HAL_StatusTypeDef
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
����������
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
*\Description   ʱ��ʹ��LSI 32kHz
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
����������
***************************************************************************************************/
void stm32_lptim_init(void)
{
    /* Clocks structure declaration */
    RCC_PeriphCLKInitTypeDef        RCC_PeriphCLKInitStruct;


    //�ȼ��LSIʱ��
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
    *  Period = 0xFFFF,���� lptim�Ѿ���������0~0xFFFFһֱ���� 1tick=1ms
    */
    if (HAL_LPTIM_Counter_Start(&LptimHandle, 0xFFFF) != HAL_OK)
    {
        printf("lptim count error\n\r");
        return;
    }
}

/***************************************************************************************************
*\Function      stm32_lptim_cnt_read
*\Description   ���ص�ǰCNTֵ
*\Parameter     
*\Return        u16
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
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
*\Description   ���ñȽ�ֵ�������ж�
*\Parameter     cmp
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_lptim_cmp_en(u16 cmp)
{
    u32 delay = 0;

    __HAL_LPTIM_CLEAR_FLAG(&LptimHandle, LPTIM_FLAG_CMPOK);
    __HAL_LPTIM_DISABLE_IT(&LptimHandle, LPTIM_IT_CMPM);
    __HAL_LPTIM_COMPARE_SET(&LptimHandle, cmp);
    //����ȴ���λ�Ż���Ч ���򴥷��жϻ���ԭ����CMP �ȴ�ʱ���Լ3ms����
    while (__HAL_LPTIM_GET_FLAG(&LptimHandle, LPTIM_FLAG_CMPOK) == RESET && delay++ < 0x3FFFF)
    {
    }
    if (delay >= 0x3FFFF)
    {
        printf("set cmp Tout\r\n");
        return FALSE;
    }
    //���жϱ�־ Ȼ��ʹ��
    __HAL_LPTIM_CLEAR_FLAG(&LptimHandle, LPTIM_FLAG_CMPM);
    __HAL_LPTIM_ENABLE_IT(&LptimHandle, LPTIM_IT_CMPM);

    return TRUE;
}

/***************************************************************************************************
*\Function      stm32_lptim_cmp_dis
*\Description   ����CMP�ж�
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
***************************************************************************************************/
void stm32_lptim_cmp_dis(void)
{
    __HAL_LPTIM_DISABLE_IT(&LptimHandle, LPTIM_IT_CMPM);
}

/***************************************************************************************************
*\Function      stm32_lptim_cmpfunc_set
*\Description   ���ûص�����
*\Parameter     func
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
				����������
***************************************************************************************************/
void stm32_lptim_cmpfunc_set(Callback func)
{
    LpTimCmpHandle = func;
}
