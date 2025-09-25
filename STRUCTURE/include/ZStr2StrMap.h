#ifndef __ZSTR2STR_MAP__H
#define __ZSTR2STR_MAP__H

#include "PublicDefine.h"
#include "ZQueue.h"


typedef struct ZStrStrPair
{
	char										*		m_pKey;
	
	char										*		m_pValue;
	
}ZStrStrPair;

ZStrStrPair * CreateStrStrPair(char * pKey, char * pValue);

void DeleteStrStrPair(ZStrStrPair * pStrStrPair);

typedef struct ZStr2StrMap
{
		ZQueue 			*				m_pQueue;
	
		ZQueueNode	    *				m_pCurrentNode;
	
}ZStr2StrMap;

ZStr2StrMap * CreateStr2StrMap(void);

void PushStrStrPair2Map(ZStr2StrMap * pStr2StrMap,ZStrStrPair * pStrStrPair);

void PushStrKeyStrValue2Map(ZStr2StrMap * pStr2StrMap,char * pKey, char * pValue);

void DeleteStr2StrMap(ZStr2StrMap * pStr2StrMap);

ZStr2StrMap * ParseStr2Map(char * pStr, char * pBigSeperator, char * pSmallSeperator);

char * GetValueFromStr2StrMap(ZStr2StrMap * pStr2StrMap, char * pKey);

char * GetNextKeyFromStr2StrMap(ZStr2StrMap * pStr2StrMap);
#endif //__ZSTR2STR_MAP__H
