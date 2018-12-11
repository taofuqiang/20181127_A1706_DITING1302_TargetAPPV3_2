/***************************************************************************************************
*                    (c) Copyright 1992-2009 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_tools.h
*\Description   ����ģ�飬�ṩ�ḻ�Ĺ��ߺ�����
*\Log           2007.12.18   Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/

#ifndef _USDL_TOOLS_H
#define _USDL_TOOLS_H

#include "usdl_type.h"
#include "usdl_string.h"


#define CODEFILE ((StrRChr(__FILE__, '\\') == NULL) ? \
    ((u8*)__FILE__) : (StrRChr(__FILE__, '\\') + 1))    /*!<��ȡ��ǰ�ļ���,����·��*/


#define CODELINE (__LINE__) /*!<��ȡ��ǰ����������*/

#define CURDATETIME (__DATE__" "__TIME__)   /*!<��ǰ��������ʱ��*/

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
#define IS_ALNUM(c)     ((IS_MASK(c)&(CTYPE_U|CTYPE_L|CTYPE_D)) != 0)   /*!<�ж��ַ�����c�Ƿ�Ϊ��ĸ������*/
#define IS_ALPHA(c)     ((IS_MASK(c)&(CTYPE_U|CTYPE_L)) != 0)   /*!<�ж��ַ�����c�Ƿ�Ϊ��ĸ*/
#define IS_CNTRL(c)     ((IS_MASK(c)&(CTYPE_C)) != 0)   /*!<�ж��ַ�����c�Ƿ�Ϊ�����ַ�*/
#define IS_DIGIT(c)     ((IS_MASK(c)&(CTYPE_D)) != 0)   /*!<�ж��ַ�����c�Ƿ�Ϊ����*/
#define IS_GRAPH(c)     ((IS_MASK(c)&(CTYPE_P|CTYPE_U|CTYPE_L|CTYPE_D)) != 0)
#define IS_LOWER(c)     ((IS_MASK(c)&(CTYPE_L)) != 0)   /*!<�ж��ַ�����c�Ƿ�ΪСд��ĸ*/
#define IS_PRINT(c)     ((IS_MASK(c)&(CTYPE_P|CTYPE_U|CTYPE_L|CTYPE_D|CTYPE_SP)) != 0)
#define IS_PUNCT(c)     ((IS_MASK(c)&(CTYPE_P)) != 0)
#define IS_SPACE(c)     ((IS_MASK(c)&(CTYPE_S)) != 0)   /*!<�ж��ַ�����c�Ƿ�Ϊ�ո�*/
#define IS_UPPER(c)     ((IS_MASK(c)&(CTYPE_U)) != 0)   /*!<�ж��ַ�����c�Ƿ�Ϊ��С��ĸ*/
#define IS_XDIGIT(c)    ((IS_MASK(c)&(CTYPE_D|CTYPE_X)) != 0)
#define IS_ASCII(c)     (((u8)(c))<=0x7f)       /*!<�ж��ַ�����c�Ƿ�ΪASCII��*/
#define TO_ASCII(c)     (((u8)(c))&0x7f)        /*!<���ַ�����cת��ΪASCII��*/
#define TO_UPPER(c)     (IS_LOWER(c) ? ((c) - 'a' + 'A') : (c)) /*!<���ַ�����cת��Ϊ��д��ĸ*/

extern u64 MemU64DoubleInvalid;

#define INVALID_DOUBLE  (*(double*)&MemU64DoubleInvalid)  /*!<��Чdoubleֵ*/ 

/***************************************************************************************************
*\Def           MinBlockSize
*\Description   �����������data_size�ֽ����ݵ���С�ռ䣬�Ҹÿռ�ɱ�block_size����
*\Parameter     data_size   ���ݴ�С
*\Parameter     block_size  ���С
*\Return        u32         �����С
*\Log           2011.11.02    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
#define MinBlockSize(data_size, block_size) ((((data_size) / (block_size))\
+ (((data_size) % (block_size)) > 0)) * (block_size))

/***************************************************************************************************
*\Def           Swap
*\Description   ��������������ֵ
*\Parameter     x   ����x
*\Parameter     y   ����y
*\Return        u8
*\Log           2008.10.06    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
#define Swap(a, b) {(a)^=(b); (b)^=(a); (a)^=(b);}

/***************************************************************************************************
*\Def           DoubleMem
*\Description   double���ڴ�ֵ
*\Parameter     var   ����var
*\Return        double���ڴ�ֵ
*\Log           2008.10.06    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
#define DoubleMem(var) (*(u64*)(&(var)))

/***************************************************************************************************
*\Def           DoubleSetInvalid
*\Description   ��double��ֵΪ�Ƿ�ֵ
*\Parameter     var   ����var
*\Return        �Ƿ���doubleֵ
*\Log           2008.10.06    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
#define DoubleSetInvalid(var) (*(u64*)(&(var)) = ~(0ULL))

/***************************************************************************************************
*\Def           DoubleIsNumber
*\Description   �ж�double�ĺϷ���
*\Parameter     var   ����var
*\Return        TRUE:�Ϸ���doubleֵ��FALSE:�Ƿ���doubleֵ
*\Log           2008.10.06    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
#define DoubleIsNumber(value) ((value) == (value))    /*!<�ж�double�ĺϷ���*/ 

/***************************************************************************************************
*\Function      GetSub
*\Description   ȡ������ľ���ֵ
*\Parameter     value1
*\Parameter     value2
*\Return        u32
*\Log           2008.01.17    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 GetSub(u32 value1, u32 value2);

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
bool InRange(u32 value, u32 begin, u32 end);

/***************************************************************************************************
*\Function      BCDAdd
*\Description   BCD������
*\Parameter     bcd     BCD
*\Parameter     value   ����DEC
*\Return        u64      ���BCD
*\Note          1)��BCD������ֹͣ���㲢���ز���bcd��
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u64 BCDAdd(u64 bcd, u64 value);

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
u64 BCDSub(u64 bcd, u64 value);

/***************************************************************************************************
*\Function      BCDFix
*\Description   BCD����
*\Parameter     bcd     ����ָ��
*\Return        bool    �Ƿ�����
*\Note          1)��������������0x99����Ϊ0x00����λ�Ϸ�����λ����0x09����Ϊ��λ��1����λΪ0��
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool BCDFix(u8* bcd);

/***************************************************************************************************
*\Function      BCDArrayFix
*\Description   BCD��������
*\Parameter     bcd     BCD����ָ��
*\Return        bool    �Ƿ�����
*\Note          1)��������������0x99����Ϊ0x00����λ�Ϸ�����λ����0x09����Ϊ��λ��1����λΪ0��
*\Log           2009.04.20    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool BCDArrayFix(void* bcd_array, u32 len);

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
void BCDLeftMov(void* bcd_array, u32 len, u32 mov_num);

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
void BCDRightMov(void* bcd_array, u32 len, u32 mov_num);

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
u8 BCDLeftMovU8(u8 bcd, u32 mov_num);

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
u16 BCDLeftMovU16(u16 bcd, u32 mov_num);

/***************************************************************************************************
*\Function      BCDLeftMovU32
*\Description   BCD��ֵ���ơ�
*\Parameter     bcd         �������BCD���u32��������߰���8��BCD��
*\Return        void
*\Note          1)�����������BCD��ȷ�ԡ�
*\Log           2010.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 BCDLeftMovU32(u32 bcd, u32 mov_num);

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
u64 BCDLeftMovU64(u64 bcd, u32 mov_num);

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
u8 BCDRightMovU8(u8 bcd, u32 mov_num);

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
u16 BCDRightMovU16(u16 bcd, u32 mov_num);

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
u32 BCDRightMovU32(u32 bcd, u32 mov_num);

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
u64 BCDRightMovU64(u64 bcd, u32 mov_num);

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
bool WaitFlag(const void* flag, const u8 flag_width, const u32 value, u32 (*wait)(u32), const u32 timeout);

#endif /*_USDL_TOOLS_H*/
