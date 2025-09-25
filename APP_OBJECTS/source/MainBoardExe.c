#include "MainBoardExe.h"
#include "Reply.h"
#include "AppMsgDefine.h"
#include "Me.h"
#include "poct_process.h"
#include "LogTask.h"


void MainBoardUrgentExe(SMsg * pMsg)
{
	WRITE_DATA_LOG(pMsg->m_pRawFrame->m_pData, pMsg->m_pRawFrame->m_uiSize);
	__set_FAULTMASK(1);
	NVIC_SystemReset();
};

void MainBoardNormalExe(SMsg * pMsg)
{	
	int iFaultCode = -1;
	
	bool bRet = User_App(pMsg, &iFaultCode);
	
	WRITE_DATA_LOG(pMsg->m_pRawFrame->m_pData, pMsg->m_pRawFrame->m_uiSize);
	
	if(bRet)
	{
		SendFrameR(pMsg, SUB_PARA_T);

		Globle.Mechine.State = IDLE;
	}
	else
	{
		SendFrameR(pMsg,Globle.Mechine.faultStr);
	}
	
};

