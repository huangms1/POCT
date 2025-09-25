#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Reply.h"
#include "Me.h"
#include "Endpoint.h"
#include "StateMachine.h"
#include "AppMsgDefine.h"
#include "FreeRTOS.h"

void AssembleAndSend(SMsg * pMsg)
{
#define APP_MSG_SEGMENTS 8
	
	uchar * pCurrent = NULL;
	
	SMsgHeader * pMsgHeader = NULL;
	
	ushort usAppMsgLen = GetAppMsgLen(pMsg);
	
	ushort usMsgLen = ADDR_FIELD_BYTES + NUMBER_FIELD_BYTES + TYPE_FIELD_BYTES + usAppMsgLen;
	
	pMsg->m_pHeader->m_usFrameLenH = usMsgLen >> 8;
	
	pMsg->m_pHeader->m_usFrameLenL = usMsgLen & 0xFF;
	
	pMsgHeader = (SMsgHeader *)pMsg->m_pRawFrame->m_pData;
	
	pMsgHeader->m_ucSynHead = pMsg->m_pHeader->m_ucSynHead;
	
	pMsgHeader->m_usFrameLenH = pMsg->m_pHeader->m_usFrameLenH;
	
	pMsgHeader->m_usFrameLenL = pMsg->m_pHeader->m_usFrameLenL;
	
	pMsgHeader->m_ucAddr = pMsg->m_pHeader->m_ucAddr;
	
	pMsgHeader->m_ucTransFrameBelong = pMsg->m_pHeader->m_ucTransFrameBelong;
	
	pMsgHeader->m_ucTransFrameNo = pMsg->m_pHeader->m_ucTransFrameNo;
	
	pMsgHeader->m_ucTransFrameType = pMsg->m_pHeader->m_ucTransFrameType;
	
	pCurrent = (uchar *)(pMsgHeader + 1);
	
	if(!IsTransAckMsg(pMsg))
	{
		char ** ppSrc = &pMsg->m_pAppMsg->m_pAppLayerNo;
		
		uint uiIndex = 0;
		
		while(uiIndex++ < APP_MSG_SEGMENTS)
		{
			char * pSrc = (char *)*ppSrc;
			
			if(NULL != pSrc)
			{
				strcpy((char *)pCurrent,pSrc);
			
				pCurrent += strlen(pSrc);
			}
			*pCurrent = COMMA_SEPARATOR;
		
			++pCurrent;
			
			++ppSrc;
		}
	}
	*pCurrent = CheckSum(pMsg->m_pRawFrame->m_pData, pCurrent - pMsg->m_pRawFrame->m_pData);
	
	++pCurrent;
	
	*pCurrent = SYN_FIN;
	
	pMsg->m_pRawFrame->m_uiSize = pCurrent - pMsg->m_pRawFrame->m_pData + 1;
	
	Push2Queue(pMsg->m_pEndpoint->m_pWaitSendQueue,pMsg);
};

SMsg * CreateTransAck(SMsg * pJustRecvMsg)
{
	SMsg * pTransAckMsg = CreateMsg();
	
	pTransAckMsg->m_pHeader->m_ucSynHead = pJustRecvMsg->m_pHeader->m_ucSynHead;
	
	pTransAckMsg->m_pHeader->m_ucAddr = pJustRecvMsg->m_pHeader->m_ucAddr;
	
	pTransAckMsg->m_pHeader->m_ucTransFrameBelong = pJustRecvMsg->m_pHeader->m_ucTransFrameBelong; //
		
	pTransAckMsg->m_pHeader->m_ucTransFrameNo = pJustRecvMsg->m_pHeader->m_ucTransFrameNo; // 
	
	pTransAckMsg->m_pHeader->m_ucTransFrameType = TYPE_FIELD_AUTO_REPLY_VALUE;
	
	pTransAckMsg->m_pEndpoint = pJustRecvMsg->m_pEndpoint;
	
	pTransAckMsg->m_pReSendControl->m_bEnabled = false;
	
	return pTransAckMsg;
};

SMsg * CreateFrameD(SMsg * pJustRecvMsg, char * pSubPara)
{
	SMsg * pFrameDMsg = CreateMsg();
	
	int iAllocateBytes = 0;
	
	pFrameDMsg->m_pHeader->m_ucSynHead = pJustRecvMsg->m_pHeader->m_ucSynHead;
	
	pFrameDMsg->m_pHeader->m_ucAddr = pJustRecvMsg->m_pHeader->m_ucAddr;
	
	pFrameDMsg->m_pHeader->m_ucTransFrameBelong = Me.m_ucTransFrameNoBelong;
	
	pFrameDMsg->m_pHeader->m_ucTransFrameNo = EndpointGetTransFrameNo(pJustRecvMsg->m_pEndpoint);
	
	pFrameDMsg->m_pHeader->m_ucTransFrameType = TYPE_FIELD_SEND_VALUE;
	
	iAllocateBytes = strlen(pJustRecvMsg->m_pAppMsg->m_pAppLayerNo) + 1;
	
	pFrameDMsg->m_pAppMsg->m_pAppLayerNo = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameDMsg->m_pAppMsg->m_pAppLayerNo,0, iAllocateBytes);
	
	strcpy(pFrameDMsg->m_pAppMsg->m_pAppLayerNo,pJustRecvMsg->m_pAppMsg->m_pAppLayerNo);
	
	iAllocateBytes = strlen(APP_MSG_TYPE_D) + 1;
	
	pFrameDMsg->m_pAppMsg->m_pAppLayerType = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameDMsg->m_pAppMsg->m_pAppLayerType,0, iAllocateBytes);
	
	strcpy(pFrameDMsg->m_pAppMsg->m_pAppLayerType,APP_MSG_TYPE_D);
	
	iAllocateBytes = strlen(pJustRecvMsg->m_pAppMsg->m_pAppLayerReceiver) + 1;
	
	pFrameDMsg->m_pAppMsg->m_pAppLayerSender = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameDMsg->m_pAppMsg->m_pAppLayerSender,0, iAllocateBytes);
	
	strcpy(pFrameDMsg->m_pAppMsg->m_pAppLayerSender,pJustRecvMsg->m_pAppMsg->m_pAppLayerReceiver);
	
	iAllocateBytes = strlen(pJustRecvMsg->m_pAppMsg->m_pAppLayerSender) + 1;
	
	pFrameDMsg->m_pAppMsg->m_pAppLayerReceiver = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameDMsg->m_pAppMsg->m_pAppLayerReceiver,0, iAllocateBytes);

	strcpy(pFrameDMsg->m_pAppMsg->m_pAppLayerReceiver,pJustRecvMsg->m_pAppMsg->m_pAppLayerSender);
	
	iAllocateBytes = strlen(pJustRecvMsg->m_pAppMsg->m_pAppLayerMainCmd) + 1;
	
	pFrameDMsg->m_pAppMsg->m_pAppLayerMainCmd = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameDMsg->m_pAppMsg->m_pAppLayerMainCmd,0, iAllocateBytes);

	strcpy(pFrameDMsg->m_pAppMsg->m_pAppLayerMainCmd,pJustRecvMsg->m_pAppMsg->m_pAppLayerMainCmd);
	
	iAllocateBytes = strlen(pJustRecvMsg->m_pAppMsg->m_pAppLayerSubCmd) + 1;
	
	pFrameDMsg->m_pAppMsg->m_pAppLayerSubCmd = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameDMsg->m_pAppMsg->m_pAppLayerSubCmd,0, iAllocateBytes);

	strcpy(pFrameDMsg->m_pAppMsg->m_pAppLayerSubCmd,pJustRecvMsg->m_pAppMsg->m_pAppLayerSubCmd);
	
	iAllocateBytes = strlen(pJustRecvMsg->m_pAppMsg->m_pAppLayerMainPara) + 1;
	
	pFrameDMsg->m_pAppMsg->m_pAppLayerMainPara = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameDMsg->m_pAppMsg->m_pAppLayerMainPara,0, iAllocateBytes);
	
	strcpy(pFrameDMsg->m_pAppMsg->m_pAppLayerMainPara,pJustRecvMsg->m_pAppMsg->m_pAppLayerMainPara);
	
	iAllocateBytes = strlen(pSubPara) + 1;
	
	pFrameDMsg->m_pAppMsg->m_pAppLayerSubPara = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameDMsg->m_pAppMsg->m_pAppLayerSubPara,0, iAllocateBytes);
	
	strcpy(pFrameDMsg->m_pAppMsg->m_pAppLayerSubPara,pSubPara);
	
	pFrameDMsg->m_pReSendControl->m_bEnabled = true;
	
	pFrameDMsg->m_pReSendControl->m_uiReSendCount = RESEND_MAX_COUNT;
	
	pFrameDMsg->m_pEndpoint = pJustRecvMsg->m_pEndpoint;
	
	return pFrameDMsg;
	
};

void CreateFrameR(SMsg * pMsg,char * pSubPara)
{
	char * pTmp = NULL;
	
	int iSubParaBeforeLength = 0;
	
	int iSubParaLength = 0;
	
	int iAllocateBytes = 0;
	
	pMsg->m_pHeader->m_ucTransFrameBelong = Me.m_ucTransFrameNoBelong;
	
	pMsg->m_pHeader->m_ucTransFrameNo = EndpointGetTransFrameNo(pMsg->m_pEndpoint);
	
	pMsg->m_pHeader->m_ucTransFrameType = TYPE_FIELD_SEND_VALUE;
	
	pTmp = pMsg->m_pAppMsg->m_pAppLayerSender;
	
	pMsg->m_pAppMsg->m_pAppLayerSender = pMsg->m_pAppMsg->m_pAppLayerReceiver;
	
	pMsg->m_pAppMsg->m_pAppLayerReceiver = pTmp;
	
	strcpy(pMsg->m_pAppMsg->m_pAppLayerType,APP_MSG_TYPE_R);
	
	iSubParaBeforeLength = strlen(pMsg->m_pAppMsg->m_pAppLayerSubPara);
	
	iSubParaLength = strlen(pSubPara);
	
	if(iSubParaBeforeLength >= iSubParaLength)
	{
		memset(pMsg->m_pAppMsg->m_pAppLayerSubPara,0, iSubParaBeforeLength);
	}
	else
	{
		vPortFree(pMsg->m_pAppMsg->m_pAppLayerSubPara);
		
		iAllocateBytes = iSubParaLength + 1;
	
		pMsg->m_pAppMsg->m_pAppLayerSubPara = (char  *)pvPortMalloc(iAllocateBytes);
	
		memset(pMsg->m_pAppMsg->m_pAppLayerSubPara,0, iAllocateBytes);
	}
	strcpy(pMsg->m_pAppMsg->m_pAppLayerSubPara,pSubPara);
	
	pMsg->m_pReSendControl->m_bEnabled = true;
	
	pMsg->m_pReSendControl->m_uiReSendCount = RESEND_MAX_COUNT;

};

void CreateFrameMainParaR(SMsg * pMsg,char * pMainPara,char * pSubPara)
{
	char * pTmp = NULL;
	
	pMsg->m_pHeader->m_ucTransFrameBelong = Me.m_ucTransFrameNoBelong;
	
	pMsg->m_pHeader->m_ucTransFrameNo = EndpointGetTransFrameNo(pMsg->m_pEndpoint);
	
	pMsg->m_pHeader->m_ucTransFrameType = TYPE_FIELD_SEND_VALUE;
	
	pTmp = pMsg->m_pAppMsg->m_pAppLayerSender;
	
	pMsg->m_pAppMsg->m_pAppLayerSender = pMsg->m_pAppMsg->m_pAppLayerReceiver;
	
	pMsg->m_pAppMsg->m_pAppLayerReceiver = pTmp;
	
	strcpy(pMsg->m_pAppMsg->m_pAppLayerType,APP_MSG_TYPE_R);
	
	strcpy(pMsg->m_pAppMsg->m_pAppLayerMainPara,pMainPara);
	
	strcpy(pMsg->m_pAppMsg->m_pAppLayerSubPara,pSubPara);
	
	pMsg->m_pReSendControl->m_bEnabled = true;
	
	pMsg->m_pReSendControl->m_uiReSendCount = RESEND_MAX_COUNT;
};

SMsg *  CreateFrameI(char * pAppSender,	char * pAppReceiver,\
										 char * pAppMainCmd,	char * pAppSubCmd,\
										 char * pAppMainPara,	char * pAppSubPara,\
										 struct SEndpoint * pEndpoint)
{
	int iAllocateBytes = 0;
	
	SMsg * pFrameIMsg = CreateMsg();
	
	pFrameIMsg->m_pHeader->m_ucSynHead = SYN_HEAD;
	
	pFrameIMsg->m_pHeader->m_ucAddr = Me.m_ucAddr;
	
	pFrameIMsg->m_pHeader->m_ucTransFrameBelong = Me.m_ucTransFrameNoBelong;
	
	pFrameIMsg->m_pHeader->m_ucTransFrameNo = EndpointGetTransFrameNo(pEndpoint);
	
	pFrameIMsg->m_pHeader->m_ucTransFrameType = TYPE_FIELD_SEND_VALUE;
	
	iAllocateBytes = strlen(APP_MSG_TYPE_I_NO) + 1;
	
	pFrameIMsg->m_pAppMsg->m_pAppLayerNo = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameIMsg->m_pAppMsg->m_pAppLayerNo,0, iAllocateBytes);
	
	strcpy(pFrameIMsg->m_pAppMsg->m_pAppLayerNo,APP_MSG_TYPE_I_NO);
	
	iAllocateBytes = strlen(APP_MSG_TYPE_I) + 1;
	
	pFrameIMsg->m_pAppMsg->m_pAppLayerType = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameIMsg->m_pAppMsg->m_pAppLayerType,0, iAllocateBytes);
	
	strcpy(pFrameIMsg->m_pAppMsg->m_pAppLayerType,APP_MSG_TYPE_I);
	
	iAllocateBytes = strlen(pAppSender) + 1;
	
	pFrameIMsg->m_pAppMsg->m_pAppLayerSender = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameIMsg->m_pAppMsg->m_pAppLayerSender,0, iAllocateBytes);
	
	strcpy(pFrameIMsg->m_pAppMsg->m_pAppLayerSender,pAppSender);
	
	iAllocateBytes = strlen(pAppReceiver) + 1;
	
	pFrameIMsg->m_pAppMsg->m_pAppLayerReceiver = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameIMsg->m_pAppMsg->m_pAppLayerReceiver,0, iAllocateBytes);

	strcpy(pFrameIMsg->m_pAppMsg->m_pAppLayerReceiver,pAppReceiver);
	
	iAllocateBytes = strlen(pAppMainCmd) + 1;
	
	pFrameIMsg->m_pAppMsg->m_pAppLayerMainCmd = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameIMsg->m_pAppMsg->m_pAppLayerMainCmd,0, iAllocateBytes);

	strcpy(pFrameIMsg->m_pAppMsg->m_pAppLayerMainCmd,pAppMainCmd);
	
	iAllocateBytes = strlen(pAppSubCmd) + 1;
	
	pFrameIMsg->m_pAppMsg->m_pAppLayerSubCmd = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameIMsg->m_pAppMsg->m_pAppLayerSubCmd,0, iAllocateBytes);

	strcpy(pFrameIMsg->m_pAppMsg->m_pAppLayerSubCmd,pAppSubCmd);
	
	iAllocateBytes = strlen(pAppMainPara) + 1;
	
	pFrameIMsg->m_pAppMsg->m_pAppLayerMainPara = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameIMsg->m_pAppMsg->m_pAppLayerMainPara,0, iAllocateBytes);
	
	strcpy(pFrameIMsg->m_pAppMsg->m_pAppLayerMainPara,pAppMainPara);
	
	iAllocateBytes = strlen(pAppSubPara) + 1;
	
	pFrameIMsg->m_pAppMsg->m_pAppLayerSubPara = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameIMsg->m_pAppMsg->m_pAppLayerSubPara,0, iAllocateBytes);
	
	strcpy(pFrameIMsg->m_pAppMsg->m_pAppLayerSubPara,pAppSubPara);
	
	pFrameIMsg->m_pReSendControl->m_bEnabled = true;
	
	pFrameIMsg->m_pReSendControl->m_uiReSendCount = RESEND_MAX_COUNT;
	
	pFrameIMsg->m_pEndpoint = pEndpoint;
	
	return pFrameIMsg;
};



void Int2Str(uint uiNumber,char * str)
{
 char temp[10]={0};
 
 uchar iLoop=0,ucLen=0;
 
 while(uiNumber)
 {
  temp[ucLen] = uiNumber % 10 + '0';
	 
  uiNumber/=10;
	 
  ucLen++;
 }
 if(ucLen==0)
 {
  str[ucLen]='0';
 }
 else
 {
  for(;iLoop<ucLen;++iLoop)
  {
   str[iLoop]=temp[ucLen-iLoop-1];
  }
 }
}

SMsg *  CreateFrameC(uint uiReceiverAddr, char * pAppSender,	char * pAppReceiver,\
										 char * pAppMainCmd,	char * pAppSubCmd,\
										 char * pAppMainPara,	char * pAppSubPara,\
										 struct SEndpoint * pEndpoint)
{
#define APP_MSG_SMALL_FIELD_MEM_BLOCK_SIZE 			8
	
 	SMsg * pFrameCMsg = CreateMsg();
	
	int iAllocateBytes = 0;
	
	char AppMsgNo[APP_MSG_SMALL_FIELD_MEM_BLOCK_SIZE] = {0};
	
	pFrameCMsg->m_pHeader->m_ucSynHead = SYN_HEAD;
	
	pFrameCMsg->m_pHeader->m_ucAddr = uiReceiverAddr;
	
	pFrameCMsg->m_pHeader->m_ucTransFrameBelong = Me.m_ucTransFrameNoBelong;
	
	pFrameCMsg->m_pHeader->m_ucTransFrameNo = EndpointGetTransFrameNo(pEndpoint);
	
	pFrameCMsg->m_pHeader->m_ucTransFrameType = TYPE_FIELD_SEND_VALUE;
		
	Int2Str(GetAppMsgNo(),AppMsgNo);
	
	iAllocateBytes = strlen(AppMsgNo) + 1;
	
	pFrameCMsg->m_pAppMsg->m_pAppLayerNo = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameCMsg->m_pAppMsg->m_pAppLayerNo,0, iAllocateBytes);
		
	strcpy(pFrameCMsg->m_pAppMsg->m_pAppLayerNo,AppMsgNo);
	
	iAllocateBytes = strlen(APP_MSG_TYPE_C) + 1;
	
	pFrameCMsg->m_pAppMsg->m_pAppLayerType = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameCMsg->m_pAppMsg->m_pAppLayerType,0, iAllocateBytes);
	
	strcpy(pFrameCMsg->m_pAppMsg->m_pAppLayerType,APP_MSG_TYPE_C);
	
	iAllocateBytes = strlen(pAppSender) + 1;
	
	pFrameCMsg->m_pAppMsg->m_pAppLayerSender = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameCMsg->m_pAppMsg->m_pAppLayerSender,0, iAllocateBytes);
	
	strcpy(pFrameCMsg->m_pAppMsg->m_pAppLayerSender,pAppSender);
	
	iAllocateBytes = strlen(pAppReceiver) + 1;
	
	pFrameCMsg->m_pAppMsg->m_pAppLayerReceiver = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameCMsg->m_pAppMsg->m_pAppLayerReceiver,0, iAllocateBytes);

	strcpy(pFrameCMsg->m_pAppMsg->m_pAppLayerReceiver,pAppReceiver);
	
	iAllocateBytes = strlen(pAppMainCmd) + 1;
	
	pFrameCMsg->m_pAppMsg->m_pAppLayerMainCmd = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameCMsg->m_pAppMsg->m_pAppLayerMainCmd,0, iAllocateBytes);

	strcpy(pFrameCMsg->m_pAppMsg->m_pAppLayerMainCmd,pAppMainCmd);
	
	iAllocateBytes = strlen(pAppSubCmd) + 1;
	
	pFrameCMsg->m_pAppMsg->m_pAppLayerSubCmd = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameCMsg->m_pAppMsg->m_pAppLayerSubCmd,0, iAllocateBytes);

	strcpy(pFrameCMsg->m_pAppMsg->m_pAppLayerSubCmd,pAppSubCmd);
	
	iAllocateBytes = strlen(pAppMainPara) + 1;
	
	pFrameCMsg->m_pAppMsg->m_pAppLayerMainPara = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameCMsg->m_pAppMsg->m_pAppLayerMainPara,0, iAllocateBytes);
	
	strcpy(pFrameCMsg->m_pAppMsg->m_pAppLayerMainPara,pAppMainPara);
	
	iAllocateBytes = strlen(pAppSubPara) + 1;
	
	pFrameCMsg->m_pAppMsg->m_pAppLayerSubPara = (char  *)pvPortMalloc(iAllocateBytes);
	
	memset(pFrameCMsg->m_pAppMsg->m_pAppLayerSubPara,0, iAllocateBytes);
	
	strcpy(pFrameCMsg->m_pAppMsg->m_pAppLayerSubPara,pAppSubPara);
	
	pFrameCMsg->m_pReSendControl->m_bEnabled = true;
	
	pFrameCMsg->m_pReSendControl->m_uiReSendCount = RESEND_MAX_COUNT;
	
	pFrameCMsg->m_pEndpoint = pEndpoint;
	
	return pFrameCMsg;
};

void SendTransAck(SMsg * pJustRecvMsg)
{
	SMsg * pTransAckMsg = CreateTransAck(pJustRecvMsg);
	
	AssembleAndSend(pTransAckMsg);
};

/*回复函数：*/
void SendFrameR(SMsg * pJustRecvMsg,char * pSubPara)
{
	CreateFrameR(pJustRecvMsg,pSubPara);
	
	AssembleAndSend(pJustRecvMsg);
};

void SendFrameMainParaR(SMsg * pJustRecvMsg,char * pMainPara,char * pSubPara)
{
	CreateFrameMainParaR(pJustRecvMsg,pMainPara,pSubPara);
	
	AssembleAndSend(pJustRecvMsg);
};

void SendFrameD(SMsg * pJustRecvMsg,char * pSubPara)
{
	SMsg * pFrameDMsg = CreateFrameD(pJustRecvMsg, pSubPara);
	
	AssembleAndSend(pFrameDMsg);
};

void SendFrameI(char * pAppSender,	char * pAppReceiver,\
								char * pAppMainCmd,	char * pAppSubCmd,\
								char * pAppMainPara,	char * pAppSubPara,\
								struct SEndpoint * pEndpoint)
{
	SMsg * pFrameIMsg = CreateFrameI(pAppSender,pAppReceiver,pAppMainCmd,	pAppSubCmd,pAppMainPara,pAppSubPara,pEndpoint);

	AssembleAndSend(pFrameIMsg);
};

void SendFrameC(uint uiReceiverAddr, char * pAppSender,	char * pAppReceiver,\
								char * pAppMainCmd,	char * pAppSubCmd,\
								char * pAppMainPara,	char * pAppSubPara,\
								struct SEndpoint * pEndpoint)
{
	SMsg * pFrameCMsg = CreateFrameC(uiReceiverAddr, pAppSender,pAppReceiver,pAppMainCmd,	pAppSubCmd,pAppMainPara,pAppSubPara,pEndpoint);

	AssembleAndSend(pFrameCMsg);
}

