#ifndef __ZRing__H
#define __ZRing__H

#include "PublicDefine.h"


typedef struct ZRingNode
{
	struct ZRingNode				*		m_pNext;
	
	void										*		m_pData;
	
}ZRingNode;

ZRingNode * CreateRingNode(void);

void DeleteRingNode(ZRingNode * pRingNode);

typedef struct ZRing
{
	struct ZRingNode		*		m_pFront;
	
	struct ZRingNode		*		m_pTail;
	
	RingDataDeleteFunPtr 		RingDataDelete;
	
	RingIsSameDataFunPtr 		RingIsSameData;
				
}ZRing;

ZRing * CreateRing(uint uiSize,RingDataDeleteFunPtr RingDataDelete,RingIsSameDataFunPtr RingIsSameData);

void Append2Ring(ZRing * pRing,void * pData);

bool ExistsInRing(ZRing * pRing,void * pData);

void DeleteRingNodeAndData(ZRing * pRing, ZRingNode * pRingNode);

void DeleteRing(ZRing * pRing);
#endif //__ZRing__H
