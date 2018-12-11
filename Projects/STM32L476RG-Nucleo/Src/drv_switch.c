/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			drv_switch.c
*\Description   输出开关量控制结构
*\Note          
*\Log           2016.07.08    Ver 1.0   Job
*               创建文件。
***************************************************************************************************/
#include "drv_switch.h"

/***************************************************************************************************
*\Function      rt_switch_on
*\Description   打开switch
*\Parameter     sw
*\Return        void
*\Note          
*\Log           2016.07.08    Ver 1.0    Job
*               创建函数。
***************************************************************************************************/
void stm32_switch_on(switch_type* sw)
{
    if (sw == NULL)
    {
        return;
    }
    if (sw->valid)
    {
        HAL_GPIO_WritePin(sw->gpio, sw->pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(sw->gpio, sw->pin, GPIO_PIN_RESET);
    }
}
/***************************************************************************************************
*\Function      rt_switch_off
*\Description   关闭输出
*\Parameter     sw
*\Return        void
*\Note          
*\Log           2016.07.08    Ver 1.0    Job
*               创建函数。
***************************************************************************************************/
void stm32_switch_off(switch_type* sw)
{
    if (sw == NULL)
    {
        return;
    }
    if (sw->valid)
    {
        HAL_GPIO_WritePin(sw->gpio, sw->pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(sw->gpio, sw->pin, GPIO_PIN_SET);
    }
}
/***************************************************************************************************
*\Function      rt_switch_evert
*\Description   反转输出
*\Parameter     sw
*\Return        void
*\Note          
*\Log           2016.07.08    Ver 1.0    Job
*               创建函数。
***************************************************************************************************/
void stm32_switch_evert(switch_type* sw)
{
    if (sw == NULL)
    {
        return;
    }
    HAL_GPIO_TogglePin(sw->gpio, sw->pin);
}
/***************************************************************************************************
*\Function      rt_switch_init
*\Description   
*\Parameter     gpio,端口
*\Parameter     pin，引脚
*\Parameter     valid,on时的有效电平
*\Return        void
*\Note          
*\Log           2016.07.08    Ver 1.0    Job
*               创建函数。
***************************************************************************************************/
void stm32_switch_init(switch_type* sw)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_TypeDef* gpio = sw->gpio;
    u16 pin = sw->pin;

    if (sw == NULL)
    {
        return;
    }

    //使能端口时钟
    switch((u32)gpio)
    {
    case GPIOA_BASE:
        __HAL_RCC_GPIOA_CLK_ENABLE();
        break;
    case GPIOB_BASE:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
    case GPIOC_BASE:
        __HAL_RCC_GPIOC_CLK_ENABLE();
        break;
    case GPIOD_BASE:
        __HAL_RCC_GPIOD_CLK_ENABLE();
        break;
    case GPIOE_BASE:
        __HAL_RCC_GPIOE_CLK_ENABLE();
        break;
    case GPIOF_BASE:
        __HAL_RCC_GPIOF_CLK_ENABLE();
        break;
    case GPIOG_BASE:
        __HAL_RCC_GPIOG_CLK_ENABLE();
        break;
    case GPIOH_BASE:
        __HAL_RCC_GPIOH_CLK_ENABLE();
        break;
    /*case GPIOI_BASE:
        __HAL_RCC_GPIOI_CLK_ENABLE();
        break;
    case GPIOJ_BASE:
        __HAL_RCC_GPIOJ_CLK_ENABLE();
        break;
    case GPIOK_BASE:
        __HAL_RCC_GPIOK_CLK_ENABLE();
        break;*/
    default:
        printf("gpio clk error!\n");
        break;
    }

    //初始化gpio引脚
    GPIO_InitStruct.Pin   = pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(gpio, &GPIO_InitStruct); 

    //默认关闭输出
    stm32_switch_off(sw);
}
