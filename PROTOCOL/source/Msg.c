#include <string.h>
#include <stdlib.h>
#include "Msg.h"
#include "FreeRTOS.h"
#include "Me.h"
#include "Reply.h"
#include "LogTask.h"

void* WholeMsgMemGet(void)
{
	SMsg * pWholeMsg = (SMsg *)pvPortMalloc(sizeof(SMsg));
	
	pWholeMsg->m_pHeader = (SMsgHeader *)pvPortMalloc(sizeof(SMsgHeader));
	
	memset(pWholeMsg->m_pHeader,0, sizeof(SMsgHeader));
	
	pWholeMsg->m_pReSendControl = (SReSendControl *)pvPortMalloc(sizeof(SReSendControl));
	
	memset(pWholeMsg->m_pReSendControl,0, sizeof(SReSendControl));
	
	pWholeMsg->m_pAppMsg = (SAppMsg *)pvPortMalloc(sizeof(SAppMsg));
	
	memset(pWholeMsg->m_pAppMsg,0, sizeof(SAppMsg));

	pWholeMsg->m_pRawFrame = (SByteArray *)pvPortMalloc(sizeof(SByteArray));
	
	pWholeMsg->m_pRawFrame->m_uiSize = 0;
	
	pWholeMsg->m_pRawFrame->m_pData = (uchar *)pvPortMalloc(MAX_MSG_SIZE);
	
	memset(pWholeMsg->m_pRawFrame->m_pData,0, MAX_MSG_SIZE);
	
//	WRITE_TEXT_LOG("MSG MEM GET");

//  WRITE_FREE_LOG("%s:%d\n","msg mem get",10);
//  WRITE_FREE_LOG("%s");

	return pWholeMsg;
};

void WholeMsgMemPut(void * ptr)
{
	SMsg * pWholeMsg = NULL;
	
	if(NULL == ptr)
		return;
	
	pWholeMsg = (SMsg * )ptr;
	
	if(NULL != pWholeMsg->m_pHeader)
		vPortFree(pWholeMsg->m_pHeader);
	
	if(NULL != pWholeMsg->m_pReSendControl)
		vPortFree(pWholeMsg->m_pReSendControl);
	
	if(NULL != pWholeMsg->m_pRawFrame)
	{
		if(NULL != pWholeMsg->m_pRawFrame->m_pData)
			vPortFree(pWholeMsg->m_pRawFrame->m_pData);
		
		vPortFree(pWholeMsg->m_pRawFrame);
	}
	
	if(NULL != pWholeMsg->m_pAppMsg)
	{
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerNo)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerNo);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerType)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerType);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerSender)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerSender);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerReceiver)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerReceiver);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerMainCmd)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerMainCmd);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerSubCmd)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerSubCmd);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerMainPara)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerMainPara);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerSubPara)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerSubPara);
		
		vPortFree(pWholeMsg->m_pAppMsg);
	}
	vPortFree(ptr);
	
//	WRITE_TEXT_LOG("MSG MEM PUT");
};

SMsg * CreateMsg(void)
{
	SMsg * pMsg = (SMsg *)WholeMsgMemGet();
	
	return pMsg;
};

uchar GetAddrFromMsg(SMsg * pMsg)
{
	if(pMsg->m_bIsInterpreted)
	{
		return pMsg->m_pHeader->m_ucAddr;
	}
	else
	{
		return pMsg->m_pRawFrame->m_pData[SYN_HEAD_BYTES + LENGTH_FIELD_BYTES];
	}
};

ushort GetAppMsgLen(SMsg * pMsg)
{
#define COMMA_COUNT_AMONG_APP_MSG 8
	
	ushort usAppMsgLen = 0;
	
	if(NULL == pMsg || IsTransAckMsg(pMsg))
		return 0;
	
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerNo);
	
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerType);
		
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerSender);
	
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerReceiver);
	
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerMainCmd);
	
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerSubCmd);
	
	if(NULL != pMsg->m_pAppMsg->m_pAppLayerMainPara)
		usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerMainPara);
	if(NULL != pMsg->m_pAppMsg->m_pAppLayerSubPara)
		usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerSubPara);
	
	usAppMsgLen += COMMA_COUNT_AMONG_APP_MSG;
	
	return usAppMsgLen;
};

bool IsTransAckMsg(SMsg * pMsg)
{
	return TYPE_FIELD_AUTO_REPLY_VALUE == pMsg->m_pHeader->m_ucTransFrameType;
};

void DeleteMsg(SMsg * pMsg)
{
	if(NULL != pMsg)
	{
		WholeMsgMemPut(pMsg);
	}
};

