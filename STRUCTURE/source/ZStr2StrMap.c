#include <stdio.h>
#include "ZStr2StrMap.h"
#include "FreeRTOS.h"

ZStrStrPair * CreateStrStrPair(char * pKey, char * pValue)
{
	ZStrStrPair * pStrStrPair = (ZStrStrPair *)pvPortMalloc(sizeof(ZStrStrPair));
	
	pStrStrPair->m_pKey = NULL;
	
	pStrStrPair->m_pValue = NULL;
	
	if(NULL != pKey)
	{
		int iLength = strlen(pKey);
		
		pStrStrPair->m_pKey = (char *)pvPortMalloc(iLength + 1);
		
		memcpy(pStrStrPair->m_pKey,pKey, iLength + 1);
	}
	if(NULL != pValue)
	{
		int iLength = strlen(pValue);
		
		pStrStrPair->m_pValue = (char *)pvPortMalloc(iLength + 1);
		
		memcpy(pStrStrPair->m_pValue,pValue, iLength + 1);
	}
	
	return pStrStrPair;
}
void DeleteStrStrPair(ZStrStrPair * pStrStrPair)
{
	if(NULL != pStrStrPair)
	{
		if(NULL != pStrStrPair->m_pKey)
		{
			vPortFree(pStrStrPair->m_pKey);
		}
		if(NULL != pStrStrPair->m_pValue)
		{
			vPortFree(pStrStrPair->m_pValue);
		}
		vPortFree(pStrStrPair);
	}
}

ZStr2StrMap * CreateStr2StrMap(void)
{
	ZStr2StrMap * pStr2StrMap = (ZStr2StrMap *)pvPortMalloc(sizeof(ZStr2StrMap));
	
	pStr2StrMap->m_pQueue = CreateQueue();
	
	pStr2StrMap->m_pCurrentNode = pStr2StrMap->m_pQueue->m_pFront;
	
	return pStr2StrMap;
}

void PushStrStrPair2Map(ZStr2StrMap * pStr2StrMap,ZStrStrPair * pStrStrPair)
{
	Push2Queue(pStr2StrMap->m_pQueue,pStrStrPair);
}	

void PushStrKeyStrValue2Map(ZStr2StrMap * pStr2StrMap,char * pKey, char * pValue)
{
	ZStrStrPair * pStrStrPair = CreateStrStrPair(pKey, pValue);
	
	Push2Queue(pStr2StrMap->m_pQueue,pStrStrPair);
}

static void DeleteStrStrPairFun(void * pData)
{
	DeleteStrStrPair((ZStrStrPair *) pData);
}

void DeleteStr2StrMap(ZStr2StrMap * pStr2StrMap)
{
	if(NULL != pStr2StrMap)
	{
		DeleteQueue(pStr2StrMap->m_pQueue, DeleteStrStrPairFun);

		vPortFree(pStr2StrMap);
	}
}

ZStr2StrMap * ParseStr2Map(char * pStr, char * pBigSeperator, char * pSmallSeperator)
{
	int iFullLength = 0;
	
	char * pFullStr = NULL;
	
	ZStr2StrMap * pStr2StrMap = NULL;
	
	if(NULL == pStr || NULL == pSmallSeperator)
		return NULL;
	iFullLength = strlen(pStr);
	
	pFullStr = (char *)pvPortMalloc(iFullLength + 1);
	
	memcpy(pFullStr, pStr, iFullLength + 1);
	
	pStr2StrMap = CreateStr2StrMap();
	
	if(NULL == pBigSeperator)
	{
		char * pValue = NULL;
		
		char * pKey = strtok(pFullStr, pSmallSeperator);
		
		if(NULL != pKey)
			pValue = strtok(NULL, pSmallSeperator);
		
		PushStrKeyStrValue2Map(pStr2StrMap,pKey, pValue);
	}
	else
	{
		ZQueue * pSegmentStrQueue = CreateQueue();
			
		char * pSegmentStr = strtok(pFullStr, pBigSeperator);
		
		while(NULL != pSegmentStr)
		{
			int iSegmentLength = strlen(pSegmentStr);
	
			char * pSegmentStrInQueue = (char *)pvPortMalloc(iSegmentLength + 1);
			
			memcpy(pSegmentStrInQueue, pSegmentStr, iSegmentLength + 1);
			
			Push2Queue(pSegmentStrQueue,pSegmentStrInQueue);
			
			pSegmentStr = strtok(NULL, pBigSeperator);
		}
		while(!IsQueueEmpty(pSegmentStrQueue))
		{
			char * pSegment = PopFromQueue(pSegmentStrQueue);
			
			char * pValue = NULL;
		
			char * pKey = strtok(pSegment, pSmallSeperator);
			
			if(NULL != pKey)
				pValue = strtok(NULL, pSmallSeperator);
			
			PushStrKeyStrValue2Map(pStr2StrMap,pKey, pValue);
			
			vPortFree(pSegment);
		}
		DeleteQueue(pSegmentStrQueue, NULL);
	}
	
	vPortFree(pFullStr);
	
	return pStr2StrMap;
}

char * GetValueFromStr2StrMap(ZStr2StrMap * pStr2StrMap, char * pKey)
{
	ZQueueNode * pQueueNode = NULL;
	
	ZStrStrPair* pStrStrPair = NULL;
	
	if(NULL == pStr2StrMap || NULL == pKey)
		return NULL;
	
	if(IsQueueEmpty(pStr2StrMap->m_pQueue))
		return NULL;
	
	pQueueNode = pStr2StrMap->m_pQueue->m_pFront;
	
	while(NULL != pQueueNode && NULL != pQueueNode->m_pData)
	{
		pStrStrPair = (ZStrStrPair*)pQueueNode->m_pData;
		
		if(!strcmp(pKey,pStrStrPair->m_pKey))
		{
			return pStrStrPair->m_pValue;
		}
		pQueueNode = pQueueNode->m_pNext;
	}
	return NULL;
}

char * GetNextKeyFromStr2StrMap(ZStr2StrMap * pStr2StrMap)
{
	ZStrStrPair* pStrStrPair = NULL;
	
	if(NULL != pStr2StrMap->m_pCurrentNode->m_pData)
	{
		pStrStrPair = (ZStrStrPair*)pStr2StrMap->m_pCurrentNode->m_pData;
		
		pStr2StrMap->m_pCurrentNode = pStr2StrMap->m_pCurrentNode->m_pNext;
		
		return pStrStrPair->m_pKey;
	}
	else
	{
		pStr2StrMap->m_pCurrentNode = pStr2StrMap->m_pQueue->m_pFront;
		
		return NULL;
	}
		
	
}
