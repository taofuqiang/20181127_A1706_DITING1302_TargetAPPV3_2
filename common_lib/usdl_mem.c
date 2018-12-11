/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_mem.c
*\Description   内存操作模块，对C标准内存函数进行扩充。
*\Log           2008.06.24    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/

#include "usdl_mem.h"
#include "usdl_conversion.h"
//#include "usdl_lib.h"
#include <string.h>

#if MUTEX_POOL_SIZE == 0
#define MUTEX_POOL_SIZE  128
#endif

#define MUTEX_SEARCH_FAILED 0xFFFFFFFF

/***************************************************************************************************
*\Function      MemAdd
*\Description   ptr指向的内存区域中的len个字节加value
*\Parameter     ptr     指向内存区域的指针
*\Parameter     value   加和值
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2008.06.24    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemAdd(u8* ptr, const u8 value, u32 length)
{
    u32 i = 0;
    for (i = 0; i < length; i++)
    {
        *(ptr + i) += value;
    }
}

/***************************************************************************************************
*\Function      MemSub
*\Description   内存区域src中len个字节减value
*\Parameter     ptr     指向内存区域的指针
*\Parameter     value   加和值
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2008.03.05    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemSub(u8* ptr, const u8 value, u32 length)
{
    u32 i = 0;
    for (i = 0; i < length; i++)
    {
        *(ptr + i) -= value;
    }
}

/***************************************************************************************************
*\Function      MemAnd
*\Description   ptr指向的内存区域的length个字节分别与value相与。
*\Parameter     ptr     指向内存区域的指针
*\Parameter     value   1字节的数值
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2008.06.24    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemAnd(u8* ptr, const u8 value, u32 length)
{
    u32 i = 0;
    for (i = 0; i < length; i++)
    {
        *(ptr + i) &= value;
    }
}

/***************************************************************************************************
*\Function      MemNot
*\Description   ptr指向的内存区域的length个字节取反。
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemNot(u8* ptr, const u32 length)
{
    u32 i = 0;
    for (i = 0; i < length; i++)
    {
        *(ptr + i) = !(*(ptr + i));
    }
}

/***************************************************************************************************
*\Function      MemBitNot
*\Description   ptr指向的内存区域的length个字节按位取反。
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemBitNot(u8* ptr, const u32 length)
{
    u32 i = 0;
    for (i = 0; i < length; i++)
    {
        *(ptr + i) = ~*(ptr + i);
    }
}

/***************************************************************************************************
*\Function      MemOr
*\Description   ptr指向的内存区域的length个字节分别与value相或。
*\Parameter     ptr     指向内存区域的指针
*\Parameter     value   1字节的数值
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2008.06.24    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemOr(u8* ptr, const u8 value, const u32 length)
{
    u32 i = 0;
    for (i = 0; i < length; i++)
    {
        *(ptr + i) |= value;
    }
}

/***************************************************************************************************
*\Function      MemXor
*\Description   ptr指向的内存区域的length个字节分别与value相异或。
*\Parameter     ptr     指向内存区域的指针
*\Parameter     value   1字节的数值
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemXor(u8* ptr, const u8 value, const u32 length)
{
    u32 i = 0;
    for (i = 0; i < length; i++)
    {
        *(ptr + i) ^= value;
    }
}

/***************************************************************************************************
*\Function      MemSum
*\Description   返回内存区域ptr中length个字节的和
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        u32
*\Log           2008.03.21    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 MemSum(const u8* ptr, const u32 length)
{
    u32 i       = 0;
    u32 result  = 0;
    for (i = 0; i < length; i++)
    {
        result += *(ptr + i);
    }

    return result;
}

/***************************************************************************************************
*\Function      MemAllAnd
*\Description   返回内存区域ptr中length个字节的And
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        u32
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 MemAllAnd(const u8* ptr, const u32 length)
{
    u32 i       = 0;
    u32 result  = ~(0UL);

    for (i = 0; i < length; i++)
    {
        result &= *(ptr + i);
    }

    return result;
}

/***************************************************************************************************
*\Function      MemAllOr
*\Description   返回内存区域ptr中length个字节的Or
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        u32
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 MemAllOr(const u8* ptr, const u32 length)
{
    u32 i       = 0;
    u32 result  = 0;

    for (i = 0; i < length; i++)
    {
        result |= *(ptr + i);
    }

    return result;
}

/***************************************************************************************************
*\Function      MemAllXor
*\Description   返回内存区域ptr中length个字节的Xor
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        u32
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 MemAllXor(const u8* ptr, const u32 length)
{
    u32 i       = 0;
    u32 result  = 0;

    for (i = 0; i < length; i++)
    {
        result ^= *(ptr + i);
    }

    return result;
}

/***************************************************************************************************
*\Function      MemReverse
*\Description   ptr指向的内存区域中的length个字节逆序
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2008.06.24    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemReverse(void* ptr, const u32 length)
{
    u32 i = 0;
    u32 j = length - 1;

    for (i = 0; i < j; i++, j--)
    {
        *((u8*)ptr + i) ^= *((u8*)ptr + j);
        *((u8*)ptr + j) ^= *((u8*)ptr + i);
        *((u8*)ptr + i) ^= *((u8*)ptr + j);
    }
}

/***************************************************************************************************
*\Function      MemReverseCpy
*\Description   内存区域src中len个字节逆序拷贝
*\Parameter     dst     目的地址
*\Parameter     src     源地址
*\Parameter     len     长度
*\Return        void
*\Log           2008.06.24    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemReverseCpy(u8* dst, const u8* src, u32 len)
{
    u32 i = 0x00;

    for (i = 0x00; i < len; i++)
    {
        *(dst + i) = *(src + len - i - 1);
    }
}

/***************************************************************************************************
*\Function      MemLineToCycle
*\Description   内存区域src中len个字节环形拷贝到dst+start_pos起始的内存中。
*\Parameter     dst             环形缓冲区
*\Parameter     dst_size        环形缓冲区长度
*\Parameter     start_offset    环形缓冲区写入起始偏移
*\Parameter     src             数据来源内存区域
*\Parameter     len             拷贝长度
*\Return        u8*             环形写入结束时的指针（写指针）
*\Log           2008.08.04    Ver 1.0    张波
*               创建函数。
*\Log           2009.07.30    Ver 1.1    张波
*               修改当start_offset + len等于dst_size时dst + start_offset + len越界BUG。
***************************************************************************************************/
u8* MemLineToCycle(u8* dst, const u32 dst_size, u32 start_offset, u8* src, u32 len)
{
    u32 copy_len = 0x00;

    if ((dst_size == 0x00) || (start_offset >= dst_size))
    {
        return dst;
    }

    copy_len = dst_size - start_offset;

    if (len > copy_len)
    {
        memcpy(dst + start_offset, src, dst_size - start_offset);
        src += copy_len;
        len -= copy_len;
        start_offset = 0x00;
        copy_len = dst_size;
    }
    while (len > copy_len)
    {
        memcpy(dst, src, dst_size);
        src += copy_len;
        len -= copy_len;
    }
    memcpy(dst + start_offset, src, len);

    return dst + (start_offset + len) % dst_size;
}

/***************************************************************************************************
*\Function      MemCycleToLine
*\Description   环形内存区域src中start_pos到end_pos的数据拷贝到dst起始的条形缓冲区中。
*\Parameter     dst         条形缓冲区
*\Parameter     src         环形缓冲区
*\Parameter     start_pos   环形缓冲区起始偏移
*\Parameter     end_pos     环形缓冲区终止偏移
*\Parameter     src_size    环形缓冲区长度
*\Return        u32         条形缓冲区实际写入长度
*\Note          1)函数不考虑目的缓冲区宽度。
*\Note          2)若需要保护可以使用MemCycleSubToLine。
*\Log           2008.08.04    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 MemCycleToLine(u8* dst, const u8* src, const u32 start_pos, const u32 end_pos, const u32 src_size)
{
    u32 len = 0x00;

    if (end_pos > start_pos)
    {
        len = end_pos - start_pos;
        memcpy(dst, src + start_pos, len);
    }
    else if (end_pos < start_pos)
    {
        len = src_size - start_pos;
        memcpy(dst, src + start_pos, len);
        memcpy(dst + len, src, end_pos);
        len += end_pos;
    }
    return len;
}

/***************************************************************************************************
*\Function      MemCycleSubToLine
*\Description   环形内存区域src中start_pos到end_pos的前len个数据拷贝到dst起始的条形缓冲区中。
*\Parameter     dst         条形缓冲区
*\Parameter     src         环形缓冲区
*\Parameter     start_pos   环形缓冲区起始偏移
*\Parameter     end_pos     环形缓冲区终止偏移
*\Parameter     src_size    环形缓冲区长度
*\Parameter     request_len 请求长度，若[请求长度]大于[环形缓冲区数据长度]则只拷贝环形缓冲区数据。
*\Return        u32         条形缓冲区实际写入长度，小于等于request_len。
*\Note          1)函数不考虑目的缓冲区宽度，若需保护见3)。
*\Note          2)适用于定期从环形缓冲区取部分数据的操作。
*\Note          3)参数len可以填写dst_size用于目的缓冲区保护。
*\Log           2009.08.21    Ver 1.0    张波
*               创建函数。
*\Log           2009.11.13    Ver 1.1    张波
*               修改回写拷贝的位置错误问题。
***************************************************************************************************/
u32 MemCycleSubToLine(u8* dst, const u8* src, const u32 start_pos, const u32 end_pos,
                      const u32 src_size, const u32 request_len)
{
    u32 head_len = 0x00;/*前段数据*/

    if (end_pos > start_pos)
    {
        head_len = end_pos - start_pos;
        if (head_len > request_len)
        {
            head_len = request_len;
        }
        memcpy(dst, src + start_pos, head_len);
        return head_len;
    }

    else if (end_pos < start_pos)
    {
        head_len = src_size - start_pos;
        if (head_len >= request_len)
        {
            /*若[前段长度][满足]则拷贝[请求长度]*/
            memcpy(dst, src + start_pos, request_len);
            return request_len;
        }
        else
        {
            /*若[前段长度][不满足]则[先]拷贝[前段长度]*/
            memcpy(dst, src + start_pos, head_len);

            if (end_pos >= (request_len - head_len))
            {
                /*若[后段长度][满足]则拷贝[请求长度]减[前段长度]*/
                memcpy(dst + head_len, src, request_len - head_len);
                return request_len;
            }
            else
            {
                /*若[后段长度][不满足]则拷贝[后段长度]*/
                memcpy(dst + head_len, src, end_pos);
                return head_len + end_pos;
            }
        }
    }

    return 0;
}

/***************************************************************************************************
*\Function      MemMatch
*\Description   使用模式内存区域来匹配目标内存区域的任意前缀,并将通配符所代表的内存区域放
*               入结果集中。
*\Parameter     pat         指向模式内存区域的指针
*\Parameter     pat_len     模式内存区域的长度
*\Parameter     dst         指向目标内存区域的指针
*\Parameter     dst_len     目标内存区域的长度
*\Parameter     wildcard    代表通配符的指定ASCII字符
*\Parameter     result[]    Result结构数组,存放通配符所通配的内容
*\Return        bool
*                           - TRUE    匹配成功
*                           - FALSE   匹配失败
*\Note          1) 模式内存区域只包含表示0个或任意多个字节的指定通配符。
*\Note          2) 结果集大小 >= 通配符数 + 2,否则会发生溢出。
*\Note          3) 结果集第0项存储所匹配的全部目标内存区域,然后依次保存通配符所代表的内存块。
*\Note          4) 参考自 Wildcard matching algorithms    by Alessandro Cantatore
*\Log           2008.08.06    Ver 1.0    gaofeng
*               创建函数。
***************************************************************************************************/
bool MemMatch(u8* pat, const u32 pat_len, u8* dst, const u32 dst_len, const u8 wildcard, DataNode result[])
{
    u8*     d               = NULL;
    u8*     p               = NULL;
    u8*     end_pat         = pat + pat_len;
    u8*     start_dst       = dst;
    u8*     end_dst         = dst + dst_len;
    u32     result_index    = 0x00;
    bool    star            = FALSE;
    u8      i               = 0x00;

    result[result_index].Data = dst;
    result_index++;

memMatchLoop:
    for (d = dst, p = pat; (d < end_dst) && (p < end_pat); ++d, ++p)
    {
        if(*p == wildcard)
        {
            result[result_index].Data = d;
            if (result_index > 1)
            {
                result[result_index - 1].Len = (d - result[result_index - 1].Data) - (p - pat);
            }

            result_index++;
            star = TRUE;
            dst = d, pat = p;
            if ((++pat) == end_pat)
            {
                result[0].Len = dst_len;

                if (result_index > 1)
                {
                    result[result_index - 1].Len = start_dst + dst_len - result[result_index - 1].Data;
                }

                return TRUE;
            }
            goto memMatchLoop;
        }
        if ((*d) != (*p))
            goto starCheck;
    } /*End For*/

    while(*p == wildcard && p < end_pat) ++p;

    if(p == end_pat)
    {
        result[0].Len = d - start_dst;

        if (result_index > 1)
        {
            result[result_index - 1].Len = (d - result[result_index - 1].Data) - (p - pat);
        }
        return TRUE;
    }
    for(i = 0; i <= result_index; i++)
    {
        result[result_index].Data = NULL;
        result[result_index].Len  = 0;
    }
    return FALSE;

starCheck:
    if (!star)
    {
        for(i = 0; i <= result_index; i++)
        {
            result[result_index].Data = NULL;
            result[result_index].Len  = 0;
        }
        return FALSE;
    }
    dst++;
    goto memMatchLoop;
}

/***************************************************************************************************
*\Function      MemMem
*\Description   returns the location of the first occurence of data pattern b2 of size len2 in
                memory block b1 of size len1 or NULL if none is found.
*\Parameter     b1      data pattern
*\Parameter     len1    data pattern length
*\Parameter     b2      memory block
*\Parameter     len2    memory block length
*\Return        void*   Location of the first occurence or NULL if none if found.
*\Note          From: www.it.freebsd.org/pub/Unix/NetBSD/misc/wulf/memmem.c
*\Log           2010.05.15    Ver 1.0    高峰
*               创建函数。
***************************************************************************************************/
u8* MemMem(u8* b1, const u32 len1, u8* b2, const u32 len2)
{
    /* Initialize search pointer */
    u8* sp = b1;

    /* Initialize pattern pointer */
    u8* pp = b2;

    /* Intialize end of search address space pointer */
    u8* eos   = sp + len1 - len2;

    /* Sanity check */
    if(!(b1 && b2 && len1 && len2))
        return NULL;

    while (sp <= eos) {
        if (*sp == *pp)
            if (memcmp(sp, pp, len2) == 0)
                return sp;

        sp++;
    }

    return NULL;
}

/***************************************************************************************************
*\Function      MemBCDCheck
*\Description   检查内存区域src中len个字节是否均为BCD码。
*\Parameter     src
*\Parameter     len
*\Return        bool
*\Log           2008.12.02    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool MemBCDCheck(const u8* src, const u32 len)
{
    u16 i = 0x00;
    for (i = 0x00; i < len; i++)
    {
        if (CheckBCD(src[i]) == FALSE)
        {
            return FALSE;
        }
    }
    return TRUE;
}


/***************************************************************************************************
*\Function      MemShiftR
*\Description   将内存区域src中src_len个字节循环右移shift_len个字节。
*\Parameter     src         内存地址
*\Parameter     src_len     数组长度
*\Parameter     shift_len   位移长度
*\Return        void
*\Log           2008.12.11    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemShiftR(u8* src, const u32 src_len, u32 shift_len)
{
    u32 i       = 0x00;
    u32 index   = 0x00;
    u32 start   = 0x00;
    u32 temp    = 0x00;

    if(!src || !src_len || !shift_len)
    {
        return;
    }

    temp = src[0];
    shift_len %= src_len;

    for (i = 0; i < src_len; i++)
    {
        index = (index + shift_len) % src_len;
        Swap(src[index], temp);
        if(index == start)
        {
            start ++;
            index = start;
            temp = src[index];
        }
    }
}

/***************************************************************************************************
*\Function      MemAllSameValue
*\Description   测试内存区域src中len个字节是否全部等于value
*\Parameter     src         内存地址
*\Parameter     len         数据长度
*\Parameter     value       值
*\Return        bool        是否全部等于value
*\Log           2009.03.03    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool MemAllSameValue(const u8* src, u16 len, const u8 value)
{
    while(len--)
    {
        if (*(src + len) != value)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/***************************************************************************************************
*\Function      MemAllSame
*\Description   测试内存区域src中len个字节是否全部相等
*\Parameter     src     内存地址
*\Parameter     len     数据长度
*\Return        bool    是否全部相等
*\Note
*\Log           2009.03.03    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool MemAllSame(u8* src, u32 len)
{
    while(len--)
    {
        if (*(src + len) != *src)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/***************************************************************************************************
*\Function      MemAddMem
*\Description   内存相加
*\Parameter     dst     目的地址
*\Parameter     src1    内存地址1
*\Parameter     src2    内存地址2
*\Parameter     len     长度
*\Return        void
*\Note          1)溢出不进位。
*\Log           2010.04.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemAddMem(u8* dst, u8* src1, u8* src2, u32 len)
{
    while(len)
    {
        *(dst + len) = *(src1 + len) + *(src2 + len);
        len--;
    }
}

/***************************************************************************************************
*\Function      MemSubMem
*\Description   内存相减
*\Parameter     dst     目的地址
*\Parameter     src1    内存地址1
*\Parameter     src2    内存地址2
*\Parameter     len     长度
*\Return        void
*\Note          1)溢出不进位。
*\Log           2010.04.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemSubMem(u8* dst, u8* src1, u8* src2, u32 len)
{
    while(len)
    {
        *(dst + len) = *(src1 + len) - *(src2 + len);
        len--;
    }
}

/***************************************************************************************************
*\Function      MemAndMem
*\Description   内存相与
*\Parameter     dst     目的地址
*\Parameter     src1    内存地址1
*\Parameter     src2    内存地址2
*\Parameter     len     长度
*\Return        void
*\Note          1)溢出不进位。
*\Log           2010.04.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemAndMem(u8* dst, u8* src1, u8* src2, u32 len)
{
    while(len)
    {
        *(dst + len) = *(src1 + len) & *(src2 + len);
        len--;
    }
}

/***************************************************************************************************
*\Function      MemAndMem
*\Description   内存或
*\Parameter     dst     目的地址
*\Parameter     src1    内存地址1
*\Parameter     src2    内存地址2
*\Parameter     len     长度
*\Return        void
*\Note          1)溢出不进位。
*\Log           2010.04.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemOrMem(u8* dst, u8* src1, u8* src2, u32 len)
{
    while(len)
    {
        *(dst + len) = *(src1 + len) | *(src2 + len);
        len--;
    }
}

/***************************************************************************************************
*\Function      MemAndMem
*\Description   内存异或
*\Parameter     dst     目的地址
*\Parameter     src1    内存地址1
*\Parameter     src2    内存地址2
*\Parameter     len     长度
*\Return        void
*\Note          1)溢出不进位。
*\Log           2010.04.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemXorMem(u8* dst, u8* src1, u8* src2, u32 len)
{
    while(len)
    {
        *(dst + len) = *(src1 + len) ^ *(src2 + len);
        len--;
    }
}

/***************************************************************************************************
*内存互斥
***************************************************************************************************/

static MutexType MutexPool[MUTEX_POOL_SIZE] = {0x00};

/***************************************************************************************************
*\Function      MutexSearch
*\Description   互斥池搜索函数
*\Parameter     start_index
*\Parameter     addr
*\Parameter     len
*\Return        u32
*\Note
*\Log           2009.12.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
static u32 MutexSearch(u32 start_index, void* addr, u32 len)
{
    u32 i = 0x00;

    for (i = start_index; i < MUTEX_POOL_SIZE; i++)
    {
        if (MutexPool[i].Valid == TRUE)
        {
            if ((MutexPool[i].Obj.Data < ((u8*)addr + len)) &&
                ((MutexPool[i].Obj.Data + MutexPool[i].Obj.Len) > (u8*)addr))
            {
                /*在互斥池中发现互斥地址*/
                return i;
            }
        }
    }

    return MUTEX_SEARCH_FAILED;
}

/***************************************************************************************************
*\Function      MutexApply
*\Description   内存区域互斥请求
*\Parameter     addr        互斥区域地址
*\Parameter     len         互斥区域长度
*\Parameter     func        互斥处理函数
*\Parameter     func_para   互斥处理函数参数
*\Parameter     timeout     互斥等待重试次数(0:无超时阻塞)
*\Return        bool        是否请求成功
*\Note          1)发现互斥后调用func(func_para)
*\Note          2)循环调用1)timeout次
*\Log           2009.12.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool MutexApply(void* addr, u32 len, u32 (*func)(u32), u32 func_para, u32 timeout)
{
    bool timeout_enabled = TRUE;
    u32 i = 0x00;

    if (timeout == 0x00)
    {
        timeout_enabled = FALSE;
    }

    DEAD_LOOP
    {
        i = MutexSearch(i, addr, len);
        if (i == MUTEX_SEARCH_FAILED)
        {
            break;
        }

        if (timeout_enabled == TRUE)
        {
            if (timeout == 0x00)
            {
                return FALSE;
            }

            timeout--;
        }
        func(func_para);
    }


    /*可以申请，查找空位*/
    for (i = 0x00; i < MUTEX_POOL_SIZE; i++)
    {
        if (MutexPool[i].Valid == FALSE)
        {
            MutexPool[i].Obj.Data = addr;
            MutexPool[i].Obj.Len = len;
            MutexPool[i].Valid = TRUE;
            return TRUE;
        }
    }

    /*若找不到空位*/
    return FALSE;
}

/***************************************************************************************************
*\Function      MutexRelease
*\Description   互斥释放
*\Parameter     addr    释放地址
*\Return        bool    是否存在互斥
*\Log           2009.12.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool MutexRelease(void* addr)
{
    u32 index = 0x00;

    index = MutexSearch(0, addr, 1);

    if (index != MUTEX_SEARCH_FAILED)
    {
        MutexPool[index].Obj.Data = NULL;
        MutexPool[index].Obj.Len = 0x00;
        MutexPool[index].Valid = FALSE;
        return TRUE;
    }

    return FALSE;
}


