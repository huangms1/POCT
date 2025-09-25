#include "ExpiredTimer.h"
#include "FreeRTOS.h"

void StartExpiredTimer(SExpiredTimer * pExpiredTimer,uint	uiExpiration)
{
	pExpiredTimer->m_bExpired = false;
	
	pExpiredTimer->m_uiExpiration = uiExpiration /1000 *configTICK_RATE_HZ;
	
	pExpiredTimer->m_uiStartTime = HAL_GetTick();
	
};

bool IsTimerExpired(SExpiredTimer * pExpiredTimer)
{
	if(!pExpiredTimer->m_bExpired)
	{
		uint uiElapsedTime = 0;
		
		uint uiTerminateTime = HAL_GetTick();
		
		if(uiTerminateTime < pExpiredTimer->m_uiStartTime)
		{
			uiElapsedTime = (uint)0xFFFFFFFF - pExpiredTimer->m_uiStartTime + uiTerminateTime;
		}
		else
		{
			uiElapsedTime = uiTerminateTime - pExpiredTimer->m_uiStartTime;
		}
		
		if(uiElapsedTime > pExpiredTimer->m_uiExpiration)
			pExpiredTimer->m_bExpired = true;
	}

	return pExpiredTimer->m_bExpired;
};

