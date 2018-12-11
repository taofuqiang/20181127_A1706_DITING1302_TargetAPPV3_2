/***************************************************************************************************
*                    (c) Copyright 2008-2014 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          usdl_time.c
*\Description   
*\Note          
*\Log           2014.11.10    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#include "usdl_time.h"

/***************************************************************************************************
*\Ver           ChineseWeekName
*\Description   中文星期表。
***************************************************************************************************/
const u8* ChineseWeekName[] = {
    "星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日", "星期错误"
};

/***************************************************************************************************
*\Ver           EnglishWeekName
*\Description   英文星期表。
***************************************************************************************************/
const u8* EnglishWeekName[] = {
    "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", "WeekErr"
};

/***************************************************************************************************
*\Ver           EnWeekName
*\Description   英文简写星期表。
***************************************************************************************************/
const u8* EnWeekName[] = {
    "MON", "TUS", "WEN", "THUR", "FRI", "SAT", "SUN", "ERR"
};

/***************************************************************************************************
*\Ver           ChineseMonthName
*\Description   中文月分表。
***************************************************************************************************/
const u8* ChineseMonthName[] = {
    "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月",
    "月份错误"
};

/***************************************************************************************************
*\Ver           EnglishMonthName
*\Description   英文简写月分表。
***************************************************************************************************/
const u8* EnglishMonthName[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "Err"
};
//默认21世纪
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
            /*高位进位*/
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
*DEC时间修改函数
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeDECAddYear
*\Description   DEC时间加年。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
*\Note          1)若计算后日超出上限则取日上限。
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
*\Description   DEC时间加月。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
*\Note          1)若计算后日超出上限则取日上限。
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
*\Description   DEC时间加日。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   DEC时间加小时。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   DEC时间加分钟
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   DEC时间加秒。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   DEC时间减年。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
*\Note          1)若计算后日超出上限则取日上限。
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
*\Description   DEC时间减月。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
*\Note          1)若计算后日超出上限则取日上限。
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
*\Description   DEC时间减天。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   DEC时间减小时。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
        time->Hour = value % 24;/*临时存放value*/
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
*\Description   DEC时间减分钟。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
        time->Minute = value % 60;/*临时存放value*/
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
*\Description   DEC时间减秒。
*\Parameter     time    时间指针DEC
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
        time->Second = value % 60;/*临时存放value*/
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
*BCD时间修改函数
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeDECAddYear
*\Description   BCD时间加年。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
*\Note          1)若计算后日超出上限则取日上限。
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
*\Description   BCD时间加月。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
*\Note          1)若计算后日超出上限则取日上限。
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
*\Description   BCD时间加日。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   BCD时间加小时。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   BCD时间加分钟。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   BCD时间加秒。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   BCD时间减年。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
*\Note          1)若计算后日超出上限则取日上限。
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
*\Description   BCD时间减月。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
*\Note          1)若计算后日超出上限则取日上限。
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
*\Description   BCD时间减日。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换。
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
*\Description   BCD时间减小时。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   BCD时间减分钟。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   BCD时间减秒。
*\Parameter     time    时间指针BCD
*\Parameter     value   数值
*\Return        bool    是否成功转换
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
*\Description   时间交换函数。
*\Parameter     time1   时间指针DEC/BCD
*\Parameter     time2   时间指针DEC/BCD
*\Return        void
*\Note          1)该函数不检查时间参数正确性。
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
*时间计算函数
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeSubTimeDEC
*\Description   计算两时间秒差。
*\Parameter     time1   时间结构DEC
*\Parameter     time2   时间结构DEC
*\Return        u32     秒差
*\Note          1)计算结果为绝对值。
*\Note          2)若时间数据错误则返回0，注意：时间相等也返回0。
***************************************************************************************************/
u32 TimeDECSubTime(TIME time1, TIME time2)
{
    u32 sum = 0;

    if ((TimeDECCheck(&time1) == FALSE) || (TimeDECCheck(&time2) == FALSE))
    {
        return 0;
    }

    /*交换*/
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
*\Description   计算两时间秒差。
*\Parameter     time1   时间结构BCD
*\Parameter     time2   时间结构BCD
*\Return        u32     秒差
*\Note          1)计算结果为绝对值。
*\Note          2)若时间数据错误则返回0，注意：时间相等也返回0。
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
*时间类型转换函数
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeBCDToDEC
*\Description   将BCD时间转为DEC时间。
*\Parameter     time    时间指针BCD
*\Return        bool    是否成功转换
*\Note          1)time为引用，原值会改变。
*\Note          2)若时间数据非法则不转换并返回FALSE。
*\Note          3)本函数已检查时间正确性，无需再次检查。
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
*\Description   将DEC时间转为BCD时间。
*\Parameter     time    时间指针DEC
*\Return        bool    是否成功转换
*\Note          1)time为引用，原值会改变。
*\Note          2)若时间数据非法则不转换并返回FALSE。
*\Note          3)本函数已检查时间正确性，无需再次检查。
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
*时间比较函数
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeComp
*\Description   时间比较
*\Parameter     time1   时间指针DEC/BCD
*\Parameter     time2   时间指针DEC/BCD
*\Return        u8      TIME_LATE:晚于 TIME_EARLY:早于 TIME_EQUAL:等于
*\Note          1)time为引用，但原值不会改变。
*\Note          2)TIME_LATE等为位标志，可以使用与或表示。如：if(TempComp()&(TIME_EARLY|TIME_EQUAL)。
*\Note          3)该函数不检查时间参数正确性。
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
*时间查询函数
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeMonthDays
*\Description   获取某月天数。
*\Parameter     year    年DEC
*\Parameter     month   月DEC
*\Return        u8      本月天数DEC
*\Note          1)若月份错误则默认返回30天。
*\Note          2)默认为21世纪年份。
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
*\Description   获取某月天数。
*\Parameter     time    时间结构DEC
*\Return        u8      本月天数DEC
*\Note          1)默认为21世纪年份。
*\Note          2)若时间数据非法则返回0。
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
*\Description   获取某月天数。
*\Parameter     time    时间结构BCD
*\Return        u8      本月天数DEC
*\Note          1)默认为21世纪年份。
*\Note          2)若时间数据非法则返回0。
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
*\Description   获取DEC时间为星期几。
*\Parameter     time    时间结构DEC
*\Return        u8      取值范围：0~7。 0：周一， 6：周日；7代表时间错误。
*\Note          1)若时间数据非法则不查询并返回7。
*\Note          2)使用Zeller公式。
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
*\Description   获取BCD时间为星期几。
*\Parameter     time    时间结构BCD
*\Return        u8      取值范围：0~7。 0：周一， 6：周日；7代表时间错误。
*\Note          1)若时间数据非法则不查询并返回7。
*\Note          2)使用Zeller公式。
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
*时间检查函数
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeDECCheck
*\Description   检查DEC时间是否正确。
*\Parameter     time    时间指针DEC
*\Return        bool    检查结果
***************************************************************************************************/
bool TimeDECCheck(TIME* time)
{
    if (
        (time->Year <= 99) &&
        (1 <= time->Month) &&
        (time->Month <= 12) &&
        (1 <= time->Day) &&
        (time->Day <= TimeMonthDays(time->Year, time->Month)) && //绝对不可使用TimeDECMonthDays函数，递归。
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
*\Description   检查BCD时间是否正确。
*\Parameter     time    时间指针BCD
*\Return        bool    检查结果
***************************************************************************************************/
bool TimeBCDCheck(TIME* time)
{
    TIME time1 = *time;
    /*TimeBCDToDEC中调用TimeDECCheck所以无需再次调用。*/
    return TimeBCDToDEC(&time1);
}

/***************************************************************************************************
*\Function      TimeZeroCheck
*\Description   检查时间是否为0。
*\Parameter     time    时间指针DEC/BCD
*\Return        bool    检查结果
*\Note          1)该函数不检查时间参数正确性。
*\Note          2)用于时间量检查。
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
*时间修正函数
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeDECFix
*\Description   DEC时间修正
*\Parameter     time    时间指针DEC
*\Return        u8      修正位集合
*\Note          1)将错误的时间按其含义取近似值。
*\Note          2)修正规则：大于上限：取上限。小于下限：取下限。
*\Note          3)该函数不检查时间参数正确性。
***************************************************************************************************/
u8 TimeDECFix(TIME* time)
{
    u8 flag = 0x00;

    while (1)
    {
        /*年修正*/
        if (time->Year > 99)
        {
            time->Year = 99;
            BitSet(flag, 5);
        }

        /*月修正*/
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

        /*日修正*/
        else if (time->Day == 0x00)
        {
            time->Day = 0x01;
            BitSet(flag, 3);
        }
        /*此时年月数据已经被修正*/
        else if (time->Day > TimeMonthDays(time->Year, time->Month))
        {
            time->Day = TimeMonthDays(time->Year, time->Month);
            BitSet(flag, 3);
        }

        /*时修正*/
        else if (time->Hour >= 24)
        {
            time->Hour = 23;
            BitSet(flag, 2);
        }

        /*分修正*/
        else if (time->Minute >= 60)
        {
            time->Minute = 59;
            BitSet(flag, 1);
        }

        /*秒修正*/
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
*\Description   BCD时间修正
*\Parameter     time    时间指针BCD
*\Return        u8      修正位集合
*\Note          1)将错误的时间按其含义取近似值。
*\Note          2)修正规则：大于上限：取上限。小于下限：取下限。
*\Note            BCD数值非法且小于上限：取下一个正确BCD值，如：0x1F -> 0x20。
*\Note          3)该函数不检查时间参数正确性。
***************************************************************************************************/
u8 TimeBCDFix(TIME* time)
{
    u8 flag = 0x00;

    while (1)
    {
        /*年修正*/
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

        /*月修正*/
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

        /*日修正*/
        else if (time->Day == 0x00)
        {
            time->Day = 0x01;
            BitSet(flag, 3);
        }
        /*此时年月数据已经被修正*/
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

        /*时修正*/
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

        /*分修正*/
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

        /*秒修正*/
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

        /*正确*/
        else
        {
            break;
        }
    }

    return flag;
}


/***************************************************************************************************
*时间转换函数
***************************************************************************************************/

/***************************************************************************************************
*\Function      TimeDECToStamp
*\Description   将DEC时间转为时间戳。
*\Parameter     time    时间结构DEC
*\Return        u32     时间戳
*\Note          1)时间戳为相对基本时间InitTime的秒差+1。
*\Note          2)若时间数据非法则不计算并返回0（ERROR_STAMP）。
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
*\Description   将BCD时间转为时间戳。
*\Parameter     time    时间结构BCD
*\Return        u32     时间戳
*\Note          1)时间戳为相对基本时间InitTime的秒差+1。
*\Note          2)若时间数据非法则不计算并返回0（ERROR_STAMP）。
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
*\Description   将时间戳转为DEC时间。
*\Parameter     stamp   时间戳
*\Return        TIME    时间结构
*\Note          1)时间戳为相对基本时间InitTime的秒差+1。
*\Note          2)若参数stamp（时间戳）为0或1均返回基本时间InitTime。
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
*\Description   将时间戳转为BCD时间。
*\Parameter     stamp   时间戳
*\Return        TIME    时间结构
*\Note          1)时间戳为相对基本时间InitTime的秒差+1。
*\Note          2)若参数stamp（时间戳）为0或1均返回基本时间InitTime。
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
*\Description   判断两时间是否处于相同的分钟。
*\Parameter     t1_dec  第一个时间，格式为DEC。
*\Parameter     t2_dec  第二个时间，格式为DEC。
*\Return        bool
*\Log           2011.11.08    Ver 1.0    张波
*               创建函数。
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
*\Description   判断两时间是否处于相同的日。
*\Parameter     t1_dec  第一个时间，格式为DEC。
*\Parameter     t2_dec  第二个时间，格式为DEC。
*\Return        bool
*\Log           2010.05.28    Ver 1.0    张波
*               创建函数。
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
*\Description   判断两时间是否处于相同的月。
*\Parameter     t1_dec  第一个时间，格式为DEC。
*\Parameter     t2_dec  第二个时间，格式为DEC。
*\Return        bool
*\Log           2010.05.28    Ver 1.0    张波
*               创建函数。
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
*\Description   两个DEC时间相差的天数
*\Parameter     time1
*\Parameter     time2
*\Return        u32
*\Note          
*\Log           2015.08.25    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 TimeDECDiffDay(TIME time1, TIME time2)
{
    u32 sum = 0;

    if ((TimeDECCheck(&time1) == FALSE) || (TimeDECCheck(&time2) == FALSE))
    {
        return 0;
    }

    /*交换*/
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
