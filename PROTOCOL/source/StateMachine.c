#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "StateMachine.h"
#include "Endpoint.h"
#include "Bridge.h"
#include "FreeRTOS.h"

void SpellMsg(SStateMachine * pStateMachine,struct SEndpoint * pEndpoint)   //  数据拆包   拆成一帧一帧
{
	ushort usLoop = 0;
	
	uchar ucCurrent = 0;
	
	ushort usRecvSize = pStateMachine->m_usRecvSize;
	
	pStateMachine->m_usRecvSize = 0;
	
	if(NULL == pStateMachine)
		return;
	
	if(IsTimerExpired(&pEndpoint->m_FrameRecvExpiredTimer))
		pStateMachine->m_eSpellState = eSpellState_Syn_Head;
		
	for (usLoop = 0; usLoop < usRecvSize; ++usLoop)
	{
		ucCurrent = pStateMachine->m_RecvBuffer[usLoop];

		switch (pStateMachine->m_eSpellState)
		{
		case eSpellState_Syn_Head:
			if (SYN_HEAD == ucCurrent)
			{
				pStateMachine->m_eSpellState = eSpellState_Length;

				pStateMachine->m_usIndex = 0;

				pStateMachine->m_usBodyLength = 0;

				pStateMachine->m_usLengthIndex = 0;

				pStateMachine->m_usBodyCopiedLength = 0;

				memset(pStateMachine->m_SpelledBuffer, 0, MAX_MSG_SIZE);

				pStateMachine->m_SpelledBuffer[pStateMachine->m_usIndex++] = ucCurrent;

				pStateMachine->m_usIndexOfSynHead = usLoop;
			}
			break;
		case eSpellState_Length:
			pStateMachine->m_usBodyLength = pStateMachine->m_usBodyLength << 8;

			pStateMachine->m_usBodyLength |= ucCurrent;

			++pStateMachine->m_usLengthIndex;

			pStateMachine->m_SpelledBuffer[pStateMachine->m_usIndex++] = ucCurrent;

			if (LENGTH_FIELD_BYTES == pStateMachine->m_usLengthIndex)
			{
				if (pStateMachine->m_usBodyLength > 0 && pStateMachine->m_usBodyLength <= MAX_MSG_SIZE)
				{
					pStateMachine->m_eSpellState = eSpellState_Body;
				}
				else
				{
					pStateMachine->m_eSpellState = eSpellState_Syn_Head;
				}
			}
			break;
		case eSpellState_Body:
			{
				ushort usNeed2CopyBytes = pStateMachine->m_usBodyLength - pStateMachine->m_usBodyCopiedLength;

				uint uiBytesLeft = usRecvSize - usLoop;

				if (usNeed2CopyBytes <= uiBytesLeft)
				{
					memcpy(pStateMachine->m_SpelledBuffer + pStateMachine->m_usIndex, pStateMachine->m_RecvBuffer + usLoop, usNeed2CopyBytes);

					pStateMachine->m_eSpellState = eSpellState_CheckSum;

					pStateMachine->m_usIndex += usNeed2CopyBytes;

					usLoop = usLoop + usNeed2CopyBytes - 1;
				}
				else
				{
					memcpy(pStateMachine->m_SpelledBuffer + pStateMachine->m_usIndex, pStateMachine->m_RecvBuffer + usLoop, uiBytesLeft);

					pStateMachine->m_usBodyCopiedLength += uiBytesLeft;

					pStateMachine->m_usIndex += uiBytesLeft;

					usLoop = usLoop + uiBytesLeft - 1;
				}
			}
			break;
		case eSpellState_CheckSum:
			{
				uchar ucCalculateSum = CheckSum(pStateMachine->m_SpelledBuffer, pStateMachine->m_usIndex);

				if (ucCalculateSum == ucCurrent)
				{
					pStateMachine->m_SpelledBuffer[pStateMachine->m_usIndex++] = ucCurrent;

					pStateMachine->m_eSpellState = eSpellState_Syn_Fin;
				}
				else
				{
					pStateMachine->m_eSpellState = eSpellState_Syn_Head;
				}
			}
			break;
		case eSpellState_Syn_Fin:
			if (SYN_FIN == ucCurrent)
			{
				pStateMachine->m_SpelledBuffer[pStateMachine->m_usIndex++] = ucCurrent;

				pStateMachine->m_eSpellState = eSpellState_Okey;
			}
			else
			{
				pStateMachine->m_eSpellState = eSpellState_Syn_Head;
			}
			break;
		case eSpellState_Okey:
				break;
		}
		if (eSpellState_Okey == pStateMachine->m_eSpellState)
		{
			//加入接收队列
			SMsg * pMsg = CreateMsg();
			
			memcpy(pMsg->m_pRawFrame->m_pData,pStateMachine->m_SpelledBuffer,pStateMachine->m_usIndex);
			
			pMsg->m_pRawFrame->m_uiSize = pStateMachine->m_usIndex;
			
			pMsg->m_pEndpoint = pEndpoint;
			
			ProcessMsg(pMsg);

			pStateMachine->m_eSpellState = eSpellState_Syn_Head;
		}
	}
	StartExpiredTimer(&pEndpoint->m_FrameRecvExpiredTimer,FRAME_MAX_RECV_MS);
};

uchar CheckSum(uchar * pData, ushort usLength)
{
	uchar ucSum = 0;
	
	ushort usLoop = 0;

	for (usLoop = 0; usLoop < usLength; ++ usLoop)
	{
		ucSum += pData[usLoop];
	}
	return ucSum;
}
