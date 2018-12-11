/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          iap_if.h
*\Description   
*\Note          
*\Log           2016.06.28    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#ifndef _IAP_IF_H
#define _IAP_IF_H
#include "stm32l4xx.h"
#include "public_type.h"
extern char* IAPoE_Name;//IAP�̼�����
extern char* IAPoE_Ver;//IAP�̼��汾
extern char* IAPoE_Date;//IAP�̼���������

extern uint64_t IAP_FLAG;//iapģʽ��־

//�̼�����С
#define OTA_BIN_MAX_SIZE     (FLASH_BANK_SIZE - 64 * 1024)

#define OTA_RUN_APP          ((uint64_t)0xFFFFFFFFFFFFFFFF)
#define OTA_Update_APP       ((uint64_t)0xFFFF5678FFFF5678)
#define OTA_Force_Update_APP ((uint64_t)0x1234567812345678)

//��������ַ
#define FLASH_APP_RUN_ADDR  (0x08010000)

#define FLASH_IAP_FLAG_ADDR  (FLASH_BASE + FLASH_BANK_SIZE + FLASH_PAGE_SIZE*12)

//�豸�����洢��ַ
#define FLASH_IAP_PARAM_ADDR  (FLASH_BASE + FLASH_BANK_SIZE + FLASH_PAGE_SIZE*13)

//APP
#define FLASH_APP_PARAM_ADDR  (FLASH_BASE + FLASH_BANK_SIZE + FLASH_PAGE_SIZE*14)

#define FLASH_APP_INFO_ADDR  (FLASH_BASE + FLASH_BANK_SIZE+ FLASH_PAGE_SIZE*15)
//app��ʼ��ַ
#define FLASH_APP_COPY_ADDR  (FLASH_BASE + FLASH_BANK_SIZE + FLASH_PAGE_SIZE*16)

void RunAppMode(void);
void UpdateAppMode(void);
void ForceUpdateAppMode(void);
uint64_t ReadIAPMode(void);

#define DEV_FIRST_POWER_FLAG 0x5678
typedef struct iap_param_type_st
{
	uint16_t Power_First_Flag;//��һ���ϵ��־
	uint16_t ChipUniqueID[6];
	uint16_t version;//�汾��Ϣ
	uint16_t IAP_flag;//IAP��־ iap 1 app 2
	uint16_t ID;//lora id
	uint16_t Channel;//�ŵ� 
	uint16_t GatewayID;//����ID
//	uint16_t Lora_fourth;//lora���õ���λ
//	uint16_t Lora_six;//lora���õ���λ
	s16 lora_rssi;//
	s8 lora_snr;//	
	u16 src_addr;
}iap_param_type;

typedef struct
{
	uint16_t CompressFlag;//ѹ����־��0δѹ����1ѹ��
	uint32_t UpdatesTotalSize;//�������ܴ�С
	uint16_t UpdatesSubSize;//�������ְ���С
	uint32_t UpdatesCRC32;//�������ļ�У��
	uint32_t ComPackOriginalSize;//ѹ��֮ǰԭʼ�ļ��ܴ�С
	uint16_t ComPackNo;//ѹ��������
	uint16_t PerComPackOriginalSize;//ѹ��֮ǰÿ��ѹ����ԭʼ���ݴ�С
	uint32_t ComPackOriginalCRC32;//lora���õ���λ
}__attribute__((packed)) iap_updata_param_type;
extern iap_param_type iap_param;
void iap_param_save(void);
void iap_param_load(void);
void iap_param_default(void);
void stm32_param_load(void);
#endif /*_IAP_IF_H*/

