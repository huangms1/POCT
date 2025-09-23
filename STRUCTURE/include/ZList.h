#ifndef __ZLIST__H
#define __ZLIST__H

#include "PublicDefine.h"

//链表节点
typedef struct ZListNode
{
	struct ZListNode					*		m_pNext;
	
	struct ZListNode					*		m_pPrevious;
	
	void								*	  m_pData;
	
}ZListNode;


/*创建一个节点*/
ZListNode * CreateListNode(void);

/*删除一个节点*/
void DeleteListNode(ZListNode * pListNode);

/*删除一个节点和数据*/
void DeleteListNodeAndData(ZListNode * pListNode);


typedef struct ZList
{
	struct ZListNode					*		m_pFront;
	
	struct ZListNode					*		m_pTail;

}ZList;

ZList * CreateList(void);

bool IsListEmpty(ZList * pList);

void ListAppend(ZList * pList,void * pData);

void ListPrepend(ZList * pList,void * pData);

void ListAppendNode(ZList * pList,ZListNode *pListNode);

void ListPrependNode(ZList * pList,ZListNode *pListNode);

void * ListTakeFirst(ZList * pList);

void * ListTakeLast(ZList * pList);

ZListNode * ListTakeFirstNode(ZList * pList);

ZListNode * ListTakeLastNode(ZList * pList);

typedef ZListNode * ListIterator;

ListIterator ListRemoveOne(ZList * pList,ListIterator it);

ListIterator ListBegin(ZList * pList);

ListIterator ListEnd(ZList * pList);

void ListClear(ZList * pList);

void DeleteList(ZList * pList);
#endif //__ZLIST__H
