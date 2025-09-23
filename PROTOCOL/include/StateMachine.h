#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__
#include "Msg.h"

struct SEndpoint;

typedef enum ESpellState
{
		eSpellState_Syn_Head,
		eSpellState_Length,
		eSpellState_Body,
		eSpellState_CheckSum,
		eSpellState_Syn_Fin,
		eSpellState_Okey,
}ESpellState;

typedef struct SStateMachine
{
	ESpellState   m_eSpellState;
	
	uchar					m_RecvBuffer[MAX_RECEIVE_SIZE];
	
	uchar					m_SpelledBuffer[MAX_MSG_SIZE];
	
	ushort				m_usRecvSize;
	
	ushort				m_usIndex;
	
	ushort				m_usBodyLength;

	ushort				m_usLengthIndex;

	ushort				m_usIndexOfSynHead;

	ushort				m_usBodyCopiedLength;

}SStateMachine;

void SpellMsg(SStateMachine * pStateMachine,struct SEndpoint * pEndpoint);

uchar CheckSum(uchar * pData, ushort usLength);

#endif //__STATE_MACHINE_H__
