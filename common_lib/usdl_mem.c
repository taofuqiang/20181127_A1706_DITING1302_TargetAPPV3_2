/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_mem.c
*\Description   �ڴ����ģ�飬��C��׼�ڴ溯���������䡣
*\Log           2008.06.24    Ver 1.0    �Ų�
*               �����ļ���
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
*\Description   ptrָ����ڴ������е�len���ֽڼ�value
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     value   �Ӻ�ֵ
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2008.06.24    Ver 1.0    �Ų�
*               ����������
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
*\Description   �ڴ�����src��len���ֽڼ�value
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     value   �Ӻ�ֵ
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2008.03.05    Ver 1.0    �Ų�
*               ����������
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
*\Description   ptrָ����ڴ������length���ֽڷֱ���value���롣
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     value   1�ֽڵ���ֵ
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2008.06.24    Ver 1.0    �Ų�
*               ����������
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
*\Description   ptrָ����ڴ������length���ֽ�ȡ����
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
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
*\Description   ptrָ����ڴ������length���ֽڰ�λȡ����
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
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
*\Description   ptrָ����ڴ������length���ֽڷֱ���value���
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     value   1�ֽڵ���ֵ
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2008.06.24    Ver 1.0    �Ų�
*               ����������
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
*\Description   ptrָ����ڴ������length���ֽڷֱ���value�����
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     value   1�ֽڵ���ֵ
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
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
*\Description   �����ڴ�����ptr��length���ֽڵĺ�
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        u32
*\Log           2008.03.21    Ver 1.0    �Ų�
*               ����������
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
*\Description   �����ڴ�����ptr��length���ֽڵ�And
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        u32
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
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
*\Description   �����ڴ�����ptr��length���ֽڵ�Or
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        u32
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
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
*\Description   �����ڴ�����ptr��length���ֽڵ�Xor
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        u32
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
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
*\Description   ptrָ����ڴ������е�length���ֽ�����
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2008.06.24    Ver 1.0    �Ų�
*               ����������
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
*\Description   �ڴ�����src��len���ֽ����򿽱�
*\Parameter     dst     Ŀ�ĵ�ַ
*\Parameter     src     Դ��ַ
*\Parameter     len     ����
*\Return        void
*\Log           2008.06.24    Ver 1.0    �Ų�
*               ����������
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
*\Description   �ڴ�����src��len���ֽڻ��ο�����dst+start_pos��ʼ���ڴ��С�
*\Parameter     dst             ���λ�����
*\Parameter     dst_size        ���λ���������
*\Parameter     start_offset    ���λ�����д����ʼƫ��
*\Parameter     src             ������Դ�ڴ�����
*\Parameter     len             ��������
*\Return        u8*             ����д�����ʱ��ָ�루дָ�룩
*\Log           2008.08.04    Ver 1.0    �Ų�
*               ����������
*\Log           2009.07.30    Ver 1.1    �Ų�
*               �޸ĵ�start_offset + len����dst_sizeʱdst + start_offset + lenԽ��BUG��
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
*\Description   �����ڴ�����src��start_pos��end_pos�����ݿ�����dst��ʼ�����λ������С�
*\Parameter     dst         ���λ�����
*\Parameter     src         ���λ�����
*\Parameter     start_pos   ���λ�������ʼƫ��
*\Parameter     end_pos     ���λ�������ֹƫ��
*\Parameter     src_size    ���λ���������
*\Return        u32         ���λ�����ʵ��д�볤��
*\Note          1)����������Ŀ�Ļ�������ȡ�
*\Note          2)����Ҫ��������ʹ��MemCycleSubToLine��
*\Log           2008.08.04    Ver 1.0    �Ų�
*               ����������
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
*\Description   �����ڴ�����src��start_pos��end_pos��ǰlen�����ݿ�����dst��ʼ�����λ������С�
*\Parameter     dst         ���λ�����
*\Parameter     src         ���λ�����
*\Parameter     start_pos   ���λ�������ʼƫ��
*\Parameter     end_pos     ���λ�������ֹƫ��
*\Parameter     src_size    ���λ���������
*\Parameter     request_len ���󳤶ȣ���[���󳤶�]����[���λ��������ݳ���]��ֻ�������λ��������ݡ�
*\Return        u32         ���λ�����ʵ��д�볤�ȣ�С�ڵ���request_len��
*\Note          1)����������Ŀ�Ļ�������ȣ����豣����3)��
*\Note          2)�����ڶ��ڴӻ��λ�����ȡ�������ݵĲ�����
*\Note          3)����len������дdst_size����Ŀ�Ļ�����������
*\Log           2009.08.21    Ver 1.0    �Ų�
*               ����������
*\Log           2009.11.13    Ver 1.1    �Ų�
*               �޸Ļ�д������λ�ô������⡣
***************************************************************************************************/
u32 MemCycleSubToLine(u8* dst, const u8* src, const u32 start_pos, const u32 end_pos,
                      const u32 src_size, const u32 request_len)
{
    u32 head_len = 0x00;/*ǰ������*/

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
            /*��[ǰ�γ���][����]�򿽱�[���󳤶�]*/
            memcpy(dst, src + start_pos, request_len);
            return request_len;
        }
        else
        {
            /*��[ǰ�γ���][������]��[��]����[ǰ�γ���]*/
            memcpy(dst, src + start_pos, head_len);

            if (end_pos >= (request_len - head_len))
            {
                /*��[��γ���][����]�򿽱�[���󳤶�]��[ǰ�γ���]*/
                memcpy(dst + head_len, src, request_len - head_len);
                return request_len;
            }
            else
            {
                /*��[��γ���][������]�򿽱�[��γ���]*/
                memcpy(dst + head_len, src, end_pos);
                return head_len + end_pos;
            }
        }
    }

    return 0;
}

/***************************************************************************************************
*\Function      MemMatch
*\Description   ʹ��ģʽ�ڴ�������ƥ��Ŀ���ڴ����������ǰ׺,����ͨ�����������ڴ������
*               �������С�
*\Parameter     pat         ָ��ģʽ�ڴ������ָ��
*\Parameter     pat_len     ģʽ�ڴ�����ĳ���
*\Parameter     dst         ָ��Ŀ���ڴ������ָ��
*\Parameter     dst_len     Ŀ���ڴ�����ĳ���
*\Parameter     wildcard    ����ͨ�����ָ��ASCII�ַ�
*\Parameter     result[]    Result�ṹ����,���ͨ�����ͨ�������
*\Return        bool
*                           - TRUE    ƥ��ɹ�
*                           - FALSE   ƥ��ʧ��
*\Note          1) ģʽ�ڴ�����ֻ������ʾ0�����������ֽڵ�ָ��ͨ�����
*\Note          2) �������С >= ͨ����� + 2,����ᷢ�������
*\Note          3) �������0��洢��ƥ���ȫ��Ŀ���ڴ�����,Ȼ�����α���ͨ�����������ڴ�顣
*\Note          4) �ο��� Wildcard matching algorithms    by Alessandro Cantatore
*\Log           2008.08.06    Ver 1.0    gaofeng
*               ����������
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
*\Log           2010.05.15    Ver 1.0    �߷�
*               ����������
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
*\Description   ����ڴ�����src��len���ֽ��Ƿ��ΪBCD�롣
*\Parameter     src
*\Parameter     len
*\Return        bool
*\Log           2008.12.02    Ver 1.0    �Ų�
*               ����������
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
*\Description   ���ڴ�����src��src_len���ֽ�ѭ������shift_len���ֽڡ�
*\Parameter     src         �ڴ��ַ
*\Parameter     src_len     ���鳤��
*\Parameter     shift_len   λ�Ƴ���
*\Return        void
*\Log           2008.12.11    Ver 1.0    �Ų�
*               ����������
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
*\Description   �����ڴ�����src��len���ֽ��Ƿ�ȫ������value
*\Parameter     src         �ڴ��ַ
*\Parameter     len         ���ݳ���
*\Parameter     value       ֵ
*\Return        bool        �Ƿ�ȫ������value
*\Log           2009.03.03    Ver 1.0    �Ų�
*               ����������
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
*\Description   �����ڴ�����src��len���ֽ��Ƿ�ȫ�����
*\Parameter     src     �ڴ��ַ
*\Parameter     len     ���ݳ���
*\Return        bool    �Ƿ�ȫ�����
*\Note
*\Log           2009.03.03    Ver 1.0    �Ų�
*               ����������
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
*\Description   �ڴ����
*\Parameter     dst     Ŀ�ĵ�ַ
*\Parameter     src1    �ڴ��ַ1
*\Parameter     src2    �ڴ��ַ2
*\Parameter     len     ����
*\Return        void
*\Note          1)�������λ��
*\Log           2010.04.09    Ver 1.0    �Ų�
*               ����������
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
*\Description   �ڴ����
*\Parameter     dst     Ŀ�ĵ�ַ
*\Parameter     src1    �ڴ��ַ1
*\Parameter     src2    �ڴ��ַ2
*\Parameter     len     ����
*\Return        void
*\Note          1)�������λ��
*\Log           2010.04.09    Ver 1.0    �Ų�
*               ����������
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
*\Description   �ڴ�����
*\Parameter     dst     Ŀ�ĵ�ַ
*\Parameter     src1    �ڴ��ַ1
*\Parameter     src2    �ڴ��ַ2
*\Parameter     len     ����
*\Return        void
*\Note          1)�������λ��
*\Log           2010.04.09    Ver 1.0    �Ų�
*               ����������
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
*\Description   �ڴ��
*\Parameter     dst     Ŀ�ĵ�ַ
*\Parameter     src1    �ڴ��ַ1
*\Parameter     src2    �ڴ��ַ2
*\Parameter     len     ����
*\Return        void
*\Note          1)�������λ��
*\Log           2010.04.09    Ver 1.0    �Ų�
*               ����������
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
*\Description   �ڴ����
*\Parameter     dst     Ŀ�ĵ�ַ
*\Parameter     src1    �ڴ��ַ1
*\Parameter     src2    �ڴ��ַ2
*\Parameter     len     ����
*\Return        void
*\Note          1)�������λ��
*\Log           2010.04.09    Ver 1.0    �Ų�
*               ����������
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
*�ڴ滥��
***************************************************************************************************/

static MutexType MutexPool[MUTEX_POOL_SIZE] = {0x00};

/***************************************************************************************************
*\Function      MutexSearch
*\Description   �������������
*\Parameter     start_index
*\Parameter     addr
*\Parameter     len
*\Return        u32
*\Note
*\Log           2009.12.12    Ver 1.0    �Ų�
*               ����������
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
                /*�ڻ�����з��ֻ����ַ*/
                return i;
            }
        }
    }

    return MUTEX_SEARCH_FAILED;
}

/***************************************************************************************************
*\Function      MutexApply
*\Description   �ڴ����򻥳�����
*\Parameter     addr        ���������ַ
*\Parameter     len         �������򳤶�
*\Parameter     func        ���⴦����
*\Parameter     func_para   ���⴦��������
*\Parameter     timeout     ����ȴ����Դ���(0:�޳�ʱ����)
*\Return        bool        �Ƿ�����ɹ�
*\Note          1)���ֻ�������func(func_para)
*\Note          2)ѭ������1)timeout��
*\Log           2009.12.12    Ver 1.0    �Ų�
*               ����������
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


    /*�������룬���ҿ�λ*/
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

    /*���Ҳ�����λ*/
    return FALSE;
}

/***************************************************************************************************
*\Function      MutexRelease
*\Description   �����ͷ�
*\Parameter     addr    �ͷŵ�ַ
*\Return        bool    �Ƿ���ڻ���
*\Log           2009.12.12    Ver 1.0    �Ų�
*               ����������
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


