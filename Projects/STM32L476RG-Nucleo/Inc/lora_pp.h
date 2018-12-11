/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          lora_pp.h
*\Description   LoRa�������Э��
*\Note          
*\Log           2018.05.11    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#ifndef _LORA_PP_H
#define _LORA_PP_H
#include "app_board.h"
#include "drv_sx1278.h"
#include "stm32_Inrtc.h"

#define PP_TX_DELAY_ENABLE

#define PP_BROADCAST_ADDR (0xFFFF)

typedef enum PP_Exe_Type_st
{
    EXE_NO = 0,//������ִ��
    EXE_NEED = !EXE_NO, //��Ҫ�ٴ�ִ��
}PP_Exe_Type;

//����Э������״̬��
typedef enum LoRa_State_Type_st
{
    PP_IDLE,

    PP_TX_CAD_REQ,//CAD����
    PP_TX_CADING,

    PP_TX_REQ,//������
    PP_TXING,//������
    
    PP_RX_CAD_REQ,//CAD����
    PP_RX_CADING,

    PP_RX_REQ,
    PP_RXING,

    PP_RTC_SYNC_CADING,//�㲥Уʱ��ͻ���
    PP_RTC_SYNC_REQ,//�����͹㲥Уʱ
    PP_RTC_SYNC_ING,//Уʱ��

    PP_SLEEP,//ǿ������
    PP_SLEEP_EXIT,
}LoRa_State_Type;

//���ڽ���״̬��
typedef enum LoRaRx_Type_st
{
    PP_RX_WUHead_Wait,//�ȴ�����ͷ
    PP_RX_WUHead_RecvDone,//�Ѿ����յ�����ͷ
    PP_RX_WUHead_RecvError,//����ͷ���մ���

    PP_RX_WUHead_ACK_SendReq,//
    PP_RX_WUHead_ACK_Sending,//�ش�����ACK
    PP_RX_WUHead_ACK_SendDone,//�������
    PP_RX_WUHead_ACK_SendError,//����ʧ��

    PP_RX_Data_RecvReady,
    PP_RX_Data_RecvWait,//��������
    PP_RX_Data_RecvDone,
    PP_RX_Data_RecvError,
    PP_RX_Data_RecvError_Delaying,
    PP_RX_Data_RecvError_DelayDone,

    PP_RX_Data_ACK_SendReq,//����ACK
    PP_RX_Data_ACK_Sending,//�ش�����ACK
    PP_RX_Data_ACK_SendDone,//�������
    PP_RX_Data_ACK_SendError,//����ʧ��

    PP_RX_Data_Done,//�������    
}LoRaRx_Type;

//����״̬��
typedef enum LoRaTx_Type_st
{
    //����ͷ���Ͳ���״̬
    PP_TX_WUHead_Ready,//���ͻ���ͷ
    PP_TX_WUHead_Sending,//������
    PP_TX_WUHead_SendDone,//�������
    PP_TX_WUHead_SendError,//���ʹ���
    
    PP_TX_BD_WUHead_Delaying,//����ͷ�׶���ʱ
    PP_TX_BD_WUHead_DelayDone,//��ʱ��

    //����ͷ����ACK����״̬
    PP_TX_WUHead_ACK_Wait,
    PP_TX_WUHead_ACK_Done,
    PP_TX_WUHead_ACK_Error,//��ʱ���߳������error

    //���ݲ��ַ���״̬
    PP_TX_Data_Ready,
    PP_TX_Data_Sending,//���ݷ�����
    PP_TX_Data_SendDone,
    PP_TX_Data_SendError,

    PP_TX_BD_Data_Delaying,//Data�׶���ʱ
    PP_TX_BD_Data_DelayDone,//��ʱ��

    //���ݲ��ֽ���ACK����״̬
    PP_TX_Data_ACK_Wait,
    PP_TX_Data_ACK_Done,
    PP_TX_Data_ACK_Error,
}LoRaTx_Type;

//ʱ��ͬ��״̬��
typedef enum LoRaRTCSync_Type_st
{
    //����ͷ���Ͳ���״̬
    PP_RTCSync_Ready,//���ͻ���ͷ
    PP_RTCSync_Sending,//������
    PP_RTCSync_SendDone,//�������
    PP_RTCSync_SendError,//���ʹ���
}LoRaRTCSync_Type;

//����ǰ��ʱ����
typedef enum LoRaTXDelay_Type_st
{
    PP_TX_Delay_REQ,
    PP_TX_Delaying,
    PP_TX_Delay_Done,
}LoRaTXDelay_Type;

//ͨ��ָ��
enum
{
    PP_CMD_WUHead = 0x01,
    PP_CMD_Data   = 0x02,
    PP_CMD_RTC_SYNC = 0x03,

    PP_CMD_WUHead_ACK = 0x81,
    PP_CMD_Data_ACK   = 0x82,
};

#define IS_PP_CMD(_cmd_)    (((_cmd_) == PP_CMD_WUHead)     ||\
    ((_cmd_) == PP_CMD_Data)        ||\
    ((_cmd_) == PP_CMD_RTC_SYNC)    ||\
    ((_cmd_) == PP_CMD_WUHead_ACK)  ||\
    ((_cmd_) == PP_CMD_Data_ACK))


/***************************************************************************************************
���ͻ���ṹ
***************************************************************************************************/
#define LORA_TX_BUF_NUM         3//���ͻ�����Ŀ ��󻺳���Ҫ��1 ����������ж������

#define LORA_FRAME_MAX_BUFSIZE  128//�������ݰ��ְ�����󳤶� ���ܴ���256-5
#define LORA_MAX_FRAME_NUM       10//���ְ���
#define LORA_BUF_MAX_SIZE        (LORA_FRAME_MAX_BUFSIZE * LORA_MAX_FRAME_NUM)//���ͽ��ջ�������󳤶�


typedef struct MsgBuf_Type_st
{
    u8  Buf[LORA_TX_BUF_NUM * LORA_BUF_MAX_SIZE];
    u32 Len[LORA_TX_BUF_NUM];
    u16 DstAddr[LORA_TX_BUF_NUM];
    u32 Write;
    u32 Read;
}MsgBuf_Type;
/***************************************************************************************************
����ͷ�ṹ
***************************************************************************************************/
#define PP_WUHead_Frame_Len    (6)//����ͷ����
#define PP_WUT_K               (5)//ʱ��ϵ��
#define PP_WUHead_With_RTC_Len  (15)//��RTC������ͷ����

#define PP_TX_TRY_MAX_COUNT     (2)//����Դ���

#pragma pack(1)
//����ͷ�ṹ��
typedef struct PP_WUHead_Type_st
{
    u8  Cmd;
    u8  SpT;//ͨ��ռ��ʱ�� 1bit = 32ms total 8160ms
    u16 Src_Addr;
    u16 Dst_Addr;
    TIME_SubSec RTC_SYNC;
    u8  Sum;
}PP_WUHead_Type;
#pragma pack()

#pragma pack(1)
//����ͷ�ṹ��
typedef struct PP_Data_Type_st
{
    u8  Cmd;
    u8  SpT;//ͨ��ռ��ʱ�� 1bit = 32ms total 8160ms
    u8  CTL;//��4λ�ְ���֡������4λ �ְ����
    u8  DT;//ֻ�������ݲ��ֵĳ���
    u8  Data[RF_FIFO_SIZE - 5];
    u8  Sum;//��cmd��ʼ��У���
}PP_Data_Type;
#pragma pack()

//Э���ܽṹ��
typedef struct LoRa_PP_type_st
{
    LoRa_Dev_type*   Dev;
    u16              Self_Addr;//Э���ַ 0 ���� 1~0xFFFE ������0xFFFF�㲥
    u8               ParamNum;//�������
    u32              RTCSync_Interval;//ʱ��ͬ����� ms
    bool             RTCSync_Enable;//ʱ��㲥���ܿ������

    u16              PreambleLong;//��ǰ��
    u16              PreambleShort;//��ǰ��

    LoRa_State_Type  LoRaState;

    //CAD���Դ���
    u8               CADTryCount;
    u8               CADDoneCount;

    LoRaRx_Type      RxState;//���ջ�״̬���
    TimerEvent_t     RxErrorTimer;//����ʧ����ʱ��ʱ��
    void             (*LoRa_Recv_CallBack)(void* p, u16 src_addr, u8* buf, u16 len, s16 rssi, s8 snr);
    void*            CallBackParam;//���ջص�����               
    u8               RxBuf[LORA_BUF_MAX_SIZE];//�����ܳ�
    u32              RxTotalLen;//�����ܳ���

    MsgBuf_Type      TxBuf;
    LoRaTx_Type      TxState;//����״̬�����
    bool             TxFlag;//���ݷ��ͱ�� ��Ϊ����Ҫ�������ݵ�ʱ�����ڽ���CAD���߽������ݵĹ��� ����ֻ�����һ����� �ȴ�״̬��ͳ��
#ifdef PP_TX_DELAY_ENABLE
    LoRaTXDelay_Type TXDelayFlag;//����ǰ��ʱ���
    TimerEvent_t     TXDelayTimer;//����ǰ��ʱ��ʱ��
#endif

    //���������еĻ���ͷ��Ϣ������֡
    PP_WUHead_Type   WUHead;//����ͷ
    u16              AFrameSize;//���ճ���
    PP_Data_Type     AFrame;//����һ֡���ݳ���
    //���ݷְ����Ƽ�¼   
    u8               FrameN;//�ְ����
    u8               FrameTotal;//�ܹ���С
    u8               FrameTry;//���͹����еĳ��Դ���

    //ʱ����ؼ��� 
    u32              MaxLenTime;//һ֡��󳤶ȵ�ռ��ʱ�� ��ǰ��
    u32              WUheadTime;//����ͷռ��ʱ�� ��ǰ��
    u32              WUheadRTCTime;//ʱ��ͬ��ͷռ��ʱ�� ��Ȼ��ǰ��
    u32              WUheadRTCFrameTime;//֡����ʱ�� ����RTC������ʱʹ��

    u16              WaitACKTime;//�ȴ��Է���Ӧ�Ķ���ʱ��

    TimerEvent_t     TxBroadcastTimer;//���͹㲥��ʱ��ʱ��

    TimerEvent_t     CADTimer;//cad���ڼ�ⶨʱ��
    u16              CADPeriod;//CAD�������
    u16              CADTimeOut;//ִ��CADʱ�ĳ�ʱʱ��

    TimerEvent_t     SleepTimer;//ǿ�����߶�ʱ��

    LoRaRTCSync_Type RTCSyncState;//״̬�����
    TimerEvent_t     RTCSync_Timer;//�㲥Уʱ��ʱ��
    u32              RTCSync_Count;//Уʱ����
    bool             RTCSync_Flag;//Уʱ���
    TIME_SubSec      RTCSync_TIME;//���һ��ͬ��ʱ���ʱ��

    s16              RSSI_Last;//��¼�������һ���ź�ǿ��ֵ
    s8               SNR_Last;

    void             (*LowPowerExitFunc)(void);//�͹����˳�ִ�к���           
}LoRa_PP_type;


void LoRa_Period_Handle(LoRa_PP_type* pp);

bool LoRa_Init(LoRa_PP_type* pp, LoRa_Dev_type* dev, u32 freq, u16 self_addr, u32 rtc_interval, bool enable, void(*func)(void),u16 pb_Time);
bool LoRa_Send(LoRa_PP_type* pp, u16 dst_addr, u8* buf, u32 len);
bool LoRa_Recv_CallBack_Set(LoRa_PP_type* pp, 
                            void (*Recv_CallBack)(void* p, u16 src_addr, u8* buf, u16 len, s16 rssi, s8 snr),
                            void* p);
bool LoRa_LowPwFunc_Set(LoRa_PP_type* pp, void (*func)(void));

bool LoRa_RTC_Sync_Request(LoRa_PP_type* pp);
bool LoRa_RTC_Sync_Control(LoRa_PP_type* pp, u32 interval, bool flag);

bool LoRa_Get_RssiAndSnr(LoRa_PP_type* pp, s16* rssi, s8* snr);
bool LoRa_Set_Freq(LoRa_PP_type* pp, u32 freq);
u32 LoRa_Get_Freq(LoRa_PP_type* pp);
TIME_SubSec LoRa_Get_RTCSyncLast(LoRa_PP_type* pp);

void LoRa_Start(LoRa_PP_type* pp);
void LoRa_Stop(LoRa_PP_type* pp);
#endif /*_LORA_PP_H*/

