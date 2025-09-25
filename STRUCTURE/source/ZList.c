#include "ZList.h"
#include "FreeRTOS.h"


/*创建一个链表节点*/
ZListNode * CreateListNode(void)
{
	ZListNode * pListNode = (ZListNode *)pvPortMalloc(sizeof(ZListNode));
	
	pListNode->m_pPrevious = NULL;
	
	pListNode->m_pNext = NULL;
	
	return pListNode;
};

/*删除一个链表节点*/
void DeleteListNode(ZListNode * pListNode)
{
	if(NULL != pListNode)
	{
		vPortFree(pListNode);
	}
};



/*删除节点和数据*/
void DeleteListNodeAndData(ZListNode * pListNode)
{
	if(NULL != pListNode)
	{
		if(NULL != pListNode->m_pData)
		{
			vPortFree(pListNode->m_pData);
		}
		vPortFree(pListNode);
	}
};

/*创建一个链表*/
ZList * CreateList(void)
{
	ZList * pList = (ZList *)pvPortMalloc(sizeof(ZList));
	
	ZListNode * pFrontNode = CreateListNode();//此节点永远在最前

	ZListNode * pTailNode = CreateListNode();//此节点永远在最后
	
	pList->m_pFront = pFrontNode;
	
	pList->m_pTail = pTailNode;
	
	pFrontNode->m_pNext = pTailNode;
	
	pTailNode->m_pPrevious = pFrontNode;
	
	return pList;
};

/*检查链表是否为空*/
bool IsListEmpty(ZList * pList)
{
	return pList->m_pFront->m_pNext == pList->m_pTail;
};


/*向链表添加数据*/
void ListAppend(ZList * pList,void * pData)
{
	ZListNode * pListNode = CreateListNode();
	
	pListNode->m_pData = pData;
	
	pListNode->m_pPrevious = pList->m_pTail->m_pPrevious;
	
	pListNode->m_pNext = pList->m_pTail;
	
	pList->m_pTail->m_pPrevious->m_pNext = pListNode;
		
	pList->m_pTail->m_pPrevious = pListNode;

};


/*读取一个数据*/
void ListPrepend(ZList * pList,void * pData)
{
	ZListNode * pListNode = CreateListNode();
	
	pListNode->m_pData = pData;
	
	pListNode->m_pNext = pList->m_pFront->m_pNext;
	
	pListNode->m_pPrevious = pList->m_pFront;
	
	pList->m_pFront->m_pNext->m_pPrevious = pListNode;
	
	pList->m_pFront->m_pNext = pListNode;
};

/*链表添加节点*/
void ListAppendNode(ZList * pList,ZListNode *pListNode)
{
	pListNode->m_pPrevious = pList->m_pTail->m_pPrevious;
	
	pListNode->m_pNext = pList->m_pTail;
	
	pList->m_pTail->m_pPrevious->m_pNext = pListNode;
		
	pList->m_pTail->m_pPrevious = pListNode;
};


/*删除节点*/
void ListPrependNode(ZList * pList,ZListNode *pListNode)
{
	pListNode->m_pNext = pList->m_pFront->m_pNext;
	
	pListNode->m_pPrevious = pList->m_pFront;
	
	pList->m_pFront->m_pNext->m_pPrevious = pListNode;
	
	pList->m_pFront->m_pNext = pListNode;
};


/*表头*/
void * ListTakeFirst(ZList * pList)
{
	ZListNode * pListFirstNode = NULL;
	
	void * pData = NULL;
	
	if(IsListEmpty(pList))
		return NULL;
	
	pListFirstNode = pList->m_pFront->m_pNext;
	
	pData = pListFirstNode->m_pData;
	
	pListFirstNode->m_pNext->m_pPrevious = pList->m_pFront;
	
	pList->m_pFront->m_pNext = pListFirstNode->m_pNext;
	
	DeleteListNode(pListFirstNode);
	
	return pData;
};


/*查找表尾*/
void * ListTakeLast(ZList * pList)
{
	ZListNode * pListLastNode = NULL;
	
	void * pData = NULL;
	
	if(IsListEmpty(pList))
		return NULL;
	
	pListLastNode = pList->m_pTail->m_pPrevious;
	
	pData = pListLastNode->m_pData;
	
	pListLastNode->m_pPrevious->m_pNext = pList->m_pTail;
	
	pList->m_pTail->m_pPrevious = pListLastNode->m_pPrevious;
	
	DeleteListNode(pListLastNode);
	
	return pData;
};


/*链表头节点*/
ZListNode * ListTakeFirstNode(ZList * pList)
{
	ZListNode * pListFirstNode = NULL;
	
	if(IsListEmpty(pList))
		return NULL;
	
	pListFirstNode = pList->m_pFront->m_pNext;
	
	pListFirstNode->m_pNext->m_pPrevious = pList->m_pFront;
	
	pList->m_pFront->m_pNext = pListFirstNode->m_pNext;
	
	return pListFirstNode;
};


/*添加尾节点*/

ZListNode * ListTakeLastNode(ZList * pList)
{
	ZListNode * pListLastNode = NULL;
	
	if(IsListEmpty(pList))
		return NULL;
	
	pListLastNode = pList->m_pTail->m_pPrevious;
	
	pListLastNode->m_pPrevious->m_pNext = pList->m_pTail;
	
	pList->m_pTail->m_pPrevious = pListLastNode->m_pPrevious;
	
	return pListLastNode;
};


ListIterator ListRemoveOne(ZList * pList,ListIterator it)
{
	ListIterator itNext = NULL;
	
	if(NULL == pList || NULL == it)
		return NULL;
	
	if(IsListEmpty(pList) || it == pList->m_pFront || it == pList->m_pTail)
		return NULL;
	
	it->m_pPrevious->m_pNext = it->m_pNext;
	
	it->m_pNext->m_pPrevious = it->m_pPrevious;
	
	itNext = it->m_pNext;
	
	DeleteListNode(it);
	
	//DeleteListNodeAndData(it);
	
	return itNext;

};

/*链表开始*/
ListIterator ListBegin(ZList * pList)
{
	if(NULL == pList)
		return NULL;
	
	return pList->m_pFront->m_pNext;
};


/*链表结束*/
ListIterator ListEnd(ZList * pList)
{
	if(NULL == pList)
		return NULL;
	
	return pList->m_pTail;
};

void ListClear(ZList * pList)
{
	while(!IsListEmpty(pList))
	{
		ZListNode * pFirstNode = ListTakeFirstNode(pList);
		
		DeleteListNodeAndData(pFirstNode);
	}
};


/*删除链表*/
void DeleteList(ZList * pList)
{
	if(NULL != pList)
	{
		ListClear(pList);
		
		DeleteListNode(pList->m_pFront);
		
		DeleteListNode(pList->m_pTail);
		
		vPortFree(pList);
	}
};
