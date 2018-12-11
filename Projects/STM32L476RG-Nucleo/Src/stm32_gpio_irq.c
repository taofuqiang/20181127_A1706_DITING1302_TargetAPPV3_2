/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_gpio_irq.c
*\Description   gpio中断函数配置
*\Note          
*\Log           2018.05.11    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "stm32_gpio_irq.h"


static HookDevType GPIO_IRQ_Funcs[16] = {NULL, NULL};


void EXTI0_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_0 );
}

void EXTI1_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_1 );
}

void EXTI2_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_2 );
}

void EXTI3_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_3 );
}

void EXTI4_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_4 );
}


void EXTI9_5_IRQHandler( void )
{  
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_5 );

    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_6 );

    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_7 );

    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_8 );

    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_9 );
}

void EXTI15_10_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_10 );

    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_11 );

    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_12 );

    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_13 );

    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_14 );

    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_15 );
}
/***************************************************************************************************
*\Function      HW_GPIO_GetBitNum
*\Description   获取序号
*\Parameter     gpio_pin
*\Return        u8
*\Note          
*\Log           2018.05.11    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static u8 HW_GPIO_GetBitNum(u16 gpio_pin)
{
    u8 PinPos=0;

    if ( ( gpio_pin & 0xFF00 ) != 0) { PinPos |= 0x8; }
    if ( ( gpio_pin & 0xF0F0 ) != 0) { PinPos |= 0x4; }
    if ( ( gpio_pin & 0xCCCC ) != 0) { PinPos |= 0x2; }
    if ( ( gpio_pin & 0xAAAA ) != 0) { PinPos |= 0x1; }

    return PinPos % 16;
}
/***************************************************************************************************
*\Function      HAL_GPIO_EXTI_Callback
*\Description   回调函数
*\Parameter     GPIO_Pin
*\Return        void
*\Note          
*\Log           2018.05.11    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{
    u8 i = 0;

    i = HW_GPIO_GetBitNum(gpio_pin);

    if (GPIO_IRQ_Funcs[i].phook)
    {
        GPIO_IRQ_Funcs[i].phook(GPIO_IRQ_Funcs[i].para);
    }
}
/***************************************************************************************************
*\Function      stm32_gpio_irq_config
*\Description   
*\Parameter     gpio_pin
*\Parameter     func
*\Parameter     p
*\Return        bool
*\Note          
*\Log           2018.05.11    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
bool stm32_gpio_irq_config(u16 gpio_pin, pHookHandle func, void* p)
{
    u8 i = 0;

    if (func == NULL)
    {
        return FALSE;
    }

    assert_param(IS_GPIO_PIN(gpio_pin));

    i = HW_GPIO_GetBitNum(gpio_pin);
    GPIO_IRQ_Funcs[i].phook = func;
    GPIO_IRQ_Funcs[i].para = p;

    return TRUE;
}
