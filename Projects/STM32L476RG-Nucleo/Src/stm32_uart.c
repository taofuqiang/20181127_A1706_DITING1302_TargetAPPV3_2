/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_uart.c
*\Description   
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "stm32_uart.h"

/***************************************************************************************************
*\Function      stm32_uart_isr
*\Description   uart 中断处理函数
*\Parameter     num
*\Return        void
*\Note          
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/


void stm32_uart_isr(UartDevType* uart_dev)
{
    u8 data = 0;
    UART_HandleTypeDef huart;

    huart.Instance = uart_dev->uart;


    /*中断接收*/
    if (__HAL_UART_GET_FLAG(&huart, USART_ISR_RXNE))
    {
//        /*接收缓冲区不空，将数据取出至接收缓冲区*/
//        data = (u8)(huart.Instance->RDR);
//        AddByteToBuf(&(uart_dev->rxbuf), data);
			HAL_UART_RxCpltCallback(&huart);//baron
    }

    if (__HAL_UART_GET_FLAG(&huart, UART_FLAG_ORE)) /*溢出错误*/
    {
        __HAL_UART_CLEAR_IT(&huart, UART_CLEAR_OREF);
    }
    if (__HAL_UART_GET_FLAG(&huart, UART_FLAG_NE))/*噪音错误*/
    {
        __HAL_UART_CLEAR_IT(&huart, UART_CLEAR_NEF);
    }
    if (__HAL_UART_GET_FLAG(&huart, UART_FLAG_FE)) /*帧错误*/
    {
        __HAL_UART_CLEAR_IT(&huart, UART_CLEAR_FEF);
    }
}
/***************************************************************************************************
*\Function      gpio_rcc_enable
*\Description   
*\Parameter     gpio
*\Return        void
*\Note          
*\Log           2016.07.11    Ver 1.0    Job
*               创建函数。
***************************************************************************************************/
static void gpio_rcc_enable(GPIO_TypeDef* gpio)
{
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
    default:
        printf("gpio clk error!\n");
        break;
    }
}
/***************************************************************************************************
*\Function      uart_rcc_enable
*\Description   uart时钟
*\Parameter     uart
*\Return        void
*\Note          
*\Log           2016.07.26    Ver 1.0    Job
*               创建函数。
***************************************************************************************************/
static void uart_rcc_enable(USART_TypeDef* uart)
{
    switch ((u32)uart)
    {
    case USART1_BASE:
        __HAL_RCC_USART1_CLK_ENABLE();
        break;
    case USART2_BASE:
        __HAL_RCC_USART2_CLK_ENABLE();
        break;
    case USART3_BASE:
        __HAL_RCC_USART3_CLK_ENABLE();
        break;
    case UART4_BASE:
        __HAL_RCC_UART4_CLK_ENABLE();
        break;
    case UART5_BASE:
        __HAL_RCC_UART5_CLK_ENABLE();
        break;
    default:
        printf("uart rcc error!\n");
        break;
    }
}
/***************************************************************************************************
*\Function      stm32_uart_init
*\Description   初始化uart函数
*\Parameter     uart，uart序号
*\Parameter     baudrate，波特率
*\Return        void
*\Note          1、uart默认初始化为8数据位、1个停止位、无检验
*\              2、在函数外部需使能uart gpio时钟<========
*\              3、232可以忽略ctrlgpio参数
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void stm32_uart_init(UartDevType* uart_dev)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_HandleTypeDef UartHandle;
    IRQn_Type	nvic_irq;

    if (uart_dev == NULL)
    {
        return;
    }

    /*初始化使用的recv缓冲区*/
    RingBufInit(&(uart_dev->rxbuf), uart_dev->RxBuf_addr, uart_dev->RxBuf_size);

    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull      = GPIO_PULLUP;
    GPIO_InitStructure.Speed     = GPIO_SPEED_HIGH;

    //接收脚
    if (uart_dev->RxGpio != NULL)
    {
        gpio_rcc_enable(uart_dev->RxGpio->gpio);
        /*rx引脚初始化*/
        /* Configure USART Rx as input floating */
        GPIO_InitStructure.Pin = (uart_dev->RxGpio)->gpio_pin;
        GPIO_InitStructure.Alternate = (uart_dev->RxGpio)->Alternate;
        HAL_GPIO_Init(uart_dev->RxGpio->gpio, &GPIO_InitStructure);
    }
    //发送脚
    if (uart_dev->TxGpio != NULL)
    {
         gpio_rcc_enable(uart_dev->TxGpio->gpio);
        /* Configure USART Tx as alternate function push-pull */
        GPIO_InitStructure.Pin = uart_dev->TxGpio->gpio_pin;
        GPIO_InitStructure.Alternate = (uart_dev->TxGpio)->Alternate;
        HAL_GPIO_Init(uart_dev->TxGpio->gpio, &GPIO_InitStructure);
    }

    //485控制角
    if (uart_dev->Rs485Ctrl != NULL)
    {
        gpio_rcc_enable(uart_dev->Rs485Ctrl->gpio);

        GPIO_InitStructure.Pin = uart_dev->Rs485Ctrl->gpio_pin;
        GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStructure.Pull  = GPIO_NOPULL;
        GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
        HAL_GPIO_Init(uart_dev->Rs485Ctrl->gpio, &GPIO_InitStructure);
    }

    //uart 时钟
    uart_rcc_enable(uart_dev->uart);

    if (uart_dev->baudrate == 0 || uart_dev->baudrate > 115200)//波特率限制
    {
//        printf("uart baudrate:%d error!\n", uart_dev->baudrate);
    }
    /*##-1- Configure the UART peripheral ######################################*/
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART configured as follows:
    - Word Length = 8 Bits
    - Stop Bit = One Stop bit
    - Parity = None
    - BaudRate = 9600 baud
    - Hardware flow control disabled (RTS and CTS signals) */
    UartHandle.Instance        = uart_dev->uart;

    UartHandle.Init.BaudRate   = uart_dev->baudrate;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;
    UartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; 
   
    HAL_UART_DeInit(&UartHandle);
    if(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
//        printf("uart init error!\n");
    }

    switch ((u32)uart_dev->uart)
    {
    case USART1_BASE:
        nvic_irq = USART1_IRQn;
        break;
    case USART2_BASE:
        nvic_irq = USART2_IRQn;
        break;
    case USART3_BASE:
        nvic_irq = USART3_IRQn;
        break;
    case UART4_BASE:
        nvic_irq = UART4_IRQn;
        break;
    case UART5_BASE:
        nvic_irq = UART5_IRQn;
        break;
    default:
//        printf("uart irq is err!\n");
        break;
    }
    HAL_NVIC_SetPriority(nvic_irq, 0, 1);
    //HAL_NVIC_EnableIRQ(nvic_irq);

    __HAL_UART_CLEAR_IT(&UartHandle, UART_CLEAR_TCF);
    /*配置相应接收中断,接收缓冲区不空*/
    __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
}
/***************************************************************************************************
*\Function      UARTSendByte
*\Description   uart字节发送函数，轮询发送
*\Parameter     uart
*\Parameter     data
*\Return        void
*\Note
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
static bool UARTSendByte(USART_TypeDef* uart, u8 data)
{
    u32 i = 0;
    UART_HandleTypeDef UartHandle;

    UartHandle.Instance = uart;

    uart->TDR =  data;/*  写入数据*/

    while(!__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_TC) && (i++ < 0x9FFFFF));
    
    if (i >= 0x9FFFFF)
    {
        return FALSE;
    }
   
    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_uart_send
*\Description   uart发送函数
*\Parameter     uart
*\Parameter     tx，发送缓冲区
*\Parameter     len，发送数据长度
*\Return        u32，成功发送的数据个数
*\Note
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 stm32_uart_send(UartDevType* uart_dev, u8* tx, u32 len)
{
    UART_HandleTypeDef huart;

    huart.Instance = uart_dev->uart;

    if (uart_dev == NULL)
    {
        return 0;
    }

    //485则控制脚拉低
    if (uart_dev->Rs485Ctrl != NULL)
    {
        HAL_GPIO_WritePin(uart_dev->Rs485Ctrl->gpio, uart_dev->Rs485Ctrl->gpio_pin, GPIO_PIN_RESET);
    }
    {
        u32 j = 0;
        /*轮询方式发送*/
        for (j = 0; j < len; j++)
        {
            UARTSendByte(uart_dev->uart, *tx++);
        }
    }    

    if (uart_dev->Rs485Ctrl != NULL)
    {
        HAL_GPIO_WritePin(uart_dev->Rs485Ctrl->gpio, uart_dev->Rs485Ctrl->gpio_pin, GPIO_PIN_SET);
    }

    return len;
}

/***************************************************************************************************
*\Function      stm32_uart_recv
*\Description   uart接收函数
*\Parameter     uart
*\Parameter     rx，数据接收缓冲区
*\Parameter     len，接收数据长度
*\Parameter     clear, TRUE,表示清楚缓冲区中的数据，FALSE，表示不清楚缓冲区中的数据
*\Return        u32，实际接收的数据长度
*\Note
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 stm32_uart_recv(UartDevType* uart_dev, u8* rx, u32 len, bool clear)
{
    u32 len_tmp = 0;

    if (uart_dev == NULL)
    {
        return 0;
    }

    len_tmp = ReadDataFromBuf(&uart_dev->rxbuf, rx, len, clear);

    return len_tmp;
}
/***************************************************************************************************
*\Function      stm32_uart_len_get
*\Description
*\Parameter     uart
*\Return        u32
*\Note
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 stm32_uart_len_get(UartDevType* uart_dev)
{
    u32 len = 0;

    if (uart_dev == NULL)
    {
        return 0;
    }

    len = GetLenFromBuf(&uart_dev->rxbuf);

    return len;
}
/***************************************************************************************************
*\Function      stm32_uart_del
*\Description   删除缓冲区中的字节数
*\Parameter     uart
*\Parameter     len
*\Return        u32，实际删除的字节数
*\Note
*\Log           2012.08.06    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 stm32_uart_del(UartDevType* uart_dev, u32 len)
{
    u32 len_tmp = 0;

    if (uart_dev == NULL)
    {
        return 0;
    }

    len_tmp = DelDataFromBuf(&uart_dev->rxbuf, len);

    return len_tmp;
}
/***************************************************************************************************
*\Function      stm32_uart_clear
*\Description   清除缓冲区中的数据
*\Parameter     uart
*\Return        bool
*\Note
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool stm32_uart_clear(UartDevType* uart_dev)
{
    if (uart_dev == NULL)
    {
        return FALSE;
    }

    uart_dev->rxbuf.Tail = uart_dev->rxbuf.Head;

    return TRUE;
}

/***************************************************************************************************
*\Function      stm32_uart_disinit
*\Description   取消初始化uart函数
*\Parameter     uart，uart序号
*\Parameter     baudrate，波特率
*\Return        void
*\Note          1、uart默认初始化为8数据位、1个停止位、无检验
*\              2、在函数外部需使能uart gpio时钟<========
*\              3、232可以忽略ctrlgpio参数
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void stm32_uart_disinit(UartDevType* uart_dev)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_HandleTypeDef UartHandle;
    IRQn_Type	nvic_irq;

//    if (uart_dev == NULL)
//    {
//        return;
//    }

    /*初始化使用的recv缓冲区*/
//    RingBufInit(&(uart_dev->rxbuf), uart_dev->RxBuf_addr, uart_dev->RxBuf_size);

//    GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
//    GPIO_InitStructure.Pull      = GPIO_PULLUP;
//    GPIO_InitStructure.Speed     = GPIO_SPEED_HIGH;

    //接收脚
    if (uart_dev->RxGpio != NULL)
    {
        gpio_rcc_enable(uart_dev->RxGpio->gpio);
        /*rx引脚初始化*/
			  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
				GPIO_InitStructure.Pin = (uart_dev->RxGpio)->gpio_pin;
				GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
				GPIO_InitStructure.Pull = GPIO_NOPULL;
			  HAL_GPIO_Init(uart_dev->RxGpio->gpio, &GPIO_InitStructure);
    }
    //发送脚
    if (uart_dev->TxGpio != NULL)
    {
         gpio_rcc_enable(uart_dev->TxGpio->gpio);
				GPIO_InitStructure.Pin = (uart_dev->TxGpio)->gpio_pin;
				GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
				GPIO_InitStructure.Pull = GPIO_NOPULL;
			  HAL_GPIO_Init(uart_dev->TxGpio->gpio, &GPIO_InitStructure);
    }

    //485控制角
//    if (uart_dev->Rs485Ctrl != NULL)
//    {
//        gpio_rcc_enable(uart_dev->Rs485Ctrl->gpio);

//        GPIO_InitStructure.Pin = uart_dev->Rs485Ctrl->gpio_pin;
//        GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
//        GPIO_InitStructure.Pull  = GPIO_NOPULL;
//        GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
//        HAL_GPIO_Init(uart_dev->Rs485Ctrl->gpio, &GPIO_InitStructure);
//    }

    //uart 时钟
    __HAL_RCC_USART1_CLK_DISABLE();//DISABLED   //uart_rcc_enable(uart_dev->uart);

//    if (uart_dev->baudrate == 0 || uart_dev->baudrate > 115200)//波特率限制
//    {
////        printf("uart baudrate:%d error!\n", uart_dev->baudrate);
//    }
    /*##-1- Configure the UART peripheral ######################################*/
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART configured as follows:
    - Word Length = 8 Bits
    - Stop Bit = One Stop bit
    - Parity = None
    - BaudRate = 9600 baud
    - Hardware flow control disabled (RTS and CTS signals) */
//    UartHandle.Instance        = uart_dev->uart;

//    UartHandle.Init.BaudRate   = uart_dev->baudrate;
//    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
//    UartHandle.Init.StopBits   = UART_STOPBITS_1;
//    UartHandle.Init.Parity     = UART_PARITY_NONE;
//    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
//    UartHandle.Init.Mode       = UART_MODE_TX_RX;
//    UartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
//    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; 
   
    HAL_UART_DeInit(&UartHandle);
//    if(HAL_UART_Init(&UartHandle) != HAL_OK)
//    {
////        printf("uart init error!\n");
//    }

    switch ((u32)uart_dev->uart)
    {
    case USART1_BASE:
        nvic_irq = USART1_IRQn;
        break;
    case USART2_BASE:
        nvic_irq = USART2_IRQn;
        break;
    case USART3_BASE:
        nvic_irq = USART3_IRQn;
        break;
    case UART4_BASE:
        nvic_irq = UART4_IRQn;
        break;
    case UART5_BASE:
        nvic_irq = UART5_IRQn;
        break;
    default:
//        printf("uart irq is err!\n");
        break;
    }
   // HAL_NVIC_SetPriority(nvic_irq, 0, 1);
    HAL_NVIC_DisableIRQ(nvic_irq);

//    __HAL_UART_CLEAR_IT(&UartHandle, UART_CLEAR_TCF);
    /*配置相应接收中断,接收缓冲区不空*/
    __HAL_UART_DISABLE_IT(&UartHandle, UART_IT_RXNE);
}
