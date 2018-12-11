/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          lora_pp.c
*\Description   
*\Note          lora_sx1278_preamble_set       37us
                lora_sx1278_send       RTC同步 3.4ms 发送长度不同时间会不同 245字节大约5.7ms 99字节大约3.7ms
                lora_sx1278_recv               3.4ms
*\Log           2018.05.11    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "lora_pp.h"
#include "timeServer.h"


#define LoRa_Printf(...)  printf(__VA_ARGS__)

#define MIN_PREAMBLE_SIZE   8

//3次CAD有2次则触发接收 否则退出接收
#define CADING_TRY_MAX_COUNT 2
#define CADDONE_TRIG_COUNT   (CADING_TRY_MAX_COUNT)//不能大于上面

typedef struct
{
    RadioBW_t    eBW;
    RadioSF_t    eSF;
    RadioFEC_t    eFEC;
} BW_SF_FEC;

const static BW_SF_FEC LoRaParamList[] =
{
    //{RF_BW_31250, RF_SF_12, RF_FEC_4_5}, /* BR_ON_AIR_66 */
    //{RF_BW_62500, RF_SF_12, RF_FEC_4_5}, /* BR_ON_AIR_132 */
    //{RF_BW_62500, RF_SF_11, RF_FEC_4_5}, /* BR_ON_AIR_243 */
    //{RF_BW_62500, RF_SF_10, RF_FEC_4_5}, /* BR_ON_AIR_443 */
    {RF_BW_125000, RF_SF_10, RF_FEC_4_5}, /* BR_ON_AIR_887 */
    {RF_BW_125000, RF_SF_9, RF_FEC_4_5}, /* BR_ON_AIR_1602 */
    {RF_BW_125000, RF_SF_8, RF_FEC_4_5}, /* BR_ON_AIR_2876 */
    {RF_BW_125000, RF_SF_7, RF_FEC_4_5}, /* BR_ON_AIR_5084 */
    {RF_BW_250000, RF_SF_7, RF_FEC_4_5}, /* BR_ON_AIR_10168 */
    {RF_BW_500000, RF_SF_7, RF_FEC_4_5}, /* BR_ON_AIR_20334 */
};

/***************************************************************************************************
*\Function      TIME_SubSec_Diff
*\Description   计算两个时间的毫秒差 
*\Parameter     t1
*\Parameter     t2
*\Return        u32
*\Note          t1 > t2 则返回+，否则返回负数
*\Log           2018.05.21    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
s64 TIME_SubSec_Diff(TIME_SubSec t1, TIME_SubSec t2)
{
    s64 t1_s = 0;
    s64 t2_s = 0;

    //当正在平移时间时 可能会出现亚秒大于999的情况 
    //时间合法性  不合法则不修改
    if ((TimeDECCheck(&t1.T) == FALSE) || (TimeDECCheck(&t2.T) == FALSE) || t1.SubSecs > 999 || t2.SubSecs > 999)
    {
        return 0;
    }

    //先转化为毫秒 然后做差
    t1_s = TimeDECToStamp(t1.T);
    t1_s = t1_s * ONE_SECOND_MS + t1.SubSecs;

    t2_s = TimeDECToStamp(t2.T);
    t2_s = t2_s * ONE_SECOND_MS + t2.SubSecs;

    return t1_s - t2_s;
}

/***************************************************************************************************
*\Function      TIME_SubSec_Add
*\Description   加毫秒
*\Parameter     t1
*\Parameter     ms
*\Return        bool
*\Note          
*\Log           2018.05.21    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
bool TIME_SubSec_AddMs(TIME_SubSec* t1, u16 nMs)
{
    u32 ms = 0;
    
    ms = t1->SubSecs + nMs;

    if (!TimeDECAddSecond(&t1->T, ms / ONE_SECOND_MS))
    {
        return FALSE;
    }
    t1->SubSecs = ms % ONE_SECOND_MS;

    return TRUE;
}
/***************************************************************************************************
*\Function      RTCSyncCorrectDelay
*\Description   RTC纠正时间 单位毫秒
*\Parameter     num
*\Return        u16
*\Note          目前暂取8000
*\Log           2018.05.21    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static u16 RTCSyncCorrectDelay(u8 num)
{
    static const u16    RTCCorrectDelay[] = 
    {
        //143, 78, 42, 25, 20, 16, 12, 9, 6, 3
        /*4, 
        4,
        4, 
        4, */
        4, 
        4, 
        4, 
        4, 
        4, 
        4
    };

    if (num >= countof(RTCCorrectDelay))
    {
        return 0;
    }

    return RTCCorrectDelay[num];
}
/***************************************************************************************************
*\Function      GetAckDelay
*\Description   发送超时额外延时
*\Parameter     num
*\Return        u16
*\Note          
*\Log           2018.05.14    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static u16 SendExtDelay(u8 num)
{
    static const u16    WaitAckDelay[] = 
    {
        /*287, 
        156,
        83, 
        50, */
        31, 
        22, 
        17, 
        16, 
        15, 
        15
    };

    if (num >= countof(WaitAckDelay))
    {
        return 0;
    }

    return WaitAckDelay[num];
}
/***************************************************************************************************
*\Function      CAD_timeout_handle
*\Description   CAD周期检测定时器超时函数
*\Parameter     p
*\Return        
*\Note          
*\Log           2018.05.12    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void CAD_Period_timeout_handle(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    //只有空闲才启用cad检测 
    if (pp->LoRaState == PP_IDLE)
    {
        //pp->LoRaState = PP_RX_CAD_REQ;
        //说明需要执行CAD检测了 中断中 防止丢失时机
        pp->LoRaState = PP_RX_CADING;

        pp->CADDoneCount = 0;
        pp->CADTryCount = 0;

        lora_sx1278_startcad(pp->Dev, pp->CADTimeOut);  
    }
    else 
    {
        //运行到此处 应该是出错了 做其他事情时应该禁用
        LoRa_Printf("Err:CAD Period Tout but not PP_IDLE[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      PP_CAD_Done
*\Description   cad中断处理
*\Parameter     p
*\Return        void
*\Note          1、检测到CAD后必须在中断中使能接收，否则在状态机中可能来不及接收而错过数据
*\Log           2018.05.12    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void PP_CAD_Done(void* p, bool flag)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LoRaState == PP_RTC_SYNC_CADING)
    {
        if (flag)
        {
            //检测到冲突 直接送去接收流程
            pp->LoRaState = PP_RX_CADING;

            pp->CADTryCount++;
            pp->CADDoneCount++;

            //3次取两次
            if (pp->CADTryCount >= CADING_TRY_MAX_COUNT)
            {
                if (pp->CADDoneCount >= CADDONE_TRIG_COUNT)
                {
                    //启动接收 在中断中 防止MCU被占用 丢失时机
                    pp->FrameTry = 0;

                    pp->RSSI_Last = 0;
                    pp->SNR_Last = 0x7F;

                    pp->LoRaState = PP_RXING;
                    pp->RxState = PP_RX_WUHead_Wait;

                    lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
                    lora_sx1278_recv(pp->Dev, pp->WUheadRTCTime, TRUE);//启动接收唤醒头
                }
                else
                {
                    LoRa_Printf("CAD[%d,%d]\r\n", pp->CADDoneCount, pp->CADTryCount);
                    //少于2次说明是干扰 退出
                    pp->LoRaState = PP_IDLE;

                    //无数据接收 再次启用CAD周期检测定时器
                    TimerStart(&pp->CADTimer);
                }      
            }
            else
            {
                lora_sx1278_startcad(pp->Dev, pp->CADTimeOut); 
            }
        }
        else 
        {          
            //没有冲突 发送数据 无需多次检测 看情形CAD只会误触发 不会少
            pp->LoRaState = PP_RTC_SYNC_REQ;
        }
    }
    else if (pp->LoRaState == PP_TX_CADING)
    {
        if (flag)
        {
            //检测到冲突 直接送去接收流程
            pp->LoRaState = PP_RX_CADING;

            pp->CADTryCount++;
            pp->CADDoneCount++;

            //3次取两次
            if (pp->CADTryCount >= CADING_TRY_MAX_COUNT)
            {
                if (pp->CADDoneCount >= CADDONE_TRIG_COUNT)
                {
                    //启动接收 在中断中 防止MCU被占用 丢失时机
                    pp->FrameTry = 0;

                    pp->RSSI_Last = 0;
                    pp->SNR_Last = 0x7F;

                    pp->LoRaState = PP_RXING;
                    pp->RxState = PP_RX_WUHead_Wait;
                    lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
                    lora_sx1278_recv(pp->Dev, pp->WUheadRTCTime, TRUE);//启动接收唤醒头
                }
                else
                {
                    LoRa_Printf("CAD[%d,%d]\r\n", pp->CADDoneCount, pp->CADTryCount);
                    //少于2次说明是干扰 退出
                    pp->LoRaState = PP_IDLE;

                    //无数据接收 再次启用CAD周期检测定时器
                    TimerStart(&pp->CADTimer);
                }      
            }
            else
            {
                lora_sx1278_startcad(pp->Dev, pp->CADTimeOut); 
            }
        }
        else 
        {          
            //没有冲突 发送数据 无需多次检测 看情形CAD只会误触发 不会少
            pp->LoRaState = PP_TX_REQ;
        }
    }
    else if (pp->LoRaState == PP_RX_CADING)
    {
        if (flag)//说明有数据要接收
        {
            pp->CADTryCount++;
            pp->CADDoneCount++;

            //3次取两次
            if (pp->CADTryCount >= CADING_TRY_MAX_COUNT)
            {
                if (pp->CADDoneCount >= CADDONE_TRIG_COUNT)
                {
                    //启动接收 在中断中 防止MCU被占用 丢失时机
                    pp->FrameTry = 0;

                    pp->RSSI_Last = 0;
                    pp->SNR_Last = 0x7F;

                    pp->LoRaState = PP_RXING;
                    pp->RxState = PP_RX_WUHead_Wait;
                    lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
                    lora_sx1278_recv(pp->Dev, pp->WUheadRTCTime, TRUE);//启动接收唤醒头
                }
                else
                {
                    LoRa_Printf("CAD[%d,%d]\r\n", pp->CADDoneCount, pp->CADTryCount);
                    //少于2次说明是干扰 退出
                    pp->LoRaState = PP_IDLE;

                    //无数据接收 再次启用CAD周期检测定时器
                    TimerStart(&pp->CADTimer);
                }      
            }
            else
            {
                lora_sx1278_startcad(pp->Dev, pp->CADTimeOut); 
            }

            //等待接收
            //pp->LoRaState = PP_RX_REQ;
        }
        else 
        {
            pp->CADTryCount++;

            if (pp->CADDoneCount != 0 && pp->CADTryCount < CADING_TRY_MAX_COUNT)
            {
                lora_sx1278_startcad(pp->Dev, pp->CADTimeOut); 
            }
            else if (pp->CADTryCount >= CADING_TRY_MAX_COUNT)
            {
                //启用接收判断
                if (pp->CADDoneCount >= CADDONE_TRIG_COUNT)
                {
                    //启动接收 在中断中 防止MCU被占用 丢失时机
                    pp->FrameTry = 0;

                    pp->RSSI_Last = 0;
                    pp->SNR_Last = 0x7F;

                    pp->LoRaState = PP_RXING;
                    pp->RxState = PP_RX_WUHead_Wait;
                    lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
                    lora_sx1278_recv(pp->Dev, pp->WUheadRTCTime, TRUE);//启动接收唤醒头
                }
                else
                {
                    LoRa_Printf("CAD[%d,%d]\r\n", pp->CADDoneCount, pp->CADTryCount);
                    //少于2次说明是干扰 退出
                    pp->LoRaState = PP_IDLE;

                    //无数据接收 再次启用CAD周期检测定时器
                    TimerStart(&pp->CADTimer);
                }  
            }
            else
            {
                pp->LoRaState = PP_IDLE;

                //无数据接收 再次启用CAD周期检测定时器
                TimerStart(&pp->CADTimer);
            }            
        }
    }
    else
    {
        //运行到此处 应该是出错了 做其他事情时应该禁用
        LoRa_Printf("Err:CADDone but not TXorRX CADing[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}

/***************************************************************************************************
*\Function      PP_CAD_Timeout
*\Description   CAD检查超时处理函数
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.12    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void PP_CAD_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    if (pp->LoRaState == PP_RTC_SYNC_CADING)
    {
        //如果超时 直接发送 概率极低
        pp->LoRaState = PP_RTC_SYNC_REQ;
    }
    else if (pp->LoRaState == PP_TX_CADING)
    {
        //如果超时 直接发送 概率极低
        pp->LoRaState = PP_TX_REQ;
    }
    else if (pp->LoRaState == PP_RX_CADING)
    {
        //cad出错超时 理论上运行到此处概率极小
        TimerStart(&pp->CADTimer);
        pp->LoRaState = PP_IDLE;//超时退出 置空闲
    }
    else
    {
        //运行到此处 应该是出错了 做其他事情时应该禁用
        LoRa_Printf("Err:CADTimeout handle but not TX or RX CADing[%d]\r\n", pp->LoRaState);
    }
    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      PP_RX_Done
*\Description   
*\Parameter     p
*\Parameter     buf
*\Parameter     size
*\Parameter     rssi
*\Parameter     snr
*\Return        void
*\Note          
*\Log           2018.05.13    Ver 1.0    Job
*               创建函数。
***************************************************************************************************/
static void PP_Rx_Done(void* p, u8* buf, u16 size, s16 rssi, s8 snr)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    lora_sx1278_sleep_set(pp->Dev);//防止连续模式一直接收

    if (!size)
    {
        pp->LoRaState = PP_IDLE;
        TimerStart(&pp->CADTimer);
        LoRa_Printf("Err:RxDone but size is 0[%d]\r\n", pp->LoRaState);
        return;
    }

    //抓取接收中的最差信号
    if(pp->RSSI_Last > rssi)
    {
        pp->RSSI_Last = rssi;
    }
    if (Abs(pp->SNR_Last) > Abs(snr))
    {
        pp->SNR_Last = snr;
    }

    if (pp->LoRaState == PP_TXING)
    {
        switch (pp->TxState)
        {
        case PP_TX_WUHead_ACK_Wait:
            //复制后在状态机中判断
            if (buf)
            {
                memcpy(&pp->WUHead, buf, Min(PP_WUHead_With_RTC_Len, size));//只需复制进去 无需
                pp->AFrameSize = size;

                pp->TxState = PP_TX_WUHead_ACK_Done;
            }
            else
            {
                pp->AFrameSize = 0;
                pp->TxState = PP_TX_WUHead_ACK_Error;
            }   
            break;
        case PP_TX_Data_ACK_Wait:
            //复制后在状态机中判断
            if (buf)
            {
                memcpy(&pp->WUHead, buf, Min(PP_WUHead_With_RTC_Len, size));//只需复制进去 无需
                pp->AFrameSize = size;

                pp->TxState = PP_TX_Data_ACK_Done;
            }
            else
            {
                pp->AFrameSize = 0;
                pp->TxState = PP_TX_Data_ACK_Error;
            }
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:RxDone TXING but no match[%d]\r\n", pp->TxState);
            break;
        } 
    }
    else if (pp->LoRaState == PP_RXING)
    {
        switch (pp->RxState)
        {
        case PP_RX_WUHead_Wait:
            //复制后在状态机中判断
            if (buf)
            {
                memcpy(&pp->WUHead, buf, Min(PP_WUHead_With_RTC_Len, size));//只需复制进去 无需
                pp->AFrameSize = size;

                pp->RxState = PP_RX_WUHead_RecvDone;
            }
            else
            {
                pp->AFrameSize = 0;
                pp->RxState = PP_RX_WUHead_RecvError;
            }     
            break;
        case PP_RX_Data_RecvWait:
            //接收到数据了
            if (buf)
            {
                memcpy(&pp->AFrame, buf, Min(RF_FIFO_SIZE, size));//只需复制进去 无需
                pp->AFrameSize = size;

                pp->RxState = PP_RX_Data_RecvDone;
            }
            else
            {
                pp->AFrameSize = 0;
                pp->RxState = PP_RX_Data_RecvError;
            }
            
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:RxDone RXING but no match[%d]\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:RxDone but not TXingOrRXing[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      PP_RX_Error
*\Description   
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.14    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void PP_Rx_Error(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    lora_sx1278_sleep_set(pp->Dev);//防止连续模式一直接收

    if (pp->LoRaState == PP_TXING)
    {
        switch (pp->TxState)
        {
        case PP_TX_WUHead_ACK_Wait:
            pp->AFrameSize = 0;
            pp->TxState = PP_TX_WUHead_ACK_Error;
            break;
        case PP_TX_Data_ACK_Wait:
            pp->AFrameSize = 0;
            pp->TxState = PP_TX_Data_ACK_Error;
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:RxErr TXING but no match[%d]\r\n", pp->TxState);
            break;
        } 
    }
    else if (pp->LoRaState == PP_RXING)
    {
        switch (pp->RxState)
        {
        case PP_RX_WUHead_Wait:
            pp->RxState = PP_RX_WUHead_RecvError;
            break;
        case PP_RX_Data_RecvWait:
            pp->RxState = PP_RX_Data_RecvError;
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:RxErr RXING but no match[%d]\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:RxErr but not TXingOrRXing[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      PP_RX_Timeout
*\Description   
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.14    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void PP_Rx_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    lora_sx1278_sleep_set(pp->Dev);//防止连续模式一直接收

    if (pp->LoRaState == PP_TXING)
    {
        switch (pp->TxState)
        {
        case PP_TX_WUHead_ACK_Wait:
            pp->AFrameSize = 0;
            pp->TxState = PP_TX_WUHead_ACK_Error;
            LoRa_Printf("TxWUHead RecvErr\r\n");
            break;
        case PP_TX_Data_ACK_Wait:
            pp->AFrameSize = 0;
            pp->TxState = PP_TX_Data_ACK_Error;
            LoRa_Printf("TxData RecvErr\r\n");
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:RxTout TXING but no match[%d]\r\n", pp->TxState);
            break;
        } 
    }
    else if (pp->LoRaState == PP_RXING)
    {
        switch (pp->RxState)
        {
        case PP_RX_WUHead_Wait:
            pp->RxState = PP_RX_WUHead_RecvError;
            LoRa_Printf("RxWUHead RecvErr\r\n");
            break;
        case PP_RX_Data_RecvWait:
            pp->RxState = PP_RX_Data_RecvError;
            LoRa_Printf("RxData RecvErr\r\n");
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:RxTout RXING but no match[%d]\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:RxTout but not TXingOrRXing[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      PP_Tx_Done
*\Description   
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.14    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void PP_Tx_Done(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LoRaState == PP_RTC_SYNC_ING)
    {
        if (pp->RTCSyncState == PP_RTCSync_Sending)
        {
            pp->RTCSyncState = PP_RTCSync_SendDone;
        }
        else
        {
            //运行到此处 应该是出错了
            LoRa_Printf("Err:TxDone RTC but no match[%d]\r\n", pp->RTCSyncState);
        }
    }
    else if (pp->LoRaState == PP_TXING)
    {
        switch (pp->TxState)
        {
        case PP_TX_WUHead_Sending:
            pp->TxState = PP_TX_WUHead_SendDone;
            break;
        case PP_TX_Data_Sending:
            pp->TxState = PP_TX_Data_SendDone;
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:TxDone TXING but no match[%d]\r\n", pp->TxState);
            break;
        }
    }
    else if (pp->LoRaState == PP_RXING)
    {
        switch(pp->RxState)
        {
        case PP_RX_WUHead_ACK_Sending:
            pp->RxState = PP_RX_WUHead_ACK_SendDone;
            break;
        case PP_RX_Data_ACK_Sending:
            pp->RxState = PP_RX_Data_ACK_SendDone;
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:TxDone RXING but no match[%d]\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:TxDone but not TXingOrRXing[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      PP_Tx_Timeout
*\Description   
*\Parameter     p
*\Return        
*\Note          
*\Log           2018.05.14    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void PP_Tx_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    if (pp->LoRaState == PP_RTC_SYNC_ING)
    {
        if (pp->RTCSyncState == PP_RTCSync_Sending)
        {
            pp->RTCSyncState = PP_RTCSync_SendError;
        }
        else
        {
            //运行到此处 应该是出错了
            LoRa_Printf("Err:TxTout RTC but no match[%d]\r\n", pp->RTCSyncState);
        }
    }
    else if (pp->LoRaState == PP_TXING)
    {
        switch (pp->TxState)
        {
        case PP_TX_WUHead_Sending:
            pp->TxState = PP_TX_WUHead_SendError;
            break;
        case PP_TX_Data_Sending:
            pp->TxState = PP_TX_Data_SendError;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:TxTout TXING but no match[%d]\r\n", pp->TxState);
            break;
        }
    }
    else if (pp->LoRaState == PP_RXING)
    {
        switch(pp->RxState)
        {
        case PP_RX_WUHead_ACK_Sending:
            pp->RxState = PP_RX_WUHead_ACK_SendError;
            break;
        case PP_RX_Data_ACK_Sending:
            pp->RxState = PP_RX_Data_ACK_SendError;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:TxTout RXING but no match[%d]\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:TxTout but not TXingOrRXing[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      SleepForce_Timeout
*\Description   强制休眠超时
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.13    Ver 1.0    Job
*               创建函数。
***************************************************************************************************/
static void Sleep_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    if (pp->LoRaState == PP_SLEEP)
    {
        pp->LoRaState = PP_SLEEP_EXIT;
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:SleepTout but not Sleep[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      RxError_Timeout
*\Description   接收错误延时定时器
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.18    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static void PP_RxError_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    if (pp->LoRaState == PP_RXING)
    {
        if (pp->RxState == PP_RX_Data_RecvError_Delaying)
        {
            pp->RxState = PP_RX_Data_RecvError_DelayDone;
        }
        else
        {
            LoRa_Printf("Err:RxErrTout but not Delaying[%d]\r\n", pp->RxState);
        }
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:RxErrTout but not RXING[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
#ifdef PP_TX_DELAY_ENABLE
/***************************************************************************************************
*\Function      PP_TxDelay_Timeout
*\Description   
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.22    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static void PP_TxDelay_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    //只要不是ing说明被接收打断过 重新启用延时请求
    if (pp->TXDelayFlag == PP_TX_Delaying)
    {
        pp->TXDelayFlag = PP_TX_Delay_Done;
    }
    else
    {
        pp->TXDelayFlag = PP_TX_Delay_REQ;
    }

    LoRa_Period_Handle(pp);
}
#endif
/***************************************************************************************************
*\Function      PP_TxBroadcast_Timeout
*\Description   广播消息发送延时回调
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.15    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
static void PP_TxBroadcast_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    if (pp->LoRaState == PP_TXING)
    {
        switch(pp->TxState)
        {
        case PP_TX_BD_WUHead_Delaying:
            pp->TxState = PP_TX_BD_WUHead_DelayDone;
            break;
        case PP_TX_BD_Data_Delaying:
            pp->TxState = PP_TX_BD_Data_DelayDone;
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:TxBDTout but no match[%d]\r\n", pp->TxState);
            break;
        }
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:TxBDTout but not TXING[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      PP_RTC_Sync_Timeout
*\Description   校时定时器管理
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.19    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
static void PP_RTC_Sync_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    //等于0 则说明定时时间到了 执行广播校时
    if (pp->RTCSync_Count == 0)
    {
        pp->RTCSync_Flag = TRUE;
    }
    else
    {
        //继续执行定时
        if (pp->RTCSync_Count > ONE_MINUTE_MS)
        {
            TimerSetValue(&pp->RTCSync_Timer, ONE_MINUTE_MS);      
            pp->RTCSync_Count -= ONE_MINUTE_MS;
        }
        else
        {          
            TimerSetValue(&pp->RTCSync_Timer, pp->RTCSync_Count);
            pp->RTCSync_Count = 0;
        }

        TimerStart(&pp->RTCSync_Timer);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      LoRa_Init
*\Description   lora协议初始化
*\Parameter     pp
*\Parameter     dev
*\Parameter     freq 通信频率
*\Parameter     self_addr 自身地址
*\Parameter     rtc_interval 校时间隔
*\Parameter     enable校时是否使能 节点角色不能使能
*\Parameter     
*\Parameter     func 低功耗回调函数 无则NULL
*\Parameter     
*\Return        bool
*\Note          
*\Log           2018.05.23    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
bool LoRa_Init(LoRa_PP_type* pp, LoRa_Dev_type* dev, u32 freq, u16 self_addr, u32 rtc_interval, bool enable, void(*func)(void),u16 pb_Time)
{
    TIME_SubSec INIT_T = {INIT_TIME, 0};
    u32 preamble_time = 0;//前导码时间 CAD检测周期的时间比这个要小
    //休眠机制和使用参数暂时内部指定 不对外开放
//    u16 PreambleTime = 300;//唤醒帧前导时间 休眠唤醒CAD是此时间的1/3
		u16 PreambleTime = pb_Time;
    u8 num = 3;//通信参数选择0~6

    if (pp == NULL || dev == NULL || num >= countof(LoRaParamList) || self_addr == 0)
    {
        LoRa_Printf("LoRa init fail!\r\n");
        return FALSE;
    }

    if (PreambleTime < 60)
    {
        PreambleTime = 60;
    }

    //最小同步时间为60秒
    /*if (rtc_interval < ONE_MINUTE_MS)
    {
        rtc_interval = ONE_MINUTE_MS;
    }*/

    pp->Dev = dev;
    pp->ParamNum = num;
    pp->Self_Addr = self_addr;

    pp->PreambleShort = MIN_PREAMBLE_SIZE;

    //TxBuf 初始化
    pp->TxBuf.Write = pp->TxBuf.Read = 0;
    pp->TxFlag = FALSE;

    pp->WaitACKTime = SendExtDelay(num) + 5;//ms

    pp->RTCSync_Interval = rtc_interval;
    pp->RTCSync_Enable = enable;
    pp->TXDelayFlag = PP_TX_Delay_REQ;

    pp->RTCSync_TIME = INIT_T;

    pp->LowPowerExitFunc = func;
    pp->LoRa_Recv_CallBack = NULL;

    //初始化LoRa及配置参数 
    lora_sx1278_init(dev);
    lora_sx1278_freq_set(dev, freq);
    lora_sx1278_lowpw_func_set(dev, func);//此函数必须在init之后 init会将内部的设置为NULL

    //计算需要的时间等参数
    //计算最小时间的前导码长度
    pp->PreambleLong = lora_calc_preamble_num(dev, PreambleTime, LoRaParamList[num].eBW, LoRaParamList[num].eSF);
    if (pp->PreambleLong < MIN_PREAMBLE_SIZE)
    {
        pp->PreambleLong = MIN_PREAMBLE_SIZE;  
    }
    //设置LoRa参数
    lora_sx1278_set_callback(dev, PP_Tx_Done, PP_Tx_Timeout, PP_Rx_Done, PP_Rx_Timeout, PP_Rx_Error,\
        PP_CAD_Done, PP_CAD_Timeout, pp);

    lora_sx1278_param_set(dev, LoRaParamList[num].eBW, LoRaParamList[num].eSF, LoRaParamList[num].eFEC, pp->PreambleLong, 100);

    lora_sx1278_sleep_set(dev);

    //计算前导码时间 此参数必须在设置参数后调用 因为使用的是内部变量计算
    preamble_time = lora_calc_preamble_time(dev, pp->PreambleLong);//此时间将作为CAD周期的参考
    pp->CADTimeOut = lora_calc_cad_time(dev) + 10;//CAD超时时间 给额外10ms 防止太短

    //此参数必须在设置参数后调用 因为使用的是内部变量计算
    //MaxLenTime特别加长
    pp->MaxLenTime = lora_calc_onair_time(dev, MIN_PREAMBLE_SIZE, (LORA_FRAME_MAX_BUFSIZE + 5)) + SendExtDelay(pp->ParamNum) * 2;
    pp->WUheadTime = lora_calc_onair_time(dev, MIN_PREAMBLE_SIZE, PP_WUHead_Frame_Len) + SendExtDelay(pp->ParamNum);
    //因为带RTC的必然存在于长前导 所以使用最大前导数计算
    pp->WUheadRTCTime = lora_calc_onair_time(dev, pp->PreambleLong, PP_WUHead_With_RTC_Len) + SendExtDelay(pp->ParamNum) * 1;
    pp->WUheadRTCFrameTime = lora_calc_onair_time(dev, pp->PreambleLong, PP_WUHead_With_RTC_Len);

    //CAD周期检测定时器
    pp->CADPeriod = preamble_time/2;// / 2;     // 减去20 为 280ms侦听间隔
    TimerInit(&pp->CADTimer, CAD_Period_timeout_handle, pp);
    TimerSetValue(&pp->CADTimer, pp->CADPeriod);

    //强制休眠定时器
    TimerInit(&pp->SleepTimer, Sleep_Timeout, pp);

    //广播消息延时
    TimerInit(&pp->TxBroadcastTimer, PP_TxBroadcast_Timeout, pp);

    //接收错误定时器
    TimerInit(&pp->RxErrorTimer, PP_RxError_Timeout, pp);
#ifdef PP_TX_DELAY_ENABLE
    //发送前延时定时器
    TimerInit(&pp->TXDelayTimer, PP_TxDelay_Timeout, pp);
    TimerSetValue(&pp->TXDelayTimer, 50);//pp->CADPeriod);
#endif
    //广播校时
    TimerInit(&pp->RTCSync_Timer, PP_RTC_Sync_Timeout, pp);
    pp->RTCSync_Flag = FALSE;
    //if (pp->RTCSync_Enable)
    //{
    //    //由于定时器最大定时65秒 当大于1分钟时 先定时60s
    //    //第一次上电10s内先触发一下
    //    pp->RTCSync_Count = 1000 * 10;//pp->RTCSync_Interval;

    //    if (pp->RTCSync_Count > ONE_MINUTE_MS)
    //    {
    //        TimerSetValue(&pp->RTCSync_Timer, ONE_MINUTE_MS);      
    //        pp->RTCSync_Count -= ONE_MINUTE_MS;
    //    }
    //    else
    //    {          
    //        TimerSetValue(&pp->RTCSync_Timer, pp->RTCSync_Count);
    //        pp->RTCSync_Count = 0;
    //    }

    //    TimerStart(&pp->RTCSync_Timer);
    //}

    pp->LoRaState = PP_IDLE;
    //启用CAD检测 初始化完成不启动
    //TimerStart(&pp->CADTimer);

    return TRUE;
}

/***************************************************************************************************
*\Function      LoRa_Start
*\Description   启动LoRa协议栈
*\Parameter     pp
*\Return        void
*\Note          
*\Log           2018.05.25    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void LoRa_Start(LoRa_PP_type* pp)
{
    if (pp == NULL)
    {
        return;
    }

    stm32_spi_init(pp->Dev->SPI);

    pp->LoRaState = PP_IDLE;
    pp->TxFlag = FALSE;
    pp->RTCSync_Flag = FALSE;
    pp->TXDelayFlag = PP_TX_Delay_REQ;
    pp->TxBuf.Write = pp->TxBuf.Read = 0;
    
    if (pp->RTCSync_Enable)
    {
        //由于定时器最大定时65秒 当大于1分钟时 先定时60s
        //第一次上电10s内先触发一下
        pp->RTCSync_Count = 1000 * 10;//pp->RTCSync_Interval;

        if (pp->RTCSync_Count > ONE_MINUTE_MS)
        {
            TimerSetValue(&pp->RTCSync_Timer, ONE_MINUTE_MS);      
            pp->RTCSync_Count -= ONE_MINUTE_MS;
        }
        else
        {          
            TimerSetValue(&pp->RTCSync_Timer, pp->RTCSync_Count);
            pp->RTCSync_Count = 0;
        }

        TimerStart(&pp->RTCSync_Timer);
    }

    TimerStart(&pp->CADTimer);
}

/***************************************************************************************************
*\Function      LoRa_Stop
*\Description   停止LoRa协议栈进入低功耗模式
*\Parameter     pp
*\Return        void
*\Note          
*\Log           2018.05.25    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void LoRa_Stop(LoRa_PP_type* pp)
{
    if (pp == NULL)
    {
        return;
    }

    //停止所有TIMER
    TimerStop(&pp->CADTimer); 
    TimerStop(&pp->TXDelayTimer);
    TimerStop(&pp->TxBroadcastTimer);
    TimerStop(&pp->RTCSync_Timer);
    TimerStop(&pp->SleepTimer);
    TimerStop(&pp->RxErrorTimer);

    lora_sx1278_sleep_set(pp->Dev);

    stm32_spi_deinit(pp->Dev->SPI);

    pp->RTCSync_Flag = FALSE;
    pp->TxFlag = FALSE;
    pp->TxBuf.Write = pp->TxBuf.Read = 0;
    pp->TXDelayFlag = PP_TX_Delay_REQ;

    pp->LoRaState = PP_SLEEP;
}
/***************************************************************************************************
*\Function      LoRa_RTCSync_Process_Running
*\Description   时间同步状态机
*\Parameter     pp
*\Return        PP_Exe_Type
*\Note          
*\Log           2018.05.19    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
PP_Exe_Type LoRa_RTCSync_Process_Running(LoRa_PP_type* pp)
{
    PP_Exe_Type state = EXE_NO;
    PP_WUHead_Type wuhead;
    TIME_SubSec   cur;
    u32 rtc_correct = 0;

    if (pp == NULL)
    {
        return state;
    }

    if (pp->LoRaState == PP_RTC_SYNC_ING)
    {
        switch (pp->RTCSyncState)
        {
        case PP_RTCSync_Ready:
            {
                //如果发送完成 接着进入rtc校时 则必须设置为长前导
                lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);   //37us

                wuhead.Cmd = PP_CMD_RTC_SYNC;
                wuhead.Src_Addr = pp->Self_Addr;
                wuhead.Dst_Addr = PP_BROADCAST_ADDR;
                wuhead.SpT = 0;//不占用接下来的时间            

                pp->RTCSyncState = PP_RTCSync_Sending;

                //目前该动作必然在中断中 所以无需关中断
                //获取时间需要关掉中断 保证不被打断
                //BACKUP_PRIMASK();
                //DISABLE_IRQ( );

                stm32_Inrtc_getTime(&cur);
                //需要纠正一下时间
                rtc_correct = pp->WUheadRTCFrameTime + RTCSyncCorrectDelay(pp->ParamNum);
                TIME_SubSec_AddMs(&cur, rtc_correct);

                memcpy(&wuhead.RTC_SYNC, &cur, sizeof(TIME_SubSec));//8us 从前导帧设置到此

                wuhead.Sum = Get_Sum8((u8*)&wuhead, PP_WUHead_With_RTC_Len - 1);//从cmd开始的校验和
                lora_sx1278_send(pp->Dev, (u8*)&wuhead, PP_WUHead_With_RTC_Len, pp->WUheadRTCTime);//3.4ms
                //使能中断
                //RESTORE_PRIMASK();
                //ENABLE_IRQ();

                pp->RTCSync_TIME = cur;
            }
            break;
        case PP_RTCSync_Sending:
            break;
        case PP_RTCSync_SendDone:
            pp->RTCSync_Flag = FALSE;
            pp->LoRaState = PP_IDLE;

            //至此接收完成 重启启动CAD定时器
            TimerStart(&pp->CADTimer);
            //再次使能RTC同步定时器
            if (pp->RTCSync_Enable)
            {
                //由于定时器最大定时65秒 当大于1分钟时 先定时60s
                pp->RTCSync_Count = pp->RTCSync_Interval;

                if (pp->RTCSync_Count > ONE_MINUTE_MS)
                {
                    TimerSetValue(&pp->RTCSync_Timer, ONE_MINUTE_MS);      
                    pp->RTCSync_Count -= ONE_MINUTE_MS;
                }
                else
                {          
                    TimerSetValue(&pp->RTCSync_Timer, pp->RTCSync_Count);
                    pp->RTCSync_Count = 0;
                }

                TimerStart(&pp->RTCSync_Timer);
            }
            state = EXE_NEED;
            break;
        case PP_RTCSync_SendError:
            if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                //重新启动发送唤醒帧流程
                pp->RTCSyncState = PP_RTCSync_Ready;    

                state = EXE_NEED;
            }
            else
            {
                //结束发送
                pp->RTCSync_Flag = FALSE;
                pp->LoRaState = PP_IDLE;
 
                //至此接收完成 重启启动CAD定时器
                TimerStart(&pp->CADTimer);
                 //再次使能RTC同步定时器
                if (pp->RTCSync_Enable)
                {
                    //由于定时器最大定时65秒 当大于1分钟时 先定时60s
                    pp->RTCSync_Count = pp->RTCSync_Interval;

                    if (pp->RTCSync_Count > ONE_MINUTE_MS)
                    {
                        TimerSetValue(&pp->RTCSync_Timer, ONE_MINUTE_MS);      
                        pp->RTCSync_Count -= ONE_MINUTE_MS;
                    }
                    else
                    {          
                        TimerSetValue(&pp->RTCSync_Timer, pp->RTCSync_Count);
                        pp->RTCSync_Count = 0;
                    }

                    TimerStart(&pp->RTCSync_Timer);
                }
                state = EXE_NEED;
            }
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:RTCProcess but not match[%d]\r\n", pp->RTCSyncState);
            break;
        }
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:RTCProcess but not RTCSyncING[%d]\r\n", pp->LoRaState);
    }

    return state;
}
/***************************************************************************************************
*\Function      LoRa_Tx_Process_Running
*\Description   发送状态机
*\Parameter     pp
*\Return        PP_Exe_Type
*\Note          发送要想确认发送完成，最后还需要反馈给对方完成帧 即需要3次握手 
*               否则发送方最后一帧ACK接收失败时，接收方接收完最后一帧认为完成了不再等待接收，
*               而发送方的却还在重传等待ACK

                !!!如果发送失败的话 需要休眠对应的时间 否则 其他节点还处于休眠中 是无法通信的
*\Log           2018.05.12    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
PP_Exe_Type LoRa_Tx_Process_Running(LoRa_PP_type* pp)
{
    PP_Exe_Type state = EXE_NO;
    static PP_WUHead_Type wuhead;
    u32 hold_time = 0;//通信占用时间
    u32 sleep_t = 0;

    if (pp == NULL)
    {
        return state;
    }

    if (pp->LoRaState == PP_TXING)
    {
        switch(pp->TxState)
        {
        case PP_TX_WUHead_Ready:
            wuhead.Cmd = PP_CMD_WUHead;
            wuhead.Src_Addr = pp->Self_Addr;
            wuhead.Dst_Addr = pp->TxBuf.DstAddr[pp->TxBuf.Read];
            if (pp->TxBuf.DstAddr[pp->TxBuf.Read] == PP_BROADCAST_ADDR)
            {
                hold_time = pp->FrameTotal * pp->MaxLenTime;//简单暴力 直接最大时间
                hold_time += (pp->FrameTotal + 1) * SendExtDelay(pp->ParamNum);
            }
            else
            {
                hold_time = pp->FrameTotal * pp->MaxLenTime;//简单暴力 直接最大时间
                //单播地址需要在加上ACK时间
                hold_time += (pp->FrameTotal + 1) * pp->WUheadTime;//ack要多一个
            }
            wuhead.SpT = hold_time >> PP_WUT_K;//表示接下来要占用的时间

            pp->TxState = PP_TX_WUHead_Sending;

            //唤醒使用长前导
            lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
            lora_sx1278_send(pp->Dev, (u8*)&wuhead, PP_WUHead_Frame_Len, pp->WUheadRTCTime);
            break;
        case PP_TX_WUHead_Sending:
            break;
        case PP_TX_WUHead_SendDone:
            
            //此处应该区分广播和单播
            if (pp->TxBuf.DstAddr[pp->TxBuf.Read] != PP_BROADCAST_ADDR)
            {
                //单播消息
                //发送完成 转为接收ACK
                //pp->FrameTry = 0; //不能置此标记 因为ACK接收失败后是重新发送数据 这样会导致持续循环
                //启用接收 等待ACK
                pp->TxState = PP_TX_WUHead_ACK_Wait;

                lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);  
                lora_sx1278_recv(pp->Dev, (pp->WUheadTime+pp->WaitACKTime), TRUE);//空中时间+对方准备时间 需考虑对方的重试次数  
            }
            else
            {
                //广播消息
                //直接转到发送数据部分流程 延时一段时间 等待对方接收
                //pp->FrameTry = 0;
                TimerSetValue(&pp->TxBroadcastTimer, SendExtDelay(pp->ParamNum));              
                pp->TxState = PP_TX_BD_WUHead_Delaying;
                TimerStart(&pp->TxBroadcastTimer);
            }
            break;     
        case PP_TX_WUHead_SendError:
            if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                //重新启动发送唤醒帧流程
                pp->TxState = PP_TX_WUHead_Ready;    

                state = EXE_NEED;
            }
            else
            {
                //结束此次发送 缓冲区+1
                pp->TxBuf.Read++;
                pp->TxBuf.Read %= LORA_TX_BUF_NUM;
                
                //发送失败休眠一下
                pp->LoRaState = PP_SLEEP;    
                sleep_t = wuhead.SpT;
                sleep_t <<= PP_WUT_K;

                TimerSetValue(&pp->SleepTimer, sleep_t);
                TimerStart(&pp->SleepTimer);

                //state = EXE_NEED;
            }
            break;
        case PP_TX_BD_WUHead_Delaying:
            break;
        case PP_TX_BD_WUHead_DelayDone:
            //只有广播才执行此处 所以不用检查
            pp->FrameTry = 0;
            pp->FrameN = 0;

            pp->TxState = PP_TX_Data_Ready;

            state = EXE_NEED;
            break;
        case PP_TX_WUHead_ACK_Wait:
            break;
        case PP_TX_WUHead_ACK_Done:
           
            //判断接收到的ACK是否符合 cmd dst 相等
            if (pp->AFrameSize == PP_WUHead_Frame_Len && pp->WUHead.Cmd == PP_CMD_WUHead_ACK && 
                pp->WUHead.Src_Addr == pp->TxBuf.DstAddr[pp->TxBuf.Read] && 
                pp->Self_Addr == pp->WUHead.Dst_Addr)
            {

                pp->FrameTry = 0;
                pp->FrameN = 0;

                pp->TxState = PP_TX_Data_Ready;

                state = EXE_NEED;
            }
            else
            {
                //参数不相等 重试次数判断是否重试
                if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
                {
                    //重新启动发送唤醒帧流程
                    pp->TxState = PP_TX_WUHead_Ready;    

                    state = EXE_NEED;
                }
                else
                {
                    //结束此次发送 缓冲区+1
                    pp->TxBuf.Read++;
                    pp->TxBuf.Read %= LORA_TX_BUF_NUM;
                   
                    //发送失败休眠一下
                    pp->LoRaState = PP_SLEEP;    
                    sleep_t = wuhead.SpT;//用上一次占用的时间休眠
                    sleep_t <<= PP_WUT_K;

                    TimerSetValue(&pp->SleepTimer, sleep_t);
                    TimerStart(&pp->SleepTimer);

                    //state = EXE_NEED;
                }
            }
            break;
        case PP_TX_WUHead_ACK_Error:
            if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                //重新启动发送唤醒帧流程
                pp->TxState = PP_TX_WUHead_Ready;    

                state = EXE_NEED;
            }
            else
            {
                //结束此次发送 缓冲区+1
                pp->TxBuf.Read++;
                pp->TxBuf.Read %= LORA_TX_BUF_NUM;
                
                //发送失败休眠一下
                pp->LoRaState = PP_SLEEP;    
                sleep_t = wuhead.SpT;
                sleep_t <<= PP_WUT_K;

                TimerSetValue(&pp->SleepTimer, sleep_t);
                TimerStart(&pp->SleepTimer);

                //state = EXE_NEED;
            }
            break;
        case PP_TX_Data_Ready:
            pp->AFrame.Cmd = PP_CMD_Data;              
            if (pp->TxBuf.DstAddr[pp->TxBuf.Read] == PP_BROADCAST_ADDR)
            {
                hold_time = (pp->FrameTotal - pp->FrameN - 1) * pp->MaxLenTime;//简单暴力 直接最大时间
                hold_time += (pp->FrameTotal - pp->FrameN) * SendExtDelay(pp->ParamNum);
            }
            else
            {
                hold_time = (pp->FrameTotal - pp->FrameN - 1) * pp->MaxLenTime;//简单暴力 直接最大时间
                //单播地址需要在加上ACK时间
                hold_time += (pp->FrameTotal - pp->FrameN) * pp->WUheadTime;
            }
            pp->AFrame.SpT = hold_time >> PP_WUT_K;
            pp->AFrame.CTL = (pp->FrameTotal << 4) | (pp->FrameN & 0x0F);
            if (pp->FrameN + 1 >= pp->FrameTotal)
            {
                //说明是最后一帧
                pp->AFrame.DT = pp->TxBuf.Len[pp->TxBuf.Read] - pp->FrameN * LORA_FRAME_MAX_BUFSIZE;
            }
            else
            {
                pp->AFrame.DT = LORA_FRAME_MAX_BUFSIZE;
            }
            memcpy(pp->AFrame.Data, &pp->TxBuf.Buf[pp->TxBuf.Read * LORA_BUF_MAX_SIZE + pp->FrameN * LORA_FRAME_MAX_BUFSIZE], pp->AFrame.DT);

            //计算最后的sum
            pp->AFrame.Sum = Get_Sum8((u8*)&pp->AFrame, pp->AFrame.DT + 4);//整帧的校验和
            //将Sum移动到合适的位置
            ((u8*)&pp->AFrame)[pp->AFrame.DT + 4] = pp->AFrame.Sum;

            pp->TxState = PP_TX_Data_Sending;

            lora_sx1278_preamble_set(pp->Dev, pp->PreambleShort);
            lora_sx1278_send(pp->Dev, (u8*)&pp->AFrame, pp->AFrame.DT + 5, pp->MaxLenTime);
            break;
        case PP_TX_Data_Sending:
            break;
        case PP_TX_Data_SendDone:
            //此处应该区分广播和单播
            if (pp->TxBuf.DstAddr[pp->TxBuf.Read] != PP_BROADCAST_ADDR)
            {
                //单播消息
                //发送完成 转为接收ACK
                //pp->FrameTry = 0;//不能置此标记 因为ACK接收失败后是重新发送数据 这样会导致持续循环
                //启用接收 等待ACK
                pp->TxState = PP_TX_Data_ACK_Wait;

                lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
                lora_sx1278_recv(pp->Dev, (pp->WUheadTime+pp->WaitACKTime), TRUE);//空中时间+对方准备时间               
            }
            else
            {
                //广播消息
                //直接转到发送数据部分流程
                TimerSetValue(&pp->TxBroadcastTimer, SendExtDelay(pp->ParamNum));              
                pp->TxState = PP_TX_BD_Data_Delaying;
                TimerStart(&pp->TxBroadcastTimer);
            }
            break;
        case PP_TX_Data_SendError:  
            //广播发送失败 直接退出
            if (pp->TxBuf.DstAddr[pp->TxBuf.Read] != PP_BROADCAST_ADDR && ++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                //重新启动发送唤醒帧流程
                pp->TxState = PP_TX_Data_Ready;    

                state = EXE_NEED;
            }
            else
            {
                //结束此次发送 缓冲区+1
                pp->TxBuf.Read++;
                pp->TxBuf.Read %= LORA_TX_BUF_NUM;
                
                //发送失败休眠一下
                pp->LoRaState = PP_SLEEP;    
                sleep_t = pp->AFrame.SpT;//用上一次占用的时间休眠
                sleep_t <<= PP_WUT_K;

                TimerSetValue(&pp->SleepTimer, sleep_t);
                TimerStart(&pp->SleepTimer);

                //state = EXE_NEED;
            }
            break;
        case PP_TX_BD_Data_Delaying:
            break;
        case PP_TX_BD_Data_DelayDone:
            //只有广播才执行此处
            if (pp->TxBuf.DstAddr[pp->TxBuf.Read] == PP_BROADCAST_ADDR)
            {
                pp->FrameTry = 0;
                pp->FrameN++;

                if (pp->FrameN >= pp->FrameTotal)
                {
                    //发送完成
                    pp->TxBuf.Read++;
                    pp->TxBuf.Read %= LORA_TX_BUF_NUM;

                    pp->LoRaState = PP_IDLE;

                    state = EXE_NEED;
                }
                else
                {
                    //未完成则继续发下一帧
                    pp->TxState = PP_TX_Data_Ready;
                }

                state = EXE_NEED;
            }
            else
            {
                //不是广播执行到次数 错误了
                pp->LoRaState =  PP_IDLE;

                state = EXE_NEED;
            }
            break;
        case PP_TX_Data_ACK_Wait:
            break;
        case PP_TX_Data_ACK_Done:
            if (pp->AFrameSize == PP_WUHead_Frame_Len && pp->WUHead.Cmd == PP_CMD_Data_ACK && 
                pp->WUHead.Src_Addr == pp->TxBuf.DstAddr[pp->TxBuf.Read] && 
                pp->Self_Addr == pp->WUHead.Dst_Addr)
            {

                pp->FrameTry = 0;
                pp->FrameN++;

                if (pp->FrameN >= pp->FrameTotal)
                {
                    //发送完成
                    pp->TxBuf.Read++;
                    pp->TxBuf.Read %= LORA_TX_BUF_NUM;

                    pp->LoRaState = PP_IDLE;

                    state = EXE_NEED;
                }
                else
                {
                    //未完成则继续发下一帧
                    pp->TxState = PP_TX_Data_Ready;
                }

                state = EXE_NEED;
            }
            else
            {
                //参数不相等 重试次数判断是否重试
                if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
                {
                    //重新启动发送唤醒帧流程
                    pp->TxState = PP_TX_Data_Ready;    

                    state = EXE_NEED;
                }
                else
                {
                    //结束此次发送 缓冲区+1
                    pp->TxBuf.Read++;
                    pp->TxBuf.Read %= LORA_TX_BUF_NUM;

                    //发送失败休眠一下
                    pp->LoRaState = PP_SLEEP;    
                    sleep_t = pp->AFrame.SpT;//用上一次占用的时间休眠
                    sleep_t <<= PP_WUT_K;

                    TimerSetValue(&pp->SleepTimer, sleep_t);
                    TimerStart(&pp->SleepTimer);

                    //state = EXE_NEED;
                }
            }
            break;
        case PP_TX_Data_ACK_Error:
            //参数不相等 重试次数判断是否重试
            if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                //重新启动发送唤醒帧流程
                pp->TxState = PP_TX_Data_Ready;    

                state = EXE_NEED;
            }
            else
            {
                //结束此次发送 缓冲区+1
                pp->TxBuf.Read++;
                pp->TxBuf.Read %= LORA_TX_BUF_NUM;

                //发送完成休眠一下
                pp->LoRaState = PP_SLEEP;    
                sleep_t = pp->AFrame.SpT;//用上一次占用的时间休眠
                sleep_t <<= PP_WUT_K;

                TimerSetValue(&pp->SleepTimer, sleep_t);
                TimerStart(&pp->SleepTimer);

                //state = EXE_NEED;
            }
            break;
        default:
            //运行到此处 应该是出错了
            LoRa_Printf("Err:TxProcess but not match[%d]\r\n", pp->TxState);
            break;
        }
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:TxProcess but not PP_TXING[%d]\r\n", pp->LoRaState);
    }

    return state;
}
/***************************************************************************************************
*\Function      LoRa_Rx_Process_Running
*\Description   接收状态机
*\Parameter     pp
*\Return        PP_Exe_Type
*\Note          中间接收失败 也需要延时对应的时间 否则再去发送 但是节点们还是处于休眠状态
*\Log           2018.05.12    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
PP_Exe_Type LoRa_Rx_Process_Running(LoRa_PP_type* pp)
{
    PP_Exe_Type state = EXE_NO;
    u32 sleep_t = 0;
    u8 abnormal_flag = 0;
    static PP_WUHead_Type ack;
    u32 send_timeout = 0;
    TIME_SubSec cur;
    s64 diff = 0;
    u32 sleep_time = 0;

    if (pp == NULL)
    {
        return state;
    }

    if (pp->LoRaState == PP_RXING)
    {
        switch(pp->RxState)
        {
        case PP_RX_WUHead_Wait:
            break;
        case PP_RX_WUHead_RecvDone:           
            //已经接收到唤醒头 进行相应处理
            if (pp->AFrameSize == PP_WUHead_With_RTC_Len && pp->WUHead.Cmd == PP_CMD_RTC_SYNC && 
                pp->WUHead.Sum == Get_Sum8((u8*)&pp->WUHead, PP_WUHead_With_RTC_Len - 1))
            {
                //需要关中断 目前必然在中断中处理 所以不用管
                //BACKUP_PRIMASK();
                //DISABLE_IRQ( );
                if (TimeDECCheck(&pp->WUHead.RTC_SYNC.T) && stm32_Inrtc_getTime(&cur))
                {
                    diff = TIME_SubSec_Diff(pp->WUHead.RTC_SYNC, cur);
                    //小于1秒使用平移 否则 直接设置
                    if (diff > -1000 && diff < 1000)
                    {
                        stm32_Inrtc_msShift(diff);
                    }
                    else
                    {
                        //差太多 直接修改时间
                        stm32_Inrtc_setTime(pp->WUHead.RTC_SYNC);
                    } 
                    //记录最后一次时间同步时间
                    pp->RTCSync_TIME = pp->WUHead.RTC_SYNC;

                    LoRa_Printf("%02d:%02d:%02d  %03d\r\n", pp->WUHead.RTC_SYNC.T.Hour, pp->WUHead.RTC_SYNC.T.Minute, pp->WUHead.RTC_SYNC.T.Second, pp->WUHead.RTC_SYNC.SubSecs);
                    LoRa_Printf("%02d:%02d:%02d  %03d\r\n", cur.T.Hour, cur.T.Minute, cur.T.Second, cur.SubSecs);
                    LoRa_Printf("RTC[%d]\r\n", (s32)diff);
                }
                //使能中断
                //RESTORE_PRIMASK();
                //ENABLE_IRQ();

                //退出接收
                pp->LoRaState = PP_IDLE;

                TimerStart(&pp->CADTimer);

                state = EXE_NEED;
            }
            else if (pp->AFrameSize == PP_WUHead_Frame_Len && pp->WUHead.Cmd == PP_CMD_WUHead)
            {
                if (pp->WUHead.Dst_Addr == PP_BROADCAST_ADDR) //广播地址
                {
                    pp->FrameTry = 0;
                    pp->FrameTotal = 0;
                    pp->RxTotalLen = 0;
                    pp->FrameN = 0;
                    //广播地址 直接进行接收数据 由于不明确发送端的发送时机 所以需要处于连续接收模式
                    pp->RxState = PP_RX_Data_RecvReady;
                    
                    state = EXE_NEED;
                }
                else if (pp->WUHead.Dst_Addr == pp->Self_Addr)
                { 
                    pp->FrameTry = 0;
                    pp->FrameTotal = 0;
                    pp->RxTotalLen = 0;
                    pp->FrameN = 0;

                    pp->RxState = PP_RX_WUHead_ACK_SendReq;

                    state = EXE_NEED;
                }
                else
                {
                    //地址不相符 根据时间睡眠去
                    //此处是否要考虑重试中占用的最大时间 暂不考虑 
                    sleep_t = pp->WUHead.SpT;
                    sleep_t <<= PP_WUT_K;
                    pp->LoRaState = PP_SLEEP;
                    TimerSetValue(&pp->SleepTimer, sleep_t);
                    TimerStart(&pp->SleepTimer);

                    LoRa_Printf("sleep[%d]\r\n", sleep_t);
                }
            }
            else
            {
                //此处应该进行异常状态的处理
                abnormal_flag = 1;
            }
            break;
        case PP_RX_WUHead_RecvError:
            //接收错误 此次唤醒头接收错误 再次启动CAD检测 确保不要丢失唤醒头       
            pp->LoRaState = PP_RX_CAD_REQ;
            state = EXE_NEED;
            break;
        case PP_RX_WUHead_ACK_SendReq:
            //SpT头部唤醒时接收的 分开是为了便于重发
            send_timeout = pp->WUheadTime;
            send_timeout >>= PP_WUT_K;
            //自己地址 发送唤醒ACK
            ack.Cmd = PP_CMD_WUHead_ACK;
            ack.SpT = pp->WUHead.SpT - send_timeout;
            ack.Dst_Addr = pp->WUHead.Src_Addr;
            ack.Src_Addr = pp->Self_Addr;
            pp->RxState = PP_RX_WUHead_ACK_Sending;
            lora_sx1278_preamble_set(pp->Dev, pp->PreambleShort);
            lora_sx1278_send(pp->Dev, (u8*)&ack, PP_WUHead_Frame_Len, pp->WUheadTime);  
            break;
        case PP_RX_WUHead_ACK_Sending:
            //等待发送ACK完成中断或超时
            break;
        case PP_RX_WUHead_ACK_SendDone:
            //ACK发送完成 转换到接收数据 
            pp->FrameTry = 0;
            pp->FrameTotal = 0;
            pp->FrameN = 0;
            pp->RxTotalLen = 0;

            pp->RxState = PP_RX_Data_RecvReady;

            state = EXE_NEED;
            break;
        case PP_RX_WUHead_ACK_SendError:
            //重试次数判断是否重试
            if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                //重新启动发送唤醒帧流程
                pp->RxState = PP_RX_WUHead_ACK_SendReq;    

                state = EXE_NEED;
            }
            else
            {   

                //接收失败 也需要休眠对应的时间 而不能发送
                sleep_time = ack.SpT;
                sleep_time <<= PP_WUT_K;
    
                if (sleep_time)
                {
                    pp->LoRaState = PP_SLEEP;
                    TimerSetValue(&pp->SleepTimer, sleep_time);
                    TimerStart(&pp->SleepTimer);
                }
                else
                {
                    pp->LoRaState = PP_IDLE;
  
                    TimerStart(&pp->CADTimer);
                    state = EXE_NEED;
                }
            }
            break;
        case PP_RX_Data_RecvReady:
            pp->RxState = PP_RX_Data_RecvWait;
            lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
            lora_sx1278_recv(pp->Dev, (pp->MaxLenTime), TRUE);  
            break;
        case PP_RX_Data_RecvWait:
            break;
        case PP_RX_Data_RecvDone:
            //接收到了则进行判断
            pp->AFrame.Sum = ((u8*)&pp->AFrame)[pp->AFrame.DT + 4];//提取sum

            if (pp->AFrameSize <= LORA_FRAME_MAX_BUFSIZE + 5 && pp->AFrameSize == (pp->AFrame.DT + 5) 
                && pp->AFrame.Cmd == PP_CMD_Data && Get_Sum8((u8*)&pp->AFrame, pp->AFrame.DT + 4) == pp->AFrame.Sum)
            {
                //经过此 基本就保证数据时正确的了
                if (pp->FrameTotal == 0)//表示第一次
                {
                    pp->FrameTotal = (pp->AFrame.CTL >> 4);
                }
                    
                if (pp->FrameTotal <= LORA_MAX_FRAME_NUM && pp->FrameN == (pp->AFrame.CTL & 0x0F))
                {
                    //复制数据到接收缓冲区
                    memcpy(pp->RxBuf + pp->FrameN * LORA_FRAME_MAX_BUFSIZE, pp->AFrame.Data, pp->AFrame.DT);
                    pp->RxTotalLen += pp->AFrame.DT;

                    pp->FrameN++;

                    //广播判断
                    if (pp->WUHead.Dst_Addr == PP_BROADCAST_ADDR)
                    {                           
                        pp->RxState = PP_RX_Data_Done;
                        state = EXE_NEED;
                        break;
                    }
                    else
                    {
                        //需要准备ACK发送
                        pp->RxState = PP_RX_Data_ACK_SendReq;
                        state = EXE_NEED;
                        break;
                    }
                }
            }
            else
            {
                //由于LoRa数据可能出现接收到数据了但是内容会出错 所以到此阶段也需要进行重试
                pp->RxState = PP_RX_Data_RecvError;
                state = EXE_NEED;
                break;
            }
            //此处表示接收异常 正常应该提前break
            abnormal_flag = 2;//by Job 2018年5月28日 此阶段不会执行了

            break;
        case PP_RX_Data_RecvError:
            //如果是广播 接收超时了 肯定就是失败了 因为没有重发
            if (pp->WUHead.Dst_Addr == PP_BROADCAST_ADDR)
            {
                //接收失败 也需要休眠对应的时间 而不能发送
                sleep_time = ack.SpT;
                sleep_time <<= PP_WUT_K;

                if (sleep_time > pp->MaxLenTime)
                {
                    sleep_time -= pp->MaxLenTime;
                }
                else
                {
                    sleep_time = 0;
                }
                if (sleep_time)
                {
                    pp->LoRaState = PP_SLEEP;
                    TimerSetValue(&pp->SleepTimer, sleep_time);
                    TimerStart(&pp->SleepTimer);
                }
                else
                {
                    pp->LoRaState = PP_IDLE;

                    TimerStart(&pp->CADTimer);
                    state = EXE_NEED;
                }
            }
            else
            {
                u16 delay_t = pp->WUheadTime + pp->WaitACKTime;

                if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
                {
                    pp->RxState = PP_RX_Data_RecvError_Delaying;
                    //由于发送方等待ACK时，会有一个ACK+waitACK的延时 因为接收方对应也需要延时 需提前一下
                    TimerSetValue(&pp->RxErrorTimer, delay_t - 3);
                    TimerStart(&pp->RxErrorTimer);
                }
                else
                {
                    //接收失败 也需要休眠对应的时间 而不能发送
                    sleep_time = ack.SpT;
                    sleep_time <<= PP_WUT_K;

                    if (sleep_time > (pp->MaxLenTime + delay_t))
                    {
                        sleep_time -= (pp->MaxLenTime+delay_t);
                    }
                    else
                    {
                        sleep_time = 0;
                    }
                    if (sleep_time)
                    {
                        pp->LoRaState = PP_SLEEP;
                        TimerSetValue(&pp->SleepTimer, sleep_time);
                        TimerStart(&pp->SleepTimer);
                    }
                    else
                    {
                        pp->LoRaState = PP_IDLE;

                        TimerStart(&pp->CADTimer);
                        state = EXE_NEED;
                    }
                }
            }         
            break;
        case PP_RX_Data_RecvError_Delaying:
            break;
        case PP_RX_Data_RecvError_DelayDone:
            pp->RxState = PP_RX_Data_RecvReady;
            state = EXE_NEED;
            break;
        case PP_RX_Data_ACK_SendReq:
            //准备ACK发送
            //pp->TxWUheadTime是之前头部唤醒时接收的 分开是为了便于重发
            //自己地址 发送唤醒ACK
            send_timeout = pp->WUheadTime;
            send_timeout >>= PP_WUT_K;
            ack.Cmd = PP_CMD_Data_ACK;
            if (pp->AFrame.SpT < send_timeout)
            {
                ack.SpT = 0;
            }
            else
            {
                ack.SpT = pp->AFrame.SpT - send_timeout;
            }
            ack.Dst_Addr = pp->WUHead.Src_Addr;
            ack.Src_Addr = pp->Self_Addr;
            pp->RxState = PP_RX_Data_ACK_Sending;

            lora_sx1278_preamble_set(pp->Dev, pp->PreambleShort);
            lora_sx1278_send(pp->Dev, (u8*)&ack, PP_WUHead_Frame_Len, pp->WUheadTime);  
            break;
        case PP_RX_Data_ACK_Sending:
            break;
        case PP_RX_Data_ACK_SendDone:
            //下一个状态判断是否完成接收
            pp->RxState = PP_RX_Data_Done;
            state = EXE_NEED;
            break;
        case PP_RX_Data_ACK_SendError:
            if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                pp->RxState = PP_RX_Data_ACK_SendReq;
            }
            else
            {
                //例外：因为是接收数据 如果最后一次ACK发送失败 但是接收的数据时正确的所以检查一下
                if (pp->FrameN >= pp->FrameTotal)
                {
                    pp->RxState = PP_RX_Data_Done;
                    state = EXE_NEED;
                }
                else
                {
                    //接收失败 也需要休眠对应的时间 而不能发送
                    sleep_time = ack.SpT;
                    sleep_time <<= PP_WUT_K;

                    if (sleep_time)
                    {
                        pp->LoRaState = PP_SLEEP;
                        TimerSetValue(&pp->SleepTimer, sleep_time);
                        TimerStart(&pp->SleepTimer);
                    }
                    else
                    {
                        pp->LoRaState = PP_IDLE;

                        TimerStart(&pp->CADTimer);
                        state = EXE_NEED;
                    }
                }       
            }
            break;
        case PP_RX_Data_Done:
            //接收完成处理
            if (pp->FrameN >= pp->FrameTotal)
            {
                //回调 
                if (pp->LoRa_Recv_CallBack)
                {
                    pp->LoRa_Recv_CallBack(pp->CallBackParam, pp->WUHead.Src_Addr, pp->RxBuf, pp->RxTotalLen, pp->RSSI_Last, pp->SNR_Last);
                }
                
                pp->LoRaState = PP_IDLE;
 
                //至此接收完成 重启启动CAD定时器
                TimerStart(&pp->CADTimer);
                state = EXE_NEED;
            }
            else
            {
                pp->FrameTry = 0;
                pp->RxState = PP_RX_Data_RecvReady;
                state = EXE_NEED;
            }
            break;
        default:
            LoRa_Printf("Err:RxProcess rx_state[%d] err!\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //运行到此处 应该是出错了
        LoRa_Printf("Err:RxProcess but not PP_RXING[%d]\r\n", pp->LoRaState);
    }


    //异常接收状态处理
    if (abnormal_flag == 1 && pp->AFrameSize && IS_PP_CMD(pp->WUHead.Cmd))
    {
        u32 n = 0;
        //如果是这种情况 说明接收过程中漏掉了某次状态 直接根据时间继续休眠
        //所有协议前两个字节都是 cmd WUT 所以直接转化使用即可
        sleep_t = pp->WUHead.SpT;
        sleep_t <<= PP_WUT_K;

        for (n = 0; n < PP_WUHead_Frame_Len;n++)
        {
            LoRa_Printf("%02X ", ((u8*)&pp->WUHead)[n]);
        }
        
        if (sleep_t)
        {
            pp->LoRaState = PP_SLEEP;
            TimerSetValue(&pp->SleepTimer, sleep_t);
            TimerStart(&pp->SleepTimer);
            LoRa_Printf("\r\nErr:LoRaState force PP_SLEEP[%d]\r\n", sleep_t);
        }
        else
        {
            pp->LoRaState = PP_IDLE;

            TimerStart(&pp->CADTimer);
            state = EXE_NEED;
            LoRa_Printf("Err:LoRaState force PP_IDLE\r\n");
        }
    }
    else if (abnormal_flag == 2 && pp->AFrameSize && IS_PP_CMD(pp->AFrame.Cmd))
    {
        u32 n = 0;

        sleep_t = pp->AFrame.SpT;
        sleep_t <<= PP_WUT_K;

        for (n = 0; n < PP_WUHead_Frame_Len;n++)
        {
            LoRa_Printf("%02X ", ((u8*)&pp->WUHead)[n]);
        }
        if (sleep_t)
        {
            pp->LoRaState = PP_SLEEP;
            TimerSetValue(&pp->SleepTimer, sleep_t);
            TimerStart(&pp->SleepTimer);
            LoRa_Printf("\r\nErr:LoRaState force PP_SLEEP[%d]\r\n", sleep_t);
        }
        else
        {
            pp->LoRaState = PP_IDLE;

            TimerStart(&pp->CADTimer);
            state = EXE_NEED;
            LoRa_Printf("Err:LoRaState force PP_IDLE\r\n");
        }
    }
    else if(abnormal_flag)
    {
        pp->LoRaState = PP_IDLE;

        TimerStart(&pp->CADTimer);
        state = EXE_NEED;
        LoRa_Printf("Err:LoRaState force PP_IDLE\r\n");
    }

    return state;
}
/***************************************************************************************************
*\Function      LoRa_Period_Handle
*\Description   lora协议状态机
*\Parameter     pp
*\Return        LP_State_Type
*\Note          1、EXE_NO 说明外部可以执行休眠 否则继续执行此函数
*               状态机模式缺陷：当主循环中有占时很长的任务时，有可能会打断接收发送的时机导致通信失败
                如果是RTOS，需要添加线程间通信机制 加速切换运行 否则也是会出错
                无OS应用解决方案：将状态机 放在每个中断中最后执行一下即可解决时机问题
                发送前延时一下 避免接收方还处于休眠状态
*\Log           2018.05.12    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
PP_Exe_Type LoRa_Process_Running(LoRa_PP_type* pp)
{
    PP_Exe_Type state = EXE_NO;

    if (pp == NULL)
    {
        return state;
    }

    switch(pp->LoRaState)
    {
    case PP_IDLE:
        //优先检查广播校时标记
        if (pp->RTCSync_Flag)
        {
#ifdef PP_TX_DELAY_ENABLE
            //先判断是否需要延时
            if (pp->TXDelayFlag == PP_TX_Delay_REQ)
            {
                //启用延时
                pp->TXDelayFlag = PP_TX_Delaying;
                //TimerSetValue(&pp->TXDelayTimer);
                TimerStart(&pp->TXDelayTimer);
                break;
            }
            else if (pp->TXDelayFlag == PP_TX_Delaying)
            {
                break;
            }
            else
            {
                //延时完成 
                pp->TXDelayFlag = PP_TX_Delay_REQ;
            }
#endif
            TimerStop(&pp->CADTimer);//停止CAD周期检测
            pp->LoRaState = PP_RTC_SYNC_CADING;//发送前CAD请求检查冲突

            pp->CADDoneCount = 0;
            pp->CADTryCount = 0;
            lora_sx1278_startcad(pp->Dev, pp->CADTimeOut);
            break;
        }
        //检查发送标记
        if (pp->TxFlag)
        {
            if (pp->TxBuf.Read == pp->TxBuf.Write)//相等则无数据
            {
                pp->TxFlag = FALSE;
                TimerStart(&pp->CADTimer);//启动CAD定时器
            }
            else
            {
#ifdef PP_TX_DELAY_ENABLE
                //先判断是否需要延时
                if (pp->TXDelayFlag == PP_TX_Delay_REQ)
                {
                    //启用延时
                    pp->TXDelayFlag = PP_TX_Delaying;
                    //TimerSetValue(&pp->TXDelayTimer);
                    TimerStart(&pp->TXDelayTimer);
                    break;
                }
                else if (pp->TXDelayFlag == PP_TX_Delaying)
                {
                    break;
                }
                else
                {
                    //延时完成 
                    pp->TXDelayFlag = PP_TX_Delay_REQ;
                }
#endif
                TimerStop(&pp->CADTimer);//停止CAD周期检测
                pp->LoRaState = PP_TX_CAD_REQ;//发送前CAD请求检查冲突
                state = EXE_NEED;
            }
        }
        break;
    case PP_RTC_SYNC_CADING:
        break;
    case PP_RTC_SYNC_REQ:
        //请求发送广播校时
        pp->FrameN = 0;
        pp->FrameTry = 0;//其实只使用重试了
        pp->FrameTotal = 0;
        
        pp->LoRaState = PP_RTC_SYNC_ING;
        pp->RTCSyncState = PP_RTCSync_Ready;

        state = EXE_NEED;
        break;
    case PP_RTC_SYNC_ING:
        state = LoRa_RTCSync_Process_Running(pp);
        break;
    case PP_TX_CAD_REQ:
        pp->CADDoneCount = 0;
        pp->CADTryCount = 0;

        pp->LoRaState = PP_TX_CADING;
        lora_sx1278_startcad(pp->Dev, pp->CADTimeOut);
        break;
    case PP_TX_CADING:
        break;
    case PP_TX_REQ:
        //发送条件初始化 准备发送包
        pp->FrameN = 0;
        pp->FrameTry = 0;
        //计算总分包数
        pp->FrameTotal = pp->TxBuf.Len[pp->TxBuf.Read] / LORA_FRAME_MAX_BUFSIZE;
        if (pp->TxBuf.Len[pp->TxBuf.Read] % LORA_FRAME_MAX_BUFSIZE)
        {
            pp->FrameTotal++;
        }
        pp->LoRaState = PP_TXING;
        pp->TxState = PP_TX_WUHead_Ready;

        state = EXE_NEED;
        break;
    case PP_TXING:
        //运行发送状态机
        state = LoRa_Tx_Process_Running(pp);
        break;
    case PP_RX_CAD_REQ:
        //说明需要执行CAD检测了
        pp->LoRaState = PP_RX_CADING;
        pp->CADDoneCount = 0;
        pp->CADTryCount = 0;

        lora_sx1278_startcad(pp->Dev, pp->CADTimeOut);   
        break;
    case PP_RX_CADING:
        break;
    //case PP_RX_REQ:
    //    pp->FrameTry = 0;
    //    pp->LoRaState = PP_RXING;
    //    pp->RxState = PP_RX_WUHead_Wait;
    //    lora_sx1278_recv(pp->Dev, pp->TxWUheadTime, FALSE);//启动接收唤醒头
    //    break;
    case PP_RXING:
        //只要进入过接收流程就得延时
        pp->TXDelayFlag = PP_TX_Delay_REQ;
        //此处应该是接收状态机
        state = LoRa_Rx_Process_Running(pp);
        break;
    case PP_SLEEP:
        break;
    case PP_SLEEP_EXIT:
        LoRa_Printf("SpExit!\r\n");
        pp->LoRaState = PP_IDLE;

        TimerStart(&pp->CADTimer);//启动CAD定时器
        break;
    default:
        LoRa_Printf("Err:LoRa Process state[%d] error!\r\n", pp->LoRaState);
        break;
    }

    return state;
}
/***************************************************************************************************
*\Function      LoRa_Period_Handle
*\Description   由于目前该处理函数中断在中断中 因此主循环可以不执行
*\Parameter     pp
*\Return        void
*\Note          1、发送、接收时机会在CAD触发时启动
*\Log           2018.05.12    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
void LoRa_Period_Handle(LoRa_PP_type* pp)
{
    if (pp == NULL)
    {
        return;
    }

    while (1)
    {
        //只有检测到允许休眠才退出
        if (LoRa_Process_Running(pp) == EXE_NO)
        {
            break;
        }
    }
}
/***************************************************************************************************
*\Function      LoRa_Send
*\Description   
*\Parameter     pp
*\Parameter     dst_addr 目标地址 0保留，0xFFFF广播 其他随意
*\Parameter     buf 发送数据缓冲区首地址
*\Parameter     len 发送长度
*\Return        bool FALSE表示缓冲区已满或参数有错误
*\Note          调用后发送缓冲区的数据会保存到LoRa协议栈中 无需保护
                调用即返回 无阻塞，由协议栈完成发送
*\Log           2018.05.14    Ver 1.0    Job               
创建函数。
***************************************************************************************************/
bool LoRa_Send(LoRa_PP_type* pp, u16 dst_addr, u8* buf, u32 len)
{
    u32 write_tmp = 0;

    if (pp == NULL || buf == NULL || len == 0 || len > LORA_BUF_MAX_SIZE || dst_addr == 0)
    {
        return FALSE;
    }
    //预判会不会满载
    write_tmp = (pp->TxBuf.Write + 1) % LORA_TX_BUF_NUM;
    if (write_tmp == pp->TxBuf.Read)
    {
        LoRa_Printf("Err:Lora Tx full!\r\n");
        return FALSE;
    }

    //不满则添加进去
    memcpy(&pp->TxBuf.Buf[pp->TxBuf.Write * LORA_BUF_MAX_SIZE], buf, len);
    pp->TxBuf.Len[pp->TxBuf.Write] = len;
    pp->TxBuf.DstAddr[pp->TxBuf.Write] = dst_addr;

    pp->TxBuf.Write++;
    pp->TxBuf.Write %= LORA_TX_BUF_NUM;

    //真正的发送请求在状态中置位
    pp->TxFlag = TRUE;

    LoRa_Period_Handle(pp);

    return TRUE;
}

/***************************************************************************************************
*\Function      LoRa_RTC_Sync_BroadCast
*\Description   RTC广播 同步时间 手动启动
*\Parameter     pp
*\Return        bool
*\Note          执行在状态机中
*\Log           2018.05.19    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
bool LoRa_RTC_Sync_Request(LoRa_PP_type* pp)
{
    if (pp == NULL)
    {
        return FALSE;
    }
    
    pp->RTCSync_Flag =  TRUE;

    return TRUE;
}

/***************************************************************************************************
*\Function      LoRa_RTC_Sync_Control
*\Description   使能或禁能时间校时
*\Parameter     pp
*\Parameter     interval
*\Parameter     flag
*\Return        bool
*\Note          
*\Log           2018.05.19    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
bool LoRa_RTC_Sync_Control(LoRa_PP_type* pp, u32 interval, bool flag)
{
    if (pp == NULL)
    {
        return FALSE;
    }

    pp->RTCSync_Interval = interval;
    pp->RTCSync_Enable = flag;

    if (pp->RTCSync_Enable)
    {
        //由于定时器最大定时65秒 当大于1分钟时 先定时60s
        pp->RTCSync_Count = pp->RTCSync_Interval;

        if (pp->RTCSync_Count > ONE_MINUTE_MS)
        {
            TimerSetValue(&pp->RTCSync_Timer, ONE_MINUTE_MS);      
            pp->RTCSync_Count -= ONE_MINUTE_MS;
        }
        else
        {          
            TimerSetValue(&pp->RTCSync_Timer, pp->RTCSync_Count);
            pp->RTCSync_Count = 0;
        }

        TimerStart(&pp->RTCSync_Timer);
    }
    else
    {
        TimerStop(&pp->RTCSync_Timer);
    }

    return TRUE;
}
/***************************************************************************************************
*\Function      LoRa_Recv_CallBack_Set
*\Description   设置接收回调函数
*\Parameter     pp
*\Parameter     
*\Parameter     LoRa_Recv_CallBack
*\Parameter     p
*\Parameter     src_addr 接收到数据的地址
*\Parameter     buf      待接收缓冲区
*\Parameter     len      接收的数据长度
*\Parameter     rssi     接收的信号强度
*\Parameter     snr      接收的信号信噪比
*\Parameter     p        接收回调函数的参数 没有则为NULL
*\Return        bool
*\Note          !!!!!!!!!不要在回调函数中进行耗时操作!!!!!!!!!
*\Log           2018.05.15    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
bool LoRa_Recv_CallBack_Set(LoRa_PP_type* pp, 
                            void (*Recv_CallBack)(void* p, u16 src_addr, u8* buf, u16 len, s16 rssi, s8 snr),
                            void* p)
{
    if (pp == NULL)
    {
        return FALSE;
    }

    pp->LoRa_Recv_CallBack = Recv_CallBack;
    pp->CallBackParam = p;

    return TRUE;
}
/***************************************************************************************************
*\Function      LoRa_LowPwFunc_Set
*\Description   
*\Parameter     pp
*\Parameter     
*\Parameter     func
*\Parameter     
*\Return        bool
*\Note          
*\Log           2018.05.22    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
bool LoRa_LowPwFunc_Set(LoRa_PP_type* pp, void (*func)(void))
{
    if (pp == NULL)
    {
        return FALSE;
    }

    pp->LowPowerExitFunc = func;
    
    lora_sx1278_lowpw_func_set(pp->Dev, func);

    return TRUE;
}
/***************************************************************************************************
*\Function      LoRa_Get_RssiAndSnr
*\Description   获取最后一次的信号信息
*\Parameter     pp
*\Parameter     rssi
*\Parameter     snr
*\Return        bool
*\Note          
*\Log           2018.05.15    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
bool LoRa_Get_RssiAndSnr(LoRa_PP_type* pp, s16* rssi, s8* snr)
{
    if (pp == NULL || rssi == NULL || snr == NULL)
    {
        return FALSE;
    }

    *rssi = pp->RSSI_Last;
    *snr = pp->SNR_Last;

    return TRUE;
}

/***************************************************************************************************
*\Function      LoRa_Set_Freq
*\Description   修改协议栈的工作频率
*\Parameter     pp
*\Parameter     freq
*\Return        bool
*\Note          只有空闲时才允许修改
*\Log           2018.05.18    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
bool LoRa_Set_Freq(LoRa_PP_type* pp, u32 freq)
{
    if (pp == NULL || pp->LoRaState != PP_IDLE)
    {
        return FALSE;
    }

    lora_sx1278_freq_set(pp->Dev, freq);

    return TRUE;
}

/***************************************************************************************************
*\Function      LoRa_Get_Freq
*\Description   返回工作频率
*\Parameter     pp
*\Return        u32
*\Note          出错返回0
*\Log           2018.05.18    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
u32 LoRa_Get_Freq(LoRa_PP_type* pp)
{
    if (pp == NULL)
    {
        return 0;
    }

    return pp->Dev->LoRaFreq;
}


/***************************************************************************************************
*\Function      LoRa_Get_RTCSyncLast
*\Description   返回最后一次时间同步的时间
*\Parameter     pp
*\Return        TIME_SubSec
*\Note          如果是主 则是发出时间同步的时间
*\Log           2018.05.25    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
TIME_SubSec LoRa_Get_RTCSyncLast(LoRa_PP_type* pp)
{
    TIME_SubSec INIT_T = {INIT_TIME, 0};

    if (pp == NULL)
    {
        return INIT_T;
    }

    return pp->RTCSync_TIME;
}
