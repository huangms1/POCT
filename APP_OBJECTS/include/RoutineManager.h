#ifndef __ROUTINE_MANAGER_H__
#define __ROUTINE_MANAGER_H__

#include "ExpiredTimer.h"

typedef enum ERunningState
{
	eRunningState_InitBegin = 0,
	
	eRunningState_InitComplete,
	
	eRunningState_Running,
	
}ERunningState;

typedef struct SRoutineManager
{
	ERunningState					m_eRunningState;

	SExpiredTimer					m_FeedWatchDogExpiredTimer;
	
	SExpiredTimer					m_HandShakeExpiredTimer;
	
}SRoutineManager;

#endif //__ROUTINE_MANAGER_H__
