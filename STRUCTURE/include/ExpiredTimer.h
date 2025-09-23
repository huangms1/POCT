#ifndef __EXPIRED_TIMER_H__
#define __EXPIRED_TIMER_H__
#include <stdio.h>
#include <stdbool.h>
#include "PublicDefine.h"

typedef struct SExpiredTimer
{
	uint	 	 m_uiStartTime;
	
	uint	 	 m_uiExpiration;
	
	bool		 m_bExpired;
	
}SExpiredTimer;

void StartExpiredTimer(SExpiredTimer * pExpiredTimer,uint	uiExpiration);

bool IsTimerExpired(SExpiredTimer * pExpiredTimer);

#endif //__EXPIRED_TIMER_H__

