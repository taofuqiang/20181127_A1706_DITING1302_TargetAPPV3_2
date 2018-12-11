/***************************************************************************************************
*                    (c) Copyright 1992-2010 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_list.h
*\Description   Linux Kernal双向循环链表。
*\Log           2010.04.07    Ver 1.0   张波
*\              创建文件。
*\              2011.04.18              张波    
*\              修改了注释
*               
***************************************************************************************************/
#ifndef _USDL_LIST_H
#define _USDL_LIST_H

#include "usdl_type.h"

/***************************************************************************************************
*\Struct        ListHeadType
*\Description   链表节点定义
***************************************************************************************************/
typedef struct ListHeadType_st {
    struct ListHeadType_st *next;   /*!<后一个节点*/
    struct ListHeadType_st *prev;   /*!<前一个节点*/
}ListHeadType;

#define LIST_HEAD_INIT(name) { &(name), &(name) }   /*!<初始化一个头节点*/

#define LIST_HEAD(name) ListHeadType name = LIST_HEAD_INIT(name)    /*!<定义一个节点*/

/***************************************************************************************************
*\Function      ListHeadInit
*\Description   初始化链表头节点
*\Parameter     ptr     链表的头节点
*\Return        void
*\Note          该函数把一个链表初始化成为一个空链表，即头节点的next、prev均指向自己，没有任何成员。
*\Log           2010.05.15    Ver 1.0
*               整理函数。
***************************************************************************************************/
void ListHeadInit(ListHeadType* ptr);

/***************************************************************************************************
*\Function      ListAdd
*\Description   将一个节点添加到链表头部，可用于实现栈数据结构
*\Parameter     new_node        要添加的节点
*\Parameter     head            链表的头节点
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListAdd(ListHeadType *new_node, ListHeadType *head);

/***************************************************************************************************
*\Function      ListAddTail
*\Description   将一个节点添加到链表的尾部(链表头节点的前面), 可用于实现队列数据结构
*\Parameter     new_node        要添加的节点
*\Parameter     head            链表的头节点
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListAddTail(ListHeadType *new_node, ListHeadType *head);

/***************************************************************************************************
*\Function      ListDel
*\Description   从链表中移除某一节点
*\Parameter     entry           要移除的节点
*\Note          节点被移除后未被初始化，处于不安全的不确定状态，如需要初始化请使用：ListDelInit。
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListDel(ListHeadType *entry);

/***************************************************************************************************
*\Function      ListDelInit
*\Description   从链表中移除某一节点并初始化它
*\Parameter     entry           要移除的节点
*\Note          节点被移除后未被初始化，即该节点的next、prev均指向自己。
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListDelInit(ListHeadType *entry);

/***************************************************************************************************
*\Function      ListReplace
*\Description   把一个节点替换为另一个节点
*\Parameter     old             旧节点
*\Parameter     new_node        新节点
*\Note          1. 旧节点不能是空节点，否则该操作无效。
*\              2. 旧节点被替换掉后，不会被初始化，如果需要替换后初始化请使用：ListReplaceInit。
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListReplace(ListHeadType* old, ListHeadType* new_node);

/***************************************************************************************************
*\Function      ListReplaceInit
*\Description   将某一旧节点替换为新节点，且初始化旧节点。
*\Parameter     old             旧节点
*\Parameter     new_node        新节点
*\Note          1. 旧节点不能是空节点，否则该操作无效。
*\              2. 旧节点被替换掉后，会被初始化，即该节点的：next、prev均指向自己。
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListReplaceInit(ListHeadType* old, ListHeadType* new_node);

/***************************************************************************************************
*\Function      ListMove
*\Description   把节点从一个链表移除并添加到另一个链表的头部
*\Parameter     entry           要移动的节点
*\Parameter     head            要移动到的目的链表的头节点
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListMove(ListHeadType *entry, ListHeadType *head);

/***************************************************************************************************
*\Function      ListMoveTail
*\Description   将节点从一个链表移除并添加到另一个链表的尾部
*\Parameter     entry           要移动的节点
*\Parameter     head            要移动到的目的链表的头节点
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListMoveTail(ListHeadType *entry, ListHeadType *head);

/***************************************************************************************************
*\Function      ListEmpty
*\Description   判断某链表是否为空。
*\Parameter     head            要判断的链表头节点
*\Return        bool            是否为空
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
bool ListEmpty(ListHeadType *head);

/***************************************************************************************************
*\Function      ListSplice
*\Description   拼接两个链表，即把一个链表插入到另一个链表中
*\Parameter     list            要插入的链表的头节点
*\Parameter     head            list要插入到另一个链表的哪个位置
*\Note          1. list插入到另一个链表后被清空。
*\              2. 头节点list不会被重新初始化，继续可能导致意想不到的后果，
*\                 如果需要插入后进行初始化，请使用：ListSpliceInit。
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListSplice(ListHeadType *list, ListHeadType *head);

/***************************************************************************************************
*\Function      ListSpliceInit
*\Description   拼接两个链表，即把一个链表插入到另一个链表中，并且初始化被清空的链表
*\Parameter     list            要插入的链表的头节点
*\Parameter     head            list要插入到另一个链表的哪个位置
*\Note          1. list插入到另一个链表后被清空，并且头节点list会被重新初始化，next、prev均指向自己。
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListSpliceInit(ListHeadType *list, ListHeadType *head);

/***************************************************************************************************
*\Macro         ListEntry
*\Description   获取节点对应的用户结构的指针
*\Parameter     ptr             节点，通过该节点获取对应的用户结构
*\Parameter     pos             用户结构的指针
*\Parameter     member          用户结构中ListHeadType类型成员名称（该节点在用户结构中的成员名称）。
*\Note          1. pos可以为任意一个用户结构的指针
*\              2. 该函数通过ptr减去member在pos中的偏移来获取用户结构指针
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
#define ListEntry(ptr, pos, member) \
    ((void* )((u8*)(ptr)-(u32)((u32)(&pos->member) - (u32)(pos))))


/***************************************************************************************************
*\Macro         ListForEach
*\Description   遍历链表
*\Parameter     pos             遍历过程中存放当前遍历到的节点
*\Parameter     head            链表头节点
*\Note          1. 在遍历过程中请不要删除、修改pos，这样可能导致链表发生错误，
*\                 如果对节点进行删除、修改，请使用ListForEachSafe，进行安全的遍历
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
#define ListForEach(pos, head) \
    for (pos = (head)->next; pos != (head); \
            pos = pos->next)

/***************************************************************************************************
*\Macro         ListForEachReverse
*\Description   反向遍历链表
*\Parameter     pos             遍历过程中存放当前遍历到的节点
*\Parameter     head            链表头节点
*\Note          1. 在遍历过程中请不要删除、修改pos，这样可能导致链表发生错误                 
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
#define ListForEachReverse(pos, head) \
    for (pos = (head)->prev; pos != (head); \
            pos = pos->prev)

/***************************************************************************************************
*\Macro         ListForEachSafe
*\Description   安全地遍历链表节点，用于在遍历的过程中需要移除或修改遍历到的节点的情况
*\Parameter     pos             遍历过程中存放当前遍历到的节点
*\Parameter     n               用于临时存储ListHeadType指针(必须是真正的变量)
*\Parameter     head            链表头节点
*\Note          1. 该函数使用n来备份当前遍历的节点，以防止用户删除、修改pos对链表的影响
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
#define ListForEachSafe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/***************************************************************************************************
*\Macro         ListForEachEntry
*\Description   遍历链表中的用户结构
*\Parameter     pos             遍历过程中存放当前遍历到的用户结构指针
*\Parameter     head            链表头指针
*\Parameter     member          用户结构中ListHeadType类型成员名称
*\              1. 在遍历过程中请不要删除、修改pos，这样可能导致链表发生错误，
*\                 如果对节点进行删除、修改，请使用ListForEachEntrySafe，进行安全的遍历
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
#define ListForEachEntry(pos, head, member)                 \
    for (pos = ListEntry((head)->next, pos, member);    \
         &pos->member != (head);                    \
         pos = ListEntry(pos->member.next, pos, member))

/***************************************************************************************************
*\Macro         ListForEachEntrySafe
*\Description   安全地遍历链表中的用户结构，用于在遍历的过程中删除、修改遍历到的用户结构的情况。
*\Parameter     pos             遍历过程中存放当前遍历到的用户结构指针
*\Parameter     n               作为临时存储的用户结构指针(必须是真正的变量)
*\Parameter     head            链表头节点
*\Parameter     member          用户结构中ListHeadType类型成员名称
*\Note          1. 该函数使用n来备份当前遍历的节点，以防止用户删除、修改pos对链表的影响
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
#define ListForEachEntrySafe(pos, n, head, member)          \
    for (pos = ListEntry((head)->next, pos, member),    \
        n = ListEntry(pos->member.next, pos, member);   \
        &pos->member != (head);                     \
        pos = n, n = ListEntry(n->member.next, n, member))


/***************************************************************************************************
*\Macro         ListForEachEntryReverse
*\Description   反向遍历链表中的用户结构
*\Parameter     pos             遍历过程中存放当前遍历到的用户结构指针
*\Parameter     head            链表头指针
*\Parameter     member          用户结构中ListHeadType类型成员名称
*\Note          1. 在遍历过程中请不要删除、修改pos，这样可能导致链表发生错误，
*\                 如果对节点进行删除、修改，请使用ListForEachEntrySafe，进行安全的遍历
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
#define ListForEachEntryReverse(pos, head, member)          \
    for (pos = ListEntry((head)->prev, pos, member);    \
        &pos->member != (head);     \
        pos = ListEntry(pos->member.prev, pos, member))

/***************************************************************************************************
*\Macro         ListForEachEntrySafeReverse
*\Description   安全地反向遍历链表中的用户结构，用于在遍历的过程中移除遍历到的用户结构的情况。
*\Parameter     pos             遍历过程中存放当前遍历到的用户结构指针
*\Parameter     n               作为临时存储的用户结构指针(必须是真正的变量)
*\Parameter     head            链表头节点
*\Note          1. 该函数使用n来备份当前遍历的节点，以防止用户删除、修改pos对链表的影响
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
#define ListForEachEntrySafeReverse(pos, n, head, member)       \
    for (pos = ListEntry((head)->prev, pos, member),    \
        n = ListEntry(pos->member.prev, pos, member);   \
         &pos->member != (head);                    \
         pos = n, n = ListEntry(n->member.prev, n, member))

/***************************************************************************************************
*\Function      ListGet
*\Description   获取链表中的某一节点
*\Parameter     head        链表头
*\Parameter     position    节点序号。
*\Return        ListHeadType*
*\Note          position的取值说明：
                1） 1，取第一个节点，2，取第二个节点，...
                2） -1，取倒数第一个节点，-2，取倒数第二个节点，...
                3） 0，直接返回head
*\Log           2010.05.17    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
ListHeadType* ListGet(ListHeadType* head, s32 position);

/***************************************************************************************************
*\Function      ListInsert
*\Description   把节点插入到链表的某个位置
*\Parameter     head        头节点
*\Parameter     position    插入到的位置
*\Parameter     new_node    新节点
*\Return        bool        是否插入成功
*\Note          position的取值说明：
                1） 1 ，插入到第一个节点后面，2，插入到第二个节点后面，...
                2） -1，插入到倒数第一个节点后面，-2，插入到倒数第二个节点，...
                3） 0 ，插入到head后面
*\Log           2011.05.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool ListInsert(ListHeadType* head, s32 position, ListHeadType* new_node);

/***************************************************************************************************
*\Function      ListDelByPosition
*\Description   删除链表中的某个节点
*\Parameter     head        头节点
*\Parameter     position    节点位置
*\Return        bool        是否删除成功
*\Note          position的取值说明：
                1）为 1，删除第一个节点，2，删除第二个节点，...
                2）为 -1，删除倒数第一个节点，-2，删除倒数第二个节点，...
                3）为 0，删除失败
*\Log           2011.05.09    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
bool ListDelByPosition(ListHeadType* head, s32 position);

#endif /*_USDL_LIST_H*/
