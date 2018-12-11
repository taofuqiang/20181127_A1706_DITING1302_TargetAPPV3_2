/**
  ******************************************************************************
  * @file    flash_if.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   This file provides high level routines to manage internal Flash 
  *          programming (erase and write). 
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include "stdio.h"
#include "string.h"
#include "iap_if.h"

/***************************************************************************************************
*\Function      GetPage
*\Description   
*\Parameter     Addr
*\Return        uint32_t
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
uint32_t GetPage(uint32_t Addr)
{
    uint32_t page = 0;

    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        /* Bank 1 */
        page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else
    {
        /* Bank 2 */
        page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }

    return page;
}

/***************************************************************************************************
*\Function      GetBank
*\Description   
*\Parameter     Addr
*\Return        uint32_t
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
uint32_t GetBank(uint32_t Addr)
{
    uint32_t bank = 0;

    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
    {
        /* No Bank swap */
        if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
        {
            bank = FLASH_BANK_1;
        }
        else
        {
            bank = FLASH_BANK_2;
        }
    }
    else
    {
        /* Bank swap */
        if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
        {
            bank = FLASH_BANK_2;
        }
        else
        {
            bank = FLASH_BANK_1;
        }
    }

    return bank;
}
/***************************************************************************************************
*\Function      Flash_Unlock
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
void Flash_Unlock(void)
{
    /* FLASH ���� */
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR | FLASH_FLAG_OPTVERR |  
        FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR | FLASH_FLAG_OPERR | FLASH_FLAG_EOP); 
}
/***************************************************************************************************
*\Function      Flash_Lock
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
void Flash_Lock(void)
{
    HAL_FLASH_Lock();
}
/***************************************************************************************************
*\Function      Flash_Erase_Page
*\Description   
*\Parameter     addr
*\Parameter     bin_size
*\Return        uint_8
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
HAL_StatusTypeDef Flash_Erase_Page(uint32_t addr, uint32_t bin_size)
{
    /*Variable used for Erase procedure*/
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t FirstPage = 0;
    uint32_t NbOfPages = 0;
    uint32_t PAGEError = 0;
    uint32_t BankNumber = 0;

    //����bin�ļ���С
    if (bin_size > OTA_BIN_MAX_SIZE || bin_size == 0)
    {
        return HAL_ERROR;
    }
    Flash_Unlock();

    /* Get the 1st page to erase */
    FirstPage = GetPage(addr);
    /* Get the number of pages to erase from 1st page */
    NbOfPages = (bin_size + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    /* Get the bank */
    BankNumber = GetBank(addr);
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
        printf("erase[%d] error!\n", NbOfPages);
        Flash_Lock();
        return HAL_ERROR;
    }

    Flash_Lock();
    return HAL_OK;
}
/***************************************************************************************************
*\Function      ArrayU8ToU64
*\Description   
*\Parameter     src
*\Return        uint64_t
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
static uint64_t ArrayU8ToU64(void* src)
{
    return ((uint64_t)*(uint8_t*)src) + (((uint64_t)*((uint8_t*)src+1))<<8) +
        (((uint64_t)*((uint8_t*)src+2))<<16) + (((uint64_t)*((uint8_t*)src+3))<<24) +
        (((uint64_t)*((uint8_t*)src+4))<<32) + (((uint64_t)*((uint8_t*)src+5))<<40) +
        (((uint64_t)*((uint8_t*)src+6))<<48) + (((uint64_t)*((uint8_t*)src+7))<<56);
}
/***************************************************************************************************
*\Function      flash_write
*\Description   ��flash��д����
*\Parameter     addr
*\Parameter     buf
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
uint32_t Flash_Write(uint32_t addr, uint8_t* buf, uint32_t len)
{
    uint32_t left_num = 0;
    uint32_t i = 0;
    uint32_t n = 0;
    uint64_t double_word = 0;

    /*�ж��Ƿ�8�ı���*/
    left_num = len % 8;
    /*ֻ����˫��д��*/
    len = len - left_num;

    //�жϵ�ַ�Ƿ���8�ı���
    if (0 != (addr % 0x08))
    {
        /*���Ƕ���ĵ�ַ*/
        return 0;
    }
    
    Flash_Unlock();

    for (i = 0; i < len; i += 8)
    {
        double_word = ArrayU8ToU64(buf + i);
				FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + i, double_word) != HAL_OK)
        {
            Flash_Lock();
            return i;
        }
    }

    /* �Ƿ��ǵ��ֽ� */
    if (0 != left_num)
    {
        double_word = 0;

        for (n = 0; n < left_num; n++)
        {
            double_word |= (((uint64_t)buf[i + n]) << (8 * n));
        }
        for (n = left_num; n < 8; n++)
        {
            double_word |= (((uint64_t)0xFF) << (n * 8));
        }
				FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
        if (HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + i, double_word))
        {
            /*���ϸղ�ȥ����*/
            len += left_num;
        }
    }

    Flash_Lock();
    return  len;
}
/***************************************************************************************************
*\Function      flash_read
*\Description   ��flash��ȡ����
*\Parameter     addr
*\Parameter     buf
*\Parameter     len
*\Return        uint32_t
*\Note          
*\Log           2015.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
uint32_t Flash_Read(uint32_t addr, uint8_t* buf, uint32_t len)
{
    memcpy(buf, (uint8_t*)(addr), len);
    return len;
}

/***************************************************************************************************
*\Function      Flash_Storage
*\Description   ���δ洢�Ƿ���ѹ������2�ֽڣ���lora�����ܴ�С��4�ֽڣ���lora�������С��2�ֽڣ���lora����CRC32У�飨4�ֽڣ���
*               BIN�ļ��ܴ�С��4�ֽڣ���ѹ�����ְ���Ŀ��2�ֽڣ���ѹ������������ԭʼ��С��2�ֽڣ���ѹ����CRC32У�飨4�ֽڣ�
*\Parameter     bin_size
*\Parameter     crc32
*\Return        uint32_t
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               ����������
*               2018.05.03�޸�
***************************************************************************************************/
uint32_t Flash_Storage_Header(uint8_t *pData)
{
	  uint64_t Tem;
    if (Flash_Erase_Page(FLASH_APP_INFO_ADDR, FLASH_PAGE_SIZE) != HAL_OK)
    {
        return 0;
    }

    Flash_Unlock();
//Tem = 0x1234567812345678;
		Tem = *((uint64_t *)pData);
	FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_APP_INFO_ADDR, Tem )!= HAL_OK)
    {
        return 0;
    }
		pData  = pData + 8;
		Tem = *((uint64_t *)pData);
	FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_APP_INFO_ADDR + 8, Tem ) != HAL_OK)
    {
        return 0;
    }
		pData  = pData + 8;
		Tem = *((uint64_t *)pData);
	FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_APP_INFO_ADDR + 16, Tem  )!= HAL_OK)
    {
        return 0;
    }

    Flash_Lock();
    return 1;
}
