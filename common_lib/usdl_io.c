/***************************************************************************************************
*                    (c) Copyright 1992-2010 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_io.c
*\Description
*\Note
*\Log           2010.12.02    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/

#include "usdl_tools.h"
#include "usdl_io.h"

#define ZEROPAD    1        /*pad with zero*/
#define SIGN    2        /*unsigned long*/
#define PLUS    4        /*show plus*/
#define SPACE    8        /*space if plus*/
#define LEFT    16        /*left justified*/
#define SPECIAL    32        /*0x*/
#define LARGE    64        /*use 'ABCDEF' instead of 'abcdef'*/

static s32 skip_atoi(const u8** s)
{
    s32 i, c;

    for (i = 0; '0' <= (c = **s)&& c <= '9'; ++*s)
        i = i * 10 + c - '0';
    return i;
}

/***************************************************************************************************
*Printf
***************************************************************************************************/
static s32 VprintfNumber(void (*output)(u8), s32 count, u64 num, s32 base, s32 size, s32 precision, s32 type)
{
    u8 c, sign, tmp[66];
    const u8* digits;
    static const u8 small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static const u8 large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    s32 i;

    digits = (type & LARGE)? large_digits : small_digits;

    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return NULL;

    c = (type & ZEROPAD)? '0' : ' ';
    sign = 0;

    if (type & SIGN)
    {
        if ((s64)num < 0)
        {
            sign = '-';
            num = - (s64)num;
            size--;
        }
        else if (type & PLUS)
        {
            sign = '+';
            size--;
        }
        else if (type & SPACE)
        {
            sign = ' ';
            size--;
        }
    }

    if (type & SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }

    i = 0;

    if (num == 0)
        tmp[i++] = '0';
    else while (num != 0)
    {
        tmp[i++] = digits[(u32)(num) % base];
        num = (u32)(num) / base;
    }

    if (i > precision)
        precision = i;
    size -= precision;
    if (!(type&(ZEROPAD + LEFT)))
    {
        while(size-->0)
        {
            output(' ');//*count = ' ';
            ++count;
        }
    }
    if (sign)
    {
        output(sign);//*count = sign;
        ++count;
    }
    if (type & SPECIAL)
    {
        if (base==8)
        {
            output('0');//*count = '0';
            ++count;
        }
        else if (base==16)
        {
            output('0');//*count = '0';
            ++count;
            output(digits[33]);//*count = digits[33];
            ++count;
        }
    }
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            output(c);//*count = c;
            ++count;
        }
    }
    while (i < precision--)
    {
        output('0');//*count = '0';
        ++count;
    }
    while (i-- > 0)
    {
        output(tmp[i]);//*count = tmp[i];
        ++count;
    }
    while (size-- > 0)
    {
        output(' ');//*count = ' ';
        ++count;
    }
    return count;
}

/***************************************************************************************************
*\Function      Vprintf
*\Description   用于Printf封装
*\Parameter     output  输出函数（输出一个字符）
*\Parameter     format  格式化数据
*\Parameter     args    变参
*\Return        u32     输出字节数
*\Note
*\Log           2010.12.02    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 Vprintf(void (*output)(u8), const u8* format, VaListType args)
{
    s32  len    = 0x00;
    u64  num    = 0x00;
    s32  i      = 0x00;
    s32  base   = 0x00;
    s32  count  = 0x00;
    u8   c      = NULL;
    const u8* s = NULL;

    s32 flags   = 0x00;     /*flags to number()*/

    s32 field_width = 0x00; /*width of output field*/
    s32 precision   = 0x00; /*min. # of digits for integers; max
                            number of u8s for from string*/
    s32 qualifier   = 0x00; /*'h', 'l', or 'L' for integer fields*/
    /*'z' support added 23/7/1999 S.H.   */
    /*'z' changed to 'Z' --davidm 1/25/99*/

    for (; *format ; ++format)
    {
        if (*format != '%')
        {
            output(*format);//*str = *format;
            ++count;
            continue;
        }

        /*process flags*/
        flags = 0;
repeat:
        ++format;        /*this also skips first '%'*/
        switch (*format)
        {
        case '-': flags |= LEFT; goto repeat;
        case '+': flags |= PLUS; goto repeat;
        case ' ': flags |= SPACE; goto repeat;
        case '#': flags |= SPECIAL; goto repeat;
        case '0': flags |= ZEROPAD; goto repeat;
        }

        /*get field width*/
        field_width = -1;
        if ('0' <= *format && *format <= '9')
        {
            field_width = skip_atoi(&format);
        }
        else if (*format == '*')
        {
            ++format;
            /*it's the next argument*/
            field_width = VA_ARG(args, s32);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /*get the precision*/
        precision = -1;
        if (*format == '.')
        {
            ++format;    
            if ('0' <= *format && *format <= '9')
            {
                precision = skip_atoi(&format);
            }
            else if (*format == '*')
            {
                ++format;
                /*it's the next argument*/
                precision = VA_ARG(args, s32);
            }
            if (precision < 0)
            {
                precision = 0;
            }
        }

        /*get the conversion qualifier*/
        qualifier = -1;
        if (*format == 'h' || *format == 'l' || *format == 'L' ||
            *format =='Z' || *format == 'z')
        {
            qualifier = *format;
            ++format;
            if (qualifier == 'l' && *format == 'l')
            {
                qualifier = 'L';
                ++format;
            }
        }

        /*default base*/
        base = 10;

        switch (*format)
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                {
                    output(' ');//*str = ' ';
                    ++count;
                }
            }
            c = (u8)VA_ARG(args, s32);
            output(c);//*str = c;
            ++count;

            while (--field_width > 0)
            {
                output(' ');//*str = ' ';
                ++count;
            }
            continue;

        case 's':
            s = VA_ARG(args, u8*);
            {
                const u8* sc = s;
                s32 sc_count = precision;
                for (; sc_count-- && *sc != '\0'; ++sc);
                len = sc - s;
            }

            if (!(flags & LEFT))
            {
                while (len < field_width--)
                {
                    output(' ');//*str = ' ';
                    ++count;
                }
            }

            for (i = 0; i < len; ++i)
            {
                output(*s);//*str = *s;
                ++count;
                ++s;
            }

            while (len < field_width--)
            {
                output(' ');//*str = ' ';
                ++count;
            }
            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = 2 * sizeof(void* );
                flags |= ZEROPAD;
            }
            count = VprintfNumber(output, count, (u32)VA_ARG(args, void*), 16, field_width, precision, flags);
            continue;

        case 'n':
            /*FIXME:
            * What does C99 say about the overflow case here?*/
            if (qualifier == 'l')
            {
                s32* ip = VA_ARG(args, s32* );
                *ip = count;//(str - buf);
            }
            else if (qualifier == 'Z' || qualifier == 'z')
            {
                s32* ip = VA_ARG(args, s32*);
                *ip = count;//(str - buf);
            }
            else
            {
                s32* ip = VA_ARG(args, s32*);
                *ip = count;//(str - buf);
            }
            continue;

        case '%':
            output('%');//*str = '%';
            ++count;
            continue;

            /*integer number formats - set up the flags and "break"*/
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            output('%');//*str = '%';
            ++count;
            if (*format)
            {
                output(*format);//*str = *format;
                ++count;
            }
            else
            {
                --format;
            }
            continue;
        }
        if (qualifier == 'L')
            num = VA_ARG(args, s64);
        else if (qualifier == 'l')
        {
            num = VA_ARG(args, u32);
            if (flags & SIGN)
                num = (s32)num;
        }
        else if (qualifier == 'Z' || qualifier == 'z')
        {
            num = VA_ARG(args, s32);
        }
        else if (qualifier == 'h')
        {
            num = (unsigned short)VA_ARG(args, s32);
            if (flags & SIGN)
                num = (signed short)num;
        }
        else
        {
            num = VA_ARG(args, u32);
            if (flags & SIGN)
                num = (s32)num;
        }
        count = VprintfNumber(output, count, num, base, field_width, precision, flags);
    }

    return count;
}

/***************************************************************************************************
*\Function      Printf
*\Description   使用output输出格式化数据。
*\Parameter     output  输出函数（输出一个字符）
*\Parameter     format  格式化数据
*\Parameter     ...     变参
*\Return        u32     输出字节数
*\Note          1)使用方法参照C标准库printf。
*\Log           2010.12.02    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 Printf(void (*output)(u8), const u8* format, ...)
{
    VaListType args;
    u32 count = 0x00;
    VA_START(args, format);
    count = Vprintf(output, format, args);
    VA_END(args);
    return count;
}

/***************************************************************************************************
*Sprintf
***************************************************************************************************/
static u8* VsnprintfNumber(u8* buf, u8* end, u64 num, s32 base, s32 size, s32 precision, s32 type)
{
    u8 c, sign, tmp[66];
    const u8* digits;
    static const u8 small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static const u8 large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    s32 i;

    digits = (type & LARGE)? large_digits : small_digits;
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return NULL;
    c = (type & ZEROPAD)? '0' : ' ';
    sign = 0;
    if (type & SIGN)
    {
        if ((s64)num < 0)
        {
            sign = '-';
            num = - (s64)num;
            size--;
        }
        else if (type & PLUS)
        {
            sign = '+';
            size--;
        }
        else if (type & SPACE)
        {
            sign = ' ';
            size--;
        }
    }
    if (type & SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }
    i = 0;
    if (num == 0)
        tmp[i++]='0';
    else while (num != 0)
    {
        tmp[i++] = digits[(u32)(num)% base];
        (num)= (u32)(num)/ base;
    }

    if (i > precision)
        precision = i;
    size -= precision;
    if (!(type&(ZEROPAD+LEFT)))
    {
        while(size-->0)
        {
            if (buf <= end)
                *buf = ' ';
            ++buf;
        }
    }
    if (sign)
    {
        if (buf <= end)
            *buf = sign;
        ++buf;
    }
    if (type & SPECIAL)
    {
        if (base==8)
        {
            if (buf <= end)
                *buf = '0';
            ++buf;
        }
        else if (base==16)
        {
            if (buf <= end)
                *buf = '0';
            ++buf;
            if (buf <= end)
                *buf = digits[33];
            ++buf;
        }
    }
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            if (buf <= end)
                *buf = c;
            ++buf;
        }
    }
    while (i < precision--)
    {
        if (buf <= end)
            *buf = '0';
        ++buf;
    }
    while (i-- > 0)
    {
        if (buf <= end)
            *buf = tmp[i];
        ++buf;
    }
    while (size-- > 0)
    {
        if (buf <= end)
            *buf = ' ';
        ++buf;
    }
    return buf;
}

/***************************************************************************************************
*\Function      Vsnprintf
*\Description   用于Snprintf封装
*\Parameter     buf     输出缓冲区
*\Parameter     size    输出缓冲区长度
*\Parameter     format  格式化数据
*\Parameter     args    变参
*\Return        u32     输出字节数
*\Note
*\Log           2010.12.02    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 Vsnprintf(u8* buf, u32 size, const u8* format, VaListType args)
{
    s32  len    = 0x00;
    u64  num    = 0x00;
    s32  i      = 0x00;
    s32  base   = 0x00;
    u8*  str    = NULL;
    u8*  end    = NULL;
    u8   c      = NULL;
    const u8* s = NULL;

    s32 flags;        /*flags to number()*/

    s32 field_width;    /*width of output field*/
    s32 precision;        /*min. # of digits for integers; max
                        number of u8s for from string*/
    s32 qualifier;        /*'h', 'l', or 'L' for integer fields*/
    /*'z' support added 23/7/1999 S.H.   */
    /*'z' changed to 'Z' --davidm 1/25/99*/

    /*Reject out-of-range values early*/
    if ((s32)size < 0)
    {
        /*There can be only one..*/
        return 0;
    }

    str = buf;
    end = buf + size - 1;

    if (end < buf - 1)
    {
        end = ((void* )-1);
        size = end - buf + 1;
    }

    for (; *format ; ++format)
    {
        if (*format != '%')
        {
            if (str <= end)
                *str = *format;
            ++str;
            continue;
        }

        /*process flags*/
        flags = 0;
repeat:
        ++format;        /*this also skips first '%'*/
        switch (*format)
        {
        case '-': flags |= LEFT; goto repeat;
        case '+': flags |= PLUS; goto repeat;
        case ' ': flags |= SPACE; goto repeat;
        case '#': flags |= SPECIAL; goto repeat;
        case '0': flags |= ZEROPAD; goto repeat;
        }

        /*get field width*/
        field_width = -1;
        if ('0' <= *format && *format <= '9')
            field_width = skip_atoi(&format);
        else if (*format == '*')
        {
            ++format;
            /*it's the next argument*/
            field_width = VA_ARG(args, s32);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /*get the precision*/
        precision = -1;
        if (*format == '.')
        {
            ++format;    
            if ('0' <= *format && *format <= '9')
                precision = skip_atoi(&format);
            else if (*format == '*')
            {
                ++format;
                /*it's the next argument*/
                precision = VA_ARG(args, s32);
            }
            if (precision < 0)
                precision = 0;
        }

        /*get the conversion qualifier*/
        qualifier = -1;
        if (*format == 'h' || *format == 'l' || *format == 'L' ||
            *format =='Z' || *format == 'z')
        {
            qualifier = *format;
            ++format;
            if (qualifier == 'l' && *format == 'l')
            {
                qualifier = 'L';
                ++format;
            }
        }

        /*default base*/
        base = 10;

        switch (*format)
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                {
                    if (str <= end)
                        *str = ' ';
                    ++str;
                }
            }
            c = (u8)VA_ARG(args, s32);
            if (str <= end)
                *str = c;
            ++str;
            while (--field_width > 0)
            {
                if (str <= end)
                    *str = ' ';
                ++str;
            }
            continue;

        case 's':
            s = VA_ARG(args, u8*);
            {
                const u8* sc = s;
                s32 sc_count = precision;
                for (; sc_count-- && *sc != '\0'; ++sc);
                len = sc - s;
            }

            if (!(flags & LEFT))
            {
                while (len < field_width--)
                {
                    if (str <= end)
                        *str = ' ';
                    ++str;
                }
            }
            for (i = 0; i < len; ++i)
            {
                if (str <= end)
                    *str = *s;
                ++str; ++s;
            }
            while (len < field_width--)
            {
                if (str <= end)
                    *str = ' ';
                ++str;
            }
            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = 2*sizeof(void*);
                flags |= ZEROPAD;
            }
            str = VsnprintfNumber(str, end,
                (u32)VA_ARG(args, void*),
                16, field_width, precision, flags);
            continue;


        case 'n':
            /*FIXME:
            * What does C99 say about the overflow case here?*/
            if (qualifier == 'l')
            {
                s32* ip = VA_ARG(args, s32* );
                *ip = (str - buf);
            }
            else if (qualifier == 'Z' || qualifier == 'z')
            {
                s32 * ip = VA_ARG(args, s32*);
                *ip = (str - buf);
            }
            else
            {
                s32 * ip = VA_ARG(args, s32*);
                *ip = (str - buf);
            }
            continue;

        case '%':
            if (str <= end)
                *str = '%';
            ++str;
            continue;

            /*integer number formats - set up the flags and "break"*/
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            if (str <= end)
                *str = '%';
            ++str;
            if (*format)
            {
                if (str <= end)
                    *str = *format;
                ++str;
            }
            else
            {
                --format;
            }
            continue;
        }
        if (qualifier == 'L')
            num = VA_ARG(args, s64);
        else if (qualifier == 'l')
        {
            num = VA_ARG(args, u32);
            if (flags & SIGN)
                num = (s32)num;
        }
        else if (qualifier == 'Z' || qualifier == 'z')
        {
            num = VA_ARG(args, s32);
        }
        else if (qualifier == 'h')
        {
            num = (unsigned short)VA_ARG(args, s32);
            if (flags & SIGN)
                num = (signed short)num;
        }
        else
        {
            num = VA_ARG(args, u32);
            if (flags & SIGN)
                num = (s32)num;
        }
        str = VsnprintfNumber(str, end, num, base,
            field_width, precision, flags);
    }
    if (str <= end)
        *str = '\0';
    else if (size > 0)
        /*don't write out a null byte if the buf size is zero*/
        *end = '\0';
    /*the trailing null byte doesn't count towards the total
    * ++str;
   */
    return (u32)(str - buf);
}

/***************************************************************************************************
*\Function      Sprintf
*\Description   输出格式化数据到指定缓冲区中。
*\Parameter     buf     输出缓冲区
*\Parameter     format  格式化数据
*\Parameter     ...     变参
*\Return        u32     输出字节数
*\Note          1)使用方法参照C标准库Sprintf。
*\Note          2)无缓冲区保护。
*\Log           2010.12.02    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 Sprintf(u8* buf, const u8* format, ...)
{
    VaListType args;
    u32 count = 0x00;
    VA_START(args, format);
    count = Vsnprintf(buf, (~0U)>>1, format, args);
    VA_END(args);
    return count;
}

/***************************************************************************************************
*\Function      Snprintf
*\Description   输出长度不大于size的格式化数据到指定缓冲区中。
*\Parameter     buf     输出缓冲区
*\Parameter     size    输出缓冲区长度
*\Parameter     format  格式化数据
*\Parameter     ...     变参
*\Return        u32     输出字节数
*\Note          1)使用方法参照C标准库扩展snprintf。
*\Note          2)有缓冲区保护。
*\Log           2010.12.02    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 Snprintf(u8* buf, u32 size, const u8* format, ...)
{
    VaListType args;
    u32 count = 0x00;
    VA_START(args, format);
    count = Vsnprintf(buf, size, format, args);
    VA_END(args);
    return count;
}


/***************************************************************************************************
*Sscanf
***************************************************************************************************/
static u32 simple_strtoul(const u8 *cp, u8 **endp, u32 base)
{
    u32 result = 0, value;

    if (!base)
    {
        base = 10;
        if (*cp == '0')
        {
            base = 8;
            cp++;
            if ((TO_UPPER(*cp) == 'X') && IS_XDIGIT(cp[1]))
            {
                cp++;
                base = 16;
            }
        }
    }
    else if (base == 16)
    {
        if (cp[0] == '0' && TO_UPPER(cp[1]) == 'X')
            cp += 2;
    }
    while (IS_XDIGIT(*cp) &&
        (value = IS_DIGIT(*cp) ? *cp-'0' : TO_UPPER(*cp)-'A'+10) < base)
    {
            result = result*base + value;
            cp++;
    }
    if (endp)
        *endp = (u8*)cp;
    return result;
}

static s32 simple_strtol(const u8 *cp, u8 **endp, u32 base)
{
    if(*cp=='-')
        return -(s32)simple_strtoul(cp + 1, endp, base);
    return simple_strtoul(cp, endp, base);
}

static u64 simple_strtoull(const u8 *cp, u8 **endp, u32 base)
{
    u64 result = 0, value;

    if (!base) {
        base = 10;
        if (*cp == '0') {
            base = 8;
            cp++;
            if ((TO_UPPER(*cp) == 'X') && IS_XDIGIT(cp[1])) {
                cp++;
                base = 16;
            }
        }
    }
    else if (base == 16) {
        if (cp[0] == '0' && TO_UPPER(cp[1]) == 'X')
            cp += 2;
    }
    while (IS_XDIGIT(*cp) && (value = IS_DIGIT(*cp) ? *cp-'0' : (IS_LOWER(*cp)
        ? TO_UPPER(*cp) : *cp)-'A'+10) < base) {
            result = result*base + value;
            cp++;
    }
    if (endp)
        *endp = (u8*)cp;
    return result;
}

static s64 simple_strtoll(const u8 *cp, u8 **endp, u32 base)
{
    if(*cp=='-')
        return -(s64)simple_strtoull(cp+1, endp, base);
    return simple_strtoull(cp, endp, base);
}

/***************************************************************************************************
*\Function      Vsscanf
*\Description   用于Sscanf封装。
*\Parameter     buf     输入缓冲区
*\Parameter     format  格式化数据
*\Parameter     args    变参
*\Return        u32     被赋值的变量数量。
*\Note
*\Log           2010.12.02    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 Vsscanf(const u8* buf, const u8* format, VaListType args)
{
    const u8* str = buf;
    u8* next = NULL;
    u8  digit;
    s32 num = 0;
    s32 qualifier;
    s32 base;
    s32 field_width;
    s32 is_sign = 0;

    while(*format && *str)
    {
        /*skip any white space in format*/
        /*white space in format matchs any amount of
        * white space, including none, in the input.
       */
        if (IS_SPACE(*format))
        {
            while (IS_SPACE(*format))
                ++format;
            while (IS_SPACE(*str))
                ++str;
        }

        /*anything that is not a conversion must match exactly*/
        if (*format != '%' && *format)
        {
            if (*format++ != *str++)
                break;
            continue;
        }

        if (!*format)
            break;
        ++format;

        /*skip this conversion.
        * advance both strings to next white space
       */
        if (*format == '*')
        {
            while (!IS_SPACE(*format) && *format)
                format++;
            while (!IS_SPACE(*str) && *str)
                str++;
            continue;
        }

        /*get field width*/
        field_width = -1;
        if (IS_DIGIT(*format))
            field_width = skip_atoi(&format);

        /*get conversion qualifier*/
        qualifier = -1;
        if (*format == 'h' || *format == 'l' || *format == 'L' ||
            *format == 'Z' || *format == 'z')
        {
            qualifier = *format++;
            if (qualifier == *format)
            {
                if (qualifier == 'h')
                {
                    qualifier = 'H';
                    format++;
                }
                else if (qualifier == 'l')
                {
                    qualifier = 'L';
                    format++;
                }
            }
        }
        base = 10;
        is_sign = 0;

        if (!*format || !*str)
            break;

        switch(*format++)
        {
        case 'c':
            {
                u8* s = (u8*)VA_ARG(args, u8*);
                if (field_width == -1)
                    field_width = 1;
                do {
                    *s++ = *str++;
                } while (--field_width > 0 && *str);
                num++;
            }
            continue;
        case 's':
            {
                u8* s = (u8*)VA_ARG(args, u8* );
                if(field_width == -1)
                    field_width = S32_MAX;
                /*first, skip leading white space in buffer*/
                while (IS_SPACE(*str))
                    str++;

                /*now copy until next white space*/
                while (*str && !IS_SPACE(*str) && field_width--)
                {
                    *s++ = *str++;
                }
                *s = '\0';
                num++;
            }
            continue;
        case 'n':
            /*return number of u8acters read so far*/
            {
                s32 *i = (s32*)VA_ARG(args, s32*);
                *i = str - buf;
            }
            continue;
        case 'o':
            base = 8;
            break;
        case 'x':
        case 'X':
            base = 16;
            break;
        case 'i':
            base = 0;
        case 'd':
            is_sign = 1;
        case 'u':
            break;
        case '%':
            /*looking for '%' in str*/
            if (*str++ != '%')
                return num;
            continue;
        default:
            /*invalid format; stop here*/
            return num;
        }

        /*have some sort of integer conversion.
        * first, skip white space in buffer.
       */
        while (IS_SPACE(*str))
            str++;

        digit = *str;
        if (is_sign && digit == '-')
            digit = *(str + 1);

        if (!digit
            || (base == 16 && !IS_XDIGIT(digit))
            || (base == 10 && !IS_DIGIT(digit))
            || (base == 8 && (!IS_DIGIT(digit) || digit > '7'))
            || (base == 0 && !IS_DIGIT(digit)))
            break;

        switch(qualifier)
        {
        case 'H':    /*that's 'hh' in format*/
            if (is_sign)
            {
                u8* s = (u8*)VA_ARG(args, u8* );
                *s = (u8) simple_strtol(str, &next, base);
            }
            else
            {
                u8* s = (u8*)VA_ARG(args, u8*);
                *s = (u8) simple_strtoul(str, &next, base);
            }
            break;
        case 'h':
            if (is_sign)
            {
                s16* s = (s16*)VA_ARG(args, s16*);
                *s = (s16)simple_strtol(str, &next, base);
            }
            else
            {
                u16* s = (u16*) VA_ARG(args, u16*);
                *s = (u16) simple_strtoul(str, &next, base);
            }
            break;
        case 'l':
            if (is_sign)
            {
                s32 *l = (s32*)VA_ARG(args, s32*);
                *l = simple_strtol(str, &next, base);
            }
            else
            {
                u32 *l = (u32*)VA_ARG(args, u32*);
                *l = simple_strtoul(str, &next, base);
            }
            break;
        case 'L':
            if (is_sign)
            {
                s64 *l = (s64*)VA_ARG(args, s64*);
                *l = simple_strtoll(str, &next, base);
            }
            else
            {
                u64 *l = (u64*)VA_ARG(args, u64*);
                *l = simple_strtoull(str, &next, base);
            }
            break;
        case 'Z':
        case 'z':
            {
                s32 *s = (s32*)VA_ARG(args, s32*);
                *s = (s32) simple_strtoul(str, &next, base);
            }
            break;
        default:
            if (is_sign)
            {
                s32 *i = (s32*)VA_ARG(args, s32*);
                *i = (s32) simple_strtol(str, &next, base);
            }
            else
            {
                u32 *i = (u32*)VA_ARG(args, u32*);
                *i = (u32)simple_strtoul(str, &next, base);
            }
            break;
        }
        num++;

        if (!next)
        {
            break;
        }
        str = next;
    }
    return num;
}

/***************************************************************************************************
*\Function      Sscanf
*\Description   从字符串buf中读进与指定格式format相符的数据。
*\Parameter     buf     输入缓冲区
*\Parameter     format  格式化数据
*\Parameter     ...     变参
*\Return        u32     被赋值的变量数量。
*\Note          1)使用方法参照C标准库扩展snprintf。
*\Log           2010.12.02    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
u32 Sscanf(const u8* buf, const u8* format, ...)
{
    VaListType args;
    u32 count = 0x00;
    VA_START(args, format);
    count = Vsscanf(buf, format, args);
    VA_END(args);
    return count;
}
