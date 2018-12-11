/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_tools.c
*\Description   ����ģ�飬�ṩ�ḻ�Ĺ��ߺ�����
*\Log           2007.12.18   Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/

#include "usdl_conversion.h"
#include "usdl_data.h"
#include "usdl_tools.h"

const u8 CType[] =
{
    CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C,                                     /*0-7*/
    CTYPE_C, CTYPE_C|CTYPE_S, CTYPE_C|CTYPE_S, CTYPE_C|CTYPE_S, CTYPE_C|CTYPE_S, CTYPE_C|CTYPE_S, CTYPE_C, CTYPE_C,                         /*8-15*/
    CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C,                                     /*16-23*/
    CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C, CTYPE_C,                                     /*24-31*/
    CTYPE_S|CTYPE_SP, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P,                                 /*32-39*/
    CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P,                                     /*40-47*/
    CTYPE_D, CTYPE_D, CTYPE_D, CTYPE_D, CTYPE_D, CTYPE_D, CTYPE_D, CTYPE_D,                                     /*48-55*/
    CTYPE_D, CTYPE_D, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P,                                     /*56-63*/
    CTYPE_P, CTYPE_U|CTYPE_X, CTYPE_U|CTYPE_X, CTYPE_U|CTYPE_X, CTYPE_U|CTYPE_X, CTYPE_U|CTYPE_X, CTYPE_U|CTYPE_X, CTYPE_U,                     /*64-71*/
    CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U,                                     /*72-79*/
    CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U,                                     /*80-87*/
    CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P,                                     /*88-95*/
    CTYPE_P, CTYPE_L|CTYPE_X, CTYPE_L|CTYPE_X, CTYPE_L|CTYPE_X, CTYPE_L|CTYPE_X, CTYPE_L|CTYPE_X, CTYPE_L|CTYPE_X, CTYPE_L,                     /*96-103*/
    CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L,                                     /*104-111*/
    CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L,                                     /*112-119*/
    CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_C,                                     /*120-127*/
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     /*128-143*/
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                     /*144-159*/
    CTYPE_S|CTYPE_SP, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, /*160-175*/
    CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P, CTYPE_P,     /*176-191*/
    CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U,     /*192-207*/
    CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_P, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_U, CTYPE_L,     /*208-223*/
    CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L,     /*224-239*/
    CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_P, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L, CTYPE_L      /*240-255*/
};

/***************************************************************************************************
*\Function      GetSub
*\Description   ȡ������ľ���ֵ
*\Parameter     value1
*\Parameter     value2
*\Return        u32
*\Log           2008.01.17    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 GetSub(u32 value1, u32 value2)
{
    if (value1 >= value2)
    {
        return value1 - value2;
    }
    else
    {
        return value2 - value1;
    }
}

/***************************************************************************************************
*\Function      InRange
*\Description   whether value in [begin, end]
*\Parameter     value
*\Parameter     begin
*\Parameter     end
*\Return        bool
*\Log           2008.11.25    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool InRange(u32 value, u32 begin, u32 end)
{
    return (bool)(((begin <= value) && (value <= end)) || ((begin >= value) && (value >= end)));
}

/***************************************************************************************************
*\Function      BCDAdd
*\Description   BCD������
*\Parameter     bcd     BCD
*\Parameter     value   ����DEC
*\Return        u64     ���BCD
*\Note          1)��BCD������ֹͣ���㲢���ز���bcd��
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u64 BCDAdd(u64 bcd, u64 value)
{
    if (CheckBCD((u8)bcd) == FALSE)
    {
        return bcd;
    }

    BCDToDECU64(&bcd);

    bcd += value;

    return GetDECToBCDU64(bcd);
}

/***************************************************************************************************
*\Function      BCDSub
*\Description   BCD������
*\Parameter     bcd     BCD
*\Parameter     value   ����DEC
*\Return        u64      ���BCD
*\Note          1)��BCD������ֹͣ���㲢���ز���bcd��
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u64 BCDSub(u64 bcd, u64 value)
{
    if (CheckBCD((u8)bcd) == FALSE)
    {
        return bcd;
    }

    BCDToDECU64(&bcd);

    bcd -= value;

    return GetDECToBCDU64(bcd);
}

/***************************************************************************************************
*\Function      BCDFix
*\Description   BCD����
*\Parameter     bcd     ����ָ��
*\Return        bool    �Ƿ�����
*\Note          1)��������������0x99����Ϊ0x00����λ�Ϸ�����λ����0x09����Ϊ��λ��1����λΪ0��
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool BCDFix(u8* bcd)
{
    if (CheckBCD(*bcd) == TRUE)
    {
        return FALSE;
    }
    else
    {
        if (*bcd > 0x99)
        {
            *bcd = 0x00;
        }
        else if (GetU8LowU4(*bcd) > 0x09)
        {
            /*��λ��λ*/
            *bcd = SetU8LowU4(*bcd, 0x00);
            *bcd = SetU8HighU4(*bcd, GetU8HighU4(*bcd) + 1);
        }
        else
        {
            return FALSE;
        }

        return TRUE;
    }
}

/***************************************************************************************************
*\Function      BCDArrayFix
*\Description   BCD��������
*\Parameter     bcd     BCD����ָ��
*\Return        bool    �Ƿ�����
*\Note          1)��������������0x99����Ϊ0x00����λ�Ϸ�����λ����0x09����Ϊ��λ��1����λΪ0��
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool BCDArrayFix(void* bcd_array, u32 len)
{
    bool fix_flag = FALSE;
    while(len--)
    {
        fix_flag |= BCDFix((u8*)bcd_array + len);
    }

    return fix_flag;
}

/***************************************************************************************************
*\Function      BCDLeftMov
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd_array   BCD�ֽ�����
*\Parameter     len         BCD�ֽ����鳤��
*\Parameter     mov_num     ������ֵ����
*\Return        void
*\Note          1)lenΪ�ֽڳ��ȣ�������ֵ����Ϊlen * 2��
*\Note          2)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void BCDLeftMov(void* bcd_array, u32 len, u32 mov_num)
{
    u8* data = bcd_array;
    u32 mov_bytes = 0x00; /*�ƶ��ֽ��������ż����λ*/
    u32 valid_data_len = 0x00;
    u32 i = 0x00;

    mov_bytes = mov_num / 2;

    if ((mov_num == 0x00) || (bcd_array == NULL))
    {
        return;
    }

    if ((mov_bytes >= len))
    {
        MemSet(data, 0x00, len);
        return;
    }

    valid_data_len = len - mov_bytes;

    if (mov_bytes > 0x00)
    {
        MemCpy(data, data + mov_bytes, valid_data_len);
        MemSet(data + mov_bytes, 0x00, valid_data_len);
    }

    /*�����жϣ�1����λ*/
    if ((mov_num & 1) == 1)
    {
        /*�����жϱ�֤valid_data_len����0*/
        valid_data_len--;
        for (i = 0x00; i < valid_data_len; i++)
        {
            data[i] = (data[i]<<4) | (data[i + 1]>>4);
        }
        data[valid_data_len] = (data[valid_data_len] << 4) | 0x00;
    }
}

/***************************************************************************************************
*\Function      BCDRightMov
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd_array   BCD�ֽ�����
*\Parameter     len         BCD�ֽ����鳤��
*\Parameter     mov_num     ������ֵ����
*\Return        void
*\Note          1)lenΪ�ֽڳ��ȣ�������ֵ����Ϊlen * 2��
*\Note          2)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void BCDRightMov(void* bcd_array, u32 len, u32 mov_num)
{
    u8* data = bcd_array;
    u32 mov_bytes = 0x00; /*�ƶ��ֽ��������ż����λ*/
    u32 valid_data_len = 0x00;
    u32 i = 0x00;

    mov_bytes = mov_num / 2;

    if ((mov_num == 0x00) || (bcd_array == NULL))
    {
        return;
    }

    if ((mov_bytes >= len))
    {
        MemSet(data, 0x00, len);
        return;
    }

    valid_data_len = len - mov_bytes;

    if (mov_bytes > 0x00)
    {
        MemCpy(data + mov_bytes, data, valid_data_len);
        MemSet(data, 0x00, mov_bytes);
    }

    /*�����жϣ�1����λ*/
    if ((mov_num & 1) == 1)
    {
        /*�����жϱ�֤valid_data_len��len����0*/
        for (i = 0x01; i < valid_data_len; i++)
        {
            data[len - i] = (data[len - i] >> 4) | (data[len - i - 1] << 4);
        }
        data[len - valid_data_len] = (data[len - valid_data_len] >> 4) | 0x00;
    }
}

/***************************************************************************************************
*\Function      BCDLeftMovU8
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd         �������BCD���u8��������߰���2��BCD��
*\Parameter     mov_num     ������ֵ����
*\Return        void
*\Note          1)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 BCDLeftMovU8(u8 bcd, u32 mov_num)
{
    return bcd << (mov_num * 4);
}

/***************************************************************************************************
*\Function      BCDLeftMovU16
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd         �������BCD���u16��������߰���4��BCD��
*\Parameter     mov_num     ������ֵ����
*\Return        void
*\Note          1)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u16 BCDLeftMovU16(u16 bcd, u32 mov_num)
{
    return bcd << (mov_num * 4);
}

/***************************************************************************************************
*\Function      BCDLeftMovU32
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd         �������BCD���u32��������߰���8��BCD��
*\Return        void
*\Note          1)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 BCDLeftMovU32(u32 bcd, u32 mov_num)
{
    return bcd << (mov_num * 4);
}

/***************************************************************************************************
*\Function      BCDLeftMovU64
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd         �������BCD���u64��������߰���16��BCD��
*\Parameter     mov_num     ������ֵ����
*\Return        void
*\Note          1)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u64 BCDLeftMovU64(u64 bcd, u32 mov_num)
{
    return bcd << (mov_num * 4);
}


/***************************************************************************************************
*\Function      BCDRightMovU8
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd         �������BCD���u8��������߰���2��BCD��
*\Parameter     mov_num     ������ֵ����
*\Return        void
*\Note          1)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 BCDRightMovU8(u8 bcd, u32 mov_num)
{
    return bcd >> (mov_num * 4);
}

/***************************************************************************************************
*\Function      BCDRightMovU16
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd         �������BCD���u16��������߰���4��BCD��
*\Parameter     mov_num     ������ֵ����
*\Return        void
*\Note          1)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u16 BCDRightMovU16(u16 bcd, u32 mov_num)
{
    return bcd >> (mov_num * 4);
}

/***************************************************************************************************
*\Function      BCDRightMovU32
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd         �������BCD���u32��������߰���8��BCD��
*\Parameter     mov_num     ������ֵ����
*\Return        void
*\Note          1)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 BCDRightMovU32(u32 bcd, u32 mov_num)
{
    return bcd >> (mov_num * 4);
}

/***************************************************************************************************
*\Function      BCDRightMovU64
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd         �������BCD���u68��������߰���16��BCD��
*\Parameter     mov_num     ������ֵ����
*\Return        void
*\Note          1)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u64 BCDRightMovU64(u64 bcd, u32 mov_num)
{
    return bcd >> (mov_num * 4);
}

/***************************************************************************************************
*\Function      WaitFlag
*\Description   �ȴ���־
*\Parameter     flag        �ȴ���־
*\Parameter     flag_width  �ȴ����ݿ��
*\Parameter     value       �ȴ�ֵ
*\Parameter     wait        �ȴ�����
*\Parameter     timeout     0��ʾ�������ȴ���
*\Return        bool        �Ƿ�ȵ�
*\Note          1)�ú��������ڲ���ϵͳ�����ж�����ʹ�á�
*\Note          2)֧�����ַ���ʡ�
*\Log           2010.01.07    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool WaitFlag(const void* flag, const u8 flag_width, const u32 value, u32 (*wait)(u32), const u32 timeout)
{
    u32 count = 0x00;
    u32 ms    = 0x01;
    u32 t     = 0x00;

    if (timeout == 0x00)
    {
        /*��ֹms��Сʱ����OS���ȶ�*/
        if (GetVoidVar(flag, flag_width) != value)
        {
            t = wait(ms);
        }

        if (GetVoidVar(flag, flag_width) != value)
        {
            t += wait(ms);
            t /= 4;
            ms = Max(ms, t);
        }

        while(GetVoidVar(flag, flag_width) != value)
        {
            wait(ms);
        }
        return TRUE;
    }
    else
    {
        DEAD_LOOP
        {
            if (GetVoidVar(flag, flag_width) == value)
            {
                return TRUE;
            }
            else
            {
                if (count >= timeout)
                {
                    return FALSE;
                }
                count += wait(ms);
            }
        }
    }
}

