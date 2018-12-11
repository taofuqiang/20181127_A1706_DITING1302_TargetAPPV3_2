/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          ota_protocol.h
*\Description   
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#ifndef _OTA_PROTOCOL_H
#define _OTA_PROTOCOL_H
#include "app_board.h"

#define COMM_PROTOCOL_HEAD    0xFE
#define COMM_PROTOCOL_TAIL    0xBE

#define COMM_PROTOCOL_MIN_LEN  8
#define COMM_PROTOCOL_MAX_LEN  1280

/*解析用结构*/
typedef enum
{
    anylise_waiting,
    anylise_accept,
    anylise_reject,   
}anylise_state;

typedef struct del_info_type_st 
{
    anylise_state state;
    u32 del_len;
}del_info_type;

enum
{
    OTA_Start  = 0xE0,
    OTA_Update = 0xE1,
    OTA_End    = 0xE2,
};

extern u8 OTA_UpdateFlag;
extern u8 OTACommBuf[2048];
extern u8 OTASendBuf[1024];
extern u16 OTACommBufLen;//接收到数据的长度，在回调函数中赋值，在解析函数中清零

#define FUC_PARAMSET_RESET          0xA4
#define FUC_PARAMSET_SAVE           0xA5

#define FUC_PARAM_IAP_READ      		0x0E
#define FUC_PARAM_IAP_WRITE        		0x0F

#define FUC_PARAM_APP_READ      		0x0A

#define FUC_PARAM_APP_RECOVER       0xA6

void OTA_Periodic_Handle(void);



uint32_t iap_param_read(uint8_t* buf, uint32_t len);
void iap_param_analysis(uint8_t* buf);
#endif /*_OTA_PROTOCOL_H*/

