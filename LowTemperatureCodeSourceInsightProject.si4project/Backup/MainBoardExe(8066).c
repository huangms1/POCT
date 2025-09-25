#include "MainBoardExe.h"
#include "Reply.h"
#include "AppMsgDefine.h"
#include "Me.h"
#include "poct_process.h"
#include "LogTask.h"





void MainBoardUrgentExe(SMsg * pMsg)
{
	WRITE_DATA_LOG(pMsg->m_pRawFrame->m_pData,pMsg->m_pRawFrame->m_uiSize);
	//ϵͳ��λ
	__set_FAULTMASK(1);
	NVIC_SystemReset();
};




void MainBoardNormalExe(SMsg * pMsg)
{
	
//	User_App(pMsg);
	/*�ظ����֡*/
	int iFaultCode = -1;
	
	bool bRet = User_App(pMsg, &iFaultCode);
	
	WRITE_DATA_LOG(pMsg->m_pRawFrame->m_pData,pMsg->m_pRawFrame->m_uiSize);
	
	if(bRet)
	{
//		if (iFaultCode!=100) //����ȡ����ʱ���ظ�
//		{
			SendFrameR(pMsg,SUB_PARA_T);
//		}
		Globle.Mechine.State = IDLE;
	}
	else
	{
		SendFrameR(pMsg,Globle.Mechine.faultStr);
	}
	
	
//	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_QUY))
//	{
////		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_ADDR))
////		{
////			ProcessQuyAddr(pMsg);
////		}
//	}
//	else
//	{
//		SendFrameR(pMsg,SUB_PARA_T);
//	}
	
	
};

