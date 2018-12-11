/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_tools.h
*\Description   工具模块，提供丰富的工具函数。
*\Log           2007.12.18   Ver 1.0    张波
*               创建文件。
***************************************************************************************************/

#ifndef _USDL_TOOLS_H
#define _USDL_TOOLS_H

#include "usdl_type.h"
#include "usdl_string.h"


#define CODEFILE ((StrRChr(__FILE__, '\\') == NULL) ? \
    ((u8*)__FILE__) : (StrRChr(__FILE__, '\\') + 1))    /*!<获取当前文件名,不含路径*/


#define CODELINE (__LINE__) /*!<获取当前代码所在行*/

#define CURDATETIME (__DATE__" "__TIME__)   /*!<当前编译日期时间*/

#define CTYPE_U     0x01    /*!<upper*/
#define CTYPE_L     0x02    /*!<lower*/
#define CTYPE_D     0x04    /*!<digit*/
#define CTYPE_C     0x08    /*!<cntrl*/
#define CTYPE_P     0x10    /*!<punct*/
#define CTYPE_S     0x20    /*!<white space (space/lf/tab)*/
#define CTYPE_X     0x40    /*!<hex digit*/
#define CTYPE_SP    0x80    /*!<hard space (0x20)*/

extern const u8 CType[];

#define IS_MASK(x)      (CType[(s32)(u8)(x)])
#define IS_ALNUM(c)     ((IS_MASK(c)&(CTYPE_U|CTYPE_L|CTYPE_D)) != 0)   /*!<判断字符变量c是否为字母或数字*/
#define IS_ALPHA(c)     ((IS_MASK(c)&(CTYPE_U|CTYPE_L)) != 0)   /*!<判断字符变量c是否为字母*/
#define IS_CNTRL(c)     ((IS_MASK(c)&(CTYPE_C)) != 0)   /*!<判断字符变量c是否为控制字符*/
#define IS_DIGIT(c)     ((IS_MASK(c)&(CTYPE_D)) != 0)   /*!<判断字符变量c是否为数字*/
#define IS_GRAPH(c)     ((IS_MASK(c)&(CTYPE_P|CTYPE_U|CTYPE_L|CTYPE_D)) != 0)
#define IS_LOWER(c)     ((IS_MASK(c)&(CTYPE_L)) != 0)   /*!<判断字符变量c是否为小写字母*/
#define IS_PRINT(c)     ((IS_MASK(c)&(CTYPE_P|CTYPE_U|CTYPE_L|CTYPE_D|CTYPE_SP)) != 0)
#define IS_PUNCT(c)     ((IS_MASK(c)&(CTYPE_P)) != 0)
#define IS_SPACE(c)     ((IS_MASK(c)&(CTYPE_S)) != 0)   /*!<判断字符变量c是否为空格*/
#define IS_UPPER(c)     ((IS_MASK(c)&(CTYPE_U)) != 0)   /*!<判断字符变量c是否为大小字母*/
#define IS_XDIGIT(c)    ((IS_MASK(c)&(CTYPE_D|CTYPE_X)) != 0)
#define IS_ASCII(c)     (((u8)(c))<=0x7f)       /*!<判断字符变量c是否为ASCII码*/
#define TO_ASCII(c)     (((u8)(c))&0x7f)        /*!<把字符变量c转换为ASCII码*/
#define TO_UPPER(c)     (IS_LOWER(c) ? ((c) - 'a' + 'A') : (c)) /*!<把字符变量c转换为大写字母*/

extern u64 MemU64DoubleInvalid;

#define INVALID_DOUBLE  (*(double*)&MemU64DoubleInvalid)  /*!<无效double值*/ 

/***************************************************************************************************
*\Def           MinBlockSize
*\Description   计算可以容纳data_size字节数据的最小空间，且该空间可被block_size整除
*\Parameter     data_size   数据大小
*\Parameter     block_size  块大小
*\Return        u32         所需大小
*\Log           2011.11.02    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
#define MinBlockSize(data_size, block_size) ((((data_size) / (block_size))\
+ (((data_size) % (block_size)) > 0)) * (block_size))

/***************************************************************************************************
*\Def           Swap
*\Description   交换两个参数的值
*\Parameter     x   参数x
*\Parameter     y   参数y
*\Return        u8
*\Log           2008.10.06    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
#define Swap(a, b) {(a)^=(b); (b)^=(a); (a)^=(b);}

/***************************************************************************************************
*\Def           DoubleMem
*\Description   double的内存值
*\Parameter     var   参数var
*\Return        double的内存值
*\Log           2008.10.06    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
#define DoubleMem(var) (*(u64*)(&(var)))

/***************************************************************************************************
*\Def           DoubleSetInvalid
*\Description   将double赋值为非法值
*\Parameter     var   参数var
*\Return        非法的double值
*\Log           2008.10.06    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
#define DoubleSetInvalid(var) (*(u64*)(&(var)) = ~(0ULL))

/***************************************************************************************************
*\Def           DoubleIsNumber
*\Description   判断double的合法性
*\Parameter     var   参数var
*\Return        TRUE:合法的double值，FALSE:非法的double值
*\Log           2008.10.06    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
#define DoubleIsNumber(value) ((value) == (value))    /*!<判断double的合法性*/ 

/***************************************************************************************************
*\Function      GetSub
*\Description   取两数差的绝对值
*\Parameter     value1
*\Parameter     value2
*\Return        u32
*\Log           2008.01.17    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 GetSub(u32 value1, u32 value2);

/***************************************************************************************************
*\Function      InRange
*\Description   whether value in [begin, end]
*\Parameter     value
*\Parameter     begin
*\Parameter     end
*\Return        bool
*\Log           2008.11.25    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool InRange(u32 value, u32 begin, u32 end);

/***************************************************************************************************
*\Function      BCDAdd
*\Description   BCD加运算
*\Parameter     bcd     BCD
*\Parameter     value   加数DEC
*\Return        u64      结果BCD
*\Note          1)若BCD错误则停止计算并返回参数bcd。
*\Log           2009.04.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u64 BCDAdd(u64 bcd, u64 value);

/***************************************************************************************************
*\Function      BCDSub
*\Description   BCD减运算
*\Parameter     bcd     BCD
*\Parameter     value   加数DEC
*\Return        u64      结果BCD
*\Note          1)若BCD错误则停止计算并返回参数bcd。
*\Log           2009.04.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u64 BCDSub(u64 bcd, u64 value);

/***************************************************************************************************
*\Function      BCDFix
*\Description   BCD修正
*\Parameter     bcd     数据指针
*\Return        bool    是否被修正
*\Note          1)修正方法：大于0x99修正为0x00；高位合法、低位大于0x09修正为高位进1，低位为0。
*\Log           2009.04.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool BCDFix(u8* bcd);

/***************************************************************************************************
*\Function      BCDArrayFix
*\Description   BCD数组修正
*\Parameter     bcd     BCD数组指针
*\Return        bool    是否被修正
*\Note          1)修正方法：大于0x99修正为0x00；高位合法、低位大于0x09修正为高位进1，低位为0。
*\Log           2009.04.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool BCDArrayFix(void* bcd_array, u32 len);

/***************************************************************************************************
*\Function      BCDLeftMov
*\Description   BCD数值左移。
*\Parameter     bcd_array   BCD字节数组
*\Parameter     len         BCD字节数组长度
*\Parameter     mov_num     左移数值个数
*\Return        void
*\Note          1)len为字节长度，包含数值个数为len * 2。
*\Note          2)本函数不检查BCD正确性。
*\Log           2010.07.14    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void BCDLeftMov(void* bcd_array, u32 len, u32 mov_num);

/***************************************************************************************************
*\Function      BCDRightMov
*\Description   BCD数值右移。
*\Parameter     bcd_array   BCD字节数组
*\Parameter     len         BCD字节数组长度
*\Parameter     mov_num     右移数值个数
*\Return        void
*\Note          1)len为字节长度，包含数值个数为len * 2。
*\Note          2)本函数不检查BCD正确性。
*\Log           2010.07.14    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void BCDRightMov(void* bcd_array, u32 len, u32 mov_num);

/***************************************************************************************************
*\Function      BCDLeftMovU8
*\Description   BCD数值左移。
*\Parameter     bcd         用来存放BCD码的u8整数，里边包含2个BCD码
*\Parameter     mov_num     右移数值个数
*\Return        void
*\Note          1)本函数不检查BCD正确性。
*\Log           2010.07.14    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8 BCDLeftMovU8(u8 bcd, u32 mov_num);

/***************************************************************************************************
*\Function      BCDLeftMovU16
*\Description   BCD数值左移。
*\Parameter     bcd         用来存放BCD码的u16整数，里边包含4个BCD码
*\Parameter     mov_num     右移数值个数
*\Return        void
*\Note          1)本函数不检查BCD正确性。
*\Log           2010.07.14    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u16 BCDLeftMovU16(u16 bcd, u32 mov_num);

/***************************************************************************************************
*\Function      BCDLeftMovU32
*\Description   BCD数值左移。
*\Parameter     bcd         用来存放BCD码的u32整数，里边包含8个BCD码
*\Return        void
*\Note          1)本函数不检查BCD正确性。
*\Log           2010.07.14    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 BCDLeftMovU32(u32 bcd, u32 mov_num);

/***************************************************************************************************
*\Function      BCDLeftMovU64
*\Description   BCD数值左移。
*\Parameter     bcd         用来存放BCD码的u64整数，里边包含16个BCD码
*\Parameter     mov_num     右移数值个数
*\Return        void
*\Note          1)本函数不检查BCD正确性。
*\Log           2010.07.14    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u64 BCDLeftMovU64(u64 bcd, u32 mov_num);

/***************************************************************************************************
*\Function      BCDRightMovU8
*\Description   BCD数值右移。
*\Parameter     bcd         用来存放BCD码的u8整数，里边包含2个BCD码
*\Parameter     mov_num     右移数值个数
*\Return        void
*\Note          1)本函数不检查BCD正确性。
*\Log           2010.07.14    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8 BCDRightMovU8(u8 bcd, u32 mov_num);

/***************************************************************************************************
*\Function      BCDRightMovU16
*\Description   BCD数值右移。
*\Parameter     bcd         用来存放BCD码的u16整数，里边包含4个BCD码
*\Parameter     mov_num     右移数值个数
*\Return        void
*\Note          1)本函数不检查BCD正确性。
*\Log           2010.07.14    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u16 BCDRightMovU16(u16 bcd, u32 mov_num);

/***************************************************************************************************
*\Function      BCDRightMovU32
*\Description   BCD数值右移。
*\Parameter     bcd         用来存放BCD码的u32整数，里边包含8个BCD码
*\Parameter     mov_num     右移数值个数
*\Return        void
*\Note          1)本函数不检查BCD正确性。
*\Log           2010.07.14    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 BCDRightMovU32(u32 bcd, u32 mov_num);

/***************************************************************************************************
*\Function      BCDRightMovU64
*\Description   BCD数值右移。
*\Parameter     bcd         用来存放BCD码的u68整数，里边包含16个BCD码
*\Parameter     mov_num     右移数值个数
*\Return        void
*\Note          1)本函数不检查BCD正确性。
*\Log           2010.07.14    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u64 BCDRightMovU64(u64 bcd, u32 mov_num);

/***************************************************************************************************
*\Function      WaitFlag
*\Description   等待标志
*\Parameter     flag        等待标志
*\Parameter     flag_width  等待数据宽度
*\Parameter     value       等待值
*\Parameter     wait        等待函数
*\Parameter     timeout     0表示无条件等待。
*\Return        bool        是否等到
*\Note          1)该函数适用于操作系统环境中多任务使用。
*\Note          2)支持奇地址访问。
*\Log           2010.01.07    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool WaitFlag(const void* flag, const u8 flag_width, const u32 value, u32 (*wait)(u32), const u32 timeout);

#endif /*_USDL_TOOLS_H*/
