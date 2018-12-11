/***************************************************************************************************
*                    (c) Copyright 2008-2014 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          usdl_buf.c
*\Description   
*\Note          
*\Log           2014.11.24    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#include "usdl_buf.h"

/***************************************************************************************************
*\Function      RxBufInit
*\Description   缓冲区初始化
*\Parameter     buf
*\Return        void
*\Note
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void RingBufInit(RingBufType* buf, u8* addr, u32 size)
{
    buf->Head = 0;
    buf->Tail = 0;
    buf->RxBufSize = size;
    buf->RxBuf = addr;
}
/***************************************************************************************************
*\Function      AddByteToBuf
*\Description   向缓冲区添加一个数据
*\Parameter     buf
*\Parameter     byte
*\Return        bool
*\Note          1.添加数据后如果缓冲区尾指针和头指针相等，说明缓冲区已满，头指针+1;
*               2.缓冲区为环形缓冲区，最大保存的数据UART_RX_BUF_SIZE - 1;
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool AddByteToBuf(RingBufType* buf, u8 byte)
{
    buf->RxBuf[buf->Tail++] = byte;

    /*修正尾指针，防止超过最大长度*/
    buf->Tail %= buf->RxBufSize;

    /*判断尾指针和头指针是否相等即缓冲区是否存满*/
    if (buf->Tail == buf->Head)
    {
        buf->Head++;
        /*修正头指针，防止超过最大长度*/
        buf->Head %= buf->RxBufSize;
    }
    return TRUE;
}
/***************************************************************************************************
*\Function      AddDataToBuf
*\Description   向缓冲去中添加一定长度的数据
*\Parameter     buf
*\Parameter     data
*\Parameter     len
*\Return        bool
*\Note          
*\Log           2015.07.11    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool AddDataToBuf(RingBufType* buf, u8* data, u32 len)
{
    u32 i = 0;

    for (i = 0; i < len; i++)
    {
        AddByteToBuf(buf, data[i]);
    }
    return TRUE;
}
/***************************************************************************************************
*\Function      GetLenFromBuf
*\Description   获取缓冲区中的数据长度
*\Parameter     buf
*\Return        u32
*\Note
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 GetLenFromBuf(RingBufType* buf)
{
    return ((buf->RxBufSize + buf->Tail - buf->Head) % buf->RxBufSize);
}

/***************************************************************************************************
*\Function      GetByteFromBuf
*\Description   从缓冲区中读出一个字节
*\Parameter     buf
*\Parameter     byte
*\Return        bool
*\Note
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool GetByteFromBuf(RingBufType* buf, u8* byte)
{
    /*头指针等于尾指针说明无数据*/
    if (buf->Head == buf->Tail)
    {
        return FALSE;
    }
    *byte = buf->RxBuf[buf->Head++];
    /*修正头指针*/
    buf->Head %= buf->RxBufSize;
    return TRUE;
}
/***************************************************************************************************
*\Function      DelDataFromBuf
*\Description   缓冲区删数据
*\Parameter     buf
*\Parameter     len，要删除的数据个数
*\Return        u32，实际删除的字节个数
*\Note
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 DelDataFromBuf(RingBufType* buf, u32 len)
{
    u32 len_tmp = 0;

    len_tmp = GetLenFromBuf(buf);
    /*长度修正，防止读取的长度大于缓冲区中的数据长度*/
    len = Min(len, len_tmp);
    buf->Head += len;
    /*修正缓冲区读指针*/
    buf->Head %= buf->RxBufSize;
    return len;
}
/***************************************************************************************************
*\Function      ReadDataFromBuf
*\Description   从缓冲区中读出数据
*\Parameter     buf
*\Parameter     byte
*\Parameter     clear,是否清除缓冲区中的数据
*\Return        u32,实际读取的长度
*\Note
*\Log           2012.12.22    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 ReadDataFromBuf(RingBufType* buf, u8* byte, u32 len, bool clear)
{
    u32 len_tmp;
    u32 i = 0;
    u32 head = 0;

    len_tmp = GetLenFromBuf(buf);
    /*长度修正，防止读取的长度大于缓冲区中的数据长度*/
    len = Min(len, len_tmp);

    if (clear)
    {
        /*清除缓冲区的操作*/
        for (i = 0; i < len; i++)
        {
            GetByteFromBuf(buf, byte++);
        }
    }
    else
    {
        /*如果不清除，暂保存下头指针，读完后在恢复*/
        head = buf->Head;
        for (i = 0; i < len; i++)
        {
            GetByteFromBuf(buf, byte++);
        }
        buf->Head = head;
    }
    return len;
}

/***************************************************************************************************
*\Function      ClearDateFromBuf
*\Description   清除缓冲区中的数据
*\Parameter     buf
*\Return        bool
*\Note          
*\Log           2014.11.24    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool ClearDateFromBuf(RingBufType* buf)
{
    buf->Tail = buf->Head;
    return TRUE;
}
