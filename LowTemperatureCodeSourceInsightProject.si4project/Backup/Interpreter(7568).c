#include "FreeRTOS.h"
#include "Interpreter.h"


bool Interprete(SMsg * pMsg)
{
	if(!InterpreteHeader(pMsg))
		return false;
	
	if(SYN_HEAD_BYTES + LENGTH_FIELD_BYTES \
			+ ADDR_FIELD_BYTES + NUMBER_FIELD_BYTES \
			+ TYPE_FIELD_BYTES + CHECK_SUM_BYTES \
			+ SYN_END_BYTES != pMsg->m_pRawFrame->m_uiSize)
	{
		if(!InterpreteAppMsg(pMsg))
			return false;
	}
	pMsg->m_bIsInterpreted = true;
	
	return true;
};

bool InterpreteHeader(SMsg * pMsg)
{
	SMsgHeader * pMsgHeader = NULL;
	
	if(pMsg->m_pRawFrame->m_uiSize <= sizeof(SMsgHeader))
	{
		//报文格式错误
		return false;
	}
	pMsgHeader = (SMsgHeader *) pMsg->m_pRawFrame->m_pData;
	
	pMsg->m_pHeader->m_ucSynHead = pMsgHeader->m_ucSynHead;
	
	pMsg->m_pHeader->m_usFrameLenH = pMsgHeader->m_usFrameLenH;
	
	pMsg->m_pHeader->m_usFrameLenL = pMsgHeader->m_usFrameLenL;
	
	pMsg->m_pHeader->m_ucAddr = pMsgHeader->m_ucAddr;
	
	pMsg->m_pHeader->m_ucTransFrameBelong = pMsgHeader->m_ucTransFrameBelong;
	
	pMsg->m_pHeader->m_ucTransFrameNo = pMsgHeader->m_ucTransFrameNo;
	
	pMsg->m_pHeader->m_ucTransFrameType = pMsgHeader->m_ucTransFrameType;
	
	return true;
};

bool InterpreteAppMsg(SMsg * pMsg)
{
#define COMMA_COUNT_BEFOR_SUB_PARA                  7
#define APP_MSG_SMALL_FIELD_MEM_BLOCK_SIZE 			8
	
	char * pCurrent = NULL;
	
	uint uiIndex = 0;
	
	int iBytes = 0;
	
	char ** ppDest = NULL;
	
	char * pSource = NULL;

	char * CommaPos[COMMA_COUNT_BEFOR_SUB_PARA] = {0};
	
	SMsgHeader * pMsgHeader = (SMsgHeader *) pMsg->m_pRawFrame->m_pData;
	
	char * pAppMsgBegin = (char *)(pMsgHeader + 1);
	
	char * pAppMsgEnd = (char *)pMsg->m_pRawFrame->m_pData + pMsg->m_pRawFrame->m_uiSize - 1 - CHECK_SUM_BYTES - SYN_END_BYTES;

	if(pAppMsgEnd <= pAppMsgBegin || *pAppMsgBegin == COMMA_SEPARATOR || *pAppMsgEnd != COMMA_SEPARATOR )
	{
		//报文格式错误
		return false;
	}
	pCurrent = pAppMsgBegin + 1;
	
	while(uiIndex < COMMA_COUNT_BEFOR_SUB_PARA && pCurrent < pAppMsgEnd)
	{
		while(* pCurrent != COMMA_SEPARATOR)
		{
			pCurrent++;
		}
		CommaPos[uiIndex++] = pCurrent;
		
		pCurrent++;
	}

	if(uiIndex != COMMA_COUNT_BEFOR_SUB_PARA)
	{
		//报文格式错误
		return false;
	}
	
	iBytes = CommaPos[0] - pAppMsgBegin;
	
	ppDest = &pMsg->m_pAppMsg->m_pAppLayerNo;
	
	pSource = pAppMsgBegin;
	
	uiIndex = 0;
	
	do
	{
		if(iBytes >= 0)
		{
			*ppDest = (char  *)pvPortMalloc(iBytes + 1);
			
			memset(*ppDest,0, iBytes + 1);
			
			memcpy(*ppDest,pSource,iBytes);
		}
		iBytes = CommaPos[uiIndex + 1] - CommaPos[uiIndex] - 1;
		
		pSource = CommaPos[uiIndex] + 1;
		
		++ppDest;
		
		++ uiIndex;
		
	}while(uiIndex < COMMA_COUNT_BEFOR_SUB_PARA);
	
	iBytes = pAppMsgEnd - CommaPos[6] - 1;
	
	if(iBytes >= 0)
	{
		*ppDest = (char  *)pvPortMalloc(iBytes + 1);
		
		memset(*ppDest,0, iBytes + 1);
		
		memcpy(*ppDest,pSource,iBytes);
	}
	return true;
	
};



