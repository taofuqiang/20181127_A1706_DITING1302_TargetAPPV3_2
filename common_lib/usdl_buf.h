/***************************************************************************************************
*                    (c) Copyright 2008-2014 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          udsl_buf.h
*\Description   
*\Note          
*\Log           2014.11.24    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#ifndef _USDL_BUF_H
#define _USDL_BUF_H
#include "usdl_type.h"

/*接收缓冲区类型*/
typedef struct  RingBuf_Type_st
{
    u32 Head;
    u32 Tail;
    u8* RxBuf;
    u32 RxBufSize;
}RingBufType;


/*发送缓冲区类型*/
typedef struct TxBufType_st
{
    u32 len;
    u32 offset;
    u8* TxBuf;
}TxBufType;

void RingBufInit(RingBufType* buf, u8* addr, u32 size);
bool AddByteToBuf(RingBufType* buf, u8 byte);
bool AddDataToBuf(RingBufType* buf, u8* data, u32 len);
u32 GetLenFromBuf(RingBufType* buf);
bool GetByteFromBuf(RingBufType* buf, u8* byte);
u32 DelDataFromBuf(RingBufType* buf, u32 len);
u32 ReadDataFromBuf(RingBufType* buf, u8* byte, u32 len, bool clear);
bool ClearDateFromBuf(RingBufType* buf);

#endif
