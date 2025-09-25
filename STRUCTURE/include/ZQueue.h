#ifndef __ZQUEUE__H
#define __ZQUEUE__H

#include "PublicDefine.h"

typedef struct ZQueueNode
{
	struct ZQueueNode				*		m_pNext;
	
	void							*		m_pData;
	
}ZQueueNode;

ZQueueNode * CreateQueueNode(void);

void DeleteQueueNode(ZQueueNode * pQueueNode);

typedef struct ZQueue
{
	struct ZQueueNode		*		m_pFront;
	
	struct ZQueueNode		*		m_pTail;
				
}ZQueue;

ZQueue * CreateQueue(void);

bool IsQueueEmpty(ZQueue * pQueue);

void * PopFromQueue(ZQueue * pQueue);

void Push2Queue(ZQueue * pQueue,void * pData);

ZQueueNode * PopNodeFromQueue(ZQueue * pQueue);

void PushNode2Queue(ZQueue * pQueue,ZQueueNode * pQueueNode);

ZQueueNode * GetFirstQueueNode(ZQueue * pQueue);

void QueueClear(ZQueue * pQueue, DeleteDataFunPtr DeleteFun);

void DeleteQueue(ZQueue * pQueue, DeleteDataFunPtr DeleteFun);
#endif //__ZQUEUE__H
