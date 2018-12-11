/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          iap_if.c
*\Description   iap相关接口
*\Note          
*\Log           2015.05.27    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#include "iap_if.h"
#include "stdio.h"
#include "string.h"
#include "flash_if.h"
#include "ota_protocol.h"

char* IAPoE_Name = "stm32_ota";//IAP固件名称
char* IAPoE_Ver = "V1.0";//IAP固件版本
char* IAPoE_Date = __DATE__;//IAP固件发布日期

uint64_t IAP_FLAG;//iap模式标志

/***************************************************************************************************
*\Function      RunAppMode
*\Description   进入正常模式
*\Parameter     
*\Return        void
*\Note          0xFFFFFFFF FFFFFFFF
*\Log           2018.01.18    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void RunAppMode(void)
{
    /*Variable used for Erase procedure*/
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t addr = FLASH_IAP_FLAG_ADDR;
    uint32_t FirstPage = 0;
    uint32_t NbOfPages = 0;
    uint32_t PAGEError = 0;
    uint32_t BankNumber = 0;

    IAP_FLAG = OTA_RUN_APP;
    /* FLASH 解锁 */
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 
    /* Get the 1st page to erase */
    FirstPage = GetPage(addr);
    /* Get the number of pages to erase from 1st page */
    NbOfPages = 1;
    /* Get the bank */
    BankNumber = GetBank(FLASH_IAP_FLAG_ADDR);
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks       = BankNumber;
    EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;

    /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
    you have to make sure that these data are rewritten before they are accessed during code
    execution. If this cannot be done safely, it is recommended to flush the caches by setting the
    DCRST and ICRST bits in the FLASH_CR register. */
		FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        printf("RunAppMode erase error!\n");
    }
    /* FLASH 上锁 */
    HAL_FLASH_Lock();
}
/***************************************************************************************************
*\Function      OutIAPMode
*\Description   进入更新App模式
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.05.27    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void UpdateAppMode(void)
{
    uint32_t addr = FLASH_IAP_FLAG_ADDR;

    //不是0xFFFFFFFFFFFF..则擦除
    if (ReadIAPMode() != OTA_RUN_APP)
    {
        RunAppMode();
    }
    /* FLASH 解锁 */
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    IAP_FLAG = OTA_Update_APP;;
    FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, IAP_FLAG) != HAL_OK)
    {
        printf("UpdateAppMode erase error!\n");
    }

    /* FLASH 上锁 */
    HAL_FLASH_Lock();
}

/***************************************************************************************************
*\Function      ForceUpdateAppMode
*\Description   强制更新模式
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void ForceUpdateAppMode(void)
{
    uint32_t addr = FLASH_IAP_FLAG_ADDR;

    //不是0xFFFFFFFFFFFF..则擦除
    if (ReadIAPMode() != OTA_RUN_APP || ReadIAPMode() != OTA_Update_APP)
    {
        RunAppMode();
    }
    /* FLASH 解锁 */
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

     IAP_FLAG = OTA_Force_Update_APP;

		FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, IAP_FLAG) != HAL_OK)
    {
        printf("ForceAppMode erase error!\n");
    }
    /* FLASH 上锁 */
    HAL_FLASH_Lock();
}
/***************************************************************************************************
*\Function      ReadIAPMode
*\Description   读取IAP_FLAG标志
*\Parameter     
*\Return        uint16_t
*\Note          
*\Log           2015.05.29    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
uint64_t ReadIAPMode(void)
{
    return *(uint64_t*)(FLASH_IAP_FLAG_ADDR);
}



void Get_ChipID(uint16_t* chipID)
{
    chipID[0] = *(__IO uint16_t*)(0x1FFF7590);  // 低字节
    chipID[1] = *(__IO uint16_t *)(0X1FFF7592); //  
    chipID[2] = *(__IO uint16_t *)(0X1FFF7594); // 高字节
	chipID[3] = *(__IO uint16_t*)(0x1FFF7596);  // 低字节
    chipID[4] = *(__IO uint16_t *)(0X1FFF7598); //  
    chipID[5] = *(__IO uint16_t *)(0X1FFF759A); // 高字节
}


iap_param_type iap_param;

void iap_param_save(void)
{
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t addr = FLASH_IAP_PARAM_ADDR;
    uint32_t FirstPage = 0;
    uint32_t NbOfPages = 0;
    uint32_t PAGEError = 0;
    uint32_t BankNumber = 0;

    HAL_FLASH_Unlock();
		/* 擦除这个扇区 */
	/* Clear OPTVERR bit set on virgin samples */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 

			/* Get the bank */
	BankNumber = GetBank(addr);
	FirstPage =GetPage(addr);
	NbOfPages = GetPage(addr) - FirstPage + 1;
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks     = BankNumber;
	EraseInitStruct.Page      = FirstPage;
	EraseInitStruct.NbPages	  = NbOfPages;
	FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
//		printf("erase error\n");
	}	
	else
	{
//		printf("erase ok\n");
	}
	
    Flash_Write(addr, (uint8_t*)&iap_param,sizeof(iap_param));
    addr += sizeof(iap_param);

    /* FLASH 上锁 */
    HAL_FLASH_Lock();

}

void iap_param_default(void)
{
    memset(&iap_param, 0, sizeof(iap_param));
	Get_ChipID(iap_param.ChipUniqueID);	
	iap_param.Power_First_Flag = DEV_FIRST_POWER_FLAG;   
	iap_param.version = 0x01;
	iap_param.IAP_flag = 0x02;
	iap_param.ID=(iap_param.ChipUniqueID[0]&0xFF)+((iap_param.ChipUniqueID[1]&0xFF)<<8);
	iap_param.Channel=475;//默认信道30  410+30=440
	iap_param.GatewayID=0x4321;//网关ID
//	iap_param.Lora_fourth=0x1A;
//	iap_param.Lora_six=0xC0;//0xC0
	iap_param.lora_rssi=0;//
	iap_param.lora_snr=0;//	
	iap_param.src_addr=iap_param.GatewayID;
    iap_param_save();
}


void iap_param_load(void)
{
    uint32_t addr = FLASH_IAP_PARAM_ADDR;

    iap_param.Power_First_Flag = *(__IO uint16_t*)(addr);
    if (iap_param.Power_First_Flag != DEV_FIRST_POWER_FLAG)
    {     
//		printf("Load Default Param...\r\n");
        //如果第一次上电，则默认值
        iap_param_default(); //调用默认参数,擦除FLASH之后调用
    }
    else
    {
//        printf("Load Param... \r\n");
    }

    addr =  FLASH_IAP_PARAM_ADDR;

    Flash_Read(addr, (uint8_t*)&iap_param, sizeof(iap_param));
    addr += sizeof(iap_param);
	iap_param.IAP_flag = 0x01;
}

void stm32_param_load(void)
{
    uint32_t addr = FLASH_APP_PARAM_ADDR;

    system_param.Power_First_Flag = *(__IO uint16_t*)(addr);
    if (system_param.Power_First_Flag != DEV_FIRST_POWER_FLAG)
    {     
//		printf("Load Default Param...\r\n");
        //如果第一次上电，则默认值
        app_param_default();//调用app默认参数，擦除FLASH之后调用
    }
    else
    {
//        printf("Load Param... \r\n");
    }

    addr =  FLASH_APP_PARAM_ADDR;

    Flash_Read(addr, (uint8_t*)&system_param, sizeof(system_param));
    addr += sizeof(system_param);
	
	HAL_Delay(1000);
	iap_param_load();//初始化IAP参数 包含lora通信用参数
	app_param_load();//app参数读取
	stm32_system_param_notsave_init();
}



