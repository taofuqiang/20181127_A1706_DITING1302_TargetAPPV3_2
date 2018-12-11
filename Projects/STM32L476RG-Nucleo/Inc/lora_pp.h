/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          lora_pp.h
*\Description   LoRa传输控制协议
*\Note          
*\Log           2018.05.11    Ver 1.0    Job
*               创建文件。
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
    EXE_NO = 0,//无需再执行
    EXE_NEED = !EXE_NO, //需要再次执行
}PP_Exe_Type;

//用于协议整体状态机
typedef enum LoRa_State_Type_st
{
    PP_IDLE,

    PP_TX_CAD_REQ,//CAD请求
    PP_TX_CADING,

    PP_TX_REQ,//请求发送
    PP_TXING,//发送中
    
    PP_RX_CAD_REQ,//CAD请求
    PP_RX_CADING,

    PP_RX_REQ,
    PP_RXING,

    PP_RTC_SYNC_CADING,//广播校时冲突检测
    PP_RTC_SYNC_REQ,//请求发送广播校时
    PP_RTC_SYNC_ING,//校时中

    PP_SLEEP,//强制休眠
    PP_SLEEP_EXIT,
}LoRa_State_Type;

//用于接收状态机
typedef enum LoRaRx_Type_st
{
    PP_RX_WUHead_Wait,//等待唤醒头
    PP_RX_WUHead_RecvDone,//已经接收到唤醒头
    PP_RX_WUHead_RecvError,//唤醒头接收错误

    PP_RX_WUHead_ACK_SendReq,//
    PP_RX_WUHead_ACK_Sending,//回传唤醒ACK
    PP_RX_WUHead_ACK_SendDone,//发送完成
    PP_RX_WUHead_ACK_SendError,//发送失败

    PP_RX_Data_RecvReady,
    PP_RX_Data_RecvWait,//接收数据
    PP_RX_Data_RecvDone,
    PP_RX_Data_RecvError,
    PP_RX_Data_RecvError_Delaying,
    PP_RX_Data_RecvError_DelayDone,

    PP_RX_Data_ACK_SendReq,//数据ACK
    PP_RX_Data_ACK_Sending,//回传唤醒ACK
    PP_RX_Data_ACK_SendDone,//发送完成
    PP_RX_Data_ACK_SendError,//发送失败

    PP_RX_Data_Done,//接收完成    
}LoRaRx_Type;

//发送状态机
typedef enum LoRaTx_Type_st
{
    //唤醒头发送部分状态
    PP_TX_WUHead_Ready,//发送唤醒头
    PP_TX_WUHead_Sending,//发送中
    PP_TX_WUHead_SendDone,//发送完成
    PP_TX_WUHead_SendError,//发送错误
    
    PP_TX_BD_WUHead_Delaying,//唤醒头阶段延时
    PP_TX_BD_WUHead_DelayDone,//延时到

    //唤醒头接收ACK部分状态
    PP_TX_WUHead_ACK_Wait,
    PP_TX_WUHead_ACK_Done,
    PP_TX_WUHead_ACK_Error,//超时或者出错均是error

    //数据部分发送状态
    PP_TX_Data_Ready,
    PP_TX_Data_Sending,//数据发送中
    PP_TX_Data_SendDone,
    PP_TX_Data_SendError,

    PP_TX_BD_Data_Delaying,//Data阶段延时
    PP_TX_BD_Data_DelayDone,//延时到

    //数据部分接收ACK部分状态
    PP_TX_Data_ACK_Wait,
    PP_TX_Data_ACK_Done,
    PP_TX_Data_ACK_Error,
}LoRaTx_Type;

//时间同步状态机
typedef enum LoRaRTCSync_Type_st
{
    //唤醒头发送部分状态
    PP_RTCSync_Ready,//发送唤醒头
    PP_RTCSync_Sending,//发送中
    PP_RTCSync_SendDone,//发送完成
    PP_RTCSync_SendError,//发送错误
}LoRaRTCSync_Type;

//发送前延时类型
typedef enum LoRaTXDelay_Type_st
{
    PP_TX_Delay_REQ,
    PP_TX_Delaying,
    PP_TX_Delay_Done,
}LoRaTXDelay_Type;

//通信指令
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
发送缓冲结构
***************************************************************************************************/
#define LORA_TX_BUF_NUM         3//发送缓冲数目 最大缓冲需要减1 相等是用来判断溢出的

#define LORA_FRAME_MAX_BUFSIZE  128//发送数据包分包的最大长度 不能大于256-5
#define LORA_MAX_FRAME_NUM       10//最大分包数
#define LORA_BUF_MAX_SIZE        (LORA_FRAME_MAX_BUFSIZE * LORA_MAX_FRAME_NUM)//发送接收缓冲区最大长度


typedef struct MsgBuf_Type_st
{
    u8  Buf[LORA_TX_BUF_NUM * LORA_BUF_MAX_SIZE];
    u32 Len[LORA_TX_BUF_NUM];
    u16 DstAddr[LORA_TX_BUF_NUM];
    u32 Write;
    u32 Read;
}MsgBuf_Type;
/***************************************************************************************************
唤醒头结构
***************************************************************************************************/
#define PP_WUHead_Frame_Len    (6)//唤醒头长度
#define PP_WUT_K               (5)//时间系数
#define PP_WUHead_With_RTC_Len  (15)//带RTC的数据头长度

#define PP_TX_TRY_MAX_COUNT     (2)//最大尝试次数

#pragma pack(1)
//唤醒头结构体
typedef struct PP_WUHead_Type_st
{
    u8  Cmd;
    u8  SpT;//通信占用时间 1bit = 32ms total 8160ms
    u16 Src_Addr;
    u16 Dst_Addr;
    TIME_SubSec RTC_SYNC;
    u8  Sum;
}PP_WUHead_Type;
#pragma pack()

#pragma pack(1)
//唤醒头结构体
typedef struct PP_Data_Type_st
{
    u8  Cmd;
    u8  SpT;//通信占用时间 1bit = 32ms total 8160ms
    u8  CTL;//高4位分包总帧数，低4位 分包序号
    u8  DT;//只包括数据部分的长度
    u8  Data[RF_FIFO_SIZE - 5];
    u8  Sum;//从cmd开始的校验和
}PP_Data_Type;
#pragma pack()

//协议总结构体
typedef struct LoRa_PP_type_st
{
    LoRa_Dev_type*   Dev;
    u16              Self_Addr;//协议地址 0 保留 1~0xFFFE 单播，0xFFFF广播
    u8               ParamNum;//参数序号
    u32              RTCSync_Interval;//时间同步间隔 ms
    bool             RTCSync_Enable;//时间广播功能开启标记

    u16              PreambleLong;//长前导
    u16              PreambleShort;//短前导

    LoRa_State_Type  LoRaState;

    //CAD尝试次数
    u8               CADTryCount;
    u8               CADDoneCount;

    LoRaRx_Type      RxState;//接收机状态标记
    TimerEvent_t     RxErrorTimer;//接收失败延时定时器
    void             (*LoRa_Recv_CallBack)(void* p, u16 src_addr, u8* buf, u16 len, s16 rssi, s8 snr);
    void*            CallBackParam;//接收回调参数               
    u8               RxBuf[LORA_BUF_MAX_SIZE];//数据总长
    u32              RxTotalLen;//接收总长度

    MsgBuf_Type      TxBuf;
    LoRaTx_Type      TxState;//发送状态机标记
    bool             TxFlag;//数据发送标记 因为可能要发送数据的时候正在进行CAD或者接收数据的过程 所以只是添加一个标记 等待状态机统筹
#ifdef PP_TX_DELAY_ENABLE
    LoRaTXDelay_Type TXDelayFlag;//发送前延时标记
    TimerEvent_t     TXDelayTimer;//发送前延时定时器
#endif

    //交互过程中的唤醒头信息及数据帧
    PP_WUHead_Type   WUHead;//唤醒头
    u16              AFrameSize;//接收长度
    PP_Data_Type     AFrame;//接收一帧数据长度
    //数据分包机制记录   
    u8               FrameN;//分包序号
    u8               FrameTotal;//总共大小
    u8               FrameTry;//发送过程中的尝试次数

    //时间相关计算 
    u32              MaxLenTime;//一帧最大长度的占用时间 短前导
    u32              WUheadTime;//唤醒头占用时间 短前导
    u32              WUheadRTCTime;//时间同步头占用时间 必然长前导
    u32              WUheadRTCFrameTime;//帧传输时间 计算RTC纠正延时使用

    u16              WaitACKTime;//等待对方响应的额外时间

    TimerEvent_t     TxBroadcastTimer;//发送广播延时定时器

    TimerEvent_t     CADTimer;//cad周期检测定时器
    u16              CADPeriod;//CAD检测周期
    u16              CADTimeOut;//执行CAD时的超时时间

    TimerEvent_t     SleepTimer;//强制休眠定时器

    LoRaRTCSync_Type RTCSyncState;//状态机标记
    TimerEvent_t     RTCSync_Timer;//广播校时定时器
    u32              RTCSync_Count;//校时计数
    bool             RTCSync_Flag;//校时标记
    TIME_SubSec      RTCSync_TIME;//最后一次同步时候的时间

    s16              RSSI_Last;//记录接收最后一次信号强度值
    s8               SNR_Last;

    void             (*LowPowerExitFunc)(void);//低功耗退出执行函数           
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

