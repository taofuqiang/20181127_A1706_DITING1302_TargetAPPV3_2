/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_sx1278.h
*\Description   
*\Note          
*\Log           2018.05.05    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#ifndef _DRV_SX1278_H
#define _DRV_SX1278_H
#include "stm32_spi.h"
#include "drv_switch.h"
#include "public_type.h"
#include "timeServer.h"
#include "sx1276Regs-LoRa.h"
#include "sx1276Regs-Fsk.h"
#include "stm32_lptim.h"
#include "stm32_gpio_irq.h"


typedef enum
{
    RF_SF_6 = 6,
    RF_SF_7,
    RF_SF_8,
    RF_SF_9,
    RF_SF_10,
    RF_SF_11,
    RF_SF_12,
} RadioSF_t;


typedef enum
{
    RF_FEC_4_5 = 1,
    RF_FEC_4_6,
    RF_FEC_4_7,
    RF_FEC_4_8,
} RadioFEC_t;


typedef enum
{
    RF_BW_7800 = 0,
    RF_BW_10400,
    RF_BW_15600,    
    RF_BW_20800,    
    RF_BW_31250,    
    RF_BW_41700,    
    RF_BW_62500,    
    RF_BW_125000,    
    RF_BW_250000,    
    RF_BW_500000,    
} RadioBW_t;

/**
 * @brief  Selects PA output pin.
 *            RFO pin: Output power is limited to +14 dBm.
 *            PA_BOOST pin: Output power is limited to +20 dBm.
 */
typedef enum
{
    PA_OUTPUT_PIN_RFO = (u8)0x00,
    PA_OUTPUT_PIN_BOOST = (u8)0x80,
} PA_OUTPUT_PIN_TypeDef;

#define RF_FIFO_SIZE    256

//״̬
typedef enum
{
    RF_IDLE = 0,
    RF_RX_RUNNING,
    RF_TX_RUNNING,
    RF_CAD_RUNNING,
} RadioState_t;

//lora����ѡ��
typedef struct
{
    u16    PreambleLen;//ǰ���볤��
    u16    symbTimeout;
    u32    BW_N;
    u32    SF;
    u8     FEC;
    u8     crcon;//������
    u8     fixlen;//����ʱ����
    u8     LowDatarateOptimize;//����ʱ����
    bool   RxContinuous;
} RadioLoRaSettings_t;

typedef struct
{
    u8    bySize;
    s8    chSNR;
    s16   nRSSI;
}RadioLoRaPacketHandler_t;

typedef struct
{
    void (*TxDone )(void* p); /*!< Tx Done callback prototype. */
    void (*TxTimeout)(void* p); /*!< Tx Timeout callback prototype. */
    /*!
     * \brief Rx Done callback prototype.
     * \param [IN] size    Received buffer size
     * \param [IN] rssi    RSSI value computed while receiving the frame [dBm]
     * \param [IN] snr    Raw SNR value given by the radio hardware, LoRa: SNR value in dB
     */
    void (*RxDone)(void* p, u8* buf, u16 size, s16 rssi, s8 snr);
    void (*RxTimeout )(void*); /*!< Rx Timeout callback prototype. */
    void (*RxError )(void*); /*!< Rx Error callback prototype. */
    /*!
     * \brief CAD Done callback prototype.
     * \param [IN] detected    TRUE=channel activity, FALSE=channel empty. 
     */
    void (*CadDone)(void* p, bool detected);
    void (*CadTimeout)(void* ); /*!< CAD Timeout callback prototype. */
    void* para;
} RadioEvents_t;

typedef struct LoRa_Dev_type_st
{
    switch_type* PwPin;//��Դ���� ����Ϊ��
    switch_type* ResetPin;//��λ��
    SpiDevType*  SPI;//spi
    GpioType*    DIO0;
#if 0 //Ŀǰֻ��Ҫʹ���ж�DIO0��OK
    GpioType*    DIO1;
    GpioType*    DIO2;
    GpioType*    DIO3;
    GpioType*    DIO4;
    GpioType*    DIO5;
#endif
    u32          LoRaFreq;//Ƶ��
    s8           LoRaTxPw;//���͹���
    /*������Ϣ��Ҫ��ʼ����*/
    RadioState_t State;//RF״̬
    RadioLoRaSettings_t RadioSetting;//Lora���ò���
    RadioEvents_t Radio;//RF�ص�����
    RadioLoRaPacketHandler_t RadioPacketInfo;//������հ����ź���Ϣ
    TimerEvent_t RadioTimer;//��ʱ��
    u8           TxRxBuffer[RF_FIFO_SIZE];

    void             (*LowPowerExitFunc)(void);//�͹����˳�ִ�к���
}LoRa_Dev_type;


#define XTAL_FREQ                                   32000000
#define FREQ_STEP                                   61.03515625
#define FREQ_STEP_8                                 15625 /* FREQ_STEP<<8 */

/* Freq = channel * FREQ_STEP */
#define SX_CHANNEL_TO_FREQ( channel, freq )                                                     \
    do                                                                                          \
{                                                                                               \
    uint32_t initialChanInt, initialChanFrac;                                                   \
    initialChanInt = channel  >> 8;                                                             \
    initialChanFrac = channel - ( initialChanInt << 8 );                                        \
    freq = initialChanInt * FREQ_STEP_8 + ( ( initialChanFrac * FREQ_STEP_8 + ( 128 ) ) >> 8 ); \
}while( 0 )

/* channel = Freq / FREQ_STEP */
#define SX_FREQ_TO_CHANNEL( channel, freq )                                                        \
    do                                                                                             \
{                                                                                                  \
    uint32_t initialFreqInt, initialFreqFrac;                                                      \
    initialFreqInt = freq / FREQ_STEP_8;                                                           \
    initialFreqFrac = freq - ( initialFreqInt * FREQ_STEP_8 );                                     \
    channel = ( initialFreqInt << 8 ) + ( ( ( initialFreqFrac << 8 ) + ( FREQ_STEP_8 / 2 ) ) / FREQ_STEP_8 ); \
}while( 0 )


void lora_sx1278_init(LoRa_Dev_type* lora);
bool lora_sx1278_param_set(LoRa_Dev_type* lora, u32 bw_num, u32 sf, u8 fec, u16 preambleLen,u16 symbTimeout);
void lora_sx1278_send(LoRa_Dev_type* lora, u8* buf, u8 size, u32 timeout);
void lora_sx1278_recv(LoRa_Dev_type* lora, u32 timeout, bool RxContinuous);
void lora_sx1278_startcad(LoRa_Dev_type* lora, u32 timeout);
void lora_sx1278_set_callback(LoRa_Dev_type* lora, pHookHandle txdone, pHookHandle txtimeout, \
                              void (*rxdone)(void* p, u8* buf, u16 size, s16 rssi, s8 snr), pHookHandle rxtimeout, pHookHandle rxerror, \
                              void (*caddone)(void* p, bool), pHookHandle cadtimeout, void* p);
void lora_sx1278_sleep_set(LoRa_Dev_type* lora);
void lora_sx1278_preamble_set(LoRa_Dev_type* lora, u16 len);
void lora_sx1278_freq_set(LoRa_Dev_type* lora, u32 freq);
void lora_sx1278_Txpower(LoRa_Dev_type* lora, s8 pw);

void lora_sx1278_lowpw_func_set(LoRa_Dev_type* lora, void (*func)(void));

RadioState_t lora_sx1278_get_status(LoRa_Dev_type* lora);

u32 lora_calc_onair_time(LoRa_Dev_type* lora, u16 PreambleLen, u8 pktLen);
u32 lora_calc_preamble_time(LoRa_Dev_type* lora, u16 PreambleLen);
u32 lora_calc_preamble_num(LoRa_Dev_type* lora, u16 wMs, u32 BW_N, u32 SF);
u32 lora_calc_cad_time(LoRa_Dev_type* lora);

#endif /*_DRV_SX1278_H*/

