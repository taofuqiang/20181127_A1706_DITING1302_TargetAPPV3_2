/***************************************************************************************************
*                    (c) Copyright 2008-2014 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          usdl_time.c
*\Description   
*\Note          
*\Log           2014.11.10    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#include "usdl_time.h"

/***************************************************************************************************
*\Ver           ChineseWeekName
*\Description   �������ڱ�
***************************************************************************************************/
const u8* ChineseWeekName[] = {
    "����һ", "���ڶ�", "������", "������", "������", "������", "������", "���ڴ���"
};

/***************************************************************************************************
*\Ver           EnglishWeekName
*\Description   Ӣ�����ڱ�
***************************************************************************************************/
const u8* EnglishWeekName[] = {
    "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", "WeekErr"
};

/***************************************************************************************************
*\Ver           EnWeekName
*\Description   Ӣ�ļ�д���ڱ�
***************************************************************************************************/
const u8* EnWeekName[] = {
    "MON", "TUS", "WEN", "THUR", "FRI", "SAT", "SUN", "ERR"
};

/***************************************************************************************************
*\Ver           ChineseMonthName
*\Description   �����·ֱ�
***************************************************************************************************/
const u8* ChineseMonthName[] = {
    "һ��", "����", "����", "����", "����", "����", "����", "����", "����", "ʮ��", "ʮһ��", "ʮ����",
    "�·ݴ���"
};

/***************************************************************************************************
*\Ver           EnglishMonthName
*\Description   Ӣ�ļ�д�·ֱ�
***************************************************************************************************/
const u8* EnglishMonthName[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "Err"
};
//Ĭ��21����
const u8 CenturyYearTable[] = {
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0
};

static void BitPointSet(void* src, u8 pos, u8 width)
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
static bool CheckBCD(u8 bcd)
{
    if (((((bcd >> 4) & 0x0F)) < 10) && ((bcd & 0x0F) < 10))
    {
        return TRUE;
    }

    return FALSE;
}
static bool CheckDECToBCD(u8 dec)
{
    return (bool)(dec <= 99);
}

static bool MemBCDCheck(const u8* src, const u32 len)
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
static u8 GetU8LowU4(u8 value)
{
    return value&0x0F;
}
static u8 SetU8LowU4(u8 value, u8 half_word)
{
    return (value&0xF0)|(half_word&0x0F);
}
static u8 SetU8HighU4(u8 value, u8 half_word)
{
    return (value&0x0F)|((half_word<<4)&0xF0);
}
static u8 GetU8HighU4(u8 value)
{
    return (value>>4)&0x0F;
}
static bool BCDFix(u8* bcd)
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
static void DECToBCDU8(u8* dec)
{
    u8 quotient = 0x00;

    quotient = ((*dec)/10);
    quotient %= 10;
    *dec = (quotient<<4)  + (*dec) - 10 * quotient;
}
static void BCDToDECU8(u8* bcd)
{
    *bcd = (((*bcd) >> 4) & 0x0F) * 10 + ((*bcd)&0x0F);
}
static u8 GetBCDToDECU8(const u8 bcd)
{
    u8 dec = 0x00;

    dec = ((bcd >> 4) & 0x0F) * 10 + (bcd & 0x0F);

    return dec;
}
static u8* DECToBCD(void* dst, u8 len, u64 dec)
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
static u8 GetDECToBCDU8(const u8 dec)
{
    DECToBCD((void*)&dec, sizeof(dec), dec);
    return dec;
}

#define BitSet(src, pos)  { BitPointSet(&(src), (pos), sizeof(src));}

/***************************************************************************************************
*DECʱ���޸ĺ���
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeDECAddYear
*\Description   DECʱ����ꡣ
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
*\Note          1)��������ճ���������ȡ�����ޡ�
***************************************************************************************************/
bool TimeDECAddYear(TIME* time, u32 value)
{
    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    time->Year = (value + time->Year) % 100;

    if (time->Day > 28)
    {
        TimeDECFix(time);
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECAddMonth
*\Description   DECʱ����¡�
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
*\Note          1)��������ճ���������ȡ�����ޡ�
***************************************************************************************************/
bool TimeDECAddMonth(TIME* time, u32 value)
{
    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    if ((value + time->Month) > 12)
    {
        value += time->Month;
        time->Month = value % 12;

        if (time->Month == 0x00)
        {
            time->Month = 12;
            if (TimeDECAddYear(time, (value / 12) - 1) == FALSE)
            {
                return FALSE;
            }
        }
        else
        {
            if (TimeDECAddYear(time, value / 12) == FALSE)
            {
                return FALSE;
            }
        }

    }
    else
    {
        time->Month += (u8)value;
    }

    if (time->Day > 28)
    {
        TimeDECFix(time);
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECAddDay
*\Description   DECʱ����ա�
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeDECAddDay(TIME* time, u32 value)
{
    u8 days_in_month = 0x00;

    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    value += time->Day;
    time->Day = 1;
    days_in_month = TimeMonthDays(time->Year, time->Month);

    while (value > days_in_month)
    {
        value -= days_in_month;
        if (TimeDECAddMonth(time, 1) == FALSE)
        {
            return FALSE;
        }
        days_in_month = TimeMonthDays(time->Year, time->Month);
    }
    time->Day = (u8)value;

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECAddHour
*\Description   DECʱ���Сʱ��
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeDECAddHour(TIME* time, u32 value)
{
    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    if ((value + time->Hour) >= 24)
    {
        value += time->Hour;
        if (TimeDECAddDay(time, value / 24) == FALSE)
        {
            return FALSE;
        }
        time->Hour = value % 24;
    }
    else
    {
        time->Hour += (u8)value;
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECAddMinute
*\Description   DECʱ��ӷ���
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeDECAddMinute(TIME* time, u32 value)
{
    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    if ((value + time->Minute) >= 60)
    {
        value += time->Minute;
        if (TimeDECAddHour(time, value / 60) == FALSE)
        {
            return FALSE;
        }
        time->Minute = value % 60;
    }
    else
    {
        time->Minute += (u8)value;
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECAddSecond
*\Description   DECʱ����롣
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeDECAddSecond(TIME* time, u32 value)
{
    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    if ((value + time->Second) >= 60)
    {
        value += time->Second;
        if (TimeDECAddMinute(time, value / 60) == FALSE)
        {
            return FALSE;
        }

        time->Second = value % 60;
    }
    else
    {
        time->Second += (u8)value;
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECSubYear
*\Description   DECʱ����ꡣ
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
*\Note          1)��������ճ���������ȡ�����ޡ�
***************************************************************************************************/
bool TimeDECSubYear(TIME* time, u32 value)
{
    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    if (time->Year >= (value % 100))
    {
        time->Year -= (value % 100);
    }
    else
    {
        time->Year += 100;
        time->Year -= (value % 100);
    }

    if (time->Day > 28)
    {
        TimeDECFix(time);
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECSubMonth
*\Description   DECʱ����¡�
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
*\Note          1)��������ճ���������ȡ�����ޡ�
***************************************************************************************************/
bool TimeDECSubMonth(TIME* time, u32 value)
{
    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    if (value >= time->Month)
    {
        value -= time->Month;
        if (TimeDECSubYear(time, 1 + value / 12) == FALSE)
        {
            return FALSE;
        }
        time->Month = 12 - (value % 12);
    }
    else
    {
        time->Month -= (u8)value;
    }

    if (time->Day > 28)
    {
        TimeDECFix(time);
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECSubDay
*\Description   DECʱ����졣
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeDECSubDay(TIME* time, u32 value)
{
    u8 day = 0x00;

    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    day = time->Day;
    time->Day = 0x01;

    if (value >= day)
    {
        value -= day;
        if (TimeDECSubMonth(time, 1) == FALSE)
        {
            return FALSE;
        }
        day = TimeMonthDays(time->Year, time->Month);
    }

    while (value >= day)
    {
        value -= day;
        if (TimeDECSubMonth(time, 1) == FALSE)
        {
            return FALSE;
        }
        day = TimeMonthDays(time->Year, time->Month);
    }

    time->Day = day - (u8)value;

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECSubHour
*\Description   DECʱ���Сʱ��
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeDECSubHour(TIME* time, u32 value)
{
    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    if (value > time->Hour)
    {
        value -= time->Hour;
        time->Hour = value % 24;/*��ʱ���value*/
        if (TimeDECSubDay(time, (time->Hour != 0) + value / 24) == FALSE)
        {
            return FALSE;
        }
        if (time->Hour != 0)
        {
            time->Hour = 24 - (value % 24);
        }
    }
    else
    {
        time->Hour -= (u8)value;
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECSubMinute
*\Description   DECʱ������ӡ�
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeDECSubMinute(TIME* time, u32 value)
{
    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    if (value > time->Minute)
    {
        value -= time->Minute;
        time->Minute = value % 60;/*��ʱ���value*/
        if (TimeDECSubHour(time, (time->Minute != 0) + value / 60) == FALSE)
        {
            return FALSE;
        }
        if (time->Minute != 0)
        {
            time->Minute = 60 - (value % 60);
        }
    }
    else
    {
        time->Minute -= (u8)value;
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECSubSecond
*\Description   DECʱ����롣
*\Parameter     time    ʱ��ָ��DEC
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeDECSubSecond(TIME* time, u32 value)
{
    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    if (value > time->Second)
    {
        value -= time->Second;
        time->Second = value % 60;/*��ʱ���value*/
        if (TimeDECSubMinute(time, (time->Second != 0) + value / 60) == FALSE)
        {
            return FALSE;
        }
        if (time->Second != 0)
        {
            time->Second = 60 - (value % 60);
        }
    }
    else
    {
        time->Second -= (u8)value;
    }

    return TRUE;
}


/***************************************************************************************************
*BCDʱ���޸ĺ���
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeDECAddYear
*\Description   BCDʱ����ꡣ
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
*\Note          1)��������ճ���������ȡ�����ޡ�
***************************************************************************************************/
bool TimeBCDAddYear(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECAddYear(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDAddMonth
*\Description   BCDʱ����¡�
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
*\Note          1)��������ճ���������ȡ�����ޡ�
***************************************************************************************************/
bool TimeBCDAddMonth(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECAddMonth(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDAddDay
*\Description   BCDʱ����ա�
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeBCDAddDay(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECAddDay(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDAddHour
*\Description   BCDʱ���Сʱ��
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeBCDAddHour(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECAddHour(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDAddMinute
*\Description   BCDʱ��ӷ��ӡ�
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeBCDAddMinute(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECAddMinute(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDAddSecond
*\Description   BCDʱ����롣
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeBCDAddSecond(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECAddSecond(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDSubYear
*\Description   BCDʱ����ꡣ
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
*\Note          1)��������ճ���������ȡ�����ޡ�
***************************************************************************************************/
bool TimeBCDSubYear(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECSubYear(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDSubMonth
*\Description   BCDʱ����¡�
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
*\Note          1)��������ճ���������ȡ�����ޡ�
***************************************************************************************************/
bool TimeBCDSubMonth(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECSubMonth(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDSubDay
*\Description   BCDʱ����ա�
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת����
***************************************************************************************************/
bool TimeBCDSubDay(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECSubDay(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDSubHour
*\Description   BCDʱ���Сʱ��
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeBCDSubHour(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECSubHour(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDSubMinute
*\Description   BCDʱ������ӡ�
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeBCDSubMinute(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECSubMinute(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeBCDSubSecond
*\Description   BCDʱ����롣
*\Parameter     time    ʱ��ָ��BCD
*\Parameter     value   ��ֵ
*\Return        bool    �Ƿ�ɹ�ת��
***************************************************************************************************/
bool TimeBCDSubSecond(TIME* time, u32 value)
{
    if (TimeBCDToDEC(time) == FALSE)
    {
        return FALSE;
    }
    if (TimeDECSubSecond(time, value) == FALSE)
    {
        return FALSE;
    }
    return TimeDECToBCD(time);
}

/***************************************************************************************************
*\Function      TimeSwap
*\Description   ʱ�佻��������
*\Parameter     time1   ʱ��ָ��DEC/BCD
*\Parameter     time2   ʱ��ָ��DEC/BCD
*\Return        void
*\Note          1)�ú��������ʱ�������ȷ�ԡ�
***************************************************************************************************/
void TimeSwap(TIME* time1, TIME* time2)
{
    Swap(time1->Year,   time2->Year);
    Swap(time1->Month,  time2->Month);
    Swap(time1->Day,    time2->Day);
    Swap(time1->Hour,   time2->Hour);
    Swap(time1->Minute, time2->Minute);
    Swap(time1->Second, time2->Second);
}

/***************************************************************************************************
*ʱ����㺯��
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeSubTimeDEC
*\Description   ������ʱ����
*\Parameter     time1   ʱ��ṹDEC
*\Parameter     time2   ʱ��ṹDEC
*\Return        u32     ���
*\Note          1)������Ϊ����ֵ��
*\Note          2)��ʱ�����ݴ����򷵻�0��ע�⣺ʱ�����Ҳ����0��
***************************************************************************************************/
u32 TimeDECSubTime(TIME time1, TIME time2)
{
    u32 sum = 0;

    if ((TimeDECCheck(&time1) == FALSE) || (TimeDECCheck(&time2) == FALSE))
    {
        return 0;
    }

    /*����*/
    if (TimeComp(&time1, &time2) & TIME_LATE)
    {
        TimeSwap(&time1, &time2);
    }

    while (!((time1.Year == time2.Year) && (time1.Month == time2.Month)))
    {
        sum += (TimeMonthDays(time1.Year, time1.Month) * ONE_DAY);

        if (time1.Month == 12)
        {
            time1.Year++;
            time1.Month = 0;
        }

        time1.Month++;
    }
    sum = (u32)(
        (s64)sum +
        ((s8)time2.Day - (s8)time1.Day) * ONE_DAY +
        ((s8)time2.Hour - (s8)time1.Hour) * ONE_HOUR +
        ((s8)time2.Minute - (s8)time1.Minute) * ONE_MINUTE +
        ((s8)time2.Second - (s8)time1.Second)
        );

    return sum;
}

/***************************************************************************************************
*\Function      TimeBCDSubTime
*\Description   ������ʱ����
*\Parameter     time1   ʱ��ṹBCD
*\Parameter     time2   ʱ��ṹBCD
*\Return        u32     ���
*\Note          1)������Ϊ����ֵ��
*\Note          2)��ʱ�����ݴ����򷵻�0��ע�⣺ʱ�����Ҳ����0��
***************************************************************************************************/
u32 TimeBCDSubTime(TIME time1, TIME time2)
{
    if ((TimeBCDToDEC(&time1) == FALSE) || (TimeBCDToDEC(&time2) == FALSE))
    {
        return 0;
    }

    return TimeDECSubTime(time1, time2);
}

/***************************************************************************************************
*ʱ������ת������
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeBCDToDEC
*\Description   ��BCDʱ��תΪDECʱ�䡣
*\Parameter     time    ʱ��ָ��BCD
*\Return        bool    �Ƿ�ɹ�ת��
*\Note          1)timeΪ���ã�ԭֵ��ı䡣
*\Note          2)��ʱ�����ݷǷ���ת��������FALSE��
*\Note          3)�������Ѽ��ʱ����ȷ�ԣ������ٴμ�顣
***************************************************************************************************/
bool TimeBCDToDEC(TIME* time)
{
    u8 i = 0x00;

    if (MemBCDCheck((u8*)time, 6) == FALSE)
    {
        return FALSE;
    }

    for (i = 0; i < 6; i++)
    {
        BCDToDECU8((u8*)time + i);
    }

    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}

/***************************************************************************************************
*\Function      TimeDECToBCD
*\Description   ��DECʱ��תΪBCDʱ�䡣
*\Parameter     time    ʱ��ָ��DEC
*\Return        bool    �Ƿ�ɹ�ת��
*\Note          1)timeΪ���ã�ԭֵ��ı䡣
*\Note          2)��ʱ�����ݷǷ���ת��������FALSE��
*\Note          3)�������Ѽ��ʱ����ȷ�ԣ������ٴμ�顣
***************************************************************************************************/
bool TimeDECToBCD(TIME* time)
{
    u8 i = 0x00;
    for (i = 0; i <= 5; i++)
    {
        if (CheckDECToBCD(*((u8*)time + i)) == FALSE)
        {
            return FALSE;
        }
    }

    if (TimeDECCheck(time) == FALSE)
    {
        return FALSE;
    }

    for (i = 0; i < 6; i++)
    {
        DECToBCDU8((u8*)time + i);
    }
    return TRUE;
}

/***************************************************************************************************
*ʱ��ȽϺ���
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeComp
*\Description   ʱ��Ƚ�
*\Parameter     time1   ʱ��ָ��DEC/BCD
*\Parameter     time2   ʱ��ָ��DEC/BCD
*\Return        u8      TIME_LATE:���� TIME_EARLY:���� TIME_EQUAL:����
*\Note          1)timeΪ���ã���ԭֵ����ı䡣
*\Note          2)TIME_LATE��Ϊλ��־������ʹ������ʾ���磺if(TempComp()&(TIME_EARLY|TIME_EQUAL)��
*\Note          3)�ú��������ʱ�������ȷ�ԡ�
***************************************************************************************************/
u8 TimeComp(TIME* time1, TIME* time2)
{
    u8 i = 0;

    for (i = 0; i < 6; i++)
    {
        if (*((u8*)(time1) + i) < *((u8*)(time2) + i))
        {
            return TIME_EARLY;
        }
        else if (*((u8*)(time1) + i) > *((u8*)(time2) + i))
        {
            return TIME_LATE;
        }
    }
    return TIME_EQUAL;
}


/***************************************************************************************************
*ʱ���ѯ����
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeMonthDays
*\Description   ��ȡĳ��������
*\Parameter     year    ��DEC
*\Parameter     month   ��DEC
*\Return        u8      ��������DEC
*\Note          1)���·ݴ�����Ĭ�Ϸ���30�졣
*\Note          2)Ĭ��Ϊ21������ݡ�
***************************************************************************************************/
u8 TimeMonthDays(u8 year, u8 month)
{
    switch(month)
    {
    case 1:case 3:case 5:case 7:case 8:case 10:case 12:
        return 31;
    case 2:
        return 28 + CenturyYearTable[year];
    case 4:case 6:case 9:case 11:
        return 30;
    default:
        return 0;
    }
}


/***************************************************************************************************
*\Function      TimeDECMonthDays
*\Description   ��ȡĳ��������
*\Parameter     time    ʱ��ṹDEC
*\Return        u8      ��������DEC
*\Note          1)Ĭ��Ϊ21������ݡ�
*\Note          2)��ʱ�����ݷǷ��򷵻�0��
***************************************************************************************************/
u8 TimeDECMonthDays(TIME time)
{
    if (TimeDECCheck(&time) == FALSE)
    {
        return 0;
    }
    return TimeMonthDays(time.Year, time.Month);
}

/***************************************************************************************************
*\Function      TimeBCDMonthDays
*\Description   ��ȡĳ��������
*\Parameter     time    ʱ��ṹBCD
*\Return        u8      ��������DEC
*\Note          1)Ĭ��Ϊ21������ݡ�
*\Note          2)��ʱ�����ݷǷ��򷵻�0��
***************************************************************************************************/
u8 TimeBCDMonthDays(TIME time)
{
    if (TimeBCDToDEC(&time) == FALSE)
    {
        return 0;
    }

    return TimeMonthDays(time.Year, time.Month);
}

/***************************************************************************************************
*\Function      TimeDECWeekDay
*\Description   ��ȡDECʱ��Ϊ���ڼ���
*\Parameter     time    ʱ��ṹDEC
*\Return        u8      ȡֵ��Χ��0~7�� 0����һ�� 6�����գ�7����ʱ�����
*\Note          1)��ʱ�����ݷǷ��򲻲�ѯ������7��
*\Note          2)ʹ��Zeller��ʽ��
***************************************************************************************************/
u8 TimeDECWeekDay(TIME time)
{
    u16 year = 0x00;

    if (TimeDECCheck(&time) == FALSE)
    {
        return 0x07;
    }

    year = 2000 + time.Year;

    if ((time.Month == 1) || (time.Month == 2))
    {
        time.Month += 12;
        year--;
    }

    return (time.Day + 2*time.Month + 3*(time.Month+1)/5 + year + year/4 - year/100 + year/400) % 7;
}

/***************************************************************************************************
*\Function      TimeBCDWeekDay
*\Description   ��ȡBCDʱ��Ϊ���ڼ���
*\Parameter     time    ʱ��ṹBCD
*\Return        u8      ȡֵ��Χ��0~7�� 0����һ�� 6�����գ�7����ʱ�����
*\Note          1)��ʱ�����ݷǷ��򲻲�ѯ������7��
*\Note          2)ʹ��Zeller��ʽ��
***************************************************************************************************/
u8 TimeBCDWeekDay(TIME time)
{
    if (TimeBCDToDEC(&time) == FALSE)
    {
        return 0x07;
    }
    return TimeDECWeekDay(time);
}

/***************************************************************************************************
*ʱ���麯��
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeDECCheck
*\Description   ���DECʱ���Ƿ���ȷ��
*\Parameter     time    ʱ��ָ��DEC
*\Return        bool    �����
***************************************************************************************************/
bool TimeDECCheck(TIME* time)
{
    if (
        (time->Year <= 99) &&
        (1 <= time->Month) &&
        (time->Month <= 12) &&
        (1 <= time->Day) &&
        (time->Day <= TimeMonthDays(time->Year, time->Month)) && //���Բ���ʹ��TimeDECMonthDays�������ݹ顣
        (time->Hour < 24) &&
        (time->Minute < 60) &&
        (time->Second < 60)
        )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/***************************************************************************************************
*\Function      TimeBCDCheck
*\Description   ���BCDʱ���Ƿ���ȷ��
*\Parameter     time    ʱ��ָ��BCD
*\Return        bool    �����
***************************************************************************************************/
bool TimeBCDCheck(TIME* time)
{
    TIME time1 = *time;
    /*TimeBCDToDEC�е���TimeDECCheck���������ٴε��á�*/
    return TimeBCDToDEC(&time1);
}

/***************************************************************************************************
*\Function      TimeZeroCheck
*\Description   ���ʱ���Ƿ�Ϊ0��
*\Parameter     time    ʱ��ָ��DEC/BCD
*\Return        bool    �����
*\Note          1)�ú��������ʱ�������ȷ�ԡ�
*\Note          2)����ʱ������顣
***************************************************************************************************/
bool TimeZeroCheck(TIME* time)
{
    if ((time->Year == 0) && (time->Month == 0)  && (time->Day == 0)
        &&(time->Hour == 0) && (time->Minute == 0) && (time->Second == 0))
    {
        return TRUE;
    }

    return FALSE;
}

/***************************************************************************************************
*ʱ����������
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeDECFix
*\Description   DECʱ������
*\Parameter     time    ʱ��ָ��DEC
*\Return        u8      ����λ����
*\Note          1)�������ʱ�䰴�京��ȡ����ֵ��
*\Note          2)�������򣺴������ޣ�ȡ���ޡ�С�����ޣ�ȡ���ޡ�
*\Note          3)�ú��������ʱ�������ȷ�ԡ�
***************************************************************************************************/
u8 TimeDECFix(TIME* time)
{
    u8 flag = 0x00;

    while (1)
    {
        /*������*/
        if (time->Year > 99)
        {
            time->Year = 99;
            BitSet(flag, 5);
        }

        /*������*/
        else if (time->Month == 0x00)
        {
            time->Month = 0x01;
            BitSet(flag, 4);
        }
        else if (time->Month > 12)
        {
            time->Month = 12;
            BitSet(flag, 4);
        }

        /*������*/
        else if (time->Day == 0x00)
        {
            time->Day = 0x01;
            BitSet(flag, 3);
        }
        /*��ʱ���������Ѿ�������*/
        else if (time->Day > TimeMonthDays(time->Year, time->Month))
        {
            time->Day = TimeMonthDays(time->Year, time->Month);
            BitSet(flag, 3);
        }

        /*ʱ����*/
        else if (time->Hour >= 24)
        {
            time->Hour = 23;
            BitSet(flag, 2);
        }

        /*������*/
        else if (time->Minute >= 60)
        {
            time->Minute = 59;
            BitSet(flag, 1);
        }

        /*������*/
        else if (time->Second >= 60)
        {
            time->Second = 59;
            BitSet(flag, 0);
        }
        else
        {
            break;
        }
    }

    return flag;
}

/***************************************************************************************************
*\Function      TimeBCDFix
*\Description   BCDʱ������
*\Parameter     time    ʱ��ָ��BCD
*\Return        u8      ����λ����
*\Note          1)�������ʱ�䰴�京��ȡ����ֵ��
*\Note          2)�������򣺴������ޣ�ȡ���ޡ�С�����ޣ�ȡ���ޡ�
*\Note            BCD��ֵ�Ƿ���С�����ޣ�ȡ��һ����ȷBCDֵ���磺0x1F -> 0x20��
*\Note          3)�ú��������ʱ�������ȷ�ԡ�
***************************************************************************************************/
u8 TimeBCDFix(TIME* time)
{
    u8 flag = 0x00;

    while (1)
    {
        /*������*/
        if (time->Year > 0x99)
        {
            time->Year = 0x99;
            BitSet(flag, 5);
        }
        else if (BCDFix((u8*)&time->Year) == TRUE)
        {
            BitSet(flag, 5);
            continue;
        }

        /*������*/
        else if (time->Month == 0x00)
        {
            time->Month = 0x01;
            BitSet(flag, 4);
        }
        else if (time->Month > 0x12)
        {
            time->Month = 0x12;
            BitSet(flag, 4);
        }
        else if (BCDFix((u8*)&time->Month) == TRUE)
        {
            BitSet(flag, 4);
            continue;
        }

        /*������*/
        else if (time->Day == 0x00)
        {
            time->Day = 0x01;
            BitSet(flag, 3);
        }
        /*��ʱ���������Ѿ�������*/
        else if (GetBCDToDECU8(time->Day) > TimeMonthDays(GetBCDToDECU8(time->Year), GetBCDToDECU8(time->Month)))
        {
            time->Day = GetDECToBCDU8(TimeMonthDays(GetBCDToDECU8(time->Year), GetBCDToDECU8(time->Month)));
            BitSet(flag, 3);
        }
        else if (BCDFix((u8*)&time->Day) == TRUE)
        {
            BitSet(flag, 3);
            continue;
        }

        /*ʱ����*/
        else if (time->Hour >= 0x24)
        {
            time->Hour = 0x23;
            BitSet(flag, 2);
        }
        else if (BCDFix((u8*)&time->Hour) == TRUE)
        {
            BitSet(flag, 2);
            continue;
        }

        /*������*/
        else if (time->Minute >= 0x60)
        {
            time->Minute = 0x59;
            BitSet(flag, 1);
        }
        else if (BCDFix((u8*)&time->Minute) == TRUE)
        {
            BitSet(flag, 1);
            continue;
        }

        /*������*/
        else if (time->Second >= 0x60)
        {
            time->Second = 0x59;
            BitSet(flag, 0);
        }
        else if (BCDFix((u8*)&time->Second) == TRUE)
        {
            BitSet(flag, 0);
            continue;
        }

        /*��ȷ*/
        else
        {
            break;
        }
    }

    return flag;
}


/***************************************************************************************************
*ʱ��ת������
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeDECToStamp
*\Description   ��DECʱ��תΪʱ�����
*\Parameter     time    ʱ��ṹDEC
*\Return        u32     ʱ���
*\Note          1)ʱ���Ϊ��Ի���ʱ��InitTime�����+1��
*\Note          2)��ʱ�����ݷǷ��򲻼��㲢����0��ERROR_STAMP����
***************************************************************************************************/
u32 TimeDECToStamp(TIME time)
{
    TIME init_time = INIT_TIME;

    if (TimeDECCheck(&time) == FALSE)
    {
        return ERROR_STAMP;
    }

    return TimeDECSubTime(time, init_time) + 1;
}

/***************************************************************************************************
*\Function      TimeBCDToStamp
*\Description   ��BCDʱ��תΪʱ�����
*\Parameter     time    ʱ��ṹBCD
*\Return        u32     ʱ���
*\Note          1)ʱ���Ϊ��Ի���ʱ��InitTime�����+1��
*\Note          2)��ʱ�����ݷǷ��򲻼��㲢����0��ERROR_STAMP����
***************************************************************************************************/
u32 TimeBCDToStamp(TIME time)
{
    TIME init_time = INIT_TIME;

    if (TimeBCDCheck(&time) == FALSE)
    {
        return ERROR_STAMP;
    }

    return TimeBCDSubTime(time, init_time) + 1;
}

/***************************************************************************************************
*\Function      TimeDECStampToTime
*\Description   ��ʱ���תΪDECʱ�䡣
*\Parameter     stamp   ʱ���
*\Return        TIME    ʱ��ṹ
*\Note          1)ʱ���Ϊ��Ի���ʱ��InitTime�����+1��
*\Note          2)������stamp��ʱ�����Ϊ0��1�����ػ���ʱ��InitTime��
***************************************************************************************************/
TIME TimeDECStampToTime(u32 stamp)
{
    TIME time = INIT_TIME;

    if (stamp != ERROR_STAMP)
    {
        TimeDECAddSecond(&time, stamp - 1);
    }

    return time;
}

/***************************************************************************************************
*\Function      TimeBCDStampToTime
*\Description   ��ʱ���תΪBCDʱ�䡣
*\Parameter     stamp   ʱ���
*\Return        TIME    ʱ��ṹ
*\Note          1)ʱ���Ϊ��Ի���ʱ��InitTime�����+1��
*\Note          2)������stamp��ʱ�����Ϊ0��1�����ػ���ʱ��InitTime��
***************************************************************************************************/
TIME TimeBCDStampToTime(u32 stamp)
{
    TIME time = INIT_TIME;

    if (stamp != ERROR_STAMP)
    {
        TimeBCDAddSecond(&time, stamp - 1);
    }

    return time;
}

/***************************************************************************************************
*\Function      TimeSameMinute
*\Description   �ж���ʱ���Ƿ�����ͬ�ķ��ӡ�
*\Parameter     t1_dec  ��һ��ʱ�䣬��ʽΪDEC��
*\Parameter     t2_dec  �ڶ���ʱ�䣬��ʽΪDEC��
*\Return        bool
*\Log           2011.11.08    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool TimeSameMinute(TIME* t1_dec, TIME* t2_dec)
{
    if ((TimeDECCheck(t1_dec) == FALSE) || (TimeDECCheck(t2_dec) == FALSE))
    {
        return FALSE;
    }
    if ((t1_dec->Year == t2_dec->Year) &&
        (t1_dec->Month == t2_dec->Month) &&
        (t1_dec->Day == t2_dec->Day) &&
        (t1_dec->Hour == t2_dec->Hour) &&
        (t1_dec->Minute == t2_dec->Minute))
    {
        return TRUE;
    }
    return FALSE;
}

/***************************************************************************************************
*\Function      TimeSameDay
*\Description   �ж���ʱ���Ƿ�����ͬ���ա�
*\Parameter     t1_dec  ��һ��ʱ�䣬��ʽΪDEC��
*\Parameter     t2_dec  �ڶ���ʱ�䣬��ʽΪDEC��
*\Return        bool
*\Log           2010.05.28    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool TimeSameDay(TIME* t1_dec, TIME* t2_dec)
{
    if ((TimeDECCheck(t1_dec) == FALSE) || (TimeDECCheck(t2_dec) == FALSE))
    {
        return FALSE;
    }
    if ((t1_dec->Year == t2_dec->Year) &&
        (t1_dec->Month == t2_dec->Month) &&
        (t1_dec->Day == t2_dec->Day))
    {
        return TRUE;
    }
    return FALSE;
}

/***************************************************************************************************
*\Function      TimeSameDay
*\Description   �ж���ʱ���Ƿ�����ͬ���¡�
*\Parameter     t1_dec  ��һ��ʱ�䣬��ʽΪDEC��
*\Parameter     t2_dec  �ڶ���ʱ�䣬��ʽΪDEC��
*\Return        bool
*\Log           2010.05.28    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool TimeSameMonth(TIME* t1_dec, TIME* t2_dec)
{
    if ((TimeDECCheck(t1_dec) == FALSE) || (TimeDECCheck(t2_dec) == FALSE))
    {
        return FALSE;
    }
    if ((t1_dec->Year == t2_dec->Year) &&
        (t1_dec->Month == t2_dec->Month))
    {
        return TRUE;
    }
    return FALSE;
}

/***************************************************************************************************
*\Function      TimeDECDiffDay
*\Description   ����DECʱ����������
*\Parameter     time1
*\Parameter     time2
*\Return        u32
*\Note          
*\Log           2015.08.25    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 TimeDECDiffDay(TIME time1, TIME time2)
{
    u32 sum = 0;

    if ((TimeDECCheck(&time1) == FALSE) || (TimeDECCheck(&time2) == FALSE))
    {
        return 0;
    }

    /*����*/
    if (TimeComp(&time1, &time2) & TIME_LATE)
    {
        TimeSwap(&time1, &time2);
    }

    while (!((time1.Year == time2.Year) && (time1.Month == time2.Month)))
    {
        sum += (TimeMonthDays(time1.Year, time1.Month));

        if (time1.Month == 12)
        {
            time1.Year++;
            time1.Month = 0;
        }

        time1.Month++;
    }
    sum = (u32)(
        (s64)sum +
        ((s8)time2.Day - (s8)time1.Day) + 1) ;

    return sum;
}
