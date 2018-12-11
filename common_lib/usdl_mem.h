/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_mem.h
*\Description   内存操作模块，对C标准内存函数进行扩充。
*\Log           2008.06.24    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/

#ifndef _USDL_MEM_H
#define _USDL_MEM_H

#include "usdl_type.h"

typedef struct
{
    u8* Data;
    u32 Len;
}DataNode;

/***************************************************************************************************
*内存互斥
***************************************************************************************************/

typedef struct MutexType_st
{
    DataNode    Obj;
    bool        Valid;
}MutexType;

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
void MemAdd(u8* ptr, const u8 value, u32 length);

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
void MemSub(u8* ptr, const u8 value, u32 length);

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
void MemAnd(u8* ptr, const u8 value, u32 length);

/***************************************************************************************************
*\Function      MemNot
*\Description   ptr指向的内存区域的length个字节取反。
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemNot(u8* ptr, const u32 length);

/***************************************************************************************************
*\Function      MemBitNot
*\Description   ptr指向的内存区域的length个字节按位取反。
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemBitNot(u8* ptr, const u32 length);

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
void MemOr(u8* ptr, const u8 value, const u32 length);

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
void MemXor(u8* ptr, const u8 value, const u32 length);

/***************************************************************************************************
*\Function      MemSum
*\Description   返回内存区域ptr中length个字节的和
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        u32
*\Log           2008.03.21    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 MemSum(const u8* ptr, const u32 length);

/***************************************************************************************************
*\Function      MemAllAnd
*\Description   返回内存区域ptr中length个字节的And
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        u32
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 MemAllAnd(const u8* ptr, const u32 length);

/***************************************************************************************************
*\Function      MemAllOr
*\Description   返回内存区域ptr中length个字节的Or
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        u32
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 MemAllOr(const u8* ptr, const u32 length);

/***************************************************************************************************
*\Function      MemAllXor
*\Description   返回内存区域ptr中length个字节的Xor
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        u32
*\Log           2009.10.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 MemAllXor(const u8* ptr, const u32 length);

/***************************************************************************************************
*\Function      MemReverse
*\Description   ptr指向的内存区域中的length个字节逆序
*\Parameter     ptr     指向内存区域的指针
*\Parameter     length  内存区域的长度
*\Return        void
*\Log           2008.06.24    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void MemReverse(void* ptr, const u32 length);

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
void MemReverseCpy(u8* dst, const u8* src, u32 len);

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
u8* MemLineToCycle(u8* dst, const u32 dst_size, u32 start_offset, u8* src, u32 len);

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
u32 MemCycleToLine(u8* dst, const u8* src, const u32 start_pos, const u32 end_pos, const u32 src_size);

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
                      const u32 src_size, const u32 request_len);

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
bool MemMatch(u8* pat, const u32 pat_len, u8* dst, const u32 dst_len, const u8 wildcard, DataNode result[]);

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
u8* MemMem(u8* b1, const u32 len1, u8* b2, const u32 len2);

/***************************************************************************************************
*\Function      MemBCDCheck
*\Description   检查内存区域src中len个字节是否均为BCD码。
*\Parameter     src
*\Parameter     head_len
*\Return        bool
*\Log           2008.12.02    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool MemBCDCheck(const u8* src, const u32 head_len);

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
void MemShiftR(u8* src, const u32 src_len, u32 shift_len);

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
bool MemAllSameValue(const u8* src, u16 len, const u8 value);

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
bool MemAllSame(u8* src, u32 len);

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
void MemAddMem(u8* dst, u8* src1, u8* src2, u32 len);

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
void MemSubMem(u8* dst, u8* src1, u8* src2, u32 len);

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
void MemAndMem(u8* dst, u8* src1, u8* src2, u32 len);

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
void MemOrMem(u8* dst, u8* src1, u8* src2, u32 len);

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
void MemXorMem(u8* dst, u8* src1, u8* src2, u32 len);

/***************************************************************************************************
*内存互斥
***************************************************************************************************/

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
bool MutexApply(void* addr, u32 len, u32 (*func)(u32), u32 func_para, u32 timeout);

/***************************************************************************************************
*\Function      MutexRelease
*\Description   互斥释放
*\Parameter     addr    释放地址
*\Return        bool    是否存在互斥
*\Log           2009.12.12    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool MutexRelease(void* addr);

#endif /*_USDL_MEM_H*/
