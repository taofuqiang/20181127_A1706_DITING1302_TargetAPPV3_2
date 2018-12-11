/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_bit.c
*\Description   位操作模块，提供丰富的位操作函数
*\Log           2008.09.26    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/

#include "usdl_bit.h"

/***************************************************************************************************
*\Function      BitPointSet
*\Description   将src指向数据的第pos位置有效。
*\Parameter     src     数据地址
*\Parameter     pos     位
*\Parameter     width   数据宽度
*\Return        void
*\Log           2008.8.22   Ver 1.0    张波
*               创建函数。
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
*\Description   将src指向数据的第pos位置无效。
*\Parameter     src     数据地址
*\Parameter     pos     位
*\Parameter     width   数据宽度
*\Return        void
*\Log           2008.09.27    Ver 1.0    张波
*               创建函数。
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
*\Description   将src指向数据的第pos位取反。
*\Parameter     src     数据地址
*\Parameter     pos     位
*\Parameter     width   数据宽度
*\Return        void
*\Log           2008.09.27    Ver 1.0    张波
*               创建函数。
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
*\Description   返回value第POS位的值。
*\Parameter     value     数据地址
*\Parameter     pos     位
*\Return        u8
*\Log           2008.09.27    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8 BitGet(u64 value, u8 pos)
{
    return (value >> pos) & 1;
}

/***************************************************************************************************
*\Function      BitValueSet
*\Description   返回value的第pos位置有效后的值。
*\Parameter     value       数据地址
*\Parameter     pos         位
*\Return        u32
*\Log           2008.09.27    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u64 BitValueSet(u64 value, u8 pos)
{
    return (value) |= (1ULL<<(pos));
}

/***************************************************************************************************
*\Function      BitValueClr
*\Description   返回value第pos位置无效后的值。
*\Parameter     value     数据地址
*\Parameter     pos       位
*\Return        u32
*\Log           2008.09.27    Ver 1.0   张波
*               创建函数。
***************************************************************************************************/
u64 BitValueClr(u64 value, u8 pos)
{
    return (value) &= ~(1UL<<pos);
}

/***************************************************************************************************
*\Function      BitValueNot
*\Description   返回value第POS位取反后的值。
*\Parameter     value       数据地址
*\Parameter     pos         位
*\Return        u32
*\Log           2008.09.27    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u64 BitValueNot(u64 value, u8 pos)
{
    return (value) ^= (1ULL<<pos);
}

/***************************************************************************************************
*\Function      BitValueGetSetNum
*\Description   返回有效位的数目。
*\Parameter     value     数据地址
*\Return        u32
*\Log           2008.09.27    Ver 1.0    张波
*               创建函数。
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
*\Description   返回8位value逆序后的值。
*\Parameter     value     数据地址
*\Return        u8
*\Log           2008.09.27    Ver 1.0    张波
*               创建函数。
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
*\Description   返回16位value逆序后的值。
*\Parameter     value     数据地址
*\Return        u16
*\Log           2008.09.27    Ver 1.0    张波
*               创建函数。
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
*\Description   返回32位value逆序后的值。
*\Parameter     value     数据地址
*\Return        u32
*\Log           2008.09.27    Ver 1.0    张波
*               创建函数。
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
*\Description   返回value的低len位反转值。
*\Parameter     value   数据地址
*\Parameter     len     低len位(0~32)
*\Return        u32
*\Note          1) Example: BitValueReverse(0x3e23L, 3) == 0x3e26
*\Log           2008.09.27    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u64 BitValueReverse(u64 value, u8 len)
{
    u64 t = value;      /*参考*/
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
*\Description   设置字节数组中的某一位为1。
*\Parameter     data    字节数组
*\Parameter     index   要设置位的位置，从0开始。
*\Return        void
*\Log           2009.03.19    Ver 1.0    张波
*               创建函数
***************************************************************************************************/
void ByteArrayBitSet(u8* data, u16 index)
{
    data[index / 8] |= (1 << (index % 8));
}

/***************************************************************************************************
*\Function      ByteArrayBitClr
*\Description   清除字节数组中的某一位为0。
*\Parameter     data    字节数组
*\Parameter     index   要清除的位的位置，从0开始。
*\Return        void
*\Log           2009.03.19    Ver 1.0    张波
*               创建函数
***************************************************************************************************/
void ByteArrayBitClr(u8* data, u16 index)
{
    data[index / 8] &= ~(1 << (index % 8));
}

/***************************************************************************************************
*\Function      ByteArrayBitGet
*\Description   获取字节数组中的某一位的状态。
*\Parameter     data    字节数组
*\Parameter     index   希望获得的位的位置，从0开始
*\Return        bool    位为1时返回TRUE，位为0时返回FALSE。
*\Log           2009.03.19    Ver 1.0    张波
*               创建函数
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
