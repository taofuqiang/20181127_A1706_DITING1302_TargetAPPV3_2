/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_data.c
*\Description   数据处理模块，用于更方便地实现复杂数据取值、赋值。
*\Log           2008.09.19    Ver 1.0    张波
*               创建文件。
*               2008.09.27    Ver 1.1    张波
*               修改注释
***************************************************************************************************/

#include "usdl_tools.h"
#include "usdl_data.h"

/***************************************************************************************************
*\Ver           InitBlock
*\Description   默认块数据。
*\Log           2009.04.15    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
const BlockType InitBlock = INIT_BLOCK;

/***************************************************************************************************
*\Function      GetU8LowU4
*\Description   获取低半字
*\Parameter     value   原数据
*\Return        u8
*\Log           2009.04.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8 GetU8LowU4(u8 value)
{
    return value&0x0F;
}

/***************************************************************************************************
*\Function      GetU8HighU4
*\Description   获取高半字
*\Parameter     value   原数据
*\Return        u8
*\Log           2009.04.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8 GetU8HighU4(u8 value)
{
    return (value>>4)&0x0F;
}

/***************************************************************************************************
*\Function      SetU8LowU4
*\Description   设置低半字
*\Parameter     value       原数据
*\Parameter     half_word   设置数据
*\Return        u8          结果
*\Note          1)half_word只截取低半字。
*\Log           2009.04.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8 SetU8LowU4(u8 value, u8 half_word)
{
    return (value&0xF0)|(half_word&0x0F);
}

/***************************************************************************************************
*\Function      SetU8HighU4
*\Description   设置高半字
*\Parameter     value       原数据
*\Parameter     half_word   设置数据
*\Return        u8          结果
*\Note          1)half_word只截取高半字。
*\Log           2009.04.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8 SetU8HighU4(u8 value, u8 half_word)
{
    return (value&0x0F)|((half_word<<4)&0xF0);
}

/***************************************************************************************************
*\Function      GetVoidVar
*\Description   以宽度width取地址src的数据。
*\Parameter     src     数据地址
*\Parameter     width   数据宽度，其取值可为8, 16和32。
*\Return        u32
*\Note          1)若width的值不是8, 16或32，则取值8。
*\Log           2008.7.28   Ver 1.0    张波
*               创建函数。
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
*\Description   以宽度width设地址dst的数据。
*\Parameter     dst     数据地址
*\Parameter     width   数据宽度，其取值可为8, 16和32。
*\Parameter     value   设置值
*\Return        void
*\Note          1)若width的值不是8, 16或32，则取值8。
*\Log           2009.09.23   Ver 1.0    张波
*               创建函数。
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
*\Description   将U8数组转为U16数据  *(u16*)方式存在隐患
*\Parameter     src     数据地址
*\Return        u16
*\Note          1)该函数保证了即使src地址不是2的倍数也能正确取值。
*\Log           2007.11.20   Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u16 ArrayU8ToU16(void* src)
{
    return (*(u8*)src) + ((*((u8*)src+1))<<8);
}

/***************************************************************************************************
*\Function      ArrayU8ToU32
*\Description   将U8数组转为U32数据  *(u32*)方式存在隐患
*\Parameter     src     数据地址
*\Return        u32
*\Note          该函数保证了即使src地址不是4的倍数也能正确取值。
*\Log           2007.11.20   Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 ArrayU8ToU32(void* src)
{
    return (*(u8*)src) + ((*((u8*)src+1))<<8) + ((*((u8*)src+2))<<16) + ((*((u8*)src+3))<<24);
}

/***************************************************************************************************
*\Function      ArrayU16ToU32
*\Description   将U16数组转为U32数据  *(u32*)方式存在隐患
*\Parameter     src     数据地址
*\Return        u32
*\Log           2007.11.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 ArrayU16ToU32(void* src)
{
    return (*(u16*)src) + ((*((u16*)src+1))<<16);
}

/***************************************************************************************************
*\Function      ArrayU8ToU64
*\Description   将U8数组转为U64数据  *(u64*)方式存在隐患
*\Parameter     src     数据地址
*\Return        u64
*\Log           2008.10.15    Ver 1.0    张波
*               创建函数。
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
*\Description   将U16数据转为U8数组
*\Parameter     dst     目的地址
*\Parameter     value   数据
*\Return        u8*
*\Log           2008.07.21    Ver 1.0    张波
*               创建函数。
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
*\Description   将U32数据转为U8数组
*\Parameter     dst     目的地址
*\Parameter     value   数据
*\Return        u8*     dst+4
*\Log           2008.07.21    Ver 1.0    张波
*               创建函数。
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
*\Description   将U64数据转为U8数组
*\Parameter     dst     目的地址
*\Parameter     value   数据
*\Return        u8*
*\Log           2008.10.15    Ver 1.0    张波
*               创建函数。
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
*\Description   对数据进行块分割计算并返回数据块结构体
*\Parameter     addr        数据首位置(相对地址base_addr)
*\Parameter     len         数据长度
*\Parameter     interval    每块分隔长度
*\Parameter     base_addr   起始地址（参照地址默认为0）
*\Return        BlockType
*\Note          1)若结构中数据长度等于0则该数据地址无效。
*               2)例如：某数据从0开始，每5字节为一块，计算起始地址为6长度为1的结果是：
*                 头地址：6；头长度：4；尾地址：15；尾长度：2；中间块地址：10；中间块长度：5；
*                 中间块数：1；
*\Log           2008.3.26   Ver 1.0    张波
*               创建函数。
*               2008.9.18   Ver 1.1    张波
*               修改函数。
*               2009.7.15   Ver 1.2    张波
*               增加起始地址参数、修改注释。
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

