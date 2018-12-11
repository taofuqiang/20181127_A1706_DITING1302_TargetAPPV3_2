/***************************************************************************************************
*                    (c) Copyright 1992-2010 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_io.h
*\Description   IO操作模块实现
*\Note
*\Log           2010.12.02    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#ifndef _USDL_IO_H
#define _USDL_IO_H

#include "usdl_type.h"

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
u32 Printf(void (*output)(u8), const u8* format, ...);

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
u32 Vprintf(void (*output)(u8), const u8* format, VaListType args);

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
u32 Sprintf(u8* buf, const u8* format, ...);

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
u32 Snprintf(u8* buf, u32 size, const u8* format, ...);

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
u32 Vsnprintf(u8* buf, u32 size, const u8* format, VaListType args);

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
u32 Sscanf(const u8* buf, const u8* format, ...);

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
u32 Vsscanf(const u8* buf, const u8* format, VaListType args);

#endif /*_USDL_IO_H*/
