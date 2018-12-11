/***************************************************************************************************
*                    (c) Copyright 2008-2014 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          usdl_buf.c
*\Description   
*\Note          
*\Log           2014.11.24    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#include "usdl_buf.h"

/***************************************************************************************************
*\Function      RxBufInit
*\Description   ��������ʼ��
*\Parameter     buf
*\Return        void
*\Note
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
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
*\Description   �򻺳������һ������
*\Parameter     buf
*\Parameter     byte
*\Return        bool
*\Note          1.������ݺ����������βָ���ͷָ����ȣ�˵��������������ͷָ��+1;
*               2.������Ϊ���λ���������󱣴������UART_RX_BUF_SIZE - 1;
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool AddByteToBuf(RingBufType* buf, u8 byte)
{
    buf->RxBuf[buf->Tail++] = byte;

    /*����βָ�룬��ֹ������󳤶�*/
    buf->Tail %= buf->RxBufSize;

    /*�ж�βָ���ͷָ���Ƿ���ȼ��������Ƿ����*/
    if (buf->Tail == buf->Head)
    {
        buf->Head++;
        /*����ͷָ�룬��ֹ������󳤶�*/
        buf->Head %= buf->RxBufSize;
    }
    return TRUE;
}
/***************************************************************************************************
*\Function      AddDataToBuf
*\Description   �򻺳�ȥ�����һ�����ȵ�����
*\Parameter     buf
*\Parameter     data
*\Parameter     len
*\Return        bool
*\Note          
*\Log           2015.07.11    Ver 1.0    �Ų�
*               ����������
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
*\Description   ��ȡ�������е����ݳ���
*\Parameter     buf
*\Return        u32
*\Note
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 GetLenFromBuf(RingBufType* buf)
{
    return ((buf->RxBufSize + buf->Tail - buf->Head) % buf->RxBufSize);
}

/***************************************************************************************************
*\Function      GetByteFromBuf
*\Description   �ӻ������ж���һ���ֽ�
*\Parameter     buf
*\Parameter     byte
*\Return        bool
*\Note
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool GetByteFromBuf(RingBufType* buf, u8* byte)
{
    /*ͷָ�����βָ��˵��������*/
    if (buf->Head == buf->Tail)
    {
        return FALSE;
    }
    *byte = buf->RxBuf[buf->Head++];
    /*����ͷָ��*/
    buf->Head %= buf->RxBufSize;
    return TRUE;
}
/***************************************************************************************************
*\Function      DelDataFromBuf
*\Description   ������ɾ����
*\Parameter     buf
*\Parameter     len��Ҫɾ�������ݸ���
*\Return        u32��ʵ��ɾ�����ֽڸ���
*\Note
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 DelDataFromBuf(RingBufType* buf, u32 len)
{
    u32 len_tmp = 0;

    len_tmp = GetLenFromBuf(buf);
    /*������������ֹ��ȡ�ĳ��ȴ��ڻ������е����ݳ���*/
    len = Min(len, len_tmp);
    buf->Head += len;
    /*������������ָ��*/
    buf->Head %= buf->RxBufSize;
    return len;
}
/***************************************************************************************************
*\Function      ReadDataFromBuf
*\Description   �ӻ������ж�������
*\Parameter     buf
*\Parameter     byte
*\Parameter     clear,�Ƿ�����������е�����
*\Return        u32,ʵ�ʶ�ȡ�ĳ���
*\Note
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 ReadDataFromBuf(RingBufType* buf, u8* byte, u32 len, bool clear)
{
    u32 len_tmp;
    u32 i = 0;
    u32 head = 0;

    len_tmp = GetLenFromBuf(buf);
    /*������������ֹ��ȡ�ĳ��ȴ��ڻ������е����ݳ���*/
    len = Min(len, len_tmp);

    if (clear)
    {
        /*����������Ĳ���*/
        for (i = 0; i < len; i++)
        {
            GetByteFromBuf(buf, byte++);
        }
    }
    else
    {
        /*�����������ݱ�����ͷָ�룬������ڻָ�*/
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
*\Description   ����������е�����
*\Parameter     buf
*\Return        bool
*\Note          
*\Log           2014.11.24    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool ClearDateFromBuf(RingBufType* buf)
{
    buf->Tail = buf->Head;
    return TRUE;
}
