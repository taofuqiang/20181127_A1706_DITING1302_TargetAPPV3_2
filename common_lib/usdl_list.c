/***************************************************************************************************
*                    (c) Copyright 1992-2010 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_list.c
*\Description   Linux Kernal双向循环链表。
*\Log           2010.05.15    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/

#include "usdl_list.h"

/***************************************************************************************************
*\Function      ListHeadInit
*\Description   初始化链表头
*\Parameter     ptr
*\Return        void
*\Note          改自宏INIT_LIST_HEAD
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListHeadInit(ListHeadType* ptr)
{
    ptr->next = ptr; ptr->prev = ptr;
}

/***************************************************************************************************
*\Function      __list_add
*\Description   Insert a new_node entry between two known consecutive entries.
*\Parameter     new_node
*\Parameter     prev
*\Parameter     next
*\Note          This is only for internal list manipulation where we know the prev/next entries
                already!
*\Log           2010.05.27    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
static void __list_add(ListHeadType *new_node,
                  ListHeadType *prev,
                  ListHeadType *next)
{
    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}

/***************************************************************************************************
*\Function      ListAdd
*\Description   将一个节点添加到某一链表头部，可用于实现栈数据结构
*\Parameter     new_node     所要添加的新节点
*\Parameter     head    将它向后添加的链表头
***************************************************************************************************/
void ListAdd(ListHeadType *new_node, ListHeadType *head)
{
    __list_add(new_node, head, head->next);
}

/***************************************************************************************************
*\Function      ListAddTail
*\Description   将一个节点添加到整个链表的尾部(某一链表头的前面), 可用于实现队列数据结构。
*\Parameter     new_node     所要添加的新节点
*\Parameter     head    将它向前添加的链表头
***************************************************************************************************/
void ListAddTail(ListHeadType *new_node, ListHeadType *head)
{
    __list_add(new_node, head->prev, head);
}

/***************************************************************************************************
*\Function      __list_del
*\Description   Delete a list entry by making the prev/next entries point to each other.
*\Parameter     prev
*\Parameter     next
*\Return        void
*\Note          This is only for internal list manipulation where we know the prev/next entries
*\Log           2010.05.27    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
static void __list_del(ListHeadType *prev, ListHeadType *next)
{
    next->prev = prev;
    prev->next = next;
}

/***************************************************************************************************
*\Function      ListDel
*\Description   从一个链表中移除某一节点
*\Parameter     entry   要从链表移除的节点
*\Note          对所移除的节点进行ListEmpty操作不会返回TRUE，因为该节点处于未定义状态。
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListDel(ListHeadType *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = (void* ) 0;
    entry->prev = (void* ) 0;
}

/***************************************************************************************************
*\Function      ListDelInit
*\Description   从链表中移除某节点并初始化它。
*\Parameter     entry   希望移除的节点
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListDelInit(ListHeadType *entry)
{
    __list_del(entry->prev, entry->next);
    ListHeadInit(entry);
}

/***************************************************************************************************
*\Function      ListReplace
*\Description   将某一旧节点替换为新节点
*\Parameter     old 旧节点
*\Parameter     new_node 新节点
*\Note          旧节点不能是空节点，否则该操作无效。
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListReplace(ListHeadType* old, ListHeadType* new_node)
{
    new_node->next = old->next;
    new_node->next->prev = new_node;
    new_node->prev = old->prev;
    new_node->prev->next = new_node;
}

/***************************************************************************************************
*\Function      ListReplaceInit
*\Description   将某一旧节点替换为新节点，且初始化旧节点。
*\Parameter     old 旧节点
*\Parameter     new_node 新节点
*\Note          旧节点不能是空节点，否则该操作无效。
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListReplaceInit(ListHeadType* old, ListHeadType* new_node)
{
    ListReplace(old, new_node);
    ListHeadInit(old);
}

/***************************************************************************************************
*\Function      ListMove
*\Description   将节点从一个链表移除并添加到另一个链表的头部。
*\Parameter     entry   希望操作的节点
*\Parameter     head    所要添加到的链表头
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListMove(ListHeadType *entry, ListHeadType *head)
{
    __list_del(entry->prev, entry->next);
    ListAdd(entry, head);
}

/***************************************************************************************************
*\Function      ListMove
*\Description   将节点从一个链表移除并添加到另一个链表的尾部。
*\Parameter     entry   希望操作的节点
*\Parameter     head    所要添加到的链表头
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListMoveTail(ListHeadType *entry, ListHeadType *head)
{
        __list_del(entry->prev, entry->next);
        ListAddTail(entry, head);
}

/***************************************************************************************************
*\Function      ListEmpty
*\Description   判断某链表是否为空。
*\Parameter     head
*\Return        bool
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
bool ListEmpty(ListHeadType *head)
{
    return (bool)(head->next == head);
}

static void __list_splice(ListHeadType *list, ListHeadType *head)
{
    ListHeadType *first = list->next;
    ListHeadType *last = list->prev;
    ListHeadType *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}


/***************************************************************************************************
*\Function      ListSplice
*\Description   拼接两个链表
*\Parameter     list    所要添加的链表
*\Parameter     head    链表所要添加到第一个链表的位置
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListSplice(ListHeadType *list, ListHeadType *head)
{
    if (!ListEmpty(list))
        __list_splice(list, head);
}

/***************************************************************************************************
*\Function      ListSplice
*\Description   拼接两个链表并初始化被清空的链表
*\Parameter     list    所要添加的链表
*\Parameter     head    链表所要添加到第一个链表的位置
*\Note          参数list会被重新初始化。
*\Log           2010.05.15    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
void ListSpliceInit(ListHeadType *list, ListHeadType *head)
{
    if (!ListEmpty(list)) {
        __list_splice(list, head);
        ListHeadInit(list);
    }
}


/***************************************************************************************************
*\Function      ListGet
*\Description   获取链表中的某一节点
*\Parameter     head        链表头
*\Parameter     position    节点序号。
*\Return        ListHeadType*
*\Note          position的取值说明：
                1） 1 ，取第一个节点，2，取第二个节点，...
                2） -1，取倒数第一个节点，-2，取倒数第二个节点，...
                3） 0 ，直接返回head
*\Log           2010.05.17    Ver 1.0    张波
*               整理函数。
***************************************************************************************************/
ListHeadType* ListGet(ListHeadType* head, s32 position)
{
    ListHeadType* ret_head = NULL;

    if (position == 0)
    {
        return head;
    }

    if (position > 0)
    {
        ListForEach(ret_head, head)
        {
            position--;
            if (position == 0)
            {
                return ret_head;
            }            
        }
    }
    else
    {
        ListForEachReverse(ret_head, head)
        {
            position++;
            if (position == 0)
            {
                return ret_head;
            }
        }
    }
    return NULL;
}

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
bool ListInsert(ListHeadType* head, s32 position, ListHeadType* new_node)
{
    ListHeadType* pos_node = ListGet(head, position);

    if (pos_node == NULL)
    {
        return FALSE;
    }

    __list_add(new_node, pos_node, pos_node->next);

    return TRUE;
}

/***************************************************************************************************
*\Function      ListDelByPosition
*\Description   按照位置删除链表中的某个节点
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
bool ListDelByPosition(ListHeadType* head, s32 position)
{
    ListHeadType* pos_node = ListGet(head, position);
    
    if (pos_node == NULL)
    {
        return FALSE;
    }

    ListDel(pos_node);

    return TRUE;
}
