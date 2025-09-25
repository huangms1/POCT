#include <stdio.h>
#include "ZRing.h"
#include "FreeRTOS.h"

ZRingNode * CreateRingNode(void)
{
	ZRingNode * pRingNode = (ZRingNode *)pvPortMalloc(sizeof(ZRingNode));
	
	pRingNode->m_pData = NULL;
	
	pRingNode->m_pNext = NULL;
	
	return pRingNode;
}
void DeleteRingNode(ZRingNode * pRingNode)
{
	if(NULL != pRingNode)
	{
		vPortFree(pRingNode);
	}
}

void DeleteRingNodeAndData(ZRing * pRing, ZRingNode * pRingNode)
{
	if(NULL != pRingNode)
	{
		if(NULL != pRingNode->m_pData)
		{
			pRing->RingDataDelete(pRingNode->m_pData);
		}
		vPortFree(pRingNode);
	}
};

/*队列创建*/
ZRing * CreateRing(uint uiSize,RingDataDeleteFunPtr RingDataDelete,RingIsSameDataFunPtr RingIsSameData)
{
	ZRing * pRing = NULL;
	
	ZRingNode * pLastCreatedRingNode = NULL;
	
	int iLoop = 0;
	
	if(0 == uiSize)
		return NULL;
	
	pRing = (ZRing *)pvPortMalloc(sizeof(ZRing));
	
	pRing->RingDataDelete = RingDataDelete;
	
	pRing->RingIsSameData = RingIsSameData;
	
	for(; iLoop < uiSize; ++ iLoop)
	{
		ZRingNode * pRingNode = CreateRingNode();
		
		if(0 == iLoop)
		{
			pRing->m_pFront = pRingNode;
			
			pLastCreatedRingNode = pRingNode;
		}
		else
		{
			pLastCreatedRingNode->m_pNext = pRingNode;
			
			pLastCreatedRingNode = pRingNode;
		}
		
		if(iLoop == uiSize - 1)
			pRing->m_pTail = pRingNode;
	}
	pRing->m_pTail->m_pNext = pRing->m_pFront;
	
	return pRing;
}

void Append2Ring(ZRing * pRing,void * pData)
{
	pRing->RingDataDelete(pRing->m_pFront->m_pData);
	
	pRing->m_pTail = pRing->m_pFront;
	
	pRing->m_pFront = pRing->m_pFront->m_pNext;
	
	pRing->m_pTail->m_pData = pData;
}

bool ExistsInRing(ZRing * pRing,void * pData)
{
	bool bExists = false;
	
	ZRingNode * pRingNode = pRing->m_pFront;
	
	do
	{
		if(!pRing->RingIsSameData(pRingNode->m_pData, pData))
		{
			pRingNode = pRingNode->m_pNext;
		}
		else
		{
			bExists = true;
			
			break;
		}
		
	}	while(pRingNode != pRing->m_pFront);
	
	return bExists;
}

void DeleteRing(ZRing * pRing)
{
	if(NULL != pRing)
	{
		ZRingNode * pRingNode = pRing->m_pFront;
	
		do
		{
			DeleteRingNodeAndData(pRing, pRingNode);
			
			pRingNode = pRingNode->m_pNext;
			
		}	while(pRingNode != pRing->m_pFront);
	
		vPortFree(pRing);
	}

}

