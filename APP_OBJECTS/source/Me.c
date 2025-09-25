#include "Me.h"
#include "CommBoardExe.h"
#include "MainBoardExe.h"
#include "TempBoardExe.h"
#include "StartTask.h"
#include "Usart.h"
#include "Bridge.h"
#include "RoutineTask.h"
#include "AppMsgDefine.h"

SMe	Me;

static void ExecutedMsgRingDataDelete(void * pData)
{
	vPortFree(pData);
}

static bool ExecutedMsgRingIsSameData(void * pData, void * pData1)
{
	if(NULL != pData && NULL != pData1)
		return *(int*)pData == *(int*)pData1;
	else
		return false;
}

void MeInit(void)
{
	Me.m_pUrgentExeQueue = CreateQueue();
	
	Me.m_pNormalExeQueue = CreateQueue();
	
	Me.m_pExecutedMsgRing = CreateRing(10,ExecutedMsgRingDataDelete,ExecutedMsgRingIsSameData);
	
	Me.m_ucTransFrameNoBelong = MAIN_BOARD_TRANS_FRAME_NO_BELONG;

//	Me.IsMsg4MeFun = IsMsgForMain;
//		
	Me.IsMsgUrgent2MeFun = IsMsgUrgent2Main;  //紧急指令
		
	Me.UrgentExe = MainBoardUrgentExe;
		
	Me.NormalExe = MainBoardNormalExe;
		
	Me.Routine = MainRoutine;
	
	Me.m_ucAddr = MAIN_1_BOARD_ADDR;
	
//	Me.m_eId = GetID();

//	switch(Me.m_eId)
//	{
//		case Enum_ID_CommunicationBoard:
//			
//			Me.m_ucAddr = COMMUNICATION_BOARD_ADDR;
//		
//			Me.m_ucTransFrameNoBelong = COMMUNICATION_BOARD_TRANS_FRAME_NO_BELONG;
//		
//			Me.IsMsg4MeFun = IsMsgForComm;
//		
//			Me.IsMsgUrgent2MeFun = IsMsgUrgent2Comm;
//		
//			Me.UrgentExe = CommBoardUrgentExe;
//		
//			Me.NormalExe = CommBoardNormalExe;
//		
//			Me.InitUsarts = InitUsarts4Comm;
//		
//			Me.CreateTasks = CreateTasks4Comm;
//		
//			Me.MsgRouter = CommBoardMsgRouter;
//		
//			Me.Routine = CommRoutine;
//			break;
//		case Enum_ID_MainControlBoard:
//			
//			Me.m_ucTransFrameNoBelong = MAIN_BOARD_TRANS_FRAME_NO_BELONG;

//			Me.IsMsg4MeFun = IsMsgForMain;
//		
//			Me.IsMsgUrgent2MeFun = IsMsgUrgent2Main;
//		
//			Me.UrgentExe = MainBoardUrgentExe;
//		
//			Me.NormalExe = MainBoardNormalExe;
//		
//			Me.InitUsarts = InitUsarts4Main;
//		
//			Me.CreateTasks = CreateTasks4Main;
//		
//			Me.MsgRouter = MainBoardMsgRouter;
//		
//			Me.Routine = MainRoutine;
//			break;
//		case Enum_ID_TempBoard:
//			
//			Me.m_ucTransFrameNoBelong = TEMP_BOARD_TRANS_FRAME_NO_BELONG;
//			
//			Me.IsMsg4MeFun = IsMsgForTemp;
//		
//			Me.IsMsgUrgent2MeFun = IsMsgUrgent2Temp;
//		
//			Me.UrgentExe = TempBoardUrgentExe;
//		
//			Me.NormalExe = TempBoardNormalExe;
//		
//			Me.InitUsarts = InitUsarts4Temp;
//		
//			Me.CreateTasks = CreateTasks4Temp;
//		
//			Me.MsgRouter = TempBoardMsgRouter;
//		
//			Me.Routine = TempRoutine;
//			break;
//		default:
//			break;
//	}
};

uint GetAppMsgNo(void)
{
	return ++Me.m_uiAppMsgNo;
}


bool IsMsgForTemp(struct SMsg * pMsg)
{
	return true;
};


bool IsMsgUrgent2Comm(struct SMsg * pMsg)
{
	return false;
};

/*紧急判断函数

@返回值：true 紧急  false 非紧急

*/
bool IsMsgUrgent2Main(struct SMsg * pMsg)
{
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_EXE))
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_RESET))
		{
			return true;
		}
	}
	return false;
	
};

bool IsMsgUrgent2Temp(struct SMsg * pMsg)
{
	return false;
};

void ExeMsgIfNotExe(struct SMsg * pMsg, MsgExeFunPtr MsgExe)
{
	int iAppLayerNo = atoi(pMsg->m_pAppMsg->m_pAppLayerNo);
	
	if(!ExistsInRing(Me.m_pExecutedMsgRing,&iAppLayerNo))
	{
		int * pData = (int *)pvPortMalloc(sizeof(int));

		*pData = iAppLayerNo;
		
		Append2Ring(Me.m_pExecutedMsgRing,pData);
		
		MsgExe(pMsg);
	}
	else
		DeleteMsg(pMsg);
}
