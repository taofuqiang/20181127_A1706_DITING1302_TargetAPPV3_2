/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_mem.h
*\Description   �ڴ����ģ�飬��C��׼�ڴ溯���������䡣
*\Log           2008.06.24    Ver 1.0    �Ų�
*               �����ļ���
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
*�ڴ滥��
***************************************************************************************************/

typedef struct MutexType_st
{
    DataNode    Obj;
    bool        Valid;
}MutexType;

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
void MemAdd(u8* ptr, const u8 value, u32 length);

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
void MemSub(u8* ptr, const u8 value, u32 length);

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
void MemAnd(u8* ptr, const u8 value, u32 length);

/***************************************************************************************************
*\Function      MemNot
*\Description   ptrָ����ڴ������length���ֽ�ȡ����
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void MemNot(u8* ptr, const u32 length);

/***************************************************************************************************
*\Function      MemBitNot
*\Description   ptrָ����ڴ������length���ֽڰ�λȡ����
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void MemBitNot(u8* ptr, const u32 length);

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
void MemOr(u8* ptr, const u8 value, const u32 length);

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
void MemXor(u8* ptr, const u8 value, const u32 length);

/***************************************************************************************************
*\Function      MemSum
*\Description   �����ڴ�����ptr��length���ֽڵĺ�
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        u32
*\Log           2008.03.21    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 MemSum(const u8* ptr, const u32 length);

/***************************************************************************************************
*\Function      MemAllAnd
*\Description   �����ڴ�����ptr��length���ֽڵ�And
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        u32
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 MemAllAnd(const u8* ptr, const u32 length);

/***************************************************************************************************
*\Function      MemAllOr
*\Description   �����ڴ�����ptr��length���ֽڵ�Or
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        u32
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 MemAllOr(const u8* ptr, const u32 length);

/***************************************************************************************************
*\Function      MemAllXor
*\Description   �����ڴ�����ptr��length���ֽڵ�Xor
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        u32
*\Log           2009.10.12    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 MemAllXor(const u8* ptr, const u32 length);

/***************************************************************************************************
*\Function      MemReverse
*\Description   ptrָ����ڴ������е�length���ֽ�����
*\Parameter     ptr     ָ���ڴ������ָ��
*\Parameter     length  �ڴ�����ĳ���
*\Return        void
*\Log           2008.06.24    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void MemReverse(void* ptr, const u32 length);

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
void MemReverseCpy(u8* dst, const u8* src, u32 len);

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
u8* MemLineToCycle(u8* dst, const u32 dst_size, u32 start_offset, u8* src, u32 len);

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
u32 MemCycleToLine(u8* dst, const u8* src, const u32 start_pos, const u32 end_pos, const u32 src_size);

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
                      const u32 src_size, const u32 request_len);

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
*\Log           2010.05.15    Ver 1.0    �߷�
*               ����������
***************************************************************************************************/
u8* MemMem(u8* b1, const u32 len1, u8* b2, const u32 len2);

/***************************************************************************************************
*\Function      MemBCDCheck
*\Description   ����ڴ�����src��len���ֽ��Ƿ��ΪBCD�롣
*\Parameter     src
*\Parameter     head_len
*\Return        bool
*\Log           2008.12.02    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool MemBCDCheck(const u8* src, const u32 head_len);

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
void MemShiftR(u8* src, const u32 src_len, u32 shift_len);

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
bool MemAllSameValue(const u8* src, u16 len, const u8 value);

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
bool MemAllSame(u8* src, u32 len);

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
void MemAddMem(u8* dst, u8* src1, u8* src2, u32 len);

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
void MemSubMem(u8* dst, u8* src1, u8* src2, u32 len);

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
void MemAndMem(u8* dst, u8* src1, u8* src2, u32 len);

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
void MemOrMem(u8* dst, u8* src1, u8* src2, u32 len);

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
void MemXorMem(u8* dst, u8* src1, u8* src2, u32 len);

/***************************************************************************************************
*�ڴ滥��
***************************************************************************************************/

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
bool MutexApply(void* addr, u32 len, u32 (*func)(u32), u32 func_para, u32 timeout);

/***************************************************************************************************
*\Function      MutexRelease
*\Description   �����ͷ�
*\Parameter     addr    �ͷŵ�ַ
*\Return        bool    �Ƿ���ڻ���
*\Log           2009.12.12    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool MutexRelease(void* addr);

#endif /*_USDL_MEM_H*/
