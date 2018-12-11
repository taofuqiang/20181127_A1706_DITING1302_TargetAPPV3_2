/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          lora_pp.c
*\Description   
*\Note          lora_sx1278_preamble_set       37us
                lora_sx1278_send       RTCͬ�� 3.4ms ���ͳ��Ȳ�ͬʱ��᲻ͬ 245�ֽڴ�Լ5.7ms 99�ֽڴ�Լ3.7ms
                lora_sx1278_recv               3.4ms
*\Log           2018.05.11    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "lora_pp.h"
#include "timeServer.h"


#define LoRa_Printf(...)  printf(__VA_ARGS__)

#define MIN_PREAMBLE_SIZE   8

//3��CAD��2���򴥷����� �����˳�����
#define CADING_TRY_MAX_COUNT 2
#define CADDONE_TRIG_COUNT   (CADING_TRY_MAX_COUNT)//���ܴ�������

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
*\Description   ��������ʱ��ĺ���� 
*\Parameter     t1
*\Parameter     t2
*\Return        u32
*\Note          t1 > t2 �򷵻�+�����򷵻ظ���
*\Log           2018.05.21    Ver 1.0    Job               
				����������
***************************************************************************************************/
s64 TIME_SubSec_Diff(TIME_SubSec t1, TIME_SubSec t2)
{
    s64 t1_s = 0;
    s64 t2_s = 0;

    //������ƽ��ʱ��ʱ ���ܻ�����������999����� 
    //ʱ��Ϸ���  ���Ϸ����޸�
    if ((TimeDECCheck(&t1.T) == FALSE) || (TimeDECCheck(&t2.T) == FALSE) || t1.SubSecs > 999 || t2.SubSecs > 999)
    {
        return 0;
    }

    //��ת��Ϊ���� Ȼ������
    t1_s = TimeDECToStamp(t1.T);
    t1_s = t1_s * ONE_SECOND_MS + t1.SubSecs;

    t2_s = TimeDECToStamp(t2.T);
    t2_s = t2_s * ONE_SECOND_MS + t2.SubSecs;

    return t1_s - t2_s;
}

/***************************************************************************************************
*\Function      TIME_SubSec_Add
*\Description   �Ӻ���
*\Parameter     t1
*\Parameter     ms
*\Return        bool
*\Note          
*\Log           2018.05.21    Ver 1.0    Job               
				����������
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
*\Description   RTC����ʱ�� ��λ����
*\Parameter     num
*\Return        u16
*\Note          Ŀǰ��ȡ8000
*\Log           2018.05.21    Ver 1.0    Job               
				����������
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
*\Description   ���ͳ�ʱ������ʱ
*\Parameter     num
*\Return        u16
*\Note          
*\Log           2018.05.14    Ver 1.0    Job               
����������
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
*\Description   CAD���ڼ�ⶨʱ����ʱ����
*\Parameter     p
*\Return        
*\Note          
*\Log           2018.05.12    Ver 1.0    Job               
����������
***************************************************************************************************/
static void CAD_Period_timeout_handle(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    //ֻ�п��в�����cad��� 
    if (pp->LoRaState == PP_IDLE)
    {
        //pp->LoRaState = PP_RX_CAD_REQ;
        //˵����Ҫִ��CAD����� �ж��� ��ֹ��ʧʱ��
        pp->LoRaState = PP_RX_CADING;

        pp->CADDoneCount = 0;
        pp->CADTryCount = 0;

        lora_sx1278_startcad(pp->Dev, pp->CADTimeOut);  
    }
    else 
    {
        //���е��˴� Ӧ���ǳ����� ����������ʱӦ�ý���
        LoRa_Printf("Err:CAD Period Tout but not PP_IDLE[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      PP_CAD_Done
*\Description   cad�жϴ���
*\Parameter     p
*\Return        void
*\Note          1����⵽CAD��������ж���ʹ�ܽ��գ�������״̬���п������������ն��������
*\Log           2018.05.12    Ver 1.0    Job               
����������
***************************************************************************************************/
static void PP_CAD_Done(void* p, bool flag)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LoRaState == PP_RTC_SYNC_CADING)
    {
        if (flag)
        {
            //��⵽��ͻ ֱ����ȥ��������
            pp->LoRaState = PP_RX_CADING;

            pp->CADTryCount++;
            pp->CADDoneCount++;

            //3��ȡ����
            if (pp->CADTryCount >= CADING_TRY_MAX_COUNT)
            {
                if (pp->CADDoneCount >= CADDONE_TRIG_COUNT)
                {
                    //�������� ���ж��� ��ֹMCU��ռ�� ��ʧʱ��
                    pp->FrameTry = 0;

                    pp->RSSI_Last = 0;
                    pp->SNR_Last = 0x7F;

                    pp->LoRaState = PP_RXING;
                    pp->RxState = PP_RX_WUHead_Wait;

                    lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
                    lora_sx1278_recv(pp->Dev, pp->WUheadRTCTime, TRUE);//�������ջ���ͷ
                }
                else
                {
                    LoRa_Printf("CAD[%d,%d]\r\n", pp->CADDoneCount, pp->CADTryCount);
                    //����2��˵���Ǹ��� �˳�
                    pp->LoRaState = PP_IDLE;

                    //�����ݽ��� �ٴ�����CAD���ڼ�ⶨʱ��
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
            //û�г�ͻ �������� �����μ�� ������CADֻ���󴥷� ������
            pp->LoRaState = PP_RTC_SYNC_REQ;
        }
    }
    else if (pp->LoRaState == PP_TX_CADING)
    {
        if (flag)
        {
            //��⵽��ͻ ֱ����ȥ��������
            pp->LoRaState = PP_RX_CADING;

            pp->CADTryCount++;
            pp->CADDoneCount++;

            //3��ȡ����
            if (pp->CADTryCount >= CADING_TRY_MAX_COUNT)
            {
                if (pp->CADDoneCount >= CADDONE_TRIG_COUNT)
                {
                    //�������� ���ж��� ��ֹMCU��ռ�� ��ʧʱ��
                    pp->FrameTry = 0;

                    pp->RSSI_Last = 0;
                    pp->SNR_Last = 0x7F;

                    pp->LoRaState = PP_RXING;
                    pp->RxState = PP_RX_WUHead_Wait;
                    lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
                    lora_sx1278_recv(pp->Dev, pp->WUheadRTCTime, TRUE);//�������ջ���ͷ
                }
                else
                {
                    LoRa_Printf("CAD[%d,%d]\r\n", pp->CADDoneCount, pp->CADTryCount);
                    //����2��˵���Ǹ��� �˳�
                    pp->LoRaState = PP_IDLE;

                    //�����ݽ��� �ٴ�����CAD���ڼ�ⶨʱ��
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
            //û�г�ͻ �������� �����μ�� ������CADֻ���󴥷� ������
            pp->LoRaState = PP_TX_REQ;
        }
    }
    else if (pp->LoRaState == PP_RX_CADING)
    {
        if (flag)//˵��������Ҫ����
        {
            pp->CADTryCount++;
            pp->CADDoneCount++;

            //3��ȡ����
            if (pp->CADTryCount >= CADING_TRY_MAX_COUNT)
            {
                if (pp->CADDoneCount >= CADDONE_TRIG_COUNT)
                {
                    //�������� ���ж��� ��ֹMCU��ռ�� ��ʧʱ��
                    pp->FrameTry = 0;

                    pp->RSSI_Last = 0;
                    pp->SNR_Last = 0x7F;

                    pp->LoRaState = PP_RXING;
                    pp->RxState = PP_RX_WUHead_Wait;
                    lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
                    lora_sx1278_recv(pp->Dev, pp->WUheadRTCTime, TRUE);//�������ջ���ͷ
                }
                else
                {
                    LoRa_Printf("CAD[%d,%d]\r\n", pp->CADDoneCount, pp->CADTryCount);
                    //����2��˵���Ǹ��� �˳�
                    pp->LoRaState = PP_IDLE;

                    //�����ݽ��� �ٴ�����CAD���ڼ�ⶨʱ��
                    TimerStart(&pp->CADTimer);
                }      
            }
            else
            {
                lora_sx1278_startcad(pp->Dev, pp->CADTimeOut); 
            }

            //�ȴ�����
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
                //���ý����ж�
                if (pp->CADDoneCount >= CADDONE_TRIG_COUNT)
                {
                    //�������� ���ж��� ��ֹMCU��ռ�� ��ʧʱ��
                    pp->FrameTry = 0;

                    pp->RSSI_Last = 0;
                    pp->SNR_Last = 0x7F;

                    pp->LoRaState = PP_RXING;
                    pp->RxState = PP_RX_WUHead_Wait;
                    lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
                    lora_sx1278_recv(pp->Dev, pp->WUheadRTCTime, TRUE);//�������ջ���ͷ
                }
                else
                {
                    LoRa_Printf("CAD[%d,%d]\r\n", pp->CADDoneCount, pp->CADTryCount);
                    //����2��˵���Ǹ��� �˳�
                    pp->LoRaState = PP_IDLE;

                    //�����ݽ��� �ٴ�����CAD���ڼ�ⶨʱ��
                    TimerStart(&pp->CADTimer);
                }  
            }
            else
            {
                pp->LoRaState = PP_IDLE;

                //�����ݽ��� �ٴ�����CAD���ڼ�ⶨʱ��
                TimerStart(&pp->CADTimer);
            }            
        }
    }
    else
    {
        //���е��˴� Ӧ���ǳ����� ����������ʱӦ�ý���
        LoRa_Printf("Err:CADDone but not TXorRX CADing[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}

/***************************************************************************************************
*\Function      PP_CAD_Timeout
*\Description   CAD��鳬ʱ������
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.12    Ver 1.0    Job               
����������
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
        //�����ʱ ֱ�ӷ��� ���ʼ���
        pp->LoRaState = PP_RTC_SYNC_REQ;
    }
    else if (pp->LoRaState == PP_TX_CADING)
    {
        //�����ʱ ֱ�ӷ��� ���ʼ���
        pp->LoRaState = PP_TX_REQ;
    }
    else if (pp->LoRaState == PP_RX_CADING)
    {
        //cad����ʱ ���������е��˴����ʼ�С
        TimerStart(&pp->CADTimer);
        pp->LoRaState = PP_IDLE;//��ʱ�˳� �ÿ���
    }
    else
    {
        //���е��˴� Ӧ���ǳ����� ����������ʱӦ�ý���
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
*               ����������
***************************************************************************************************/
static void PP_Rx_Done(void* p, u8* buf, u16 size, s16 rssi, s8 snr)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    lora_sx1278_sleep_set(pp->Dev);//��ֹ����ģʽһֱ����

    if (!size)
    {
        pp->LoRaState = PP_IDLE;
        TimerStart(&pp->CADTimer);
        LoRa_Printf("Err:RxDone but size is 0[%d]\r\n", pp->LoRaState);
        return;
    }

    //ץȡ�����е�����ź�
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
            //���ƺ���״̬�����ж�
            if (buf)
            {
                memcpy(&pp->WUHead, buf, Min(PP_WUHead_With_RTC_Len, size));//ֻ�踴�ƽ�ȥ ����
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
            //���ƺ���״̬�����ж�
            if (buf)
            {
                memcpy(&pp->WUHead, buf, Min(PP_WUHead_With_RTC_Len, size));//ֻ�踴�ƽ�ȥ ����
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
            //���е��˴� Ӧ���ǳ�����
            LoRa_Printf("Err:RxDone TXING but no match[%d]\r\n", pp->TxState);
            break;
        } 
    }
    else if (pp->LoRaState == PP_RXING)
    {
        switch (pp->RxState)
        {
        case PP_RX_WUHead_Wait:
            //���ƺ���״̬�����ж�
            if (buf)
            {
                memcpy(&pp->WUHead, buf, Min(PP_WUHead_With_RTC_Len, size));//ֻ�踴�ƽ�ȥ ����
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
            //���յ�������
            if (buf)
            {
                memcpy(&pp->AFrame, buf, Min(RF_FIFO_SIZE, size));//ֻ�踴�ƽ�ȥ ����
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
            //���е��˴� Ӧ���ǳ�����
            LoRa_Printf("Err:RxDone RXING but no match[%d]\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //���е��˴� Ӧ���ǳ�����
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
����������
***************************************************************************************************/
static void PP_Rx_Error(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    lora_sx1278_sleep_set(pp->Dev);//��ֹ����ģʽһֱ����

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
            //���е��˴� Ӧ���ǳ�����
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
            //���е��˴� Ӧ���ǳ�����
            LoRa_Printf("Err:RxErr RXING but no match[%d]\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //���е��˴� Ӧ���ǳ�����
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
����������
***************************************************************************************************/
static void PP_Rx_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    lora_sx1278_sleep_set(pp->Dev);//��ֹ����ģʽһֱ����

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
            //���е��˴� Ӧ���ǳ�����
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
            //���е��˴� Ӧ���ǳ�����
            LoRa_Printf("Err:RxTout RXING but no match[%d]\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //���е��˴� Ӧ���ǳ�����
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
����������
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
            //���е��˴� Ӧ���ǳ�����
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
            //���е��˴� Ӧ���ǳ�����
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
            //���е��˴� Ӧ���ǳ�����
            LoRa_Printf("Err:TxDone RXING but no match[%d]\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //���е��˴� Ӧ���ǳ�����
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
����������
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
            //���е��˴� Ӧ���ǳ�����
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
            //���е��˴� Ӧ���ǳ�����
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
            //���е��˴� Ӧ���ǳ�����
            LoRa_Printf("Err:TxTout RXING but no match[%d]\r\n", pp->RxState);
            break;
        }
    }
    else
    {
        //���е��˴� Ӧ���ǳ�����
        LoRa_Printf("Err:TxTout but not TXingOrRXing[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      SleepForce_Timeout
*\Description   ǿ�����߳�ʱ
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.13    Ver 1.0    Job
*               ����������
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
        //���е��˴� Ӧ���ǳ�����
        LoRa_Printf("Err:SleepTout but not Sleep[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      RxError_Timeout
*\Description   ���մ�����ʱ��ʱ��
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.18    Ver 1.0    Job               
				����������
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
        //���е��˴� Ӧ���ǳ�����
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
				����������
***************************************************************************************************/
static void PP_TxDelay_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    //ֻҪ����ing˵�������մ�Ϲ� ����������ʱ����
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
*\Description   �㲥��Ϣ������ʱ�ص�
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.15    Ver 1.0    Job               
����������
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
            //���е��˴� Ӧ���ǳ�����
            LoRa_Printf("Err:TxBDTout but no match[%d]\r\n", pp->TxState);
            break;
        }
    }
    else
    {
        //���е��˴� Ӧ���ǳ�����
        LoRa_Printf("Err:TxBDTout but not TXING[%d]\r\n", pp->LoRaState);
    }

    LoRa_Period_Handle(pp);
}
/***************************************************************************************************
*\Function      PP_RTC_Sync_Timeout
*\Description   Уʱ��ʱ������
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.05.19    Ver 1.0    Job               
				����������
***************************************************************************************************/
static void PP_RTC_Sync_Timeout(void* p)
{
    LoRa_PP_type* pp = (LoRa_PP_type*) p;

    if (pp->LowPowerExitFunc)
    {
        pp->LowPowerExitFunc();
    }

    //����0 ��˵����ʱʱ�䵽�� ִ�й㲥Уʱ
    if (pp->RTCSync_Count == 0)
    {
        pp->RTCSync_Flag = TRUE;
    }
    else
    {
        //����ִ�ж�ʱ
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
*\Description   loraЭ���ʼ��
*\Parameter     pp
*\Parameter     dev
*\Parameter     freq ͨ��Ƶ��
*\Parameter     self_addr �����ַ
*\Parameter     rtc_interval Уʱ���
*\Parameter     enableУʱ�Ƿ�ʹ�� �ڵ��ɫ����ʹ��
*\Parameter     
*\Parameter     func �͹��Ļص����� ����NULL
*\Parameter     
*\Return        bool
*\Note          
*\Log           2018.05.23    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool LoRa_Init(LoRa_PP_type* pp, LoRa_Dev_type* dev, u32 freq, u16 self_addr, u32 rtc_interval, bool enable, void(*func)(void),u16 pb_Time)
{
    TIME_SubSec INIT_T = {INIT_TIME, 0};
    u32 preamble_time = 0;//ǰ����ʱ�� CAD������ڵ�ʱ������ҪС
    //���߻��ƺ�ʹ�ò�����ʱ�ڲ�ָ�� �����⿪��
//    u16 PreambleTime = 300;//����֡ǰ��ʱ�� ���߻���CAD�Ǵ�ʱ���1/3
		u16 PreambleTime = pb_Time;
    u8 num = 3;//ͨ�Ų���ѡ��0~6

    if (pp == NULL || dev == NULL || num >= countof(LoRaParamList) || self_addr == 0)
    {
        LoRa_Printf("LoRa init fail!\r\n");
        return FALSE;
    }

    if (PreambleTime < 60)
    {
        PreambleTime = 60;
    }

    //��Сͬ��ʱ��Ϊ60��
    /*if (rtc_interval < ONE_MINUTE_MS)
    {
        rtc_interval = ONE_MINUTE_MS;
    }*/

    pp->Dev = dev;
    pp->ParamNum = num;
    pp->Self_Addr = self_addr;

    pp->PreambleShort = MIN_PREAMBLE_SIZE;

    //TxBuf ��ʼ��
    pp->TxBuf.Write = pp->TxBuf.Read = 0;
    pp->TxFlag = FALSE;

    pp->WaitACKTime = SendExtDelay(num) + 5;//ms

    pp->RTCSync_Interval = rtc_interval;
    pp->RTCSync_Enable = enable;
    pp->TXDelayFlag = PP_TX_Delay_REQ;

    pp->RTCSync_TIME = INIT_T;

    pp->LowPowerExitFunc = func;
    pp->LoRa_Recv_CallBack = NULL;

    //��ʼ��LoRa�����ò��� 
    lora_sx1278_init(dev);
    lora_sx1278_freq_set(dev, freq);
    lora_sx1278_lowpw_func_set(dev, func);//�˺���������init֮�� init�Ὣ�ڲ�������ΪNULL

    //������Ҫ��ʱ��Ȳ���
    //������Сʱ���ǰ���볤��
    pp->PreambleLong = lora_calc_preamble_num(dev, PreambleTime, LoRaParamList[num].eBW, LoRaParamList[num].eSF);
    if (pp->PreambleLong < MIN_PREAMBLE_SIZE)
    {
        pp->PreambleLong = MIN_PREAMBLE_SIZE;  
    }
    //����LoRa����
    lora_sx1278_set_callback(dev, PP_Tx_Done, PP_Tx_Timeout, PP_Rx_Done, PP_Rx_Timeout, PP_Rx_Error,\
        PP_CAD_Done, PP_CAD_Timeout, pp);

    lora_sx1278_param_set(dev, LoRaParamList[num].eBW, LoRaParamList[num].eSF, LoRaParamList[num].eFEC, pp->PreambleLong, 100);

    lora_sx1278_sleep_set(dev);

    //����ǰ����ʱ�� �˲������������ò�������� ��Ϊʹ�õ����ڲ���������
    preamble_time = lora_calc_preamble_time(dev, pp->PreambleLong);//��ʱ�佫��ΪCAD���ڵĲο�
    pp->CADTimeOut = lora_calc_cad_time(dev) + 10;//CAD��ʱʱ�� ������10ms ��ֹ̫��

    //�˲������������ò�������� ��Ϊʹ�õ����ڲ���������
    //MaxLenTime�ر�ӳ�
    pp->MaxLenTime = lora_calc_onair_time(dev, MIN_PREAMBLE_SIZE, (LORA_FRAME_MAX_BUFSIZE + 5)) + SendExtDelay(pp->ParamNum) * 2;
    pp->WUheadTime = lora_calc_onair_time(dev, MIN_PREAMBLE_SIZE, PP_WUHead_Frame_Len) + SendExtDelay(pp->ParamNum);
    //��Ϊ��RTC�ı�Ȼ�����ڳ�ǰ�� ����ʹ�����ǰ��������
    pp->WUheadRTCTime = lora_calc_onair_time(dev, pp->PreambleLong, PP_WUHead_With_RTC_Len) + SendExtDelay(pp->ParamNum) * 1;
    pp->WUheadRTCFrameTime = lora_calc_onair_time(dev, pp->PreambleLong, PP_WUHead_With_RTC_Len);

    //CAD���ڼ�ⶨʱ��
    pp->CADPeriod = preamble_time/2;// / 2;     // ��ȥ20 Ϊ 280ms�������
    TimerInit(&pp->CADTimer, CAD_Period_timeout_handle, pp);
    TimerSetValue(&pp->CADTimer, pp->CADPeriod);

    //ǿ�����߶�ʱ��
    TimerInit(&pp->SleepTimer, Sleep_Timeout, pp);

    //�㲥��Ϣ��ʱ
    TimerInit(&pp->TxBroadcastTimer, PP_TxBroadcast_Timeout, pp);

    //���մ���ʱ��
    TimerInit(&pp->RxErrorTimer, PP_RxError_Timeout, pp);
#ifdef PP_TX_DELAY_ENABLE
    //����ǰ��ʱ��ʱ��
    TimerInit(&pp->TXDelayTimer, PP_TxDelay_Timeout, pp);
    TimerSetValue(&pp->TXDelayTimer, 50);//pp->CADPeriod);
#endif
    //�㲥Уʱ
    TimerInit(&pp->RTCSync_Timer, PP_RTC_Sync_Timeout, pp);
    pp->RTCSync_Flag = FALSE;
    //if (pp->RTCSync_Enable)
    //{
    //    //���ڶ�ʱ�����ʱ65�� ������1����ʱ �ȶ�ʱ60s
    //    //��һ���ϵ�10s���ȴ���һ��
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
    //����CAD��� ��ʼ����ɲ�����
    //TimerStart(&pp->CADTimer);

    return TRUE;
}

/***************************************************************************************************
*\Function      LoRa_Start
*\Description   ����LoRaЭ��ջ
*\Parameter     pp
*\Return        void
*\Note          
*\Log           2018.05.25    Ver 1.0    Job               
				����������
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
        //���ڶ�ʱ�����ʱ65�� ������1����ʱ �ȶ�ʱ60s
        //��һ���ϵ�10s���ȴ���һ��
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
*\Description   ֹͣLoRaЭ��ջ����͹���ģʽ
*\Parameter     pp
*\Return        void
*\Note          
*\Log           2018.05.25    Ver 1.0    Job               
				����������
***************************************************************************************************/
void LoRa_Stop(LoRa_PP_type* pp)
{
    if (pp == NULL)
    {
        return;
    }

    //ֹͣ����TIMER
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
*\Description   ʱ��ͬ��״̬��
*\Parameter     pp
*\Return        PP_Exe_Type
*\Note          
*\Log           2018.05.19    Ver 1.0    Job               
				����������
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
                //���������� ���Ž���rtcУʱ ���������Ϊ��ǰ��
                lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);   //37us

                wuhead.Cmd = PP_CMD_RTC_SYNC;
                wuhead.Src_Addr = pp->Self_Addr;
                wuhead.Dst_Addr = PP_BROADCAST_ADDR;
                wuhead.SpT = 0;//��ռ�ý�������ʱ��            

                pp->RTCSyncState = PP_RTCSync_Sending;

                //Ŀǰ�ö�����Ȼ���ж��� ����������ж�
                //��ȡʱ����Ҫ�ص��ж� ��֤�������
                //BACKUP_PRIMASK();
                //DISABLE_IRQ( );

                stm32_Inrtc_getTime(&cur);
                //��Ҫ����һ��ʱ��
                rtc_correct = pp->WUheadRTCFrameTime + RTCSyncCorrectDelay(pp->ParamNum);
                TIME_SubSec_AddMs(&cur, rtc_correct);

                memcpy(&wuhead.RTC_SYNC, &cur, sizeof(TIME_SubSec));//8us ��ǰ��֡���õ���

                wuhead.Sum = Get_Sum8((u8*)&wuhead, PP_WUHead_With_RTC_Len - 1);//��cmd��ʼ��У���
                lora_sx1278_send(pp->Dev, (u8*)&wuhead, PP_WUHead_With_RTC_Len, pp->WUheadRTCTime);//3.4ms
                //ʹ���ж�
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

            //���˽������ ��������CAD��ʱ��
            TimerStart(&pp->CADTimer);
            //�ٴ�ʹ��RTCͬ����ʱ��
            if (pp->RTCSync_Enable)
            {
                //���ڶ�ʱ�����ʱ65�� ������1����ʱ �ȶ�ʱ60s
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
                //�����������ͻ���֡����
                pp->RTCSyncState = PP_RTCSync_Ready;    

                state = EXE_NEED;
            }
            else
            {
                //��������
                pp->RTCSync_Flag = FALSE;
                pp->LoRaState = PP_IDLE;
 
                //���˽������ ��������CAD��ʱ��
                TimerStart(&pp->CADTimer);
                 //�ٴ�ʹ��RTCͬ����ʱ��
                if (pp->RTCSync_Enable)
                {
                    //���ڶ�ʱ�����ʱ65�� ������1����ʱ �ȶ�ʱ60s
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
            //���е��˴� Ӧ���ǳ�����
            LoRa_Printf("Err:RTCProcess but not match[%d]\r\n", pp->RTCSyncState);
            break;
        }
    }
    else
    {
        //���е��˴� Ӧ���ǳ�����
        LoRa_Printf("Err:RTCProcess but not RTCSyncING[%d]\r\n", pp->LoRaState);
    }

    return state;
}
/***************************************************************************************************
*\Function      LoRa_Tx_Process_Running
*\Description   ����״̬��
*\Parameter     pp
*\Return        PP_Exe_Type
*\Note          ����Ҫ��ȷ�Ϸ�����ɣ������Ҫ�������Է����֡ ����Ҫ3������ 
*               �����ͷ����һ֡ACK����ʧ��ʱ�����շ����������һ֡��Ϊ����˲��ٵȴ����գ�
*               �����ͷ���ȴ�����ش��ȴ�ACK

                !!!�������ʧ�ܵĻ� ��Ҫ���߶�Ӧ��ʱ�� ���� �����ڵ㻹���������� ���޷�ͨ�ŵ�
*\Log           2018.05.12    Ver 1.0    Job               
����������
***************************************************************************************************/
PP_Exe_Type LoRa_Tx_Process_Running(LoRa_PP_type* pp)
{
    PP_Exe_Type state = EXE_NO;
    static PP_WUHead_Type wuhead;
    u32 hold_time = 0;//ͨ��ռ��ʱ��
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
                hold_time = pp->FrameTotal * pp->MaxLenTime;//�򵥱��� ֱ�����ʱ��
                hold_time += (pp->FrameTotal + 1) * SendExtDelay(pp->ParamNum);
            }
            else
            {
                hold_time = pp->FrameTotal * pp->MaxLenTime;//�򵥱��� ֱ�����ʱ��
                //������ַ��Ҫ�ڼ���ACKʱ��
                hold_time += (pp->FrameTotal + 1) * pp->WUheadTime;//ackҪ��һ��
            }
            wuhead.SpT = hold_time >> PP_WUT_K;//��ʾ������Ҫռ�õ�ʱ��

            pp->TxState = PP_TX_WUHead_Sending;

            //����ʹ�ó�ǰ��
            lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
            lora_sx1278_send(pp->Dev, (u8*)&wuhead, PP_WUHead_Frame_Len, pp->WUheadRTCTime);
            break;
        case PP_TX_WUHead_Sending:
            break;
        case PP_TX_WUHead_SendDone:
            
            //�˴�Ӧ�����ֹ㲥�͵���
            if (pp->TxBuf.DstAddr[pp->TxBuf.Read] != PP_BROADCAST_ADDR)
            {
                //������Ϣ
                //������� תΪ����ACK
                //pp->FrameTry = 0; //�����ô˱�� ��ΪACK����ʧ�ܺ������·������� �����ᵼ�³���ѭ��
                //���ý��� �ȴ�ACK
                pp->TxState = PP_TX_WUHead_ACK_Wait;

                lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);  
                lora_sx1278_recv(pp->Dev, (pp->WUheadTime+pp->WaitACKTime), TRUE);//����ʱ��+�Է�׼��ʱ�� �迼�ǶԷ������Դ���  
            }
            else
            {
                //�㲥��Ϣ
                //ֱ��ת���������ݲ������� ��ʱһ��ʱ�� �ȴ��Է�����
                //pp->FrameTry = 0;
                TimerSetValue(&pp->TxBroadcastTimer, SendExtDelay(pp->ParamNum));              
                pp->TxState = PP_TX_BD_WUHead_Delaying;
                TimerStart(&pp->TxBroadcastTimer);
            }
            break;     
        case PP_TX_WUHead_SendError:
            if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                //�����������ͻ���֡����
                pp->TxState = PP_TX_WUHead_Ready;    

                state = EXE_NEED;
            }
            else
            {
                //�����˴η��� ������+1
                pp->TxBuf.Read++;
                pp->TxBuf.Read %= LORA_TX_BUF_NUM;
                
                //����ʧ������һ��
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
            //ֻ�й㲥��ִ�д˴� ���Բ��ü��
            pp->FrameTry = 0;
            pp->FrameN = 0;

            pp->TxState = PP_TX_Data_Ready;

            state = EXE_NEED;
            break;
        case PP_TX_WUHead_ACK_Wait:
            break;
        case PP_TX_WUHead_ACK_Done:
           
            //�жϽ��յ���ACK�Ƿ���� cmd dst ���
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
                //��������� ���Դ����ж��Ƿ�����
                if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
                {
                    //�����������ͻ���֡����
                    pp->TxState = PP_TX_WUHead_Ready;    

                    state = EXE_NEED;
                }
                else
                {
                    //�����˴η��� ������+1
                    pp->TxBuf.Read++;
                    pp->TxBuf.Read %= LORA_TX_BUF_NUM;
                   
                    //����ʧ������һ��
                    pp->LoRaState = PP_SLEEP;    
                    sleep_t = wuhead.SpT;//����һ��ռ�õ�ʱ������
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
                //�����������ͻ���֡����
                pp->TxState = PP_TX_WUHead_Ready;    

                state = EXE_NEED;
            }
            else
            {
                //�����˴η��� ������+1
                pp->TxBuf.Read++;
                pp->TxBuf.Read %= LORA_TX_BUF_NUM;
                
                //����ʧ������һ��
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
                hold_time = (pp->FrameTotal - pp->FrameN - 1) * pp->MaxLenTime;//�򵥱��� ֱ�����ʱ��
                hold_time += (pp->FrameTotal - pp->FrameN) * SendExtDelay(pp->ParamNum);
            }
            else
            {
                hold_time = (pp->FrameTotal - pp->FrameN - 1) * pp->MaxLenTime;//�򵥱��� ֱ�����ʱ��
                //������ַ��Ҫ�ڼ���ACKʱ��
                hold_time += (pp->FrameTotal - pp->FrameN) * pp->WUheadTime;
            }
            pp->AFrame.SpT = hold_time >> PP_WUT_K;
            pp->AFrame.CTL = (pp->FrameTotal << 4) | (pp->FrameN & 0x0F);
            if (pp->FrameN + 1 >= pp->FrameTotal)
            {
                //˵�������һ֡
                pp->AFrame.DT = pp->TxBuf.Len[pp->TxBuf.Read] - pp->FrameN * LORA_FRAME_MAX_BUFSIZE;
            }
            else
            {
                pp->AFrame.DT = LORA_FRAME_MAX_BUFSIZE;
            }
            memcpy(pp->AFrame.Data, &pp->TxBuf.Buf[pp->TxBuf.Read * LORA_BUF_MAX_SIZE + pp->FrameN * LORA_FRAME_MAX_BUFSIZE], pp->AFrame.DT);

            //��������sum
            pp->AFrame.Sum = Get_Sum8((u8*)&pp->AFrame, pp->AFrame.DT + 4);//��֡��У���
            //��Sum�ƶ������ʵ�λ��
            ((u8*)&pp->AFrame)[pp->AFrame.DT + 4] = pp->AFrame.Sum;

            pp->TxState = PP_TX_Data_Sending;

            lora_sx1278_preamble_set(pp->Dev, pp->PreambleShort);
            lora_sx1278_send(pp->Dev, (u8*)&pp->AFrame, pp->AFrame.DT + 5, pp->MaxLenTime);
            break;
        case PP_TX_Data_Sending:
            break;
        case PP_TX_Data_SendDone:
            //�˴�Ӧ�����ֹ㲥�͵���
            if (pp->TxBuf.DstAddr[pp->TxBuf.Read] != PP_BROADCAST_ADDR)
            {
                //������Ϣ
                //������� תΪ����ACK
                //pp->FrameTry = 0;//�����ô˱�� ��ΪACK����ʧ�ܺ������·������� �����ᵼ�³���ѭ��
                //���ý��� �ȴ�ACK
                pp->TxState = PP_TX_Data_ACK_Wait;

                lora_sx1278_preamble_set(pp->Dev, pp->PreambleLong);
                lora_sx1278_recv(pp->Dev, (pp->WUheadTime+pp->WaitACKTime), TRUE);//����ʱ��+�Է�׼��ʱ��               
            }
            else
            {
                //�㲥��Ϣ
                //ֱ��ת���������ݲ�������
                TimerSetValue(&pp->TxBroadcastTimer, SendExtDelay(pp->ParamNum));              
                pp->TxState = PP_TX_BD_Data_Delaying;
                TimerStart(&pp->TxBroadcastTimer);
            }
            break;
        case PP_TX_Data_SendError:  
            //�㲥����ʧ�� ֱ���˳�
            if (pp->TxBuf.DstAddr[pp->TxBuf.Read] != PP_BROADCAST_ADDR && ++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                //�����������ͻ���֡����
                pp->TxState = PP_TX_Data_Ready;    

                state = EXE_NEED;
            }
            else
            {
                //�����˴η��� ������+1
                pp->TxBuf.Read++;
                pp->TxBuf.Read %= LORA_TX_BUF_NUM;
                
                //����ʧ������һ��
                pp->LoRaState = PP_SLEEP;    
                sleep_t = pp->AFrame.SpT;//����һ��ռ�õ�ʱ������
                sleep_t <<= PP_WUT_K;

                TimerSetValue(&pp->SleepTimer, sleep_t);
                TimerStart(&pp->SleepTimer);

                //state = EXE_NEED;
            }
            break;
        case PP_TX_BD_Data_Delaying:
            break;
        case PP_TX_BD_Data_DelayDone:
            //ֻ�й㲥��ִ�д˴�
            if (pp->TxBuf.DstAddr[pp->TxBuf.Read] == PP_BROADCAST_ADDR)
            {
                pp->FrameTry = 0;
                pp->FrameN++;

                if (pp->FrameN >= pp->FrameTotal)
                {
                    //�������
                    pp->TxBuf.Read++;
                    pp->TxBuf.Read %= LORA_TX_BUF_NUM;

                    pp->LoRaState = PP_IDLE;

                    state = EXE_NEED;
                }
                else
                {
                    //δ������������һ֡
                    pp->TxState = PP_TX_Data_Ready;
                }

                state = EXE_NEED;
            }
            else
            {
                //���ǹ㲥ִ�е����� ������
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
                    //�������
                    pp->TxBuf.Read++;
                    pp->TxBuf.Read %= LORA_TX_BUF_NUM;

                    pp->LoRaState = PP_IDLE;

                    state = EXE_NEED;
                }
                else
                {
                    //δ������������һ֡
                    pp->TxState = PP_TX_Data_Ready;
                }

                state = EXE_NEED;
            }
            else
            {
                //��������� ���Դ����ж��Ƿ�����
                if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
                {
                    //�����������ͻ���֡����
                    pp->TxState = PP_TX_Data_Ready;    

                    state = EXE_NEED;
                }
                else
                {
                    //�����˴η��� ������+1
                    pp->TxBuf.Read++;
                    pp->TxBuf.Read %= LORA_TX_BUF_NUM;

                    //����ʧ������һ��
                    pp->LoRaState = PP_SLEEP;    
                    sleep_t = pp->AFrame.SpT;//����һ��ռ�õ�ʱ������
                    sleep_t <<= PP_WUT_K;

                    TimerSetValue(&pp->SleepTimer, sleep_t);
                    TimerStart(&pp->SleepTimer);

                    //state = EXE_NEED;
                }
            }
            break;
        case PP_TX_Data_ACK_Error:
            //��������� ���Դ����ж��Ƿ�����
            if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                //�����������ͻ���֡����
                pp->TxState = PP_TX_Data_Ready;    

                state = EXE_NEED;
            }
            else
            {
                //�����˴η��� ������+1
                pp->TxBuf.Read++;
                pp->TxBuf.Read %= LORA_TX_BUF_NUM;

                //�����������һ��
                pp->LoRaState = PP_SLEEP;    
                sleep_t = pp->AFrame.SpT;//����һ��ռ�õ�ʱ������
                sleep_t <<= PP_WUT_K;

                TimerSetValue(&pp->SleepTimer, sleep_t);
                TimerStart(&pp->SleepTimer);

                //state = EXE_NEED;
            }
            break;
        default:
            //���е��˴� Ӧ���ǳ�����
            LoRa_Printf("Err:TxProcess but not match[%d]\r\n", pp->TxState);
            break;
        }
    }
    else
    {
        //���е��˴� Ӧ���ǳ�����
        LoRa_Printf("Err:TxProcess but not PP_TXING[%d]\r\n", pp->LoRaState);
    }

    return state;
}
/***************************************************************************************************
*\Function      LoRa_Rx_Process_Running
*\Description   ����״̬��
*\Parameter     pp
*\Return        PP_Exe_Type
*\Note          �м����ʧ�� Ҳ��Ҫ��ʱ��Ӧ��ʱ�� ������ȥ���� ���ǽڵ��ǻ��Ǵ�������״̬
*\Log           2018.05.12    Ver 1.0    Job               
����������
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
            //�Ѿ����յ�����ͷ ������Ӧ����
            if (pp->AFrameSize == PP_WUHead_With_RTC_Len && pp->WUHead.Cmd == PP_CMD_RTC_SYNC && 
                pp->WUHead.Sum == Get_Sum8((u8*)&pp->WUHead, PP_WUHead_With_RTC_Len - 1))
            {
                //��Ҫ���ж� Ŀǰ��Ȼ���ж��д��� ���Բ��ù�
                //BACKUP_PRIMASK();
                //DISABLE_IRQ( );
                if (TimeDECCheck(&pp->WUHead.RTC_SYNC.T) && stm32_Inrtc_getTime(&cur))
                {
                    diff = TIME_SubSec_Diff(pp->WUHead.RTC_SYNC, cur);
                    //С��1��ʹ��ƽ�� ���� ֱ������
                    if (diff > -1000 && diff < 1000)
                    {
                        stm32_Inrtc_msShift(diff);
                    }
                    else
                    {
                        //��̫�� ֱ���޸�ʱ��
                        stm32_Inrtc_setTime(pp->WUHead.RTC_SYNC);
                    } 
                    //��¼���һ��ʱ��ͬ��ʱ��
                    pp->RTCSync_TIME = pp->WUHead.RTC_SYNC;

                    LoRa_Printf("%02d:%02d:%02d  %03d\r\n", pp->WUHead.RTC_SYNC.T.Hour, pp->WUHead.RTC_SYNC.T.Minute, pp->WUHead.RTC_SYNC.T.Second, pp->WUHead.RTC_SYNC.SubSecs);
                    LoRa_Printf("%02d:%02d:%02d  %03d\r\n", cur.T.Hour, cur.T.Minute, cur.T.Second, cur.SubSecs);
                    LoRa_Printf("RTC[%d]\r\n", (s32)diff);
                }
                //ʹ���ж�
                //RESTORE_PRIMASK();
                //ENABLE_IRQ();

                //�˳�����
                pp->LoRaState = PP_IDLE;

                TimerStart(&pp->CADTimer);

                state = EXE_NEED;
            }
            else if (pp->AFrameSize == PP_WUHead_Frame_Len && pp->WUHead.Cmd == PP_CMD_WUHead)
            {
                if (pp->WUHead.Dst_Addr == PP_BROADCAST_ADDR) //�㲥��ַ
                {
                    pp->FrameTry = 0;
                    pp->FrameTotal = 0;
                    pp->RxTotalLen = 0;
                    pp->FrameN = 0;
                    //�㲥��ַ ֱ�ӽ��н������� ���ڲ���ȷ���Ͷ˵ķ���ʱ�� ������Ҫ������������ģʽ
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
                    //��ַ����� ����ʱ��˯��ȥ
                    //�˴��Ƿ�Ҫ����������ռ�õ����ʱ�� �ݲ����� 
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
                //�˴�Ӧ�ý����쳣״̬�Ĵ���
                abnormal_flag = 1;
            }
            break;
        case PP_RX_WUHead_RecvError:
            //���մ��� �˴λ���ͷ���մ��� �ٴ�����CAD��� ȷ����Ҫ��ʧ����ͷ       
            pp->LoRaState = PP_RX_CAD_REQ;
            state = EXE_NEED;
            break;
        case PP_RX_WUHead_ACK_SendReq:
            //SpTͷ������ʱ���յ� �ֿ���Ϊ�˱����ط�
            send_timeout = pp->WUheadTime;
            send_timeout >>= PP_WUT_K;
            //�Լ���ַ ���ͻ���ACK
            ack.Cmd = PP_CMD_WUHead_ACK;
            ack.SpT = pp->WUHead.SpT - send_timeout;
            ack.Dst_Addr = pp->WUHead.Src_Addr;
            ack.Src_Addr = pp->Self_Addr;
            pp->RxState = PP_RX_WUHead_ACK_Sending;
            lora_sx1278_preamble_set(pp->Dev, pp->PreambleShort);
            lora_sx1278_send(pp->Dev, (u8*)&ack, PP_WUHead_Frame_Len, pp->WUheadTime);  
            break;
        case PP_RX_WUHead_ACK_Sending:
            //�ȴ�����ACK����жϻ�ʱ
            break;
        case PP_RX_WUHead_ACK_SendDone:
            //ACK������� ת������������ 
            pp->FrameTry = 0;
            pp->FrameTotal = 0;
            pp->FrameN = 0;
            pp->RxTotalLen = 0;

            pp->RxState = PP_RX_Data_RecvReady;

            state = EXE_NEED;
            break;
        case PP_RX_WUHead_ACK_SendError:
            //���Դ����ж��Ƿ�����
            if (++(pp->FrameTry) < PP_TX_TRY_MAX_COUNT)
            {
                //�����������ͻ���֡����
                pp->RxState = PP_RX_WUHead_ACK_SendReq;    

                state = EXE_NEED;
            }
            else
            {   

                //����ʧ�� Ҳ��Ҫ���߶�Ӧ��ʱ�� �����ܷ���
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
            //���յ���������ж�
            pp->AFrame.Sum = ((u8*)&pp->AFrame)[pp->AFrame.DT + 4];//��ȡsum

            if (pp->AFrameSize <= LORA_FRAME_MAX_BUFSIZE + 5 && pp->AFrameSize == (pp->AFrame.DT + 5) 
                && pp->AFrame.Cmd == PP_CMD_Data && Get_Sum8((u8*)&pp->AFrame, pp->AFrame.DT + 4) == pp->AFrame.Sum)
            {
                //������ �����ͱ�֤����ʱ��ȷ����
                if (pp->FrameTotal == 0)//��ʾ��һ��
                {
                    pp->FrameTotal = (pp->AFrame.CTL >> 4);
                }
                    
                if (pp->FrameTotal <= LORA_MAX_FRAME_NUM && pp->FrameN == (pp->AFrame.CTL & 0x0F))
                {
                    //�������ݵ����ջ�����
                    memcpy(pp->RxBuf + pp->FrameN * LORA_FRAME_MAX_BUFSIZE, pp->AFrame.Data, pp->AFrame.DT);
                    pp->RxTotalLen += pp->AFrame.DT;

                    pp->FrameN++;

                    //�㲥�ж�
                    if (pp->WUHead.Dst_Addr == PP_BROADCAST_ADDR)
                    {                           
                        pp->RxState = PP_RX_Data_Done;
                        state = EXE_NEED;
                        break;
                    }
                    else
                    {
                        //��Ҫ׼��ACK����
                        pp->RxState = PP_RX_Data_ACK_SendReq;
                        state = EXE_NEED;
                        break;
                    }
                }
            }
            else
            {
                //����LoRa���ݿ��ܳ��ֽ��յ������˵������ݻ���� ���Ե��˽׶�Ҳ��Ҫ��������
                pp->RxState = PP_RX_Data_RecvError;
                state = EXE_NEED;
                break;
            }
            //�˴���ʾ�����쳣 ����Ӧ����ǰbreak
            abnormal_flag = 2;//by Job 2018��5��28�� �˽׶β���ִ����

            break;
        case PP_RX_Data_RecvError:
            //����ǹ㲥 ���ճ�ʱ�� �϶�����ʧ���� ��Ϊû���ط�
            if (pp->WUHead.Dst_Addr == PP_BROADCAST_ADDR)
            {
                //����ʧ�� Ҳ��Ҫ���߶�Ӧ��ʱ�� �����ܷ���
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
                    //���ڷ��ͷ��ȴ�ACKʱ������һ��ACK+waitACK����ʱ ��Ϊ���շ���ӦҲ��Ҫ��ʱ ����ǰһ��
                    TimerSetValue(&pp->RxErrorTimer, delay_t - 3);
                    TimerStart(&pp->RxErrorTimer);
                }
                else
                {
                    //����ʧ�� Ҳ��Ҫ���߶�Ӧ��ʱ�� �����ܷ���
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
            //׼��ACK����
            //pp->TxWUheadTime��֮ǰͷ������ʱ���յ� �ֿ���Ϊ�˱����ط�
            //�Լ���ַ ���ͻ���ACK
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
            //��һ��״̬�ж��Ƿ���ɽ���
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
                //���⣺��Ϊ�ǽ������� ������һ��ACK����ʧ�� ���ǽ��յ�����ʱ��ȷ�����Լ��һ��
                if (pp->FrameN >= pp->FrameTotal)
                {
                    pp->RxState = PP_RX_Data_Done;
                    state = EXE_NEED;
                }
                else
                {
                    //����ʧ�� Ҳ��Ҫ���߶�Ӧ��ʱ�� �����ܷ���
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
            //������ɴ���
            if (pp->FrameN >= pp->FrameTotal)
            {
                //�ص� 
                if (pp->LoRa_Recv_CallBack)
                {
                    pp->LoRa_Recv_CallBack(pp->CallBackParam, pp->WUHead.Src_Addr, pp->RxBuf, pp->RxTotalLen, pp->RSSI_Last, pp->SNR_Last);
                }
                
                pp->LoRaState = PP_IDLE;
 
                //���˽������ ��������CAD��ʱ��
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
        //���е��˴� Ӧ���ǳ�����
        LoRa_Printf("Err:RxProcess but not PP_RXING[%d]\r\n", pp->LoRaState);
    }


    //�쳣����״̬����
    if (abnormal_flag == 1 && pp->AFrameSize && IS_PP_CMD(pp->WUHead.Cmd))
    {
        u32 n = 0;
        //������������ ˵�����չ�����©����ĳ��״̬ ֱ�Ӹ���ʱ���������
        //����Э��ǰ�����ֽڶ��� cmd WUT ����ֱ��ת��ʹ�ü���
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
*\Description   loraЭ��״̬��
*\Parameter     pp
*\Return        LP_State_Type
*\Note          1��EXE_NO ˵���ⲿ����ִ������ �������ִ�д˺���
*               ״̬��ģʽȱ�ݣ�����ѭ������ռʱ�ܳ�������ʱ���п��ܻ��Ͻ��շ��͵�ʱ������ͨ��ʧ��
                �����RTOS����Ҫ����̼߳�ͨ�Ż��� �����л����� ����Ҳ�ǻ����
                ��OSӦ�ý����������״̬�� ����ÿ���ж������ִ��һ�¼��ɽ��ʱ������
                ����ǰ��ʱһ�� ������շ�����������״̬
*\Log           2018.05.12    Ver 1.0    Job               
����������
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
        //���ȼ��㲥Уʱ���
        if (pp->RTCSync_Flag)
        {
#ifdef PP_TX_DELAY_ENABLE
            //���ж��Ƿ���Ҫ��ʱ
            if (pp->TXDelayFlag == PP_TX_Delay_REQ)
            {
                //������ʱ
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
                //��ʱ��� 
                pp->TXDelayFlag = PP_TX_Delay_REQ;
            }
#endif
            TimerStop(&pp->CADTimer);//ֹͣCAD���ڼ��
            pp->LoRaState = PP_RTC_SYNC_CADING;//����ǰCAD�������ͻ

            pp->CADDoneCount = 0;
            pp->CADTryCount = 0;
            lora_sx1278_startcad(pp->Dev, pp->CADTimeOut);
            break;
        }
        //��鷢�ͱ��
        if (pp->TxFlag)
        {
            if (pp->TxBuf.Read == pp->TxBuf.Write)//�����������
            {
                pp->TxFlag = FALSE;
                TimerStart(&pp->CADTimer);//����CAD��ʱ��
            }
            else
            {
#ifdef PP_TX_DELAY_ENABLE
                //���ж��Ƿ���Ҫ��ʱ
                if (pp->TXDelayFlag == PP_TX_Delay_REQ)
                {
                    //������ʱ
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
                    //��ʱ��� 
                    pp->TXDelayFlag = PP_TX_Delay_REQ;
                }
#endif
                TimerStop(&pp->CADTimer);//ֹͣCAD���ڼ��
                pp->LoRaState = PP_TX_CAD_REQ;//����ǰCAD�������ͻ
                state = EXE_NEED;
            }
        }
        break;
    case PP_RTC_SYNC_CADING:
        break;
    case PP_RTC_SYNC_REQ:
        //�����͹㲥Уʱ
        pp->FrameN = 0;
        pp->FrameTry = 0;//��ʵֻʹ��������
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
        //����������ʼ�� ׼�����Ͱ�
        pp->FrameN = 0;
        pp->FrameTry = 0;
        //�����ְܷ���
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
        //���з���״̬��
        state = LoRa_Tx_Process_Running(pp);
        break;
    case PP_RX_CAD_REQ:
        //˵����Ҫִ��CAD�����
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
    //    lora_sx1278_recv(pp->Dev, pp->TxWUheadTime, FALSE);//�������ջ���ͷ
    //    break;
    case PP_RXING:
        //ֻҪ������������̾͵���ʱ
        pp->TXDelayFlag = PP_TX_Delay_REQ;
        //�˴�Ӧ���ǽ���״̬��
        state = LoRa_Rx_Process_Running(pp);
        break;
    case PP_SLEEP:
        break;
    case PP_SLEEP_EXIT:
        LoRa_Printf("SpExit!\r\n");
        pp->LoRaState = PP_IDLE;

        TimerStart(&pp->CADTimer);//����CAD��ʱ��
        break;
    default:
        LoRa_Printf("Err:LoRa Process state[%d] error!\r\n", pp->LoRaState);
        break;
    }

    return state;
}
/***************************************************************************************************
*\Function      LoRa_Period_Handle
*\Description   ����Ŀǰ�ô������ж����ж��� �����ѭ�����Բ�ִ��
*\Parameter     pp
*\Return        void
*\Note          1�����͡�����ʱ������CAD����ʱ����
*\Log           2018.05.12    Ver 1.0    Job               
����������
***************************************************************************************************/
void LoRa_Period_Handle(LoRa_PP_type* pp)
{
    if (pp == NULL)
    {
        return;
    }

    while (1)
    {
        //ֻ�м�⵽�������߲��˳�
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
*\Parameter     dst_addr Ŀ���ַ 0������0xFFFF�㲥 ��������
*\Parameter     buf �������ݻ������׵�ַ
*\Parameter     len ���ͳ���
*\Return        bool FALSE��ʾ����������������д���
*\Note          ���ú��ͻ����������ݻᱣ�浽LoRaЭ��ջ�� ���豣��
                ���ü����� ����������Э��ջ��ɷ���
*\Log           2018.05.14    Ver 1.0    Job               
����������
***************************************************************************************************/
bool LoRa_Send(LoRa_PP_type* pp, u16 dst_addr, u8* buf, u32 len)
{
    u32 write_tmp = 0;

    if (pp == NULL || buf == NULL || len == 0 || len > LORA_BUF_MAX_SIZE || dst_addr == 0)
    {
        return FALSE;
    }
    //Ԥ�л᲻������
    write_tmp = (pp->TxBuf.Write + 1) % LORA_TX_BUF_NUM;
    if (write_tmp == pp->TxBuf.Read)
    {
        LoRa_Printf("Err:Lora Tx full!\r\n");
        return FALSE;
    }

    //��������ӽ�ȥ
    memcpy(&pp->TxBuf.Buf[pp->TxBuf.Write * LORA_BUF_MAX_SIZE], buf, len);
    pp->TxBuf.Len[pp->TxBuf.Write] = len;
    pp->TxBuf.DstAddr[pp->TxBuf.Write] = dst_addr;

    pp->TxBuf.Write++;
    pp->TxBuf.Write %= LORA_TX_BUF_NUM;

    //�����ķ���������״̬����λ
    pp->TxFlag = TRUE;

    LoRa_Period_Handle(pp);

    return TRUE;
}

/***************************************************************************************************
*\Function      LoRa_RTC_Sync_BroadCast
*\Description   RTC�㲥 ͬ��ʱ�� �ֶ�����
*\Parameter     pp
*\Return        bool
*\Note          ִ����״̬����
*\Log           2018.05.19    Ver 1.0    Job               
				����������
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
*\Description   ʹ�ܻ����ʱ��Уʱ
*\Parameter     pp
*\Parameter     interval
*\Parameter     flag
*\Return        bool
*\Note          
*\Log           2018.05.19    Ver 1.0    Job               
				����������
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
        //���ڶ�ʱ�����ʱ65�� ������1����ʱ �ȶ�ʱ60s
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
*\Description   ���ý��ջص�����
*\Parameter     pp
*\Parameter     
*\Parameter     LoRa_Recv_CallBack
*\Parameter     p
*\Parameter     src_addr ���յ����ݵĵ�ַ
*\Parameter     buf      �����ջ�����
*\Parameter     len      ���յ����ݳ���
*\Parameter     rssi     ���յ��ź�ǿ��
*\Parameter     snr      ���յ��ź������
*\Parameter     p        ���ջص������Ĳ��� û����ΪNULL
*\Return        bool
*\Note          !!!!!!!!!��Ҫ�ڻص������н��к�ʱ����!!!!!!!!!
*\Log           2018.05.15    Ver 1.0    Job               
				����������
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
				����������
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
*\Description   ��ȡ���һ�ε��ź���Ϣ
*\Parameter     pp
*\Parameter     rssi
*\Parameter     snr
*\Return        bool
*\Note          
*\Log           2018.05.15    Ver 1.0    Job               
				����������
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
*\Description   �޸�Э��ջ�Ĺ���Ƶ��
*\Parameter     pp
*\Parameter     freq
*\Return        bool
*\Note          ֻ�п���ʱ�������޸�
*\Log           2018.05.18    Ver 1.0    Job               
				����������
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
*\Description   ���ع���Ƶ��
*\Parameter     pp
*\Return        u32
*\Note          ������0
*\Log           2018.05.18    Ver 1.0    Job               
				����������
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
*\Description   �������һ��ʱ��ͬ����ʱ��
*\Parameter     pp
*\Return        TIME_SubSec
*\Note          ������� ���Ƿ���ʱ��ͬ����ʱ��
*\Log           2018.05.25    Ver 1.0    Job               
				����������
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
