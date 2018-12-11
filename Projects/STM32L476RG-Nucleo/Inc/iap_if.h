/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          iap_if.h
*\Description   
*\Note          
*\Log           2016.06.28    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#ifndef _IAP_IF_H
#define _IAP_IF_H
#include "stm32l4xx.h"
#include "public_type.h"
extern char* IAPoE_Name;//IAP固件名称
extern char* IAPoE_Ver;//IAP固件版本
extern char* IAPoE_Date;//IAP固件发布日期

extern uint64_t IAP_FLAG;//iap模式标志

//固件最大大小
#define OTA_BIN_MAX_SIZE     (FLASH_BANK_SIZE - 64 * 1024)

#define OTA_RUN_APP          ((uint64_t)0xFFFFFFFFFFFFFFFF)
#define OTA_Update_APP       ((uint64_t)0xFFFF5678FFFF5678)
#define OTA_Force_Update_APP ((uint64_t)0x1234567812345678)

//运行区地址
#define FLASH_APP_RUN_ADDR  (0x08010000)

#define FLASH_IAP_FLAG_ADDR  (FLASH_BASE + FLASH_BANK_SIZE + FLASH_PAGE_SIZE*12)

//设备参数存储地址
#define FLASH_IAP_PARAM_ADDR  (FLASH_BASE + FLASH_BANK_SIZE + FLASH_PAGE_SIZE*13)

//APP
#define FLASH_APP_PARAM_ADDR  (FLASH_BASE + FLASH_BANK_SIZE + FLASH_PAGE_SIZE*14)

#define FLASH_APP_INFO_ADDR  (FLASH_BASE + FLASH_BANK_SIZE+ FLASH_PAGE_SIZE*15)
//app起始地址
#define FLASH_APP_COPY_ADDR  (FLASH_BASE + FLASH_BANK_SIZE + FLASH_PAGE_SIZE*16)

void RunAppMode(void);
void UpdateAppMode(void);
void ForceUpdateAppMode(void);
uint64_t ReadIAPMode(void);

#define DEV_FIRST_POWER_FLAG 0x5678
typedef struct iap_param_type_st
{
	uint16_t Power_First_Flag;//第一次上电标志
	uint16_t ChipUniqueID[6];
	uint16_t version;//版本信息
	uint16_t IAP_flag;//IAP标志 iap 1 app 2
	uint16_t ID;//lora id
	uint16_t Channel;//信道 
	uint16_t GatewayID;//网关ID
//	uint16_t Lora_fourth;//lora配置第四位
//	uint16_t Lora_six;//lora配置第六位
	s16 lora_rssi;//
	s8 lora_snr;//	
	u16 src_addr;
}iap_param_type;

typedef struct
{
	uint16_t CompressFlag;//压缩标志，0未压缩，1压缩
	uint32_t UpdatesTotalSize;//升级包总大小
	uint16_t UpdatesSubSize;//升级包分包大小
	uint32_t UpdatesCRC32;//升级包文件校验
	uint32_t ComPackOriginalSize;//压缩之前原始文件总大小
	uint16_t ComPackNo;//压缩包个数
	uint16_t PerComPackOriginalSize;//压缩之前每个压缩包原始数据大小
	uint32_t ComPackOriginalCRC32;//lora配置第四位
}__attribute__((packed)) iap_updata_param_type;
extern iap_param_type iap_param;
void iap_param_save(void);
void iap_param_load(void);
void iap_param_default(void);
void stm32_param_load(void);
#endif /*_IAP_IF_H*/

