#include <stdio.h>
#include "ZQueue.h"
#include "FreeRTOS.h"

ZQueueNode * CreateQueueNode(void)
{
	ZQueueNode * pQueueNode = (ZQueueNode *)pvPortMalloc(sizeof(ZQueueNode));
	
	pQueueNode->m_pNext = NULL;
	
	pQueueNode->m_pData = NULL;
	
	return pQueueNode;
}
void DeleteQueueNode(ZQueueNode * pQueueNode)
{
	if(NULL != pQueueNode)
	{
		vPortFree(pQueueNode);
	}
}

/*队列创建*/
ZQueue * CreateQueue(void)
{
	ZQueueNode * pQueueNode = NULL;
	
	ZQueue * pQueue = (ZQueue *)pvPortMalloc(sizeof(ZQueue));
	
	pQueueNode = CreateQueueNode();
	
	pQueue->m_pFront = pQueueNode;
	
	pQueue->m_pTail = pQueueNode;
	
	return pQueue;
}

bool IsQueueEmpty(ZQueue * pQueue)
{
	if(pQueue->m_pFront == pQueue->m_pTail)
		return true;
	else
		return false;
}

void * PopFromQueue(ZQueue * pQueue)
{
	ZQueueNode * pQueueFirstNode = NULL;
	
	void * pData = NULL;
	
	if(IsQueueEmpty(pQueue))
		return NULL;
	
	pQueueFirstNode = pQueue->m_pFront;
	
	pData = pQueueFirstNode->m_pData;
	
	pQueue->m_pFront = pQueue->m_pFront->m_pNext;
	
	DeleteQueueNode(pQueueFirstNode);
	
	return pData;
}

void Push2Queue(ZQueue * pQueue,void * pData)
{
	ZQueueNode * pQueueNode = CreateQueueNode();
	
	pQueue->m_pTail->m_pData = pData;
	
	pQueue->m_pTail->m_pNext = pQueueNode;
	
	pQueue->m_pTail = pQueueNode;
}	

ZQueueNode * PopNodeFromQueue(ZQueue * pQueue)
{
	ZQueueNode * pQueueFirstNode = NULL;
	
	if(IsQueueEmpty(pQueue))
		return NULL;
	
	pQueueFirstNode = pQueue->m_pFront;
	
	pQueue->m_pFront = pQueue->m_pFront->m_pNext;
	
	return pQueueFirstNode;
};

void PushNode2Queue(ZQueue * pQueue,ZQueueNode * pQueueNode)
{
	pQueue->m_pTail->m_pNext = pQueueNode;
	
	pQueue->m_pTail = pQueueNode;
};

ZQueueNode * GetFirstQueueNode(ZQueue * pQueue)
{
	if(NULL == pQueue || IsQueueEmpty(pQueue))
		return NULL;
	
	return pQueue->m_pFront;
}

void QueueClear(ZQueue * pQueue, DeleteDataFunPtr DeleteFun)
{
	ZQueueNode * pQueueNode = NULL;
	
	while(!IsQueueEmpty(pQueue))
	{
		pQueueNode = PopNodeFromQueue(pQueue);
		
		if(NULL != pQueueNode->m_pData && NULL != DeleteFun)
			DeleteFun(pQueueNode->m_pData);
		
		DeleteQueueNode(pQueueNode);
	}
};

void DeleteQueue(ZQueue * pQueue, DeleteDataFunPtr DeleteFun)
{
	if(NULL != pQueue)
	{
		ZQueueNode * pCurrentQueueNode = pQueue->m_pFront;
	
		ZQueueNode * pNextQueueNode = NULL;
	
		while(NULL != pCurrentQueueNode)
		{
			pNextQueueNode = pCurrentQueueNode->m_pNext;
			
			if(NULL != pCurrentQueueNode->m_pData && NULL != DeleteFun)
				DeleteFun(pCurrentQueueNode->m_pData);
		
			DeleteQueueNode(pCurrentQueueNode);
		
			pCurrentQueueNode = pNextQueueNode;
		}
		vPortFree(pQueue);
	}

}

