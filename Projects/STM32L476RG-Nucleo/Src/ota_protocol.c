/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          ota_protocol.c
*\Description   
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "ota_protocol.h"
#include "flash_if.h"
#include "iap_if.h"
#include "app_board.h"
//#include "lora.h"
#include "iap_if.h"
#include "rtc.h"
//OTAЭ���־
u8 OTA_UpdateFlag = 0;// 0 δ������1 ��ʼ��2 �����У�3 �������

u16 OTA_Version = 0;//�汾
u64 OTA_TotalSize = 0;//�����ܳ��ȣ�ѹ����ʱ��ѹ������ܳ��ȣ�δѹ��ʱ��BIN�ļ����ܳ���
u32 OTA_SubSize = 0;//�ְ��̶���
u64 OTA_CRC32 = 0;//У��

u16 OTA_Frame_Num = 0;//���������

//����Э��ʹ��
u8 OTACommBuf[2048];
u8 OTASendBuf[1024];
u16 OTACommBufLen = 0;//���յ����ݵĳ��ȣ��ڻص������и�ֵ���ڽ�������������
//u16 OTASourceId = 0;//�յ����ݵ�ID
 
static int OTAWaitTicks;

extern LoRa_PP_type LoRa_PP;

uint32_t iap_param_read(uint8_t* buf, uint32_t len)
{
	u32 data_len;
	int i=0;
	//��ѯ��ǰID �ŵ��Ƿ��flash�浵��һ�£�һ����ʼͨ�ţ���һ�¿�ʼ����flash
	data_len=29;
	if (len < data_len)
    {
        return 0;
    }

	buf[i++] = COMM_PROTOCOL_HEAD;
	buf[i++] = (data_len)%256;
	buf[i++] = (data_len)/256;
	buf[i++] = FUC_PARAM_IAP_READ;//������	
	buf[i++] = iap_param.ID%256;
	buf[i++] = iap_param.ID/256;
	buf[i++] = (iap_param.Channel-410);
	buf[i++] = iap_param.GatewayID%256;
	buf[i++] = iap_param.GatewayID/256;	
	
	buf[i++] = iap_param.ChipUniqueID[0]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[0]>>8)&0xFF;
	buf[i++] = iap_param.ChipUniqueID[1]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[1]>>8)&0xFF;	
	buf[i++] = iap_param.ChipUniqueID[2]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[2]>>8)&0xFF;	
	buf[i++] = iap_param.ChipUniqueID[3]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[3]>>8)&0xFF;	
	buf[i++] = iap_param.ChipUniqueID[4]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[4]>>8)&0xFF;
	buf[i++] = iap_param.ChipUniqueID[5]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[5]>>8)&0xFF;
	buf[i++] =(u8)iap_param.lora_rssi;//iap_param.Lora_fourth;
	buf[i++] = ((u8)(iap_param.lora_rssi>>8));//iap_param.Lora_six;
	buf[i++] = iap_param.IAP_flag;
	buf[i++] = (u8)iap_param.lora_snr;
	buf[i++] = 0x00;
	buf[i++] = iap_param.version;
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = COMM_PROTOCOL_TAIL;
	

	
	return i;
}


void iap_param_analysis(u8* buf)
{
	 u16 data = 0;

	data =  buf[5];
	data = (data << 8) | buf[4];
	iap_param.ID=data;   //�豸ID
	iap_param.Channel=buf[6]+410;//�ŵ�
	data =  buf[8];
	data = (data << 8) | buf[7];
	iap_param.GatewayID=data;//����ID
//	iap_param.Lora_fourth=buf[21];
//	iap_param.Lora_six=buf[22];		

}


void iap_param_send_lora(void)
{
	u32 send_len = 0;
	send_len = iap_param_read(OTASendBuf, sizeof(OTASendBuf));		
	if (send_len)
	{
		//iap_param.src_addr=0x2642;
		LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  
	}
}

///***************************************************************************************************
//*\Function      Get_Sum8
//*\Description   У���
//*\Parameter     buf
//*\Parameter     len
//*\Return        u8
//*\Note          
//*\Log           2018.01.19    Ver 1.0    Job
//*               ����������
//***************************************************************************************************/
//u8 Get_Sum8(u8* buf, u32 len)
//{
//    u8 sum = 0;
//    u32 i = 0;

//    for (i = 0; i < len; i++)
//    {
//        sum += buf[i];
//    }

//    return sum;
//}

///***************************************************************************************************
//*\Function      ArrayU8ToU16
//*\Description   
//*\Parameter     src
//*\Return        u16
//*\Note          
//*\Log           2018.01.19    Ver 1.0    Job
//*               ����������
//***************************************************************************************************/
//u16 ArrayU8ToU16(void* src)
//{
//    return (*(u8*)src) + ((*((u8*)src+1))<<8);
//}
///***************************************************************************************************
//*\Function      ArrayU8ToU32
//*\Description   
//*\Parameter     src
//*\Return        uint32_t
//*\Note          
//*\Log           2018.01.19    Ver 1.0    Job
//*               ����������
//***************************************************************************************************/
//u32 ArrayU8ToU32(void* src)
//{
//    return ((u32)*(u8*)src) + (((u32)*((u8*)src+1))<<8) +
//           (((u32)*((u8*)src+2))<<16) + (((u32)*((u8*)src+3))<<24);
//}


/***************************************************************************************************
*\Function      OTA_Start_ACK_Construct
*\Description   OTA��ʼACK����
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 OTA_Start_ACK_Construct(u8* buf, u32 len)
{
    u32 data_len = 8;
    u32 i = 0;

    if (len < data_len)
    {
        return 0;
    }
    buf[i++] = COMM_PROTOCOL_HEAD;
    buf[i++] = (u8)(data_len);
    buf[i++] = (u8)((data_len) >> 8);
    buf[i++] = OTA_Start;
    buf[i++] = iap_param.src_addr/256;//ID
    buf[i++] = iap_param.src_addr%256;//ID
    buf[i++] = Get_Sum8(buf + 1, data_len - 3);
    buf[i++] = COMM_PROTOCOL_TAIL;

    return i;
}

/***************************************************************************************************
*\Function      OTA_Update_ACK_Construct
*\Description   ���������ACK
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 OTA_Update_ACK_Construct(u8* buf, u32 len, u8 state_code)
{
    u32 data_len = 11;
    u32 i = 0;

    if (len < data_len)
    {
        return 0;
    }
    buf[i++] = COMM_PROTOCOL_HEAD;
    buf[i++] = (u8)(data_len);
    buf[i++] = (u8)((data_len) >> 8);
    buf[i++] = OTA_Update;
    buf[i++] = iap_param.src_addr/256;//ID
    buf[i++] = iap_param.src_addr%256;//ID

    buf[i++] = (u8)OTA_Frame_Num;
    buf[i++] = (u8)(OTA_Frame_Num >> 8);

    buf[i++] = state_code;

    buf[i++] = Get_Sum8(buf + 1, data_len - 3);
    buf[i++] = COMM_PROTOCOL_TAIL;

    return i;
}
/***************************************************************************************************
*\Function      OTA_End_ACK_Construct
*\Description   
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 OTA_End_ACK_Construct(u8* buf, u32 len)
{
    u32 data_len = 8;
    u32 i = 0;

    if (len < data_len)
    {
        return 0;
    }
    buf[i++] = COMM_PROTOCOL_HEAD;
    buf[i++] = (u8)(data_len);
    buf[i++] = (u8)((data_len) >> 8);
    buf[i++] = OTA_End;
    buf[i++] = iap_param.src_addr/256;//ID
    buf[i++] = iap_param.src_addr%256;//ID
    buf[i++] = Get_Sum8(buf + 1, data_len - 3);
    buf[i++] = COMM_PROTOCOL_TAIL;

    return i;
}
/***************************************************************************************************
*\Function      OTA_Protocol_Anylise
*\Description   
*\Parameter     buf
*\Parameter     len
*\Return        del_info_type
*\Note          
*\Log           2018.01.19    Ver 1.0    Job               
				����������
***************************************************************************************************/
extern void system_reset(void);
del_info_type OTA_Protocol_Anylise(u8* buf, u32 len)
{
    del_info_type info = {anylise_waiting, 0};
    u32 i = 0;
    u32 data_len = 0;
    u16 dev_ID = 0;
    u32 updata_len = 0;//������С������
    u32 send_len = 0;

    if (len >= COMM_PROTOCOL_MIN_LEN)
    {
        /*��ʼ����*/
        i = 0;
        while (1)
        {
            /*ͬ��ͷ��β����ȷ����У��Ͳ���ȷ*/
            if ((buf[i] !=  COMM_PROTOCOL_HEAD))
            {
                for (i += 1; i < len; i++)
                {
                    if (buf[i] == COMM_PROTOCOL_HEAD)
                    {
                        break;
                    }
                }
                if (len - i >=  COMM_PROTOCOL_MIN_LEN)
                {
                    continue;
                }
                else
                {
                    info.state = anylise_reject;
                    info.del_len = i;
                    return info;
                }      
            }
            else
            {
                /*�������ݰ�����*/
                data_len = buf[i + 2];
                data_len = (data_len << 8) + buf[i + 1];
                 //���ȺϷ��ж� Ŀǰ��������8-1200
                if (data_len > COMM_PROTOCOL_MAX_LEN || data_len < COMM_PROTOCOL_MIN_LEN)
                {
                    i++;
                    continue;
                }
                /*����������ݳ���С��֡���ɳ��� �����ȴ�*/
                if (len - i < data_len)
                {
                    /*i == 0��ʾ��ȷ��֡ͷǰ��û�д������� ���صȴ�,����ɾ�����������*/
                    if (i == 0)
                    {
                        info.state = anylise_waiting;
                        info.del_len = 0;
                    }
                    else
                    {
                        info.state = anylise_reject;
                        info.del_len = i;
                    }
                    return info;
                }
                /*���֡β,֡β���Ϲ��� ����Ϊ����һ������������֡*/
                if (buf[i + data_len - 1] == COMM_PROTOCOL_TAIL && 
                    buf[i + data_len - 2] == Get_Sum8(buf + i + 1, data_len - 3))
                {
                    break;
                }
                else//֡βУ�鲻���� ɾ��һ���ֽ� ��������
                {
                    i += 1;
                    continue;
                }
            }
        }
        //��֤ͨ��������Э�鴦��
        info.state = anylise_accept;//��Ԥ��Ϊ����
        //��ɸѡ��ID ����
        dev_ID = buf[i + 5];
        dev_ID = (dev_ID << 8) | buf[i + 4];
				//system_param.src_addr = dev_ID;//0x4321;//    //���԰�ID
        switch (buf[i + 3])
        {
					case OTA_Start:
					{
							HAL_Delay(20);
							//�����汾 ���� �ְ��� ��У��
							OTA_UpdateFlag = 1;

							OTA_Version = buf[i+6];
							OTA_Version = (OTA_Version << 8)|buf[i+7];

							OTA_TotalSize = ArrayU8ToU32(buf + i + 10);

							OTA_SubSize = ArrayU8ToU16(buf + i + 14);

							OTA_CRC32 = ArrayU8ToU32(buf + i + 16);
							//�洢bin����+crc32
							Flash_Storage_Header(buf + i + 8);
							//������д����
							Flash_Erase_Page(FLASH_APP_COPY_ADDR, OTA_TotalSize);
							//�ظ�ACK
							send_len = OTA_Start_ACK_Construct(OTASendBuf, sizeof(OTASendBuf));
							if (send_len)
							{
									LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);
							}
						if( last_state == State_Lp_Run)
							Marker_State = State_Lp_Run;
						else if( last_state == State_LED_Debug)
							Marker_State = State_LED_Debug;
						else if( last_state == State_GPS_Data_Handle)
							Marker_State = State_GPS_Data_Handle;
						}
					 break;
					case OTA_Update:
					{
							OTA_UpdateFlag = 2;
							//������ ����
							OTA_Frame_Num = ArrayU8ToU16(buf + i + 6);
							
							if (data_len > 10 && OTA_Frame_Num * OTA_SubSize < OTA_BIN_MAX_SIZE)
							{
									//updata_lenӦ����8�ı���
									updata_len = data_len - 10;
									Flash_Write(FLASH_APP_COPY_ADDR + (OTA_Frame_Num) * OTA_SubSize, buf + i + 8, updata_len);
									//ack
									send_len = OTA_Update_ACK_Construct(OTASendBuf, sizeof(OTASendBuf), 0);
									if (send_len)
									{
										LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);
									}
							}
							else
							{
									send_len = OTA_Update_ACK_Construct(OTASendBuf, sizeof(OTASendBuf), 1);
									if (send_len)
									{
										LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);
									}
							}
						if( last_state == State_Lp_Run)
							Marker_State = State_Lp_Run;
						else if( last_state == State_LED_Debug)
							Marker_State = State_LED_Debug;
						else if( last_state == State_GPS_Data_Handle)
							Marker_State = State_GPS_Data_Handle;
						}
					 break;
					case OTA_End:
					{
							//������� ��������
							OTA_UpdateFlag = 3;
							//�ظ�ACK
							send_len = OTA_End_ACK_Construct(OTASendBuf, sizeof(OTASendBuf));
							if (send_len)
							{
									LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);
									HAL_Delay(2000);
							}
							UpdateAppMode();
						 // NVIC_SystemReset();
							system_reset();//BARON
					}
					 break;
					case FUC_PARAM_IAP_READ://��ȡIAP����
					{
							send_len = iap_param_read(OTASendBuf, sizeof(OTASendBuf));		
							if (send_len)
							{
								LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  
							}
						if( last_state == State_Lp_Run)
							Marker_State = State_Lp_Run;
						else if( last_state == State_LED_Debug)
							Marker_State = State_LED_Debug;
						else if( last_state == State_GPS_Data_Handle)
							Marker_State = State_GPS_Data_Handle;
					}
					break;	
					case FUC_PARAM_IAP_WRITE://�����ŵ�
					{
												
//						iap_param.Channel=buf[6]+410;//�ŵ�
//						iap_param_save();
//						LoRa_Set();
						iap_param_analysis(buf);
						
						//���û�Ӧ
						send_len = 0x0a;
					  OTASendBuf[0] = 0xfe;
						OTASendBuf[1] = 0x0a;
						OTASendBuf[2] = 0x00;
						OTASendBuf[3] = FUC_PARAM_IAP_WRITE;
						OTASendBuf[4] = iap_param.ID%256;//����lora��ַ
						OTASendBuf[5] = iap_param.ID/256;
						OTASendBuf[6] = (iap_param.Channel-410);//�������ŵ�
						OTASendBuf[7] = 0x01;
						OTASendBuf[8] = Get_Sum8(&OTASendBuf[1],7);
						OTASendBuf[9] = 0xbe;
						LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  //���û�Ӧ
						if( last_state == State_Lp_Run)
							Marker_State = State_Lp_Run;
						else if( last_state == State_LED_Debug)
							Marker_State = State_LED_Debug;
						else if( last_state == State_GPS_Data_Handle)
							Marker_State = State_GPS_Data_Handle;
					}
					 break;					
					case FUC_PARAMSET_SAVE://��������
					{
						iap_param_save();
						app_param_save();//save to flash
						if( last_state == State_Lp_Run)
							Marker_State = State_Lp_Run;
						else if( last_state == State_LED_Debug)
							Marker_State = State_LED_Debug;
						else if( last_state == State_GPS_Data_Handle)
							Marker_State = State_GPS_Data_Handle;
						//���û�Ӧ
						send_len = 0x0a;
					  OTASendBuf[0] = 0xfe;
						OTASendBuf[1] = 0x0a;
						OTASendBuf[2] = 0x00;
						OTASendBuf[3] = FUC_PARAMSET_SAVE;
						OTASendBuf[4] = iap_param.ID%256;//����lora��ַ
						OTASendBuf[5] = iap_param.ID/256;
						OTASendBuf[6] = (iap_param.Channel-410);//�������ŵ�
						OTASendBuf[7] = 0x01;
						OTASendBuf[8] = Get_Sum8(&OTASendBuf[1],7);
						OTASendBuf[9] = 0xbe;
						LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  //���û�Ӧ						
						
					}
					 break;
					case FUC_PARAMSET_RESET://ϵͳ����
					{
						//���û�Ӧ
						send_len = 0x0a;
					  OTASendBuf[0] = 0xfe;
						OTASendBuf[1] = 0x0a;
						OTASendBuf[2] = 0x00;
						OTASendBuf[3] = FUC_PARAMSET_RESET;
						OTASendBuf[4] = iap_param.ID%256;//����lora��ַ
						OTASendBuf[5] = iap_param.ID/256;
						OTASendBuf[6] = (iap_param.Channel-410);//�������ŵ�
						OTASendBuf[7] = 0x01;
						OTASendBuf[8] = Get_Sum8(&OTASendBuf[1],7);
						OTASendBuf[9] = 0xbe;
						LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  //���û�Ӧ
						
						HAL_Delay(10);
					//NVIC_SystemReset();
						system_reset();//BARON
					}
					 break;
					case FUC_PARAM_APP_RECOVER://�ָ�Ĭ�ϲ���
					{
					 	  iap_param_default(); //����Ĭ�ϲ���,����FLASH֮�����
			        app_param_default();//����appĬ�ϲ���������FLASH֮�����

						if( last_state == State_Lp_Run)
							Marker_State = State_Lp_Run;
						else if( last_state == State_LED_Debug)
							Marker_State = State_LED_Debug;
						else if( last_state == State_GPS_Data_Handle)
							Marker_State = State_GPS_Data_Handle;
							
						//���û�Ӧ
						send_len = 0x0a;
					  OTASendBuf[0] = 0xfe;
						OTASendBuf[1] = 0x0a;
						OTASendBuf[2] = 0x00;
						OTASendBuf[3] = FUC_PARAM_APP_RECOVER;
						OTASendBuf[4] = iap_param.ID%256;//����lora��ַ
						OTASendBuf[5] = iap_param.ID/256;
						OTASendBuf[6] = (iap_param.Channel-410);//�������ŵ�
						OTASendBuf[7] = 0x01;
						OTASendBuf[8] = Get_Sum8(&OTASendBuf[1],7);
						OTASendBuf[9] = 0xbe;
						LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  //���û�Ӧ
							
					}
					break;
					case 0x01://���ü����˸ʱ��
					{ 
//						system_param.start_time[0] = buf[i + 7];//((buf[i + 7]&0xf0)>>4)*10 + (buf[i + 7]&0x0f);
//						system_param.start_time[1] = buf[i + 8];//((buf[i + 8]&0xf0)>>4)*10 + (buf[i + 8]&0x0f);
//						system_param.start_time[2] = buf[i + 9];//((buf[i + 9]&0xf0)>>4)*10 + (buf[i + 9]&0x0f);
//						
						system_param.led_period_interval[0] = buf[i + 0x0c];
						system_param.led_period_interval[1] = buf[i + 0x0b];
						system_param.led_period_interval[2] = buf[i + 0x0a];
//						system_param.RTC_WakeUp_Time = buf[i + 0x0d];

//						
//						Start_Time.Hours = buf[i + 7];
//						Start_Time.Minutes = buf[i + 8];
//						Start_Time.Seconds = buf[i + 9];
						LED_Period = (buf[i + 0x0c] << 16) | (buf[i + 0x0b] << 8) | buf[i + 0x0a];
//						RTC_WakeUp_Period = buf[i + 0x0d];
//						
//						RTC_Alarm(Start_Time,Alarm_LED_Period);
//						stm32_Inrtc_EnWakeup(RTC_WakeUp_Period*1000);
						
						
//						system_param.led_period_interval[0] = buf[i + 0x09];
//						system_param.led_period_interval[1] = buf[i + 0x08];
//						system_param.led_period_interval[2] = buf[i + 0x07];
//						LED_Period = (buf[i + 0x09] << 16) | (buf[i + 0x08] << 8) | buf[i + 0x07];
						app_param_save();//save to flash
						
						//���û�Ӧ
						send_len = 0x0a;
					  OTASendBuf[0] = 0xfe;
						OTASendBuf[1] = 0x0a;
						OTASendBuf[2] = 0x00;
						OTASendBuf[3] = 0x01;
						OTASendBuf[4] = iap_param.ID%256;//����lora��ַ
						OTASendBuf[5] = iap_param.ID/256;
						OTASendBuf[6] = (iap_param.Channel-410);//�������ŵ�
						OTASendBuf[7] = 0x01;
						OTASendBuf[8] = Get_Sum8(&OTASendBuf[1],7);
						OTASendBuf[9] = 0xbe;
						LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  //���û�Ӧ
						
						if( last_state == State_Lp_Run)
							Marker_State = State_Lp_Run;
						else if( last_state == State_LED_Debug)
							Marker_State = State_LED_Debug;
						else if( last_state == State_GPS_Data_Handle)
							Marker_State = State_GPS_Data_Handle;
						
					}
					break;
					case 0x02://����LED  debug�ڼ䳣����˸ģʽ
					{
						send_len = 0x0b;
						for(uint8_t j=0;j<11;j++)
						{
							OTASendBuf[j] = buf[i + j];
						}
						OTASendBuf[4] = iap_param.ID%256;//����lora��ַ
						OTASendBuf[5] = iap_param.ID/256;
						OTASendBuf[6] = (iap_param.Channel-410);//�������ŵ�
						OTASendBuf[9] = Get_Sum8(&OTASendBuf[1],8);
						 
						 
					  if(buf[i + 7] == 0x01)//LED����
						{
						 LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  //���û�Ӧ
						 if(buf[i + 8] == 0x01)
						 {
						   LED1_ON;
						   LED2_OFF;
						 }
						 if(buf[i + 8] == 0x02)
						 {
						   LED1_OFF;
						   LED2_ON;
						 }
						 if(buf[i + 8] == 0x03)
						 {
						   LED1_ON;
						   LED2_ON;
						 }
                         led_flag = 1;			
                         LED_Timeout.Enable = 1;					
						 LED_Timeout.Counter=0;						
						 Marker_State = State_LED_Debug;							 
						}
						else if(buf[i + 7] == 0x00)
						{
						 LED_Timeout.Enable=0;
						 LED_Timeout.Counter=0;
						 LED1_OFF;
					     LED2_OFF;
						 led_flag = 0;
						 LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  //���û�Ӧ
						 Marker_State = State_Lp_Run;
						}
//						 send_len = 0x0b;
//						 for(uint8_t j=0;j<11;j++)
//						 {
//							 OTASendBuf[j] = buf[i + j];
//						 }
//						 	OTASendBuf[4] = iap_param.ID%256;//����lora��ַ
//	            OTASendBuf[5] = iap_param.ID/256;
//						  OTASendBuf[6] = (iap_param.Channel-410);//�������ŵ�
//						  OTASendBuf[9] = Get_Sum8(&OTASendBuf[1],8);
//						 LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  //���û�Ӧ
					}
					break;
					case 0x13://��ȡ�豸״̬��Ϣ��ADCֵ����Ӳ���汾��Ϣ�����õ���ʼʱ�䡢�����˸ʱ�䡢RTC����ʱ�䡢����ʱ��
					{	
						u16VrefintCal =  (*(__IO uint16_t*) 0x1FFF75AA);//��ȡADCֵ
						ADC_GetVol(ADC_CHANNEL_VREFINT,&u16VrefintVal,2);
						ADC_GetVol(VCC_CHECK_CHANNEL,&adc_data,2);
						VddVal = (3.0 * u16VrefintCal / u16VrefintVal) - vddval_offset;
						//ADC_Value = (uint16_t)((adc_data*(VddVal*1000.0/4096.00)) * 2.0);
						ADC_Value = (uint16_t)(VddVal*1000);
						u16VrefintCal = 0;
						u16VrefintVal = 0;
						adc_data = 0;
						//����adcֵ
						system_param.adc_data_h = (ADC_Value >> 8)&0x00ff;
						system_param.adc_data_l = ADC_Value & 0x00ff;
	
					  send_len = device_param_read(OTASendBuf,0x13, sizeof(OTASendBuf));//��ȡ�����豸��Ϣ		
						if (send_len)
						{
							LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  
						}
						if( last_state == State_Lp_Run)
							Marker_State = State_Lp_Run;
						else if( last_state == State_LED_Debug)
							Marker_State = State_LED_Debug;
						else if( last_state == State_GPS_Data_Handle)
							Marker_State = State_GPS_Data_Handle;
					}
					break;
					case 0x14://��ȡGPS��Ϣ
					{
					  //Marker_State=State_GPS_Data_Read;
						Lora_SendGPS_lastData();
						if( last_state == State_Lp_Run)
							Marker_State = State_Lp_Run;
						else if( last_state == State_LED_Debug)
							Marker_State = State_LED_Debug;
						else if( last_state == State_GPS_Data_Handle)
							Marker_State = State_GPS_Data_Handle;
					}
					break;
					case 0x05://����RTC  ppmֵ
					{
					  ROOM_TEMPERATURE_PPM = buf[i + 7];
						system_param.ROOM_TEMPERATURE_PPM = ROOM_TEMPERATURE_PPM;
						app_param_save();//save to flash
						
						//���û�Ӧ
						send_len = 0x0a;
					  OTASendBuf[0] = 0xfe;
						OTASendBuf[1] = 0x0a;
						OTASendBuf[2] = 0x00;
						OTASendBuf[3] = 0x05;
						OTASendBuf[4] = iap_param.ID%256;//����lora��ַ
						OTASendBuf[5] = iap_param.ID/256;
						OTASendBuf[6] = (iap_param.Channel-410);//�������ŵ�
						OTASendBuf[7] = 0x01;
						OTASendBuf[8] = Get_Sum8(&OTASendBuf[1],7);
						OTASendBuf[9] = 0xbe;
						LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  //���û�Ӧ
					}
					break;
					case 0x0a://��ȡ�����Ӳ���汾�š��豸id
					{
							send_len = app_param_read(OTASendBuf,FUC_PARAM_APP_READ, sizeof(OTASendBuf));		
							if (send_len)
							{
								LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  
							}
						  if( last_state == State_Lp_Run)
							 Marker_State = State_Lp_Run;
						  else if( last_state == State_LED_Debug)
							 Marker_State = State_LED_Debug;
						  else if( last_state == State_GPS_Data_Handle)
							 Marker_State = State_GPS_Data_Handle;
					}
					 break;
					default:
					{
						//printf("the cmd[%d] is no support!\n", buf[i + 4]);
						if( last_state == State_Lp_Run)
							Marker_State = State_Lp_Run;
						else if( last_state == State_LED_Debug)
							Marker_State = State_LED_Debug;
						else if( last_state == State_GPS_Data_Handle)
							Marker_State = State_GPS_Data_Handle;
					}
					 break;		
        }
        info.del_len = data_len + i; 
        return info;
    }
    else
    {
        return info;
    } 
}
/***************************************************************************************************
*\Function      OTA_Periodic_Handle
*\Description   Э�鴦���߳�
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.01.19    Ver 1.0    Job               
				����������
***************************************************************************************************/
void OTA_Periodic_Handle(void)
{
    u32 len = 0;
    del_info_type info = {anylise_waiting, 0};
    int tick_diff;

    if (OTACommBufLen == 0)
    {
        OTAWaitTicks = HAL_GetTick();
        return;
    }
    
		len = OTACommBufLen;
		OTACommBufLen = 0;

    info = OTA_Protocol_Anylise(OTACommBuf, len);

//    if (info.state != anylise_waiting)
//    {
//        //ɾ��
//        stm32_uart_del(&UartTTL, info.del_len);
//        OTAWaitTicks = HAL_GetTick();
//    }
//    else
//    {
//        tick_diff = HAL_GetTick() - OTAWaitTicks;
//        if (Abs(tick_diff) > 5*1000)//����5S�������ɹ�������ջ��������ݣ����߱���һ�㣬ɾ��һ���ֽڣ�������������Ƚ�����
//        {
//            stm32_uart_clear(&UartTTL);
//            OTAWaitTicks = HAL_GetTick();
//        } 
//    }
}
