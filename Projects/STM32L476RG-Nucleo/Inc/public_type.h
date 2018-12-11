/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          public_type.h
*\Description   
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#ifndef _PUBLIC_TYPE_H
#define _PUBLIC_TYPE_H
#include "stm32l4xx.h"
#include "stdio.h"
#include "string.h"

typedef int64_t  s64;
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int64_t sc64;
typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef __IO int64_t  vs64;
typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int64_t vsc64;
typedef __I int32_t vsc32;  /*!< Read Only */
typedef __I int16_t vsc16;  /*!< Read Only */
typedef __I int8_t vsc8;   /*!< Read Only */

typedef uint64_t  u64;
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint64_t uc64;
typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef __IO uint64_t  vu64;
typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint64_t vuc64;
typedef __I uint32_t vuc32;  /*!< Read Only */
typedef __I uint16_t vuc16;  /*!< Read Only */
typedef __I uint8_t vuc8;   /*!< Read Only */


/**************************************************************************************************/
/* 自定义类型定义                                           张波                                  */
/**************************************************************************************************/
typedef enum
{
    FALSE = 0,
    TRUE = !0,
}bool;

#define Max(a,b)    (((a) > (b)) ? (a) : (b))
#define Min(a,b)    (((a) < (b)) ? (a) : (b))
#define Abs(a)      ((a) > 0 ? (a) : -(a))
#define Swap(a, b) {(a)^=(b); (b)^=(a); (a)^=(b);}

/*gpio类型*/
typedef struct GpioType_st
{
    GPIO_TypeDef* gpio;
    u16           gpio_pin;
    u32           Alternate;
}GpioType;

/*中断处理函数*/
typedef void(*pHookHandle)(void *);

typedef struct HookDevType_st
{
    pHookHandle phook;
    void*       para;
}HookDevType;

//中断控制
#define BACKUP_PRIMASK()  u32 primask_bit= __get_PRIMASK()
#define DISABLE_IRQ() __disable_irq()
#define ENABLE_IRQ() __enable_irq()
#define RESTORE_PRIMASK() __set_PRIMASK(primask_bit)

#endif /*_PUBLIC_TYPE_H*/

