#ifndef __ENDPOINT_H__
#define __ENDPOINT_H__
#include <stdio.h>
#include <stdbool.h>
#include "StateMachine.h"
#include "ZQueue.h"
#include "ZList.h"
#include "PublicDefine.h"
#include "ExpiredTimer.h"


typedef struct SEndpoint
{
	char						m_Name[ENDPOINT_NAME_LEN];
	
	bool						m_bEnabled;
	
	SStateMachine 	m_RecvStateMachine;
	
	ZQueue				*	m_pWaitSendQueue;
	
	ZList					*	m_pReSendList;
	
	uchar						m_ucTransFrameNo;
	
	SendFunPtr			SendFun;
	
	RecvFunPtr			RecvFun;
	
	bool						m_bSendControl;
	
	SExpiredTimer		m_FrameRecvExpiredTimer;
		
}SEndpoint;

extern SEndpoint SocketEndpoint;

void EndpointInit(SEndpoint * pEndpoint,char * pName,SendFunPtr pSendFun,RecvFunPtr pRecvFun);

void EndpointRecvAndSpell(SEndpoint * pEndpoint);

void EndpointSpell(SEndpoint * pEndpoint);

void EndpointSendOne(SEndpoint * pEndpoint);

void EndpointSendOneFromWaitQueue(SEndpoint * pEndpoint);

void EndpointSendOneFromReSendList(SEndpoint * pEndpoint);

uchar EndpointGetTransFrameNo(SEndpoint * pEndpoint);

void EndpointRemoveAckedMsg(SEndpoint * pEndpoint,SMsg * pTransAckMsg);

bool IsSendMsgInEndpointReSendList(SEndpoint * pEndpoint,SMsg * pSendMsg);

void EndpointRemoveFirstMsgIfMemAlmostUsedUp(SEndpoint * pEndpoint);


#endif //__ENDPOINT_H__
