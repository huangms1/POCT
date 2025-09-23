#ifndef __ME_H__
#define __ME_H__
#include <stdio.h>
#include <stdbool.h>

#include "ZQueue.h"

#include "ZList.h"

#include "ZRing.h"

#include "Msg.h"

#include "PublicDefine.h"

#include "RoutineManager.h"

typedef struct SMe
{
	uchar														m_ucAddr;
	
	uint														m_uiAppMsgNo;
	
	SRoutineManager									m_RoutineManager;
	
	uchar														m_ucTransFrameNoBelong;
	
	ZQueue								*					m_pUrgentExeQueue;
	
	ZQueue								*					m_pNormalExeQueue;
	
	ZRing									*					m_pExecutedMsgRing;
	
	IsMsg4MeFunPtr 									IsMsg4MeFun;
	
	IsMsgUrgent2MeFunPtr						IsMsgUrgent2MeFun;//Ѕфј±ЦёБо
	
	UrgentExeFunPtr									UrgentExe;
	
	NormalExeFunPtr									NormalExe;
	
	InitUsartsFunPtr								InitUsarts;
	
	MsgRouterFunPtr									MsgRouter;
	
	RoutineFunPtr										Routine;
	
}SMe;

extern SMe Me;

void MeInit(void);

uint GetAppMsgNo(void);

bool IsMsgForComm(struct SMsg * pMsg);

bool IsMsgForMain(struct SMsg * pMsg);

bool IsMsgForTemp(struct SMsg * pMsg);


bool IsMsgUrgent2Comm(struct SMsg * pMsg);

bool IsMsgUrgent2Main(struct SMsg * pMsg);

bool IsMsgUrgent2Temp(struct SMsg * pMsg);

void ExeMsgIfNotExe(struct SMsg * pMsg, MsgExeFunPtr MsgExe);

#endif //__ME_H__
