/***************************************************************************************************
*                    (c) Copyright 1992-2010 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          usdl_list.c
*\Description   Linux Kernal˫��ѭ������
*\Log           2010.05.15    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/

#include "usdl_list.h"

/***************************************************************************************************
*\Function      ListHeadInit
*\Description   ��ʼ������ͷ
*\Parameter     ptr
*\Return        void
*\Note          ���Ժ�INIT_LIST_HEAD
*\Log           2010.05.15    Ver 1.0    �Ų�
*               ��������
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
*\Log           2010.05.27    Ver 1.0    �Ų�
*               ��������
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
*\Description   ��һ���ڵ���ӵ�ĳһ����ͷ����������ʵ��ջ���ݽṹ
*\Parameter     new_node     ��Ҫ��ӵ��½ڵ�
*\Parameter     head    ���������ӵ�����ͷ
***************************************************************************************************/
void ListAdd(ListHeadType *new_node, ListHeadType *head)
{
    __list_add(new_node, head, head->next);
}

/***************************************************************************************************
*\Function      ListAddTail
*\Description   ��һ���ڵ���ӵ����������β��(ĳһ����ͷ��ǰ��), ������ʵ�ֶ������ݽṹ��
*\Parameter     new_node     ��Ҫ��ӵ��½ڵ�
*\Parameter     head    ������ǰ��ӵ�����ͷ
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
*\Log           2010.05.27    Ver 1.0    �Ų�
*               ��������
***************************************************************************************************/
static void __list_del(ListHeadType *prev, ListHeadType *next)
{
    next->prev = prev;
    prev->next = next;
}

/***************************************************************************************************
*\Function      ListDel
*\Description   ��һ���������Ƴ�ĳһ�ڵ�
*\Parameter     entry   Ҫ�������Ƴ��Ľڵ�
*\Note          �����Ƴ��Ľڵ����ListEmpty�������᷵��TRUE����Ϊ�ýڵ㴦��δ����״̬��
*\Log           2010.05.15    Ver 1.0    �Ų�
*               ��������
***************************************************************************************************/
void ListDel(ListHeadType *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = (void* ) 0;
    entry->prev = (void* ) 0;
}

/***************************************************************************************************
*\Function      ListDelInit
*\Description   ���������Ƴ�ĳ�ڵ㲢��ʼ������
*\Parameter     entry   ϣ���Ƴ��Ľڵ�
*\Log           2010.05.15    Ver 1.0    �Ų�
*               ��������
***************************************************************************************************/
void ListDelInit(ListHeadType *entry)
{
    __list_del(entry->prev, entry->next);
    ListHeadInit(entry);
}

/***************************************************************************************************
*\Function      ListReplace
*\Description   ��ĳһ�ɽڵ��滻Ϊ�½ڵ�
*\Parameter     old �ɽڵ�
*\Parameter     new_node �½ڵ�
*\Note          �ɽڵ㲻���ǿսڵ㣬����ò�����Ч��
*\Log           2010.05.15    Ver 1.0    �Ų�
*               ��������
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
*\Description   ��ĳһ�ɽڵ��滻Ϊ�½ڵ㣬�ҳ�ʼ���ɽڵ㡣
*\Parameter     old �ɽڵ�
*\Parameter     new_node �½ڵ�
*\Note          �ɽڵ㲻���ǿսڵ㣬����ò�����Ч��
*\Log           2010.05.15    Ver 1.0    �Ų�
*               ��������
***************************************************************************************************/
void ListReplaceInit(ListHeadType* old, ListHeadType* new_node)
{
    ListReplace(old, new_node);
    ListHeadInit(old);
}

/***************************************************************************************************
*\Function      ListMove
*\Description   ���ڵ��һ�������Ƴ�����ӵ���һ�������ͷ����
*\Parameter     entry   ϣ�������Ľڵ�
*\Parameter     head    ��Ҫ��ӵ�������ͷ
*\Log           2010.05.15    Ver 1.0    �Ų�
*               ��������
***************************************************************************************************/
void ListMove(ListHeadType *entry, ListHeadType *head)
{
    __list_del(entry->prev, entry->next);
    ListAdd(entry, head);
}

/***************************************************************************************************
*\Function      ListMove
*\Description   ���ڵ��һ�������Ƴ�����ӵ���һ�������β����
*\Parameter     entry   ϣ�������Ľڵ�
*\Parameter     head    ��Ҫ��ӵ�������ͷ
*\Log           2010.05.15    Ver 1.0    �Ų�
*               ��������
***************************************************************************************************/
void ListMoveTail(ListHeadType *entry, ListHeadType *head)
{
        __list_del(entry->prev, entry->next);
        ListAddTail(entry, head);
}

/***************************************************************************************************
*\Function      ListEmpty
*\Description   �ж�ĳ�����Ƿ�Ϊ�ա�
*\Parameter     head
*\Return        bool
*\Log           2010.05.15    Ver 1.0    �Ų�
*               ��������
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
*\Description   ƴ����������
*\Parameter     list    ��Ҫ��ӵ�����
*\Parameter     head    ������Ҫ��ӵ���һ�������λ��
*\Log           2010.05.15    Ver 1.0    �Ų�
*               ��������
***************************************************************************************************/
void ListSplice(ListHeadType *list, ListHeadType *head)
{
    if (!ListEmpty(list))
        __list_splice(list, head);
}

/***************************************************************************************************
*\Function      ListSplice
*\Description   ƴ������������ʼ������յ�����
*\Parameter     list    ��Ҫ��ӵ�����
*\Parameter     head    ������Ҫ��ӵ���һ�������λ��
*\Note          ����list�ᱻ���³�ʼ����
*\Log           2010.05.15    Ver 1.0    �Ų�
*               ��������
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
*\Description   ��ȡ�����е�ĳһ�ڵ�
*\Parameter     head        ����ͷ
*\Parameter     position    �ڵ���š�
*\Return        ListHeadType*
*\Note          position��ȡֵ˵����
                1�� 1 ��ȡ��һ���ڵ㣬2��ȡ�ڶ����ڵ㣬...
                2�� -1��ȡ������һ���ڵ㣬-2��ȡ�����ڶ����ڵ㣬...
                3�� 0 ��ֱ�ӷ���head
*\Log           2010.05.17    Ver 1.0    �Ų�
*               ��������
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
*\Description   �ѽڵ���뵽�����ĳ��λ��
*\Parameter     head        ͷ�ڵ�
*\Parameter     position    ���뵽��λ��
*\Parameter     new_node    �½ڵ�
*\Return        bool        �Ƿ����ɹ�
*\Note          position��ȡֵ˵����
                1�� 1 �����뵽��һ���ڵ���棬2�����뵽�ڶ����ڵ���棬...
                2�� -1�����뵽������һ���ڵ���棬-2�����뵽�����ڶ����ڵ㣬...
                3�� 0 �����뵽head����
*\Log           2011.05.09    Ver 1.0    �Ų�
*               ����������
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
*\Description   ����λ��ɾ�������е�ĳ���ڵ�
*\Parameter     head        ͷ�ڵ�
*\Parameter     position    �ڵ�λ��
*\Return        bool        �Ƿ�ɾ���ɹ�
*\Note          position��ȡֵ˵����
                1��Ϊ 1��ɾ����һ���ڵ㣬2��ɾ���ڶ����ڵ㣬...
                2��Ϊ -1��ɾ��������һ���ڵ㣬-2��ɾ�������ڶ����ڵ㣬...
                3��Ϊ 0��ɾ��ʧ��
*\Log           2011.05.09    Ver 1.0    �Ų�
*               ����������
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
