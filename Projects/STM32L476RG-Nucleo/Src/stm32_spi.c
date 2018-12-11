/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stm32_spi.c
*\Description   
*\Note          
*\Log           2018.05.05    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "stm32_spi.h"


/***************************************************************************************************
*\Function      SpiFrequency
*\Description   频率换算
*\Parameter     hz
*\Return        u32
*\Note          
*\Log           2018.05.05    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static u32 SpiFrequency(u32 badurate)
{
    u32 SysClkTmp = HAL_RCC_GetSysClockFreq();//外设时钟等于系统时钟 不区分

    /*根据配置选不超过用户配置的波特率的最大波特率*/
    if ((SysClkTmp / badurate) > 128)
    {
        return SPI_BAUDRATEPRESCALER_256;
    }

    if ((SysClkTmp / badurate) > 64)
    {
        return SPI_BAUDRATEPRESCALER_128;
    }

    if ((SysClkTmp / badurate) > 32)
    {
        return SPI_BAUDRATEPRESCALER_64;
    }

    if((SysClkTmp / badurate) > 16)
    {
        return SPI_BAUDRATEPRESCALER_32;
    }

    if ((SysClkTmp / badurate) > 8)
    {
        return SPI_BAUDRATEPRESCALER_16;
    }

    if((SysClkTmp / badurate) > 4)
    {
        return SPI_BAUDRATEPRESCALER_8;
    }

    if ((SysClkTmp / badurate) > 2)
    {
        return SPI_BAUDRATEPRESCALER_4;
    }

    return SPI_BAUDRATEPRESCALER_2;
}
/***************************************************************************************************
*\Function      spi_rcc_enable
*\Description   
*\Parameter     spi
*\Return        void
*\Note          
*\Log           2018.05.05    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static void spi_rcc_enable(SPI_TypeDef* spi)
{
    switch((u32)spi)
    {
    case SPI1_BASE:
        __HAL_RCC_SPI1_CLK_ENABLE();
        break;
    case SPI2_BASE:
        __HAL_RCC_SPI2_CLK_ENABLE();
        break;
    case SPI3_BASE:
        __HAL_RCC_SPI3_CLK_ENABLE();
        break;
    default:
        break;
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
*\Function      stm32_spi_init
*\Description   
*\Parameter     spi_dev
*\Return        void
*\Note          
*\Log           2018.05.05    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void stm32_spi_init(SpiDevType* spi_dev)
{
    GPIO_InitTypeDef initStruct={0};

    spi_dev->hspi.Instance = spi_dev->spi;

    spi_dev->hspi.Init.BaudRatePrescaler = SpiFrequency(spi_dev->Freq);
    spi_dev->hspi.Init.Direction      = SPI_DIRECTION_2LINES;
    spi_dev->hspi.Init.Mode           = SPI_MODE_MASTER;
    spi_dev->hspi.Init.CLKPolarity    = spi_dev->CLKPolarity;
    spi_dev->hspi.Init.CLKPhase       = spi_dev->CLKPhase;
    spi_dev->hspi.Init.DataSize       = spi_dev->DataBit;
    spi_dev->hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;  
    spi_dev->hspi.Init.FirstBit       = spi_dev->FirstBit;
    spi_dev->hspi.Init.NSS            = SPI_NSS_SOFT;
    spi_dev->hspi.Init.TIMode         = SPI_TIMODE_DISABLE;
    spi_dev->hspi.Init.CRCPolynomial     = 7;
    spi_dev->hspi.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
    spi_dev->hspi.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;

    spi_rcc_enable(spi_dev->spi);


    if(HAL_SPI_Init(&spi_dev->hspi) != HAL_OK)
    {
        /* Initialization Error */
        printf("SPI error!\n");
        return;
    }

    /*##-2- Configure the SPI GPIOs */
    initStruct.Mode =GPIO_MODE_AF_PP;
    initStruct.Pull = GPIO_PULLDOWN;
    initStruct.Speed = GPIO_SPEED_HIGH;

    if (spi_dev->SCLK)
    {
        gpio_rcc_enable(spi_dev->SCLK->gpio);
        initStruct.Pin = spi_dev->SCLK->gpio_pin;
        initStruct.Alternate= spi_dev->SCLK->Alternate;
        HAL_GPIO_Init(spi_dev->SCLK->gpio, &initStruct); 
    }
    if (spi_dev->MOSI)
    {
        gpio_rcc_enable(spi_dev->MOSI->gpio);
        initStruct.Pin = spi_dev->MOSI->gpio_pin;
        initStruct.Alternate= spi_dev->MOSI->Alternate;
        HAL_GPIO_Init(spi_dev->MOSI->gpio, &initStruct); 
    }
    if (spi_dev->MISO)
    {
        gpio_rcc_enable(spi_dev->MISO->gpio);
        initStruct.Alternate= spi_dev->MISO->Alternate;
        initStruct.Pin = spi_dev->MISO->gpio_pin;
        HAL_GPIO_Init(spi_dev->MISO->gpio, &initStruct); 
    }
    
    //片选
    stm32_switch_init(spi_dev->SPI_CS);
}

/***************************************************************************************************
*\Function      stm32_spi_sendrecvByte
*\Description   发送或接收一个数据
*\Parameter     spi_dev
*\Parameter     byte
*\Return        u8
*\Note          
*\Log           2018.05.05    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
u16 stm32_spi_aByte(SpiDevType* spi_dev, u16 txData)
{
    u16 rxData ;

    HAL_SPI_TransmitReceive(&spi_dev->hspi, (u8*) &txData, (u8*)&rxData, 1, 0x3FFFF);	

    return rxData;
}

/***************************************************************************************************
*\Function      sm32_spi_deinit
*\Description   
*\Parameter     spi_dev
*\Return        void
*\Note          
*\Log           2018.05.05    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void stm32_spi_deinit(SpiDevType* spi_dev)
{
    GPIO_InitTypeDef initStruct={0};

    HAL_SPI_DeInit(&spi_dev->hspi);

    /*##-1- Reset peripherals ####*/
    if (spi_dev->spi == SPI1)
    {
        __HAL_RCC_SPI1_FORCE_RESET();
        __HAL_RCC_SPI1_RELEASE_RESET();
    }
    else if (spi_dev->spi == SPI2)
    {
        __HAL_RCC_SPI2_FORCE_RESET();
        __HAL_RCC_SPI2_RELEASE_RESET();
    }
    else if (spi_dev->spi == SPI3)
    {
        __HAL_RCC_SPI3_FORCE_RESET();
        __HAL_RCC_SPI3_RELEASE_RESET();
    }
    else
    {
        return;
    }

    initStruct.Mode = GPIO_MODE_OUTPUT_PP;

    if (spi_dev->MOSI)
    {   
        initStruct.Pull =GPIO_NOPULL  ; 
        initStruct.Pin = spi_dev->MOSI->gpio_pin;
        HAL_GPIO_Init(spi_dev->MOSI->gpio, &initStruct);

        HAL_GPIO_WritePin(spi_dev->MOSI->gpio, spi_dev->MOSI->gpio_pin, GPIO_PIN_RESET);
    }
    
    if (spi_dev->MISO)
    {
        initStruct.Pull =GPIO_PULLDOWN; 
        initStruct.Pin = spi_dev->MISO->gpio_pin;
        HAL_GPIO_Init(spi_dev->MISO->gpio, &initStruct);

        HAL_GPIO_WritePin(spi_dev->MISO->gpio, spi_dev->MISO->gpio_pin, GPIO_PIN_RESET);
    }
    
    if (spi_dev->SCLK)
    {
        initStruct.Pull =GPIO_NOPULL;
        initStruct.Pin = spi_dev->SCLK->gpio_pin;
        HAL_GPIO_Init(spi_dev->SCLK->gpio, &initStruct);

        HAL_GPIO_WritePin(spi_dev->SCLK->gpio, spi_dev->SCLK->gpio_pin, GPIO_PIN_RESET);
    }
    
    if (spi_dev->SPI_CS)
    {
        initStruct.Pull = GPIO_PULLUP;
        initStruct.Pin = spi_dev->SPI_CS->pin;
        HAL_GPIO_Init(spi_dev->SPI_CS->gpio, &initStruct);

        HAL_GPIO_WritePin(spi_dev->SPI_CS->gpio, spi_dev->SPI_CS->pin, GPIO_PIN_SET);
    }
}
