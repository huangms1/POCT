#include <string.h>
#include <stdlib.h>
#include "CommBoardExe.h"
#include "Reply.h"
#include "AppMsgDefine.h"
#include "Endpoint.h"

static void ProcessExePow(SMsg * pMsg);

static void ProcessQuyAddr(SMsg * pMsg);

void CommBoardUrgentExe(SMsg * pMsg)
{
	
};

void CommBoardNormalExe(SMsg * pMsg)
{
//	SendFrameD(pMsg,"32.8");
//	
//	SendFrameR(pMsg,SUB_PARA_T);
//	
//	SendFrameI(APP_MSG_UNIT_COMM,	APP_MSG_UNIT_PC,\
//								APP_MSG_MAIN_CMD_EXE,	APP_MSG_SUB_CMD_PCR,\
//								"",	"",\
//								pMsg->m_pEndpoint);
	
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_EXE))
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_POW))
		{
			ProcessExePow(pMsg);
		}
	}
	else if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_QUY))
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_ADDR))
		{
			ProcessQuyAddr(pMsg);
		}
	}
	else
		WholeMsgMemPut(pMsg);

};

static void ProcessExePow(SMsg * pMsg)
{
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_ON))
	{
		unsigned int iChannel = atoi(pMsg->m_pAppMsg->m_pAppLayerSubPara);
		
		//ChannelSwitchOn(iChannel);
	}
	else if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_OFF))
	{
		unsigned int iChannel = atoi(pMsg->m_pAppMsg->m_pAppLayerSubPara);
		
		//ChannelSwitchOff(iChannel);
	}
	SendFrameR(pMsg,SUB_PARA_T);
};

static void ProcessQuyAddr(SMsg * pMsg)
{
//	if(pMsg->m_pEndpoint == &CommSerial1Endpoint)
//		SendFrameMainParaR(pMsg,"1",SUB_PARA_T);
//	else if(pMsg->m_pEndpoint == &CommSerial2Endpoint)
//		SendFrameMainParaR(pMsg,"2",SUB_PARA_T);
//	else if(pMsg->m_pEndpoint == &CommSerial3Endpoint)
//		SendFrameMainParaR(pMsg,"3",SUB_PARA_T);
//	else if(pMsg->m_pEndpoint == &CommSerial4Endpoint)
//		SendFrameMainParaR(pMsg,"4",SUB_PARA_T);
//	else
//		WholeMsgMemPut(pMsg);
};
