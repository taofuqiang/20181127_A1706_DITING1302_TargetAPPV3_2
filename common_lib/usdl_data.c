/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_data.c
*\Description   ���ݴ���ģ�飬���ڸ������ʵ�ָ�������ȡֵ����ֵ��
*\Log           2008.09.19    Ver 1.0    �Ų�
*               �����ļ���
*               2008.09.27    Ver 1.1    �Ų�
*               �޸�ע��
***************************************************************************************************/

#include "usdl_tools.h"
#include "usdl_data.h"

/***************************************************************************************************
*\Ver           InitBlock
*\Description   Ĭ�Ͽ����ݡ�
*\Log           2009.04.15    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
const BlockType InitBlock = INIT_BLOCK;

/***************************************************************************************************
*\Function      GetU8LowU4
*\Description   ��ȡ�Ͱ���
*\Parameter     value   ԭ����
*\Return        u8
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 GetU8LowU4(u8 value)
{
    return value&0x0F;
}

/***************************************************************************************************
*\Function      GetU8HighU4
*\Description   ��ȡ�߰���
*\Parameter     value   ԭ����
*\Return        u8
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 GetU8HighU4(u8 value)
{
    return (value>>4)&0x0F;
}

/***************************************************************************************************
*\Function      SetU8LowU4
*\Description   ���õͰ���
*\Parameter     value       ԭ����
*\Parameter     half_word   ��������
*\Return        u8          ���
*\Note          1)half_wordֻ��ȡ�Ͱ��֡�
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 SetU8LowU4(u8 value, u8 half_word)
{
    return (value&0xF0)|(half_word&0x0F);
}

/***************************************************************************************************
*\Function      SetU8HighU4
*\Description   ���ø߰���
*\Parameter     value       ԭ����
*\Parameter     half_word   ��������
*\Return        u8          ���
*\Note          1)half_wordֻ��ȡ�߰��֡�
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 SetU8HighU4(u8 value, u8 half_word)
{
    return (value&0x0F)|((half_word<<4)&0xF0);
}

/***************************************************************************************************
*\Function      GetVoidVar
*\Description   �Կ��widthȡ��ַsrc�����ݡ�
*\Parameter     src     ���ݵ�ַ
*\Parameter     width   ���ݿ�ȣ���ȡֵ��Ϊ8, 16��32��
*\Return        u32
*\Note          1)��width��ֵ����8, 16��32����ȡֵ8��
*\Log           2008.7.28   Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 GetVoidVar(const void* src, const u8 width)
{
    switch (width)
    {
    case sizeof(u8):
        return *(u8*)src;

    case sizeof(u16):
        return ArrayU8ToU16((u8*)src);

    case sizeof(u32):
        return ArrayU8ToU32((u8*)src);

    default:
        return *(u8*)src;
    }
}

/***************************************************************************************************
*\Function      SetVoidVar
*\Description   �Կ��width���ַdst�����ݡ�
*\Parameter     dst     ���ݵ�ַ
*\Parameter     width   ���ݿ�ȣ���ȡֵ��Ϊ8, 16��32��
*\Parameter     value   ����ֵ
*\Return        void
*\Note          1)��width��ֵ����8, 16��32����ȡֵ8��
*\Log           2009.09.23   Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void SetVoidVar(const void* dst, const u8 width, u32 value)
{
    switch (width)
    {
    case sizeof(u8):
        *(u8*)dst = (u8)value;
        break;

    case sizeof(u16):
        DataU16ToArrayU8((u8*)dst, (u16)value);
        break;

    case sizeof(u32):
        DataU32ToArrayU8((u8*)dst, value);
        break;

    default:
        *(u8*)dst = (u8)value;
        break;
    }
}

/***************************************************************************************************
*\Function      ArrayU8ToU16
*\Description   ��U8����תΪU16����  *(u16*)��ʽ��������
*\Parameter     src     ���ݵ�ַ
*\Return        u16
*\Note          1)�ú�����֤�˼�ʹsrc��ַ����2�ı���Ҳ����ȷȡֵ��
*\Log           2007.11.20   Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u16 ArrayU8ToU16(void* src)
{
    return (*(u8*)src) + ((*((u8*)src+1))<<8);
}

/***************************************************************************************************
*\Function      ArrayU8ToU32
*\Description   ��U8����תΪU32����  *(u32*)��ʽ��������
*\Parameter     src     ���ݵ�ַ
*\Return        u32
*\Note          �ú�����֤�˼�ʹsrc��ַ����4�ı���Ҳ����ȷȡֵ��
*\Log           2007.11.20   Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 ArrayU8ToU32(void* src)
{
    return (*(u8*)src) + ((*((u8*)src+1))<<8) + ((*((u8*)src+2))<<16) + ((*((u8*)src+3))<<24);
}

/***************************************************************************************************
*\Function      ArrayU16ToU32
*\Description   ��U16����תΪU32����  *(u32*)��ʽ��������
*\Parameter     src     ���ݵ�ַ
*\Return        u32
*\Log           2007.11.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 ArrayU16ToU32(void* src)
{
    return (*(u16*)src) + ((*((u16*)src+1))<<16);
}

/***************************************************************************************************
*\Function      ArrayU8ToU64
*\Description   ��U8����תΪU64����  *(u64*)��ʽ��������
*\Parameter     src     ���ݵ�ַ
*\Return        u64
*\Log           2008.10.15    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u64 ArrayU8ToU64(void* src)
{
    return ((u64)*(u8*)src) + (((u64)*((u8*)src+1))<<8) +
        (((u64)*((u8*)src+2))<<16) + (((u64)*((u8*)src+3))<<24) +
        (((u64)*((u8*)src+4))<<32) + (((u64)*((u8*)src+5))<<40) +
        (((u64)*((u8*)src+6))<<48) + (((u64)*((u8*)src+7))<<56);
}

/***************************************************************************************************
*\Function      DataU16ToArrayU8
*\Description   ��U16����תΪU8����
*\Parameter     dst     Ŀ�ĵ�ַ
*\Parameter     value   ����
*\Return        u8*
*\Log           2008.07.21    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* DataU16ToArrayU8(void* dst, u16 value)
{
    u8* r = (u8*)dst;
    *(r++) = (u8)(value);
    *(r++) = (u8)(value >> 8);
    return r;
}

/***************************************************************************************************
*\Function      DataU32ToArrayU8
*\Description   ��U32����תΪU8����
*\Parameter     dst     Ŀ�ĵ�ַ
*\Parameter     value   ����
*\Return        u8*     dst+4
*\Log           2008.07.21    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* DataU32ToArrayU8(void* dst, u32 value)
{
    u8* r = (u8*)dst;
    *(r++) = (u8)(value);
    *(r++) = (u8)(value >> 8);
    *(r++) = (u8)(value >> 16);
    *(r++) = (u8)(value >> 24);
    return r;
}

/***************************************************************************************************
*\Function      DataU64ToArrayU8
*\Description   ��U64����תΪU8����
*\Parameter     dst     Ŀ�ĵ�ַ
*\Parameter     value   ����
*\Return        u8*
*\Log           2008.10.15    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* DataU64ToArrayU8(void* dst, u64 value)
{
    u8* r = (u8*)dst;
    *(r++) = (u8)(value);
    *(r++) = (u8)(value >> 8);
    *(r++) = (u8)(value >> 16);
    *(r++) = (u8)(value >> 24);
    *(r++) = (u8)(value >> 32);
    *(r++) = (u8)(value >> 40);
    *(r++) = (u8)(value >> 48);
    *(r++) = (u8)(value >> 56);
    return r;
}

/***************************************************************************************************
*\Function      BlockCalculate
*\Description   �����ݽ��п�ָ���㲢�������ݿ�ṹ��
*\Parameter     addr        ������λ��(��Ե�ַbase_addr)
*\Parameter     len         ���ݳ���
*\Parameter     interval    ÿ��ָ�����
*\Parameter     base_addr   ��ʼ��ַ�����յ�ַĬ��Ϊ0��
*\Return        BlockType
*\Note          1)���ṹ�����ݳ��ȵ���0������ݵ�ַ��Ч��
*               2)���磺ĳ���ݴ�0��ʼ��ÿ5�ֽ�Ϊһ�飬������ʼ��ַΪ6����Ϊ1�Ľ���ǣ�
*                 ͷ��ַ��6��ͷ���ȣ�4��β��ַ��15��β���ȣ�2���м���ַ��10���м�鳤�ȣ�5��
*                 �м������1��
*\Log           2008.3.26   Ver 1.0    �Ų�
*               ����������
*               2008.9.18   Ver 1.1    �Ų�
*               �޸ĺ�����
*               2009.7.15   Ver 1.2    �Ų�
*               ������ʼ��ַ�������޸�ע�͡�
***************************************************************************************************/
BlockType BlockCalculate(u32 addr, u32 len, u32 interval, u32 base_addr)
{
    BlockType result = InitBlock;

    if (base_addr > addr)
    {
        return result;
    }

    result.HeadAddress = addr;
    result.HeadLength = (interval - ((result.HeadAddress - base_addr) % interval)) % interval;
    result.HeadLength = Min(result.HeadLength, len);

    result.BodyAddress = result.HeadAddress + result.HeadLength;
    result.BodyBlockCount = (len - result.HeadLength) / interval;
    result.BodyLength = interval * result.BodyBlockCount;

    result.TailAddress = result.BodyAddress + result.BodyLength;
    result.TailLength = len - result.HeadLength - result.BodyLength;

    return result;
}

