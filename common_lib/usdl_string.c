/***************************************************************************************************
*                    (c) Copyright 1992-2010 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_string.c
*\Description   �ַ�������ģ��
*\Note          1��StrSplit ��ӷ��������
*               2������滻�ַ�����
*\Log           2008.10.08    Ver 1.1    �Ų�
*\Log           2017��11��8�� Ver 1.2  Job 
*               �����ļ���
***************************************************************************************************/

#include "usdl_string.h"

/***************************************************************************************************
*�ַ�������
***************************************************************************************************/

/***************************************************************************************************
*\Function      StrCpy
*\Description
*\Parameter     dst
*\Parameter     src
*\Return        u8*
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* StrCpy(u8* dst, const u8* src)
{
    u8* save = dst;

    while ((*dst++ = *src++) != NULL);

    return (save);
}

/***************************************************************************************************
*\Function      StrNCpy
*\Description
*\Parameter     dst
*\Parameter     src
*\Parameter     n
*\Return        u8*
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* StrNCpy(u8* dst, const u8* src, u32 n)
{
    u8* d = dst;

    if (n != 0)
    {
        /*copy src, checking size <n>*/
        while ((*d++ = *src++) != 0)
        {
            if (--n == 0)
            {
                return (dst);
            }
        }

        while (--n > 0)
        {
            /*NULL terminate string*/
            *d++ = NULL;
        }
    }

    return (dst);
}

/***************************************************************************************************
*\Function      StrCat
*\Description
*\Parameter     dst
*\Parameter     str
*\Return        u8*
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* StrCat(u8* dst, const u8* str)
{
    u8* save = dst;

    /*find end of string*/
    while (*dst++ != '\0');
    dst--;
    while ((*dst++ = *str++) != '\0');
    return save;
}

/***************************************************************************************************
*\Function      StrNCat
*\Description
*\Parameter     dst
*\Parameter     src
*\Parameter     n
*\Return        u8*
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* StrNCat(u8* dst, const u8* src, u32 n)
{
    u8* d = dst;
    if (n != 0)
    {
        /*find end of string*/
        while (*d++ != NULL);

        /*rewind back of NULL*/
        d--;

        while (((*d++ = *src++) != NULL) && (--n > 0));

        if (n == 0)
        {
            /*NULL terminate string*/
            *d = NULL;
        }
    }

    return (dst);
}

/***************************************************************************************************
*�ַ���ͳ�ơ��Ƚϡ�����
***************************************************************************************************/

/***************************************************************************************************
*\Function      StrLen
*\Description
*\Parameter     str
*\Return        u32
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 StrLen(const u8* str)
{
    const u8* save = str + 1;

    while (*str++ != NULL);

    return (str - save);
}

/***************************************************************************************************
*\Function      StrNLen
*\Description   ��ȫ��ȡ�ַ����ĳ���
*\Parameter     str     �ַ���
*\Parameter     max_len str��ָ�򻺳�������󳤶�
*\Return        u32     ����
*\Log           2011.07.21    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 StrNLen(const u8* str, u32 max_len)
{
    u32 re_len = 0;

    while (*str++ != NULL)
    {
        re_len++;
        if (re_len >= max_len)
        {
            return max_len;
        }
    }

    return re_len;
}

/***************************************************************************************************
*\Function      StrCmp
*\Description
*\Parameter     str1
*\Parameter     str2
*\Return        s8
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
s8 StrCmp(const u8* str1, const u8* str2)
{
    while (*str1++ == *str2++)
    {
        if (*(str1 - 1) == NULL)
        {
            return (0);
        }
    }

    return *(str1 - 1) - *(str2 - 1);
}

/***************************************************************************************************
*\Function      StrICmp
*\Description   �ַ����ȽϺ���,�����ִ�Сд
*\Parameter     str1    �ַ���1
*\Parameter     str2    �ַ���2
*\Return        s8      �ȽϽ��
*\Note
*\Log           2011.03.15    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
s8 StrICmp(const u8* str1, const u8* str2)
{
    u8 ch1 = 0;
    u8 ch2 = 0;

    do
    {
        if (((ch1 = (u8)(*(str1++))) >= 'A') && (ch1 <= 'Z'))
        {
            ch1 += 0x20;
        }

        if (((ch2 = (u8)(*(str2++))) >= 'A') &&(ch2 <= 'Z'))
        {
            ch2 += 0x20;
        }

    }while (ch1 && (ch1 == ch2));

    return (ch1 - ch2);
}

/***************************************************************************************************
*\Function      StrNCmp
*\Description
*\Parameter     str1
*\Parameter     str2
*\Parameter     n
*\Return        u32
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 StrNCmp(const u8* str1, const u8* str2, u32 n)
{
    if (n == 0)
    {
        return (0);
    }

    while (*str1++ == *str2++)
    {
        if ((*(str1 - 1) == NULL) || (--n == 0))
        {
            return (0);
        }
    }

    return *(str1 - 1) - *(str2 - 1);
}

/***************************************************************************************************
*\Function      StrNICmp
*\Description   �����ַ����ȽϺ���,�����ִ�Сд
*\Parameter     str1    �ַ���1
*\Parameter     str2    �ַ���2
*\Parameter     count   Ҫ�Ƚϵĳ���
*\Return        s8      �ȽϽ��
*\Note
*\Log           2011.03.15    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
s8 StrNICmp(const u8* str1, const u8* str2, u32 count)
{
    u8 ch1 = 0;
    u8 ch2 = 0;

    do
    {
        if (((ch1 = (u8)(*(str1++))) >= 'A') && (ch1 <= 'Z'))
        {
            ch1 += 0x20;
        }

        if (((ch2 = (u8)(*(str2++))) >= 'A') &&(ch2 <= 'Z'))
        {
            ch2 += 0x20;
        }

    }while ((--count) && ch1 && (ch1 == ch2));

    return (ch1 - ch2);
}

/***************************************************************************************************
*\Function      StrChr
*\Description
*\Parameter     str
*\Parameter     ch
*\Return        u8*
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* StrChr(const u8* str, u8 ch)
{
    u8* r = (u8*)str;

    /*search loop*/
    while (*r != ch)
    {
        /*end of string*/
        if (*r++ == NULL)
        {
            return (NULL);
        }
    }

    return (r);
}


/***************************************************************************************************
*\Function      StrRChr
*\Description
*\Parameter     str
*\Parameter     ch
*\Return        u8*
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* StrRChr(const u8* str, u8 ch)
{
    u8* save = NULL;

    /*loop, searching for character*/
    do
    {
        if (*str == (u8)ch)
        {
            save = (u8*)str;
        }
    } while (*str++ != NULL);

    return ((u8*)save);
}

/***************************************************************************************************
*\Function      StrCSpn
*\Description
*\Parameter     str
*\Parameter     ch_set
*\Return        u32
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 StrCSpn(const u8* str, const u8* ch_set)
{
    const u8*   save = NULL;
    const u8*   p    = NULL;
    u8          c1   = 0x00;
    u8          c2   = 0x00;

    /*search for NULL*/
    for (save = str + 1; (c1 = *str++) != NULL;)
    {
        /*search for first occurance*/
        for (p = ch_set; (c2 = *p++) != NULL;)
        {
            if (c1 == c2)
            {
                /*return index of substring*/
                return (str - save);
            }
        }
    }

    return (str - save);
}

/***************************************************************************************************
*\Function      StrSpn
*\Description
*\Parameter     str
*\Parameter     ch_set
*\Return        u32
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 StrSpn(const u8* str, const u8* ch_set)
{
    const u8*   save = NULL;
    const u8*   p    = NULL;
    u8          c1   = 0x00;
    u8          c2   = 0x00;

    for (save = str + 1; (c1 = *str++) != NULL;)
    {
        for (p = ch_set; (c2 = *p++) != c1;)
        {
            if (c2 == NULL)
            {
                return (str - save);
            }
        }
    }

    return (str - save);
}

/***************************************************************************************************
*\Function      StrStr
*\Description
*\Parameter     str
*\Parameter     sub_str
*\Return        u8*
*\Note
*\Log           2010.12.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* StrStr(const u8* str, const u8* sub_str)
{
    u8* t1 = NULL;
    u8* t2 = NULL;
    u8  c  = NULL;
    u8  c2 = NULL;

    if ((c = *sub_str++) == NULL)        /*<find> an empty string*/
        return ((u8*)(str));

    DEAD_LOOP
    {
        while (((c2 = *str++) != NULL) && (c2 != c));

        if (c2 == NULL)
        {
            return (NULL);
        }

        t1 = (u8*)(str);
        t2 = (u8*)(sub_str);

        while (((c2 = *t2++) != 0) && (*t1++ == c2));

        if (c2 == NULL)
        {
            return ((u8*)(str - 1));
        }
    }
}

/***************************************************************************************************
*\Function      Strip
*\Description   ���ַ���str��ȥ���ַ�ch��
*\Parameter     str
*\Parameter     ch
*\Return        u8*
*\Note
*\Log           2010.12.21    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8* Strip(u8* str, u8 ch)
{
    u8* p1 = str;
    u8* p2 = NULL;
    while (*p1++)
    {
        if(*p1 == ch)
        {
            p2 = p1;
            while (*p2 && *p2 == ch)
            {
                ++ p2;
            }
            if (*p2)
            {
                *p1 = *p2;
                *p2 = ch;
            }
            else
            {
                *p1 = '\0';
                break;
            }
        }
    }
    return str;
}

/***************************************************************************************************
*\Function      StrSplit
*\Description   �ָ��ַ���
*\Parameter     str
*\Parameter     split
*\Parameter     replace Ҫ�滻���ַ� ���滻��ʹ�÷ָ��
*\Parameter     result
*\Parameter     node_num result���� ����ֹ���
*\Return        u32,�ָ��ĸ���
*\Note          
*\Log           2017.02.27    Ver 1.0    Job               
				����������
***************************************************************************************************/
u32 StrSplit(u8* str, const u8 split, const u8 replace, DataNode result[], u32 node_num)
{
    u8* str_start = str;
    u32 str_len = 0;
    u32 index = 0;

    if (str == NULL)
    {
        return 0;
    }

    while (*str)
    {
        if (*str == split)
        {
            if (str_len != 0)
            {
                result[index].Data = str_start;
                result[index].Len = str_len;
            }
            else
            {
                result[index].Data = NULL;
                result[index].Len = 0;
            }
            index++;
            //�ж�һ���Ƿ񳬹���ԭ������
            if (index >= node_num)
            {
                return index;
            }
            str_start = str + 1;
            str_len = 0;
            //�滻�ַ�
            *str = replace;
        }
        else
        {
            str_len++;
        }
        str++;
    }

    //�������Ƿ���δ����ƥ������ַ���
    if (str_len != 0)
    {
        result[index].Data = str_start;
        result[index].Len = str_len;
        index++;
    }

    return index;
}


