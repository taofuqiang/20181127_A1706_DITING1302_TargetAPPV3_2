/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_conversion.c
*\Description   ����ת��ģ�飬Ϊ��ͬ���ơ���ͬ���͡���ͬ�ṹ�ȶ����ṩ�໥ת������
*\Note          ����Map:�����������ַ���֮����ʾֵ�����ת��
*\Log           2008.6.24   Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#include "usdl_string.h"
#include "usdl_tools.h"
#include "usdl_io.h"
#include "usdl_conversion.h"

const u8 HexList[] = "0123456789ABCDEF";

/***************************************************************************************************
*\Function      BCDToDECU8
*\Description   U8BCDתΪDEC
*\Parameter     bcd BCDֵ��ָ�룩
*\Return        void
*\Log           2008.11.17    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void BCDToDECU8(u8* bcd)
{
    *bcd = (((*bcd) >> 4) & 0x0F) * 10 + ((*bcd)&0x0F);
}

/***************************************************************************************************
*\Function      BCDToDECU16
*\Description   U16BCDתΪDEC
*\Parameter     bcd BCDֵ��ָ�룩
*\Return        void
*\Log           2008.12.09    Ver 1.0    �Ų�
*               ����������
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
*\Description   U32BCDתΪDEC
*\Parameter     bcd BCDֵ��ָ�룩
*\Return        void
*\Log           2008.12.09    Ver 1.0    �Ų�
*               ����������
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
*\Description   U64BCDתΪDEC
*\Parameter     bcd BCDֵ��ָ�룩
*\Return        void
*\Log           2008.12.09    Ver 1.0    �Ų�
*               ����������
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
*\Description   BCDu8תΪDEC
*\Parameter     bcd BCDֵ
*\Return        u8  DECֵ
*\Log           2008.6.24   Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 GetBCDToDECU8(const u8 bcd)
{
    u8 dec = 0x00;

    dec = ((bcd >> 4) & 0x0F) * 10 + (bcd & 0x0F);

    return dec;
}

/***************************************************************************************************
*\Function      GetBCDToDECU16
*\Description   BCDu16תΪDEC
*\Parameter     bcd BCDֵ
*\Return        u16 DECֵ
*\Log           2009.08.28    Ver 1.0    �Ų�
*               ����������
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
*\Description   BCDu32תΪDEC
*\Parameter     bcd BCDֵ
*\Return        u32 DECֵ
*\Log           2009.08.28    Ver 1.0    �Ų�
*               ����������
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
*\Description   BCDu64תΪDEC
*\Parameter     bcd BCDֵ
*\Return        u64 DECֵ
*\Log           2009.08.28    Ver 1.0    �Ų�
*               ����������
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
*\Description   DECתΪBCD
*\Parameter     dec DECֵ��ָ�룩
*\Return        void
*\Log           2008.11.17    Ver 1.0    �Ų�
*               ����������
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
*\Description   U16BCDתΪDEC
*\Parameter     dec DECֵ��ָ�룩
*\Return        void
*\Log           2008.12.09    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void DECToBCDU16(u16* dec)
{
    DECToBCD(dec, sizeof(*dec), *dec);
}

/***************************************************************************************************
*\Function      DECToBCDU32
*\Description   U32DECתΪBCD
*\Parameter     dec DECֵ��ָ�룩
*\Return        void
*\Log           2008.12.09    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void DECToBCDU32(u32* dec)
{
    DECToBCD(dec, sizeof(*dec), *dec);
}

/***************************************************************************************************
*\Function      DECToBCDU64
*\Description   U64DECתΪBCD
*\Parameter     dec DECֵ��ָ�룩
*\Return        void
*\Log           2008.12.09    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void DECToBCDU64(u64* dec)
{
    DECToBCD(dec, sizeof(*dec), *dec);
}

/***************************************************************************************************
*\Function      GetDECToBCDU8
*\Description   DECu8תΪBCD
*\Parameter     dec DECֵ
*\Return        u8  BCDֵ
*\Log           2008.6.24   Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 GetDECToBCDU8(const u8 dec)
{
    DECToBCD((void*)&dec, sizeof(dec), dec);
    return dec;
}

/***************************************************************************************************
*\Function      GetDECToBCDU16
*\Description   BCDu16תΪDEC
*\Parameter     bcd BCDֵ
*\Return        u16 DECֵ
*\Log           2009.08.28    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u16 GetDECToBCDU16(const u16 dec)
{
    DECToBCD((void*)&dec, sizeof(dec), dec);
    return dec;
}

/***************************************************************************************************
*\Function      GetDECToBCDU32
*\Description   DECu32תΪBCD
*\Parameter     bcd decֵ
*\Return        u32 BCDֵ
*\Log           2009.08.28    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 GetDECToBCDU32(const u32 dec)
{
    DECToBCD((void*)&dec, sizeof(dec), dec);
    return dec;
}

/***************************************************************************************************
*\Function      GetDECToBCDU64
*\Description   DECu64תΪBCD
*\Parameter     dec DECֵ
*\Return        u64 BCDֵ
*\Log           2009.08.28    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u64 GetDECToBCDU64(const u64 dec)
{
    DECToBCD((void*)&dec, sizeof(dec), dec);
    return dec;
}

/***************************************************************************************************
*\Function      DECToBCD
*\Description   DECתΪBCD
*\Parameter     dst BCDֵ�洢�ռ�
*\Parameter     len �ռ䳤��
*\Parameter     dec DECֵ
*\Return        u8* dst + ʵ�ʴ洢����
*\Log           2008.12.10    Ver 1.0    �Ų�
*               ����������
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
*\Description   ���BCD���Ƿ���ȷ
*\Parameter     bcd     Ҫ����BCD��
*\Return        bool    �����
*\Log           2008.03.05    Ver 1.0    �Ų�
*               ����������
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
*\Description   �������תΪBCD���DEC���Ƿ���ȷ
*\Parameter     dec     DECֵ
*\Return        bool    �����
*\Note          �ж�dec�Ƿ�С��99
*\Log           2008.10.08    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool CheckDECToBCD(u8 dec)
{
    return (bool)(dec <= 99);
}

/***************************************************************************************************
*\Function      BCDToXS3
*\Description   ��BCD��תΪ������(Excess-3)
*\Parameter     bcd DECֵ
*\Return        u8  ������
*\Log           2008.02.18    Ver 1.0    �Ų�
*               ����������
*               2008.10.06      Ver 1.1    �Ų�
*                ����, ԭ��ThreeCode��
***************************************************************************************************/
u8 BCDToXS3(u8 bcd)
{
    return bcd + 0x33;
}

/***************************************************************************************************
*\Function      XS3ToBCD
*\Description   ��������(Excess-3)תΪBCD��
*\Parameter     xs3 ������
*\Return        u8    DECֵ
*\Log           2008.02.18    Ver 1.0    �Ų�
*               ����������
*               2008.10.06      Ver 1.1    �Ų�
*                ����, ԭ��UnThreeCode��
***************************************************************************************************/
u8 XS3ToBCD(u8 xs3)
{
    return xs3 - 0x33;
}

/***************************************************************************************************
*\Function      ByteMapChar
*\Description   ��ָ�ֽ�ת��Ϊ16������ʽ����0��β���ַ���
*\Parameter     str     ���16�����ַ����Ļ�����
*\Parameter     byte    �ֽ�
*\Return        void
*\Log           2008.9.19   Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void ByteMapChar(u8* str, u8 byte)
{
    str[0] = HexList[byte / 16];
    str[1] = HexList[byte % 16];
    str[2] = 0x00;
}

/***************************************************************************************************
*\Function      BytesMapChar
*\Description   ��ָ�����ȵ��ֽڴ����תΪ16�����ַ������û�����
*\Parameter     bytes   �ֽڴ�
*\Parameter     len     �ֽڴ�����
*\Parameter     func    �û�����
*\Return        void
*\Note
*\Log           2010.06.18    Ver 1.0    �Ų�
*               ����������
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
*\Description   ������������bufת��Ϊ��seperator�ָ��16���Ʊ�ʾ���ַ�����
*\Parameter     str             Ŀ���ַ���������
*\Parameter     max_str_len     Ŀ�껺��������
*\Parameter     buf             Դ������
*\Parameter     buf_len         Դ����������
*\Parameter     seperator       �ָ��ַ���, NULL��ʾû�зָ
*\Return        u32             ת������ַ�������
*\Log           2010.03.26    Ver 1.0    �Ų�
*               ����������
*               2011.05.03    Ver 1.1    �Ų�
*               ��PrintHex�ϲ��Ż���
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
    /*��û�зָ�������ʹ�ÿ���ת������*/
    if (seperator == NULL)
    {
        /*����Ƿ񳬳�Ŀ�껺��������*/
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
    /*�зָ����ʹ��Ч�ʽϵ͵�ת������*/
    trunk_len = 2 + StrLen((u8*)seperator);
    /*����Ƿ񳬳�Ŀ�껺��������*/
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
    /*ȥ�����һ���ָ��*/
    if (buf_len > 0)
    {
        str_len -= StrLen((u8*)seperator);
    }
    /*�ַ�����0x00��β*/
    str[str_len] = 0x00;
    return str_len;
}

/***************************************************************************************************
*\Function      HexStrToBytes
*\Description   HEX�ַ���ת��Ϊ�ֽ�����
*\Parameter     buf        ������ָ��
*\Parameter     buf_len    ����������
*\Parameter     hex         HEX�ַ���
*\Parameter     skip_invalid    �Ƿ������Ƿ��ַ�(��0~9, a~f)
*\Return        u32     ��ת�����ֽ����鳤�ȣ���skip_invalidΪFALSE����hex�ַ����������Ƿ��ַ�
                        �����̷���0.
*\Note          �硰12345678����ת��Ϊ0x12 0x34 0x56 0x78���ηŵ�buf��
*\Log           2010.11.10    Ver 1.0    �Ų�
*               ����������
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
*\Description   ��BCD��ʽ���ַ���ת��ΪBCD��
*\Parameter     str
*\Parameter     bcd
*\Return        u32
*\Note          �硰12345678����ת��Ϊ0x12 0x34 0x56 0x78���ηŵ�buf��
*\Log           2010.04.15    Ver 1.0    �Ų�
*               ����������
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
*�ַ���������ת��
***************************************************************************************************/

/***************************************************************************************************
*\Function      AToI
*\Description
*\Parameter     str
*\Return        s64
*\Note
*\Log           2010.12.15    Ver 1.0    �Ų�
*               ����������
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
*\Description   ��������ת�ַ���
*\Parameter     val     ֵ
*\Parameter     buf     �ַ���
*\Parameter     radix   ����ֵ
*\Parameter     is_neg  ���ű�־
*\Return        void
*\Note
*\Log           2010.12.15    Ver 1.0    �Ų�
*               ����������
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
*\Description   ����ת�ַ���
*\Parameter     val     ֵ
*\Parameter     buf     �ַ���
*\Parameter     radix   ����ֵ
*\Return        u8*     �ַ���������
*\Note
*\Log           2010.12.15    Ver 1.0    �Ų�
*               ����������
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
*\Description   �޷�����ת�ַ���
*\Parameter     val     ֵ
*\Parameter     buf     �ַ���
*\Parameter     radix   ����ֵ
*\Return        u8*     �ַ���������
*\Note
*\Log           2010.12.15    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* UToA(s64 val, u8* buf, s32 radix)
{
    XToA(val, buf, radix, 0);
    return buf;
}

/***************************************************************************************************
*\Function      SToA
*\Description   �з�����ת�ַ���
*\Parameter     val     ֵ
*\Parameter     buf     �ַ���
*\Parameter     radix   ����ֵ
*\Return        u8*     �ַ���������
*\Note
*\Log           2010.12.15    Ver 1.0    �Ų�
*               ����������
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
*\Log           2010.12.20    Ver 1.0    �Ų�
*               ����������
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
*\Description   �ַ���ת���ݡ�
*\Parameter     nptr
*\Parameter     endptr
*\Parameter     ibase
*\Return        s32
*\Note          1)��nptrָ����ַ��������ݲ���base����Ȩת��Ϊs32, Ȼ�󷵻����ֵ��
*\Note          2)����base�ķ�ΧΪ2~36,��0�����������ַ����Ա�ת��Ϊ������Ȩֵ��
*\Log           2010.12.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
s32 StrToL(const u8* nptr, const u8** endptr, s32 ibase)
{
    return (s32)StrToXL(nptr, endptr, ibase, 0);
}

/***************************************************************************************************
*\Function      StrToUL
*\Description   �ַ���ת�޷������ݡ�
*\Parameter     nptr
*\Parameter     endptr
*\Parameter     ibase
*\Return        u32
*\Note
*\Log           2010.12.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 StrToUL(const u8* nptr, const u8** endptr, s32 ibase)
{
    return StrToXL(nptr, endptr, ibase, FL_UNSIGNED);
}
