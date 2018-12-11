
/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_sx1278.c
*\Description   
*\Note          
*\Log           2018.05.05    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "drv_sx1278.h"
#include "math.h"

#define RF_MID_BAND_THRESH  525000000
#define RSSI_OFFSET_LF      -164
#define RSSI_OFFSET_HF      -157

const static double   BWList[] =    \
{78e2, 104e2, 156e2, 208e2, 312e2, 414e2, 625e2, 125e3, 250e3, 500e3};

/***************************************************************************************************
*\Function      DelayMs
*\Description   延时
*\Parameter     nMs
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void DelayMs(u16 nMs)
{
    u16 now = TimerGetCurrentTime();
    u16 diff = 0;

    while (1)
    {
        diff = TimerGetCurrentTime() - now;
        if (diff > nMs)
        {
            break;
        }
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
*\Function      gpio_nvic_irq_get
*\Description   
*\Parameter     pin
*\Return        IRQn_Type
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static IRQn_Type gpio_nvic_irq_get(u16 pin)
{
    IRQn_Type IRQnb = EXTI0_IRQn;

    switch(pin)
    {
    case GPIO_PIN_0:
        IRQnb = EXTI0_IRQn;
        break;
    case GPIO_PIN_1:
        IRQnb = EXTI1_IRQn;
        break;
    case GPIO_PIN_2:
        IRQnb = EXTI2_IRQn;
        break;
    case GPIO_PIN_3:
        IRQnb = EXTI3_IRQn;
        break;
    case GPIO_PIN_4:
        IRQnb = EXTI4_IRQn;
        break;
    case GPIO_PIN_5:
    case GPIO_PIN_6:
    case GPIO_PIN_7:
    case GPIO_PIN_8:
    case GPIO_PIN_9:
        IRQnb = EXTI9_5_IRQn;
        break;
    case GPIO_PIN_10:
    case GPIO_PIN_11:
    case GPIO_PIN_12:
    case GPIO_PIN_13:
    case GPIO_PIN_14:
    case GPIO_PIN_15:
        IRQnb = EXTI15_10_IRQn;
        break;
    default:
        break;
    }

    return IRQnb;
}
/***************************************************************************************************
*\Function      lora_sx1278_reset
*\Description   复位
*\Parameter     lora
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
void lora_sx1278_reset(LoRa_Dev_type* lora)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    stm32_switch_init(lora->ResetPin);
    stm32_switch_on(lora->ResetPin);
    DelayMs(1);
    stm32_switch_off(lora->ResetPin);
    //复位完成配置为浮空输入 降功耗
    GPIO_InitStruct.Pin   = lora->ResetPin->pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(lora->ResetPin->gpio, &GPIO_InitStruct); 
    /* Wait 30ms */
    DelayMs(30);
}

/***************************************************************************************************
*\Function      SX1278WriteBuffer
*\Description   spi写入
*\Parameter     addr
*\Parameter     buffer
*\Parameter     size
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void SX1278SpiWrite(SpiDevType* spi, u8 addr, u8 *buffer, u8 size)
{
    u8    i;

    stm32_switch_on(spi->SPI_CS); /* NSS = 0 */

    stm32_spi_aByte(spi, addr | 0x80 ); /* Bit_7=1 equal write */
    for( i = 0; i < size; i++ )
    {
        stm32_spi_aByte(spi, buffer[i] );
    }

    stm32_switch_off(spi->SPI_CS);  /* NSS = 1 */

    return;	
}


/***************************************************************************************************
*\Function      SX1278ReadBuffer
*\Description   spi读取
*\Parameter     addr
*\Parameter     buffer
*\Parameter     size
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
void SX1278SpiRead(SpiDevType* spi, u8 addr, u8 *buffer, u8 size)
{
    u8 i;

    stm32_switch_on(spi->SPI_CS); /* NSS = 0 */

    stm32_spi_aByte(spi, addr & 0x7F ); /* Bit_7=1 equal write */
    for( i = 0; i < size; i++ )
    {
        buffer[i] = stm32_spi_aByte(spi, 0x00);
    }

    stm32_switch_off(spi->SPI_CS);  /* NSS = 1 */

    return;	
}
/*-------------------------------------------------------------------------*/
static void SX1278Write(LoRa_Dev_type* lora,u8 addr, u8 data)
{
    SX1278SpiWrite(lora->SPI, addr, &data, 1);
}

/*-------------------------------------------------------------------------*/
u8 SX1278Read(LoRa_Dev_type* lora, u8 addr)
{
    u8 data;

    SX1278SpiRead(lora->SPI, addr, &data, 1);

    return data;
}

/*-------------------------------------------------------------------------*/
static void SX1278WriteFifo(LoRa_Dev_type* lora, u8 *p_byBuf, u8 bySize)
{
    SX1278SpiWrite(lora->SPI, 0, p_byBuf, bySize);
}

/*-------------------------------------------------------------------------*/
static void SX1278ReadFifo(LoRa_Dev_type* lora, u8* p_byBuf, u8 bySize)
{
    SX1278SpiRead(lora->SPI, 0, p_byBuf, bySize);
}
/***************************************************************************************************
*\Function      SX1276SetChannel
*\Description   
*\Parameter     freq
*\Return        void
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
void SX1278SetChannel(LoRa_Dev_type* lora, u32 freq)
{
    u32 channel;

    SX_FREQ_TO_CHANNEL(channel, freq );

    SX1278Write(lora, REG_FRFMSB, ( u8 )( ( channel >> 16 ) & 0xFF ) );
    SX1278Write(lora, REG_FRFMID, ( u8 )( ( channel >> 8 ) & 0xFF ) );
    SX1278Write(lora, REG_FRFLSB, ( u8 )(  channel & 0xFF ) );
}
/***************************************************************************************************
*\Function      SX1278_RxChainCalibration
*\Description   校准接收
*\Parameter     lora
*\Return        void
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void SX1278_RxChainCalibration(LoRa_Dev_type* lora)
{
    u8 regPaConfigInitVal;
    u32 initialFreq;
    u32 channel;

    // Save context
    regPaConfigInitVal = SX1278Read(lora, REG_PACONFIG);

    channel = ( ( ( u32 )SX1278Read(lora, REG_FRFMSB ) << 16 ) |
        ( ( u32 )SX1278Read(lora, REG_FRFMID ) << 8 ) |
        ( ( u32 )SX1278Read(lora, REG_FRFLSB ) ) );

    SX_CHANNEL_TO_FREQ(channel, initialFreq);

    // Cut the PA just in case, RFO output, power = -1 dBm
    SX1278Write(lora, REG_PACONFIG, 0x00 );

    // Launch Rx chain calibration for LF band
    SX1278Write(lora, REG_IMAGECAL, ( SX1278Read(lora, REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
    while( ( SX1278Read(lora, REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
    {
    }

    // Sets a Frequency in HF band
    //SX1276SetChannel( 868000000 );

    //// Launch Rx chain calibration for HF band
    //SX1278Write(lora, REG_IMAGECAL, ( SX1278Read(lora, REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
    //while( ( SX1278Read(lora, REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
    //{
    //}

    // Restore context
    SX1278Write(lora, REG_PACONFIG, regPaConfigInitVal );
    SX1278SetChannel(lora, initialFreq);
}
/***************************************************************************************************
*\Function      SX1278SetOpMode
*\Description   设置工作模式
*\Parameter     spi
*\Parameter     opMode
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void SX1278SetOpMode(LoRa_Dev_type* lora, u8 opMode )
{
    //static u8 opModePrev = RF_OPMODE_STANDBY;

    //if (opMode != opModePrev)
    //{
        //opModePrev = opMode;
        SX1278Write(lora, REG_LR_OPMODE, 
            (SX1278Read(lora, REG_LR_OPMODE) & RF_OPMODE_MASK) | opMode );
    //}
}
/***************************************************************************************************
*\Function      SX1278SetStby
*\Description   
*\Parameter     lora
*\Return        void
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static void SX1278SetStby(LoRa_Dev_type* lora)
{
    TimerStop(&lora->RadioTimer);
    lora->State = RF_IDLE;

    SX1278SetOpMode(lora, RF_OPMODE_STANDBY);
}
/***************************************************************************************************
*\Function      SX1278SetPAOutput
*\Description   设置PA输出
*\Parameter     ePAOutput
*\Return        void
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void SX1278SetPAOutput(LoRa_Dev_type* lora, PA_OUTPUT_PIN_TypeDef ePAOutput)
{
    SX1278Write(lora, REG_LR_PACONFIG,
        (SX1278Read(lora, REG_LR_PACONFIG) & RFLR_PACONFIG_PASELECT_MASK) | 
        ePAOutput );

    if (PA_OUTPUT_PIN_BOOST != ePAOutput)
    {
        /* High Power settings must be turned off when using PA_LF */
        SX1278Write(lora, REG_PADAC,
            (SX1278Read(lora, REG_PADAC) & RF_PADAC_20DBM_MASK) |
            RF_PADAC_20DBM_OFF );
    }

    return;
}

/***************************************************************************************************
*\Function      SX1278SetTxPower
*\Description   设置发送功率
*\Parameter     chPower
*\Return        void
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void SX1278SetTxPower(LoRa_Dev_type* lora, s8 chPower)
{
    u8  paConfig, paDac;

    /* Set power to LoRa Radio */	
    paConfig = SX1278Read(lora, REG_PACONFIG);
    paDac = SX1278Read(lora, REG_PADAC);

    /* Set <6:4>=7: Select max output power. */
    paConfig = (paConfig & RF_PACONFIG_MAX_POWER_MASK) |0x70;

    if ((paConfig & RF_PACONFIG_PASELECT_PABOOST) == RF_PACONFIG_PASELECT_PABOOST)
    {
        if (chPower > 17)
        {
            paDac = (paDac & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_ON;
        }
        else
        {
            paDac = (paDac & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_OFF;
        }

        if ((paDac & RF_PADAC_20DBM_ON) == RF_PADAC_20DBM_ON)
        {
            if (chPower < 5)
            {
                chPower = 5;
            }
            if (chPower > 20)
            {
                chPower = 20;
            }
            paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) |
                (u8)((u16)(chPower - 5) & 0x0F);
        }
        else
        {
            if (chPower < 2)
            {
                chPower = 2;
            }
            if (chPower > 17)
            {
                chPower = 17;
            }
            paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) | 
                (u8)((u16)(chPower - 2) & 0x0F);
        }
    }
    else
    {
        if (chPower < -1)
        {
            chPower = -1;
        }
        if (chPower > 14)
        {
            chPower = 14;
        }
        paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) | 
            (u8)((u16)(chPower + 1) & 0x0F);
    }

    SX1278Write(lora, REG_PACONFIG, paConfig);
    SX1278Write(lora, REG_PADAC, paDac);

    /* Set OCP(Over Current Protection)=160mA, 
    the current is 120mA when RFOP=+20 dBm, on PA_BOOST */
    //SX1278Write(lora, REG_LR_OCP, 0x33);	

    return;	
}

/***************************************************************************************************
*\Function      SX1278_RadioTimer_Handler
*\Description   超时处理函数
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void SX1278_RadioTimer_Handler(void* p)
{
    LoRa_Dev_type* lora = (LoRa_Dev_type*) p;

    if (lora->LowPowerExitFunc)
    {
        lora->LowPowerExitFunc();
    }

    //发送超时
    if (lora->State == RF_TX_RUNNING)
    {
        lora->State = RF_IDLE;
        SX1278SetOpMode(lora, RF_OPMODE_SLEEP);

        if (lora->Radio.TxTimeout)
        {
            lora->Radio.TxTimeout(lora->Radio.para);
        }
    }
    else if (lora->State == RF_RX_RUNNING)
    {
        if (lora->RadioSetting.RxContinuous == FALSE)
        {
            lora->State = RF_IDLE;
            SX1278SetOpMode(lora, RF_OPMODE_SLEEP); 
        }

        if (lora->Radio.RxTimeout)
        {
            lora->Radio.RxTimeout(lora->Radio.para);
        }
    }
    else if (lora->State == RF_CAD_RUNNING)
    {
        lora->State = RF_IDLE;
        SX1278SetOpMode(lora, RF_OPMODE_SLEEP);

        if (lora->Radio.CadTimeout)
        {
            lora->Radio.CadTimeout(lora->Radio.para);
        }
    }
    else
    {
        printf("Error:RF is IDLE but is timeout!\r\n");
    }
}
/***************************************************************************************************
*\Function      SX1278Dio0IrqHandler
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void SX1278Dio0IrqHandler(void* p)
{
    //u8 byRegVal = 0;
    LoRa_Dev_type* lora = (LoRa_Dev_type*) p;

    if (lora->LowPowerExitFunc)
    {
        lora->LowPowerExitFunc();
    }

    //先停止Timer
    TimerStop(&lora->RadioTimer);

    if (lora->State == RF_TX_RUNNING)
    {
        SX1278Write(lora, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE);

        lora->State = RF_IDLE;
        SX1278SetOpMode(lora, RF_OPMODE_SLEEP); 

        if (lora->Radio.TxDone)
        {
            lora->Radio.TxDone(lora->Radio.para);
        }
    }
    else if (lora->State == RF_RX_RUNNING)
    {
        SX1278Write(lora, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE);

        if ( (SX1278Read(lora, REG_LR_IRQFLAGS) & RFLR_IRQFLAGS_PAYLOADCRCERROR_MASK) ==
            RFLR_IRQFLAGS_PAYLOADCRCERROR ) /* Is PayloadCrcError */
        {
            /* Clear Irq of "PayloadCrcError" */
            SX1278Write(lora, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR);

            if (lora->Radio.RxError)
            {
                lora->Radio.RxError(lora->Radio.para);
            }    
        }
        else /* Is a integrity packet */
        {
            s8 snr = 0;

            /* Get value of SNR */
            lora->RadioPacketInfo.chSNR = SX1278Read(lora, REG_LR_PKTSNRVALUE);			

            if(lora->RadioPacketInfo.chSNR & 0x80 ) // The SNR sign bit is 1
            {
                // Invert and divide by 4
                snr = ( ( ~lora->RadioPacketInfo.chSNR + 1 ) & 0xFF ) >> 2;
                snr = -snr;
            }
            else
            {
                // Divide by 4
                snr = (lora->RadioPacketInfo.chSNR & 0xFF ) >> 2;
            }

            s16 rssi = SX1278Read(lora, REG_LR_PKTRSSIVALUE );
            if( snr < 0 )
            {
                if(lora->LoRaFreq > RF_MID_BAND_THRESH )
                {
                    lora->RadioPacketInfo.nRSSI = RSSI_OFFSET_HF + rssi + ( rssi >> 4 ) + snr;
                }
                else
                {
                    lora->RadioPacketInfo.nRSSI = RSSI_OFFSET_LF + rssi + ( rssi >> 4 ) + snr;
                }
            }
            else
            {
                if(lora->LoRaFreq > RF_MID_BAND_THRESH )
                {
                    lora->RadioPacketInfo.nRSSI = RSSI_OFFSET_HF + rssi + ( rssi >> 4 );
                }
                else
                {
                    lora->RadioPacketInfo.nRSSI = RSSI_OFFSET_LF + rssi + ( rssi >> 4 );
                }
            }

            /* Get length and data of packet */
            lora->RadioPacketInfo.bySize = SX1278Read(lora, REG_LR_RXNBBYTES);
            SX1278Write(lora, REG_LR_FIFOADDRPTR, SX1278Read(lora, REG_LR_FIFORXCURRENTADDR));
            SX1278ReadFifo(lora, lora->TxRxBuffer, lora->RadioPacketInfo.bySize);

            if (lora->Radio.RxDone)
            {
                lora->Radio.RxDone(lora->Radio.para, lora->TxRxBuffer, lora->RadioPacketInfo.bySize,
                    lora->RadioPacketInfo.nRSSI,
                    lora->RadioPacketInfo.chSNR );
            }
        }

        if (FALSE == lora->RadioSetting.RxContinuous)
        {
            lora->State = RF_IDLE;
            SX1278SetOpMode(lora, RF_OPMODE_SLEEP);
        }
    }
    else if (lora->State == RF_CAD_RUNNING)
    {
        bool    bIsCadDetected;


        if ( (SX1278Read(lora, REG_LR_IRQFLAGS) & RFLR_IRQFLAGS_CADDETECTED_MASK) ==
            RFLR_IRQFLAGS_CADDETECTED ) /* Is CadDetected */
        {
            /* Clear Irq of "CadDetected" and "cadDone" */
            SX1278Write(lora, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED | RFLR_IRQFLAGS_CADDONE);

            bIsCadDetected = TRUE;
        }
        else
        {
            /* Clear Irq of "cadDone" */
            SX1278Write(lora, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE);

            bIsCadDetected = FALSE;
        }

        lora->State = RF_IDLE;
        SX1278SetOpMode(lora, RF_OPMODE_SLEEP);

        if (lora->Radio.CadDone)
        {
            lora->Radio.CadDone(lora->Radio.para, bIsCadDetected);
        }    
    }
    else /* Error */
    {
        /* EXPLAIN: run to here meas timeout is too short! */
        printf("Error: RF is IDLE but DIO0 IRQ!\r\n");
    }
}

/***************************************************************************************************
*\Function      lora_sx1278_init
*\Description   初始化
*\Parameter     lora
*\Return        void
*\Note          
*\Log           2018.05.09    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
void lora_sx1278_init(LoRa_Dev_type* lora)
{
    GPIO_InitTypeDef   GPIO_InitStructure;

    if (lora == NULL || lora->ResetPin == NULL || lora->SPI == NULL)
    {
        return;
    }
    lora->State = RF_IDLE;

    stm32_lptim_init();

    //电源引脚
    if (lora->PwPin)
    {
        stm32_switch_init(lora->PwPin);
        stm32_switch_on(lora->PwPin);
    }
    //中断引脚
    if (lora->DIO0)
    {
        gpio_rcc_enable(lora->DIO0->gpio);
        /* Configure pin as input floating */
        GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
        GPIO_InitStructure.Pin = lora->DIO0->gpio_pin;
        HAL_GPIO_Init(lora->DIO0->gpio, &GPIO_InitStructure);

        __HAL_GPIO_EXTI_CLEAR_IT(lora->DIO0->gpio_pin);
        /* Enable and set EXTI lines 10 to 15 Interrupt to the lowest priority */
        HAL_NVIC_SetPriority(gpio_nvic_irq_get(lora->DIO0->gpio_pin), 7, 0);
        HAL_NVIC_EnableIRQ(gpio_nvic_irq_get(lora->DIO0->gpio_pin));

        //配置中断处理函数
        stm32_gpio_irq_config(lora->DIO0->gpio_pin, SX1278Dio0IrqHandler, lora);
    }
    //复位引脚
    if (lora->ResetPin)
    {
        lora_sx1278_reset(lora);
    }
    //spi
    if (lora->SPI)
    {
        stm32_spi_init(lora->SPI);
        printf("LoRa:%02x\n\r", SX1278Read(lora, 0x4d));
    }

    lora->LowPowerExitFunc = NULL;

    TimerInit(&lora->RadioTimer, SX1278_RadioTimer_Handler, lora);

    SX1278_RxChainCalibration(lora);
    //休眠模式
    SX1278SetOpMode(lora, RF_OPMODE_SLEEP);  

    /* Set to LoRa modem */	
    SX1278Write(lora, REG_LR_OPMODE, 
        (SX1278Read(lora, REG_LR_OPMODE) & RF_OPMODE_LONGRANGEMODE_MASK) |
        RF_OPMODE_LONGRANGEMODE_ON);
    SX1278Write(lora, REG_LR_DIOMAPPING1, 0x00);
    SX1278Write(lora, REG_LR_DIOMAPPING2, 0x00);

    //配置LoRa
    SX1278SetPAOutput(lora, PA_OUTPUT_PIN_BOOST);
    SX1278SetTxPower(lora, lora->LoRaTxPw);
    SX1278SetChannel(lora, lora->LoRaFreq);

    //休眠模式
    SX1278SetOpMode(lora, RF_OPMODE_SLEEP);  
}
/***************************************************************************************************
*\Function      lora_config_set
*\Description   参数配置
*\Parameter     lora
*\Parameter     bandwidth
*\Parameter     datarate
*\Parameter     coderate
*\Parameter     preambleLen
*\Parameter     symbTimeout
*\Return        bool
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
bool lora_sx1278_param_set(LoRa_Dev_type* lora, u32 bw_num, u32 sf, u8 fec, u16 preambleLen,u16 symbTimeout)
{
    u8 fixLen = 0;//通信长度不固定
    
    if (lora == NULL)
    {
        return FALSE;
    }

    assert_param( (RF_BW_7800 <= bw_num && bw_num <= RF_BW_500000) && 
        (RF_SF_6 <= sf && sf <= RF_SF_12) );

    lora->RadioSetting.crcon = 1;
    lora->RadioSetting.fixlen = 0;

    lora->RadioSetting.BW_N = bw_num;
    lora->RadioSetting.FEC = fec;
    lora->RadioSetting.SF = sf;
    lora->RadioSetting.PreambleLen = preambleLen;
    lora->RadioSetting.symbTimeout = symbTimeout;
    lora->RadioSetting.RxContinuous = TRUE;//默认持续接收

    if( sf > 12 )
    {
        sf = 12;
    }
    else if( sf < 6 )
    {
        sf = 6;
    }

    if( ( ( bw_num == 7 ) && ( ( sf == 11 ) || ( sf == 12 ) ) ) ||
        ( ( bw_num == 8 ) && ( sf == 12 ) ) )
    {
        lora->RadioSetting.LowDatarateOptimize = 0x01;
    }
    else
    {
        lora->RadioSetting.LowDatarateOptimize = 0x00;
    }

    SX1278Write(lora, REG_LR_MODEMCONFIG1,
        ( SX1278Read(lora, REG_LR_MODEMCONFIG1 ) &
        RFLR_MODEMCONFIG1_BW_MASK &
        RFLR_MODEMCONFIG1_CODINGRATE_MASK &
        RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) |
        ( bw_num << 4 ) | ( fec << 1 ) |
        fixLen );

    SX1278Write(lora, REG_LR_MODEMCONFIG2,
        ( SX1278Read(lora, REG_LR_MODEMCONFIG2 ) &
        RFLR_MODEMCONFIG2_SF_MASK &
        RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK &
        RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) |
        ( sf << 4 ) | ( lora->RadioSetting.crcon << 2 ) |
        ( ( symbTimeout >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) );

    SX1278Write(lora, REG_LR_MODEMCONFIG3,
        ( SX1278Read(lora, REG_LR_MODEMCONFIG3 ) &
        RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) |
        (lora->RadioSetting.LowDatarateOptimize << 3 ));

    SX1278Write(lora, REG_LR_SYMBTIMEOUTLSB, (u8)(symbTimeout & 0xFF));

    SX1278Write(lora, REG_LR_PREAMBLEMSB, ( preambleLen >> 8 ) & 0x00FF );
    SX1278Write(lora, REG_LR_PREAMBLELSB, preambleLen & 0xFF );

    if( ( bw_num == 9 ) && (lora->LoRaFreq > RF_MID_BAND_THRESH ) )
    {
        // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
        SX1278Write(lora, REG_LR_TEST36, 0x02 );
        SX1278Write(lora, REG_LR_TEST3A, 0x64 );
    }
    else if( bw_num == 9 )
    {
        // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
        SX1278Write(lora, REG_LR_TEST36, 0x02 );
        SX1278Write(lora, REG_LR_TEST3A, 0x7F );
    }
    else
    {
        // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
        SX1278Write(lora, REG_LR_TEST36, 0x03 );
    }

    if( sf == 6 )
    {
        SX1278Write(lora, REG_LR_DETECTOPTIMIZE,
            ( SX1278Read(lora, REG_LR_DETECTOPTIMIZE ) &
            RFLR_DETECTIONOPTIMIZE_MASK ) |
            RFLR_DETECTIONOPTIMIZE_SF6 );
        SX1278Write(lora, REG_LR_DETECTIONTHRESHOLD,
            RFLR_DETECTIONTHRESH_SF6 );
    }
    else
    {
        SX1278Write(lora, REG_LR_DETECTOPTIMIZE,
            ( SX1278Read(lora, REG_LR_DETECTOPTIMIZE ) &
            RFLR_DETECTIONOPTIMIZE_MASK ) |
            RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12 );
        SX1278Write(lora, REG_LR_DETECTIONTHRESHOLD,
            RFLR_DETECTIONTHRESH_SF7_TO_SF12 );
    }

    SX1278Write(lora, REG_LR_INVERTIQ, ( ( SX1278Read(lora, REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF ) );
    SX1278Write(lora, REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF );

    // ERRATA 2.3 - Receiver Spurious Reception of a LoRa Signal
    if(lora->RadioSetting.BW_N < 9 )
    {
        SX1278Write(lora, REG_LR_DETECTOPTIMIZE, SX1278Read(lora, REG_LR_DETECTOPTIMIZE ) & 0x7F );
        SX1278Write(lora, REG_LR_TEST30, 0x00 );
        switch(lora->RadioSetting.BW_N )
        {
        case 0: // 7.8 kHz
            SX1278Write(lora, REG_LR_TEST2F, 0x48 );
            SX1278SetChannel(lora, lora->LoRaFreq + 7810 );
            break;
        case 1: // 10.4 kHz
            SX1278Write(lora, REG_LR_TEST2F, 0x44 );
            SX1278SetChannel(lora, lora->LoRaFreq  + 10420 );
            break;
        case 2: // 15.6 kHz
            SX1278Write(lora, REG_LR_TEST2F, 0x44 );
            SX1278SetChannel(lora, lora->LoRaFreq  + 15620 );
            break;
        case 3: // 20.8 kHz
            SX1278Write(lora, REG_LR_TEST2F, 0x44 );
            SX1278SetChannel(lora, lora->LoRaFreq  + 20830 );
            break;
        case 4: // 31.2 kHz
            SX1278Write(lora, REG_LR_TEST2F, 0x44 );
            SX1278SetChannel(lora, lora->LoRaFreq + 31250 );
            break;
        case 5: // 41.4 kHz
            SX1278Write(lora, REG_LR_TEST2F, 0x44 );
            SX1278SetChannel(lora, lora->LoRaFreq + 41670 );
            break;
        case 6: // 62.5 kHz
            SX1278Write(lora, REG_LR_TEST2F, 0x40 );
            break;
        case 7: // 125 kHz
            SX1278Write(lora, REG_LR_TEST2F, 0x40 );
            break;
        case 8: // 250 kHz
            SX1278Write(lora, REG_LR_TEST2F, 0x40 );
            break;
        }
    }
    else
    {
        SX1278Write(lora, REG_LR_DETECTOPTIMIZE, SX1278Read(lora, REG_LR_DETECTOPTIMIZE ) | 0x80 );
    }

    return TRUE;
}
/***************************************************************************************************
*\Function      lora_send
*\Description   发送函数
*\Parameter     lora
*\Parameter     buf
*\Parameter     size
*\Parameter     timeout
*\Return        u32
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void lora_sx1278_send(LoRa_Dev_type* lora, u8* buf, u8 size, u32 timeout)
{
    if (lora == NULL || buf == NULL || timeout == 0)
    {
        return;
    }

    //printf("Send:[%d], %d\r\n", size, timeout);

    SX1278Write(lora, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE);

    lora->RadioPacketInfo.bySize = size;

    SX1278Write(lora, REG_LR_PAYLOADLENGTH, size );

    // Full buffer used for Tx
    SX1278Write(lora, REG_LR_FIFOTXBASEADDR, 0 );
    SX1278Write(lora, REG_LR_FIFOADDRPTR, 0 );

    // FIFO operations can not take place in Sleep mode
    if( ( SX1278Read(lora, REG_OPMODE ) & ~RF_OPMODE_MASK ) == RF_OPMODE_SLEEP )
    {
        SX1278SetStby(lora);   
    }
    DelayMs(1);//为了RTC同步精确 都延时
    // Write payload buffer
    SX1278WriteFifo(lora, buf, size);

    SX1278Write(lora, REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
        RFLR_IRQFLAGS_RXDONE |
        RFLR_IRQFLAGS_PAYLOADCRCERROR |
        RFLR_IRQFLAGS_VALIDHEADER |
        //RFLR_IRQFLAGS_TXDONE |
        RFLR_IRQFLAGS_CADDONE |
        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
        RFLR_IRQFLAGS_CADDETECTED );

    // DIO0=TxDone
    SX1278Write(lora, REG_DIOMAPPING1, ( SX1278Read(lora, REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_01 );

    lora->State = RF_TX_RUNNING;
    TimerSetValue(&lora->RadioTimer, timeout);
    TimerStart(&lora->RadioTimer);
    SX1278SetOpMode(lora, RF_OPMODE_TRANSMITTER);
}
/***************************************************************************************************
*\Function      lora_rx
*\Description   接收处理
*\Parameter     lora
*\Parameter     timeout 连续模式则为0
*\Parameter     RxContinuous
*\Return        void
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void lora_sx1278_recv(LoRa_Dev_type* lora, u32 timeout, bool RxContinuous)
{
    if(lora == NULL)
    {
        return;
    }

    lora->RadioSetting.RxContinuous = RxContinuous;

    SX1278Write(lora, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE | RFLR_IRQFLAGS_PAYLOADCRCERROR);

    SX1278Write(lora, REG_LR_IRQFLAGSMASK, //RFLR_IRQFLAGS_RXTIMEOUT |
        //RFLR_IRQFLAGS_RXDONE |
        //RFLR_IRQFLAGS_PAYLOADCRCERROR |
        RFLR_IRQFLAGS_VALIDHEADER |
        RFLR_IRQFLAGS_TXDONE |
        RFLR_IRQFLAGS_CADDONE |
        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
        RFLR_IRQFLAGS_CADDETECTED );

    // DIO0=RxDone
    SX1278Write(lora, REG_DIOMAPPING1, ( SX1278Read(lora, REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_00 );

    SX1278Write(lora, REG_LR_FIFORXBASEADDR, 0 );
    SX1278Write(lora, REG_LR_FIFOADDRPTR, 0 );    

    //memset(lora->TxRxBuffer, 0, sizeof(lora->TxRxBuffer));

    lora->State = RF_RX_RUNNING;

    if(timeout != 0 )
    {
        TimerSetValue(&lora->RadioTimer, timeout );
        TimerStart(&lora->RadioTimer);
    }

    if( RxContinuous == TRUE )
    {
        SX1278SetOpMode(lora, RFLR_OPMODE_RECEIVER);
    }
    else
    {
        SX1278SetOpMode(lora, RFLR_OPMODE_RECEIVER_SINGLE);
    }
}
/***************************************************************************************************
*\Function      lora_cad
*\Description   
*\Parameter     lora
*\Parameter     timeout
*\Return        void
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void lora_sx1278_startcad(LoRa_Dev_type* lora, u32 timeout)
{
    if (lora == NULL || timeout == 0)
    {
        return;
    }

    //先清除标记
    SX1278Write(lora, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED | RFLR_IRQFLAGS_CADDONE);

    SX1278Write(lora, REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
        RFLR_IRQFLAGS_RXDONE |
        RFLR_IRQFLAGS_PAYLOADCRCERROR |
        RFLR_IRQFLAGS_VALIDHEADER |
        RFLR_IRQFLAGS_TXDONE |
        //RFLR_IRQFLAGS_CADDONE |
        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL // |
        //RFLR_IRQFLAGS_CADDETECTED
        );

    // DIO3=CADDone
    SX1278Write(lora, REG_DIOMAPPING1, ( SX1278Read(lora, REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_10 );

    TimerSetValue(&lora->RadioTimer, timeout );
    TimerStart(&lora->RadioTimer);

    lora->State = RF_CAD_RUNNING;
    SX1278SetOpMode(lora, RFLR_OPMODE_CAD);
}
/***************************************************************************************************
*\Function      lora_sx1278_preamble_set
*\Description   设置前导码长度
*\Parameter     lora
*\Parameter     len
*\Return        void
*\Note          
*\Log           2018.05.17    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void lora_sx1278_preamble_set(LoRa_Dev_type* lora, u16 len)
{
    if (lora == NULL)
    {
        return;
    }

    lora->RadioSetting.PreambleLen = len;

    SX1278Write(lora, REG_LR_PREAMBLEMSB, ( len >> 8 ) & 0x00FF );
    SX1278Write(lora, REG_LR_PREAMBLELSB, len & 0xFF );
}
/***************************************************************************************************
*\Function      lora_sx1278_freq_set
*\Description   频率设置
*\Parameter     lora
*\Parameter     freq
*\Return        void
*\Note          
*\Log           2018.05.18    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void lora_sx1278_freq_set(LoRa_Dev_type* lora, u32 freq)
{
    if (lora == NULL)
    {
        return;
    }

    lora->LoRaFreq = freq;

    SX1278SetChannel(lora, lora->LoRaFreq);
}
/***************************************************************************************************
*\Function      lora_sx1278_Txpower
*\Description   发送功率设置
*\Parameter     lora
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.18    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void lora_sx1278_Txpower(LoRa_Dev_type* lora, s8 pw)
{
    if (lora == NULL)
    {
        return;
    }

    lora->LoRaTxPw = pw;

    SX1278SetTxPower(lora, lora->LoRaTxPw);
}
/***************************************************************************************************
*\Function      lora_set_callback
*\Description   设置回调函数
*\Parameter     lora
*\Parameter     txdone
*\Parameter     txtimeout
*\Parameter     rxdone
*\Parameter     rxtimeout
*\Parameter     rxerror
*\Parameter     caddone
*\Parameter     cadtimeout
*\Return        void
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void lora_sx1278_set_callback(LoRa_Dev_type* lora, pHookHandle txdone, pHookHandle txtimeout, \
                       void (*rxdone)(void* p, u8* buf, u16 size, s16 rssi, s8 snr), pHookHandle rxtimeout, pHookHandle rxerror, \
                       void (*caddone)(void* p, bool), pHookHandle cadtimeout, void* p)
{
    if (lora == NULL)
    {
        return;
    }

    lora->Radio.TxDone = txdone;
    lora->Radio.TxTimeout = txtimeout;
    lora->Radio.RxDone = rxdone;
    lora->Radio.RxTimeout = rxtimeout;
    lora->Radio.RxError = rxerror;
    lora->Radio.CadDone = caddone;
    lora->Radio.CadTimeout = cadtimeout;

    lora->Radio.para = p;
}
/***************************************************************************************************
*\Function      lora_sx1278_lowpw_func_set
*\Description   低功耗回调
*\Parameter     lora
*\Parameter     
*\Parameter     func
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.22    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void lora_sx1278_lowpw_func_set(LoRa_Dev_type* lora, void (*func)(void))
{
    if (lora == NULL)
    {
        return;
    }

    lora->LowPowerExitFunc = func;

    return;
}
/***************************************************************************************************
*\Function      lora_get_status
*\Description   获取状态
*\Parameter     lora
*\Return        RadioState_t
*\Note          
*\Log           2018.05.10    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
RadioState_t lora_sx1278_get_status(LoRa_Dev_type* lora)
{
    return lora->State;
}
/***************************************************************************************************
*\Function      lora_sx1278_sleep_set
*\Description   设置休眠
*\Parameter     lora
*\Return        void
*\Note          
*\Log           2018.05.13    Ver 1.0    Job
*               创建函数。
***************************************************************************************************/
void lora_sx1278_sleep_set(LoRa_Dev_type* lora)
{
    if (lora == NULL)
    {
        return;
    }
    TimerStop(&lora->RadioTimer);
    lora->State = RF_IDLE;
    SX1278SetOpMode(lora, RF_OPMODE_SLEEP); 
}
/***************************************************************************************************
*\Function      lora_timeonair_get
*\Description   获取时间 必须在设置参数后调用
*\Parameter     lora
*\Parameter     PreambleLen
*\Parameter     pktLen
*\Return        u32
*\Note          包含前导码时间+数据时间
*\Log           2018.05.11    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
u32 lora_calc_onair_time(LoRa_Dev_type* lora, u16 PreambleLen, u8 pktLen)
{
    u32 airTime = 0;

    if (lora == NULL)
    {
        return 0;
    }
    // Symbol rate : time for one symbol (secs)
    double rs = BWList[lora->RadioSetting.BW_N] / ( 1 << lora->RadioSetting.SF);
    double ts = 1 / rs;
    // time of preamble
    double tPreamble = (PreambleLen + 4.25 ) * ts;
    // Symbol length of payload and time
    double tmp = ceil( ( 8 * pktLen - 4 * lora->RadioSetting.SF +
        28 + 16 * lora->RadioSetting.crcon -
        ( lora->RadioSetting.fixlen ? 20 : 0 ) ) /
        ( double )( 4 * ( lora->RadioSetting.SF -
        ( ( lora->RadioSetting.LowDatarateOptimize > 0 ) ? 2 : 0 ) ) ) ) *
        ( lora->RadioSetting.FEC + 4 );
    double nPayload = 8 + ( ( tmp > 0 ) ? tmp : 0 );
    double tPayload = nPayload * ts;
    // Time on air
    double tOnAir = tPreamble + tPayload;
    // return ms secs
    airTime = (u32) floor( tOnAir * 1000 + 0.999 );

    return airTime;
}

/***************************************************************************************************
*\Function      lora_calc_cad_time
*\Description   计算cad一次需要时间 必须在设置参数后调用
*\Parameter     
*\Return        u32
*\Note          
*\Log           2018.05.11    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
u32 lora_calc_cad_time(LoRa_Dev_type* lora)
{
    /* Symbols=[1.7, 2.3] when SF=[6, 12] */
#define MAX_SYMB_OF_CAD    2.3

    double    dTemp;

    if (lora == NULL)
    {
        return 0;
    }

    /* Get time of symbol rate */
    dTemp =  (double)(1 << lora->RadioSetting.SF) / BWList[lora->RadioSetting.BW_N];
    dTemp = MAX_SYMB_OF_CAD * dTemp * 1000.0;

    return (u32)ceil(dTemp); /* Return ms secs */
}

/***************************************************************************************************
*\Function      lora_calc_preamble_num
*\Description   根据时间计算前导码个数 
*\Parameter     lora
*\Parameter     wMs
*\Parameter     tBW
*\Parameter     tSF
*\Return        u32
*\Note          
*\Log           2018.05.11    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
u32 lora_calc_preamble_num(LoRa_Dev_type* lora, u16 wMs, u32 BW_N, u32 SF)
{
    double    dTemp;
    
    if (lora == NULL || BW_N > RF_BW_500000 || SF < RF_SF_6 || SF > RF_SF_12)
    {
        return 0;
    }

    dTemp = BWList[BW_N - RF_BW_7800] / (double)(1 << SF);
    dTemp = dTemp * wMs / 1000.0 - 4.25;

    return (u32)ceil(dTemp);
}
/***************************************************************************************************
*\Function      lora_calc_preamble_time
*\Description    计算前导码时间 必须在设置参数后调用
*\Parameter     lora
*\Parameter     PreambleLen
*\Return        u32
*\Note          
*\Log           2018.05.17    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
u32 lora_calc_preamble_time(LoRa_Dev_type* lora, u16 PreambleLen)
{
    double    dTemp;

    if (lora == NULL)
    {
        return 0;
    }

    /* Get time of symbol rate */
    dTemp =  (double)(1 << lora->RadioSetting.SF) / BWList[lora->RadioSetting.BW_N - RF_BW_7800];

    dTemp *= (PreambleLen + 4.25);
    dTemp *= 1000.0; /* Multi 1000 for Sed=>MS */

    return (u32)ceil(dTemp); /* Return ms secs */
}
