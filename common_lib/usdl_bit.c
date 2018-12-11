/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_bit.c
*\Description   λ����ģ�飬�ṩ�ḻ��λ��������
*\Log           2008.09.26    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/

#include "usdl_bit.h"

/***************************************************************************************************
*\Function      BitPointSet
*\Description   ��srcָ�����ݵĵ�posλ����Ч��
*\Parameter     src     ���ݵ�ַ
*\Parameter     pos     λ
*\Parameter     width   ���ݿ��
*\Return        void
*\Log           2008.8.22   Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void BitPointSet(void* src, u8 pos, u8 width)
{
    switch (width)
    {
    case sizeof(u8):
        (*(u8*)src) |= (1UL<<(pos));
        break;

    case sizeof(u16):
        (*(u16*)src) |= (1UL<<(pos));
        break;

    case sizeof(u32):
        (*(u32*)src) |= (1UL<<(pos));
        break;

    case sizeof(u64):
        (*(u64*)src) |= (1ULL<<(pos));
        break;

    default:

        break;
    }
}

/***************************************************************************************************
*\Function      BitPointClr
*\Description   ��srcָ�����ݵĵ�posλ����Ч��
*\Parameter     src     ���ݵ�ַ
*\Parameter     pos     λ
*\Parameter     width   ���ݿ��
*\Return        void
*\Log           2008.09.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void BitPointClr(void* src, u8 pos, u8 width)
{
    switch (width)
    {
    case sizeof(u8):
        (*(u8*)src) &= ~(1UL<<(pos));
        break;

    case sizeof(u16):
        (*(u16*)src) &= ~(1UL<<(pos));
        break;

    case sizeof(u32):
        (*(u32*)src) &= ~(1UL<<(pos));
        break;

    case sizeof(u64):
        (*(u64*)src) &= ~(1ULL<<(pos));
        break;

    default:

        break;
    }
}

/***************************************************************************************************
*\Function      BitPointNot
*\Description   ��srcָ�����ݵĵ�posλȡ����
*\Parameter     src     ���ݵ�ַ
*\Parameter     pos     λ
*\Parameter     width   ���ݿ��
*\Return        void
*\Log           2008.09.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void BitPointNot(void* src, u8 pos, u8 width)
{
    switch (width)
    {
    case sizeof(u8):
        (*(u8*)src) ^= (1UL<<(pos));
        break;

    case sizeof(u16):
        (*(u16*)src) ^= (1UL<<(pos));
        break;

    case sizeof(u32):
        (*(u32*)src) ^= (1UL<<(pos));
        break;

    case sizeof(u64):
        (*(u64*)src) ^= (1ULL<<(pos));
        break;

    default:

        break;
    }
}

/***************************************************************************************************
*\Function      BitGet
*\Description   ����value��POSλ��ֵ��
*\Parameter     value     ���ݵ�ַ
*\Parameter     pos     λ
*\Return        u8
*\Log           2008.09.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 BitGet(u64 value, u8 pos)
{
    return (value >> pos) & 1;
}

/***************************************************************************************************
*\Function      BitValueSet
*\Description   ����value�ĵ�posλ����Ч���ֵ��
*\Parameter     value       ���ݵ�ַ
*\Parameter     pos         λ
*\Return        u32
*\Log           2008.09.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u64 BitValueSet(u64 value, u8 pos)
{
    return (value) |= (1ULL<<(pos));
}

/***************************************************************************************************
*\Function      BitValueClr
*\Description   ����value��posλ����Ч���ֵ��
*\Parameter     value     ���ݵ�ַ
*\Parameter     pos       λ
*\Return        u32
*\Log           2008.09.27    Ver 1.0   �Ų�
*               ����������
***************************************************************************************************/
u64 BitValueClr(u64 value, u8 pos)
{
    return (value) &= ~(1UL<<pos);
}

/***************************************************************************************************
*\Function      BitValueNot
*\Description   ����value��POSλȡ�����ֵ��
*\Parameter     value       ���ݵ�ַ
*\Parameter     pos         λ
*\Return        u32
*\Log           2008.09.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u64 BitValueNot(u64 value, u8 pos)
{
    return (value) ^= (1ULL<<pos);
}

/***************************************************************************************************
*\Function      BitValueGetSetNum
*\Description   ������Чλ����Ŀ��
*\Parameter     value     ���ݵ�ַ
*\Return        u32
*\Log           2008.09.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 BitValueGetSetNum(u32 value)
{
    value = (value & 0x55555555) + ((value >> 1)  & 0x55555555);
    value = (value & 0x33333333) + ((value >> 2)  & 0x33333333);
    value = (value & 0x0F0F0F0F) + ((value >> 4)  & 0x0F0F0F0F);
    value = (value & 0x00FF00FF) + ((value >> 8)  & 0x00FF00FF);
    value = (value & 0x0000FFFF) + ((value >> 16) & 0x0000FFFF);
    return value;
}

/***************************************************************************************************
*\Function      BitValueU8Reverse
*\Description   ����8λvalue������ֵ��
*\Parameter     value     ���ݵ�ַ
*\Return        u8
*\Log           2008.09.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 BitValueU8Reverse(u8 value)
{
    value = (value & 0x55) << 1  | (value & 0xAA) >> 1;
    value = (value & 0x33) << 2  | (value & 0xCC) >> 2;
    value = (value & 0x0F) << 4  | (value & 0xF0) >> 4;

    return value;
}

/***************************************************************************************************
*\Function      BitValueU16Reverse
*\Description   ����16λvalue������ֵ��
*\Parameter     value     ���ݵ�ַ
*\Return        u16
*\Log           2008.09.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u16 BitValueU16Reverse(u16 value)
{
    value = (value & 0x5555) << 1  | (value & 0xAAAA) >> 1;
    value = (value & 0x3333) << 2  | (value & 0xCCCC) >> 2;
    value = (value & 0x0F0F) << 4  | (value & 0xF0F0) >> 4;
    value = (value & 0x00FF) << 8  | (value & 0xFF00) >> 8;

    return value;
}

/***************************************************************************************************
*\Function      BitValueU32Reverse
*\Description   ����32λvalue������ֵ��
*\Parameter     value     ���ݵ�ַ
*\Return        u32
*\Log           2008.09.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 BitValueU32Reverse(u32 value)
{
    value = (value & 0x55555555) << 1  | (value & 0xAAAAAAAA) >> 1;
    value = (value & 0x33333333) << 2  | (value & 0xCCCCCCCC) >> 2;
    value = (value & 0x0F0F0F0F) << 4  | (value & 0xF0F0F0F0) >> 4;
    value = (value & 0x00FF00FF) << 8  | (value & 0xFF00FF00) >> 8;
    value = (value & 0x0000FFFF) << 16 | (value & 0xFFFF0000) >> 16;

    return value;
}

/***************************************************************************************************
*\Function      BitValueReverse
*\Description   ����value�ĵ�lenλ��תֵ��
*\Parameter     value   ���ݵ�ַ
*\Parameter     len     ��lenλ(0~32)
*\Return        u32
*\Note          1) Example: BitValueReverse(0x3e23L, 3) == 0x3e26
*\Log           2008.09.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u64 BitValueReverse(u64 value, u8 len)
{
    u64 t = value;      /*�ο�*/
    u8  i = 0x00;

    for (i = 0; i < len; i++)
    {
        if (t & 1ULL)
            value |=  BitMask((len-1)-i);
        else
            value &= ~BitMask((len-1)-i);
        t >>= 1;
    }

    return value;
}

/***************************************************************************************************
*\Function      ByteArrayBitSet
*\Description   �����ֽ������е�ĳһλΪ1��
*\Parameter     data    �ֽ�����
*\Parameter     index   Ҫ����λ��λ�ã���0��ʼ��
*\Return        void
*\Log           2009.03.19    Ver 1.0    �Ų�
*               ��������
***************************************************************************************************/
void ByteArrayBitSet(u8* data, u16 index)
{
    data[index / 8] |= (1 << (index % 8));
}

/***************************************************************************************************
*\Function      ByteArrayBitClr
*\Description   ����ֽ������е�ĳһλΪ0��
*\Parameter     data    �ֽ�����
*\Parameter     index   Ҫ�����λ��λ�ã���0��ʼ��
*\Return        void
*\Log           2009.03.19    Ver 1.0    �Ų�
*               ��������
***************************************************************************************************/
void ByteArrayBitClr(u8* data, u16 index)
{
    data[index / 8] &= ~(1 << (index % 8));
}

/***************************************************************************************************
*\Function      ByteArrayBitGet
*\Description   ��ȡ�ֽ������е�ĳһλ��״̬��
*\Parameter     data    �ֽ�����
*\Parameter     index   ϣ����õ�λ��λ�ã���0��ʼ
*\Return        bool    λΪ1ʱ����TRUE��λΪ0ʱ����FALSE��
*\Log           2009.03.19    Ver 1.0    �Ų�
*               ��������
***************************************************************************************************/
bool ByteArrayBitGet(u8* data, u16 index)
{
    if (data[index / 8] & (1 << (index % 8)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
