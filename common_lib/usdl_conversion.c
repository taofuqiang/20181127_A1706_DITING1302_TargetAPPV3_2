/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_conversion.c
*\Description   数据转换模块，为不同进制、不同类型、不同结构等对象提供相互转换函数
*\Note          术语Map:用于数据与字符串之间显示值不变的转换
*\Log           2008.6.24   Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#include "usdl_string.h"
#include "usdl_tools.h"
#include "usdl_io.h"
#include "usdl_conversion.h"

const u8 HexList[] = "0123456789ABCDEF";

/***************************************************************************************************
*\Function      BCDToDECU8
*\Description   U8BCD转为DEC
*\Parameter     bcd BCD值（指针）
*\Return        void
*\Log           2008.11.17    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void BCDToDECU8(u8* bcd)
{
    *bcd = (((*bcd) >> 4) & 0x0F) * 10 + ((*bcd)&0x0F);
}

/***************************************************************************************************
*\Function      BCDToDECU16
*\Description   U16BCD转为DEC
*\Parameter     bcd BCD值（指针）
*\Return        void
*\Log           2008.12.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void BCDToDECU16(u16* bcd)
{
    *bcd =
        (((*bcd) >> 12) & 0x0F) * 1000 +
        (((*bcd) >> 8) & 0x0F) * 100 +
        (((*bcd) >> 4) & 0x0F) * 10 +
        ((*bcd) & 0x0F);
}

/***************************************************************************************************
*\Function      BCDToDECU32
*\Description   U32BCD转为DEC
*\Parameter     bcd BCD值（指针）
*\Return        void
*\Log           2008.12.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void BCDToDECU32(u32* bcd)
{
    *bcd =
        (((*bcd) >> 28) & 0x0F) * 10000000 +
        (((*bcd) >> 24) & 0x0F) * 1000000 +
        (((*bcd) >> 20) & 0x0F) * 100000 +
        (((*bcd) >> 16) & 0x0F) * 10000 +
        (((*bcd) >> 12) & 0x0F) * 1000 +
        (((*bcd) >> 8)  & 0x0F) * 100 +
        (((*bcd) >> 4)  & 0x0F) * 10 +
        ((*bcd) & 0x0F);
}

/***************************************************************************************************
*\Function      BCDToDECU64
*\Description   U64BCD转为DEC
*\Parameter     bcd BCD值（指针）
*\Return        void
*\Log           2008.12.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void BCDToDECU64(u64* bcd)
{
    *bcd =
        (((*bcd) >> 60) & 0x0F) * 1000000000000000 +
        (((*bcd) >> 56) & 0x0F) * 100000000000000 +
        (((*bcd) >> 52) & 0x0F) * 10000000000000 +
        (((*bcd) >> 48) & 0x0F) * 1000000000000 +
        (((*bcd) >> 44) & 0x0F) * 100000000000 +
        (((*bcd) >> 40) & 0x0F) * 10000000000 +
        (((*bcd) >> 36) & 0x0F) * 1000000000 +
        (((*bcd) >> 32) & 0x0F) * 100000000 +
        (((*bcd) >> 28) & 0x0F) * 10000000 +
        (((*bcd) >> 24) & 0x0F) * 1000000 +
        (((*bcd) >> 20) & 0x0F) * 100000 +
        (((*bcd) >> 16) & 0x0F) * 10000 +
        (((*bcd) >> 12) & 0x0F) * 1000 +
        (((*bcd) >> 8)  & 0x0F) * 100 +
        (((*bcd) >> 4)  & 0x0F) * 10 +
        (((*bcd) >> 0)  & 0x0F) * 1;
}

/***************************************************************************************************
*\Function      GetBCDToDECU8
*\Description   BCDu8转为DEC
*\Parameter     bcd BCD值
*\Return        u8  DEC值
*\Log           2008.6.24   Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8 GetBCDToDECU8(const u8 bcd)
{
    u8 dec = 0x00;

    dec = ((bcd >> 4) & 0x0F) * 10 + (bcd & 0x0F);

    return dec;
}

/***************************************************************************************************
*\Function      GetBCDToDECU16
*\Description   BCDu16转为DEC
*\Parameter     bcd BCD值
*\Return        u16 DEC值
*\Log           2009.08.28    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u16 GetBCDToDECU16(const u16 bcd)
{
    return ((bcd >> 12) & 0x0F) * 1000 +
        ((bcd >> 8) & 0x0F) * 100 +
        ((bcd >> 4) & 0x0F) * 10 +
        (bcd & 0x0F);
}

/***************************************************************************************************
*\Function      GetBCDToDECU32
*\Description   BCDu32转为DEC
*\Parameter     bcd BCD值
*\Return        u32 DEC值
*\Log           2009.08.28    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 GetBCDToDECU32(const u32 bcd)
{
    return ((bcd>>28)&0x0F)*10000000 +
        ((bcd>>24)&0x0F)*1000000 +
        ((bcd>>20)&0x0F)*100000 +
        ((bcd>>16)&0x0F)*10000 +
        ((bcd>>12)&0x0F)*1000 +
        ((bcd>>8) &0x0F)*100 +
        ((bcd>>4) &0x0F)*10 +
        (bcd&0x0F);
}

/***************************************************************************************************
*\Function      GetBCDToDECU64
*\Description   BCDu64转为DEC
*\Parameter     bcd BCD值
*\Return        u64 DEC值
*\Log           2009.08.28    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u64 GetBCDToDECU64(const u64 bcd)
{
    return ((bcd>>60)&0x0F)*1000000000000000 +
        ((bcd>>56)&0x0F)*100000000000000 +
        ((bcd>>52)&0x0F)*10000000000000 +
        ((bcd>>48)&0x0F)*1000000000000 +
        ((bcd>>44)&0x0F)*100000000000 +
        ((bcd>>40)&0x0F)*10000000000 +
        ((bcd>>36)&0x0F)*1000000000 +
        ((bcd>>32)&0x0F)*100000000 +
        ((bcd>>28)&0x0F)*10000000 +
        ((bcd>>24)&0x0F)*1000000 +
        ((bcd>>20)&0x0F)*100000 +
        ((bcd>>16)&0x0F)*10000 +
        ((bcd>>12)&0x0F)*1000 +
        ((bcd>>8) &0x0F)*100 +
        ((bcd>>4) &0x0F)*10 +
        ((bcd>>0) &0x0F)*1;
}

/***************************************************************************************************
*\Function      DECToBCDU8
*\Description   DEC转为BCD
*\Parameter     dec DEC值（指针）
*\Return        void
*\Log           2008.11.17    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void DECToBCDU8(u8* dec)
{
    u8 quotient = 0x00;

    quotient = ((*dec)/10);
    quotient %= 10;
    *dec = (quotient<<4)  + (*dec) - 10 * quotient;
}

/***************************************************************************************************
*\Function      DECToBCDU16
*\Description   U16BCD转为DEC
*\Parameter     dec DEC值（指针）
*\Return        void
*\Log           2008.12.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void DECToBCDU16(u16* dec)
{
    DECToBCD(dec, sizeof(*dec), *dec);
}

/***************************************************************************************************
*\Function      DECToBCDU32
*\Description   U32DEC转为BCD
*\Parameter     dec DEC值（指针）
*\Return        void
*\Log           2008.12.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void DECToBCDU32(u32* dec)
{
    DECToBCD(dec, sizeof(*dec), *dec);
}

/***************************************************************************************************
*\Function      DECToBCDU64
*\Description   U64DEC转为BCD
*\Parameter     dec DEC值（指针）
*\Return        void
*\Log           2008.12.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void DECToBCDU64(u64* dec)
{
    DECToBCD(dec, sizeof(*dec), *dec);
}

/***************************************************************************************************
*\Function      GetDECToBCDU8
*\Description   DECu8转为BCD
*\Parameter     dec DEC值
*\Return        u8  BCD值
*\Log           2008.6.24   Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8 GetDECToBCDU8(const u8 dec)
{
    DECToBCD((void*)&dec, sizeof(dec), dec);
    return dec;
}

/***************************************************************************************************
*\Function      GetDECToBCDU16
*\Description   BCDu16转为DEC
*\Parameter     bcd BCD值
*\Return        u16 DEC值
*\Log           2009.08.28    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u16 GetDECToBCDU16(const u16 dec)
{
    DECToBCD((void*)&dec, sizeof(dec), dec);
    return dec;
}

/***************************************************************************************************
*\Function      GetDECToBCDU32
*\Description   DECu32转为BCD
*\Parameter     bcd dec值
*\Return        u32 BCD值
*\Log           2009.08.28    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 GetDECToBCDU32(const u32 dec)
{
    DECToBCD((void*)&dec, sizeof(dec), dec);
    return dec;
}

/***************************************************************************************************
*\Function      GetDECToBCDU64
*\Description   DECu64转为BCD
*\Parameter     dec DEC值
*\Return        u64 BCD值
*\Log           2009.08.28    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u64 GetDECToBCDU64(const u64 dec)
{
    DECToBCD((void*)&dec, sizeof(dec), dec);
    return dec;
}

/***************************************************************************************************
*\Function      DECToBCD
*\Description   DEC转为BCD
*\Parameter     dst BCD值存储空间
*\Parameter     len 空间长度
*\Parameter     dec DEC值
*\Return        u8* dst + 实际存储长度
*\Log           2008.12.10    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8* DECToBCD(void* dst, u8 len, u64 dec)
{
    u8 index = 0x00;
    u8 h = 0x00;
    u8 l = 0x00;

    MemSet((u8*)dst, 0x00, len);

    while (dec > 0x00)
    {
        l = dec % 10;
        dec /= 10;
        h = dec % 10;
        dec /= 10;
        ((u8*)dst)[index] = (h<<4)|l;
        index++;

        if (index >= len)
        {
            return (u8*)dst + len;
        }
    }

    return (u8*)dst + index;
}

/***************************************************************************************************
*\Function      CheckBCD
*\Description   检查BCD码是否正确
*\Parameter     bcd     要检查的BCD码
*\Return        bool    检查结果
*\Log           2008.03.05    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool CheckBCD(u8 bcd)
{
    if (((((bcd >> 4) & 0x0F)) < 10) && ((bcd & 0x0F) < 10))
    {
        return TRUE;
    }

    return FALSE;
}

/***************************************************************************************************
*\Function      CheckDECToBCD
*\Description   检查用来转为BCD码的DEC码是否正确
*\Parameter     dec     DEC值
*\Return        bool    检查结果
*\Note          判断dec是否小于99
*\Log           2008.10.08    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool CheckDECToBCD(u8 dec)
{
    return (bool)(dec <= 99);
}

/***************************************************************************************************
*\Function      BCDToXS3
*\Description   将BCD码转为余三码(Excess-3)
*\Parameter     bcd DEC值
*\Return        u8  余三码
*\Log           2008.02.18    Ver 1.0    张波
*               创建函数。
*               2008.10.06      Ver 1.1    张波
*                改名, 原名ThreeCode。
***************************************************************************************************/
u8 BCDToXS3(u8 bcd)
{
    return bcd + 0x33;
}

/***************************************************************************************************
*\Function      XS3ToBCD
*\Description   将余三码(Excess-3)转为BCD码
*\Parameter     xs3 余三码
*\Return        u8    DEC值
*\Log           2008.02.18    Ver 1.0    张波
*               创建函数。
*               2008.10.06      Ver 1.1    张波
*                改名, 原名UnThreeCode。
***************************************************************************************************/
u8 XS3ToBCD(u8 xs3)
{
    return xs3 - 0x33;
}

/***************************************************************************************************
*\Function      ByteMapChar
*\Description   将指字节转化为16进制形式的以0结尾的字符串
*\Parameter     str     存放16进制字符串的缓冲区
*\Parameter     byte    字节
*\Return        void
*\Log           2008.9.19   Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void ByteMapChar(u8* str, u8 byte)
{
    str[0] = HexList[byte / 16];
    str[1] = HexList[byte % 16];
    str[2] = 0x00;
}

/***************************************************************************************************
*\Function      BytesMapChar
*\Description   将指定长度的字节串逐个转为16进制字符并由用户处理。
*\Parameter     bytes   字节串
*\Parameter     len     字节串长度
*\Parameter     func    用户函数
*\Return        void
*\Note
*\Log           2010.06.18    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void BytesMapChar(u8* bytes, u32 len, void (*func)(u8))
{
    u16 i = 0x00;
    for(i = 0; i < len; i++)
    {
        func(HexList[bytes[i]/16]);
        func(HexList[bytes[i]%16]);
    }
}

/***************************************************************************************************
*\Function      BytesMapHexStr
*\Description   将定长缓冲区buf转换为以seperator分割的16进制表示的字符串。
*\Parameter     str             目标字符串缓冲区
*\Parameter     max_str_len     目标缓冲区长度
*\Parameter     buf             源缓冲区
*\Parameter     buf_len         源缓冲区长度
*\Parameter     seperator       分隔字符串, NULL表示没有分割。
*\Return        u32             转换后的字符串长度
*\Log           2010.03.26    Ver 1.0    张波
*               创建函数。
*               2011.05.03    Ver 1.1    张波
*               与PrintHex合并优化。
***************************************************************************************************/
u32 BytesMapHexStr(u8* str, u32 max_str_len, u8* buf, u32 buf_len, const u8* seperator)
{
    u32 i;
    u32 trunk_len = 0;
    u32 str_len = 0;

    if ((str == NULL) || (buf == NULL))
    {
        return 0;
    }
    /*若没有分隔符，则使用快速转换方法*/
    if (seperator == NULL)
    {
        /*检查是否超出目标缓冲区长度*/
        if ((buf_len * 2) > max_str_len)
        {
            return 0;
        }
        for(i = 0; i < buf_len; i++)
        {
            str[i*2]   = HexList[buf[i] / 16];
            str[i*2+1] = HexList[buf[i] % 16];
        }
        str[buf_len * 2] = 0x00;
        return buf_len * 2;
    }
    /*有分割符，使用效率较低的转换方法*/
    trunk_len = 2 + StrLen((u8*)seperator);
    /*检查是否超出目标缓冲区长度*/
    if (max_str_len < (buf_len * trunk_len))
    {
        buf_len = max_str_len / trunk_len;
    }
    str_len = 0;
    for (i = 0; i < buf_len; i++)
    {
        Sprintf(str + str_len, "%0*X%s", 2, buf[i], seperator);
        str_len += trunk_len;
    }
    /*去掉最后一个分割符*/
    if (buf_len > 0)
    {
        str_len -= StrLen((u8*)seperator);
    }
    /*字符串以0x00结尾*/
    str[str_len] = 0x00;
    return str_len;
}

/***************************************************************************************************
*\Function      HexStrToBytes
*\Description   HEX字符串转化为字节数组
*\Parameter     buf        缓冲区指针
*\Parameter     buf_len    缓冲区长度
*\Parameter     hex         HEX字符串
*\Parameter     skip_invalid    是否跳过非法字符(非0~9, a~f)
*\Return        u32     所转化的字节数组长度，若skip_invalid为FALSE，在hex字符串中遇到非法字符
                        则立刻返回0.
*\Note          如“12345678”会转化为0x12 0x34 0x56 0x78依次放到buf里
*\Log           2010.11.10    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 HexStrToBytes(u8* buf, u32 buf_len, u8* hex, bool skip_invalid)
{
    u32 str_len = 0;
    bool odd_digit = FALSE;
    u32 i;
    u8 ch;
    u8 tmp;

    str_len = StrLen(hex);
    if (str_len > buf_len * 2)
    {
        str_len = buf_len * 2;
    }

    for (i = 0; i < str_len; ++i)
    {
        ch = hex[i];

        if (ch >= '0' && ch <= '9')
        {
            tmp = ch - '0';
        }
        else if (ch >= 'a' && ch <= 'f')
        {
            tmp = ch - 'a' + 10;
        }
        else if (ch >= 'A' && ch <= 'F')
        {
            tmp = ch - 'A' + 10;
        }
        else
        {
            if (skip_invalid == TRUE)
            {
                continue;
            }
            else
            {
                return 0;
            }
        }

        if (odd_digit)
        {
            *buf |= tmp;
            buf++;
            odd_digit = FALSE;
        }
        else
        {
            *buf = tmp << 4;
            odd_digit = TRUE;
        }
    }
    return (str_len + 1) / 2;
}

/***************************************************************************************************
*\Function      BCDStrToBytes
*\Description   将BCD形式的字符串转化为BCD码
*\Parameter     str
*\Parameter     bcd
*\Return        u32
*\Note          如“12345678”会转化为0x12 0x34 0x56 0x78依次放到buf里
*\Log           2010.04.15    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 BCDStrToBytes(u8* str, u8* bcd, u32 bcd_max_len)
{
    u32 bcd_len;
    u32 bcd_index;
    u8 digit = 0;

    bcd_len = StrLen(str) / 2;
    if (bcd_max_len < bcd_len)
    {
        bcd_len = bcd_max_len;
    }
    for (bcd_index = 0; bcd_index < bcd_len; bcd_index++)
    {
        bcd[bcd_index] = 0;
        digit = str[bcd_index * 2] - '0';
        if (InRange(digit, 0, 9) == FALSE)
        {
            break;
        }
        bcd[bcd_index] = digit << 4;
        digit = str[bcd_index * 2 + 1] - '0';
        if (InRange(digit, 0, 9) == FALSE)
        {
            break;
        }
        bcd[bcd_index] += digit;
    }
    bcd_index--;
    return bcd_index;
}

/***************************************************************************************************
*字符串与数据转换
***************************************************************************************************/

/***************************************************************************************************
*\Function      AToI
*\Description
*\Parameter     str
*\Return        s64
*\Note
*\Log           2010.12.15    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
s64 AToI(const u8* str)
{
    s32 c       = 0x00; /*current char*/
    s64 total   = 0x00; /*current total*/
    s32 sign    = 0x00; /*if '-', then negative, otherwise positive*/

    /*skip whitespace*/
    while (IS_SPACE((s32)(u8)*str))
    {
        ++str;
    }

    c = (s32)(u8)*str++;

    sign = c;           /*save sign indication*/

    if (c == '-' || c == '+')
    {
        c = (s32)(u8)*str++;    /*skip sign*/
    }

    total = 0;

    while (IS_DIGIT(c))
    {
        total = 10 * total + (c - '0');     /*accumulate digit*/
        c = (s32)(u8)*str++;    /*get next char*/
    }

    if (sign == '-')
        return -total;
    else
        return total;   /*return result, negated if necessary*/
}


/***************************************************************************************************
*\Function      X64ToA
*\Description   任意数据转字符串
*\Parameter     val     值
*\Parameter     buf     字符串
*\Parameter     radix   进制值
*\Parameter     is_neg  负号标志
*\Return        void
*\Note
*\Log           2010.12.15    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
static void XToA (u64 val, u8* buf, u32 radix, s32 is_neg)
{
    u8* p;          /*pointer to traverse string*/
    u8* firstdig;   /*pointer to first digit*/
    u8  temp;       /*temp u8*/
    u32 digval;     /*value of digit*/

    p = buf;

    if (is_neg)
    {
        *p++ = '-';         /*negative, so output '-' and negate*/
        val = (u64)(-(s64)val);
    }

    firstdig = p;           /*save pointer to first digit*/

    do
    {
        digval = (u32)(val % radix);
        val /= radix;       /*get next digit*/

        /*convert to ascii and store*/
        if (digval > 9)
            *p++ = (u8)(digval - 10 + 'a');  /*a letter*/
        else
            *p++ = (u8)(digval + '0');       /*a digit*/
    } while (val > 0);

    /*We now have the digit of the number in the buffer, but in reverse
    order.  Thus we reverse them now.*/

    *p-- = '\0';            /*terminate string; p points to last digit*/

    do
    {
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;   /*swap *p and *firstdig*/
        --p;
        ++firstdig;         /*advance to next two digits*/
    } while (firstdig < p); /*repeat until halfway*/
}

/***************************************************************************************************
*\Function      IToA
*\Description   数据转字符串
*\Parameter     val     值
*\Parameter     buf     字符串
*\Parameter     radix   进制值
*\Return        u8*     字符串缓冲区
*\Note
*\Log           2010.12.15    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8* IToA(s64 val, u8* buf, s32 radix)
{
    if (radix == 10 && val < 0)
    {
        XToA(val, buf, radix, 1);
    }
    else
    {
        XToA(val, buf, radix, 0);
    }

    return buf;
}

/***************************************************************************************************
*\Function      UToA
*\Description   无符号数转字符串
*\Parameter     val     值
*\Parameter     buf     字符串
*\Parameter     radix   进制值
*\Return        u8*     字符串缓冲区
*\Note
*\Log           2010.12.15    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8* UToA(s64 val, u8* buf, s32 radix)
{
    XToA(val, buf, radix, 0);
    return buf;
}

/***************************************************************************************************
*\Function      SToA
*\Description   有符号数转字符串
*\Parameter     val     值
*\Parameter     buf     字符串
*\Parameter     radix   进制值
*\Return        u8*     字符串缓冲区
*\Note
*\Log           2010.12.15    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u8* SToA(s64 val, u8* buf, s32 radix)
{
    XToA(val, buf, radix, (radix == 10 && val < 0));
    return buf;
}


/*flag values*/
#define FL_UNSIGNED   1       /*strtoul called*/
#define FL_NEG        2       /*negative sign found*/
#define FL_OVERFLOW   4       /*overflow occured*/
#define FL_READDIGIT  8       /*we've read at least one correct digit*/


/***************************************************************************************************
*\Function      StrToXL
*\Description
*\Parameter     nptr
*\Parameter     endptr
*\Parameter     ibase
*\Parameter     flags
*\Return        u32
*\Note
*\Log           2010.12.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
static u32 StrToXL(const u8* nptr, const u8** endptr, s32 ibase, s32 flags)
{
    const u8* p;
    u8 c;
    u32 number;
    u32 digval;
    u32 maxval;

    p = nptr;                       /*p is our scanning pointer*/
    number = 0;                     /*start with zero*/

    c = *p++;                       /*read u8*/
    while ( IS_SPACE((s32)(u8)c) )
        c = *p++;               /*skip whitespace*/

    if (c == '-')
    {
        flags |= FL_NEG;        /*remember minus sign*/
        c = *p++;
    }
    else if (c == '+')
        c = *p++;               /*skip sign*/

    if (ibase < 0 || ibase == 1 || ibase > 36)
    {
        /*bad base!*/
        if (endptr)
            /*store beginning of string in endptr*/
            *endptr = nptr;
        return 0L;              /*return 0*/
    }
    else if (ibase == 0)
    {
        /*determine base free-lance, based on first two chars of
        string*/
        if (c != '0')
            ibase = 10;
        else if (*p == 'x' || *p == 'X')
            ibase = 16;
        else
            ibase = 8;
    }

    if (ibase == 16)
    {
        /*we might have 0x in front of number; remove if there*/
        if (c == '0' && (*p == 'x' || *p == 'X'))
        {
            ++p;
            c = *p++;       /*advance past prefix*/
        }
    }

    /*if our number exceeds this, we will overflow on multiply*/
    maxval = U32_MAX / ibase;


    for (;;)
    {
        /*exit in middle of loop*/
        /*convert c to value*/
        if ( IS_DIGIT((s32)(u8)c) )
            digval = c - '0';
        else if ( IS_ALPHA((s32)(u8)c) )
            digval = TO_UPPER(c) - 'A' + 10;
        else
            break;
        if (digval >= (u32)ibase)
            break;  /*exit loop if bad digit found*/

        /*record the fact we have read one digit*/
        flags |= FL_READDIGIT;

        /*we now need to compute number = number * base + digval,
        but we need to know if overflow occured.  This requires
        a tricky pre-check.*/

        if (number < maxval || (number == maxval &&
            (u32)digval <= U32_MAX % ibase))
        {
                /*we won't overflow, go ahead and multiply*/
                number = number * ibase + digval;
        }
        else
        {
            /*we would have overflowed -- set the overflow flag*/
            flags |= FL_OVERFLOW;
        }

        c = *p++;               /*read next digit*/
    }

    --p;                            /*point to place that stopped scan*/

    if (!(flags & FL_READDIGIT)) {
        /*no number there; return 0 and point to beginning of
        string*/
        if (endptr)
            /*store beginning of string in endptr later on*/
            p = nptr;
        number = 0L;            /*return 0*/
    }
    else if ( (flags & FL_OVERFLOW) ||
        ( !(flags & FL_UNSIGNED) &&
        ( ( (flags & FL_NEG) && (number > (u32)S32_MIN) ) ||
        ( !(flags & FL_NEG) && (number > S32_MAX) ) ) ) )
    {
        /*overflow or signed overflow occurred*/
        if ( flags & FL_UNSIGNED )
            number = U32_MAX;
        else if ( flags & FL_NEG )
            number = (u32)S32_MIN;
        else
            number = S32_MAX;
    }

    if (endptr != NULL)
        /*store pointer to u8 that stopped the scan*/
        *endptr = p;

    if (flags & FL_NEG)
        /*negate result if there was a neg sign*/
        number = (u32)(-(s32)number);

    return number;                  /*done.*/
}

/***************************************************************************************************
*\Function      StrToL
*\Description   字符串转数据。
*\Parameter     nptr
*\Parameter     endptr
*\Parameter     ibase
*\Return        s32
*\Note          1)将nptr指向的字符串，根据参数base，按权转化为s32, 然后返回这个值。
*\Note          2)参数base的范围为2~36,和0；它决定了字符串以被转换为整数的权值。
*\Log           2010.12.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
s32 StrToL(const u8* nptr, const u8** endptr, s32 ibase)
{
    return (s32)StrToXL(nptr, endptr, ibase, 0);
}

/***************************************************************************************************
*\Function      StrToUL
*\Description   字符串转无符号数据。
*\Parameter     nptr
*\Parameter     endptr
*\Parameter     ibase
*\Return        u32
*\Note
*\Log           2010.12.20    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 StrToUL(const u8* nptr, const u8** endptr, s32 ibase)
{
    return StrToXL(nptr, endptr, ibase, FL_UNSIGNED);
}
