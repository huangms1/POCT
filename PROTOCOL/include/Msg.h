#ifndef __MSG_H__
#define __MSG_H__
#include <stdio.h>
#include <stdbool.h>
#include "includes.h"
#include "PublicDefine.h"
#include "ExpiredTimer.h"


#define SYN_HEAD                         0x55 //ͷ

#define SYN_FIN                          0xAA //β

#define SYN_HEAD_BYTES                   1

#define LENGTH_FIELD_BYTES               2

#define ADDR_FIELD_BYTES                 1

#define NUMBER_FIELD_BYTES               2

#define TYPE_FIELD_BYTES                 1

#define NUMBER_FIELD_HIGH_VALUE_PC_2_MCU 0x01

#define NUMBER_FIELD_HIGH_VALUE_MCU_2_PC 0x02

#define TYPE_FIELD_SEND_VALUE            0x00

#define TYPE_FIELD_AUTO_REPLY_VALUE      0xFF

#define CHECK_SUM_BYTES                  1

#define SYN_END_BYTES                    1

#define COMMA_SEPARATOR				    ','

#define RESEND_MAX_COUNT			    3

#define RESEND_INTERVAL_MS			    5000


typedef struct SByteArray
{
	uchar  * m_pData;
	
	uint	 	 m_uiSize;
	
}SByteArray;

typedef struct SReSendControl
{
	uint	 	 				m_uiReSendCount;
	
	SExpiredTimer	 	        m_ExpiredTimer;
	
	bool		 				m_bEnabled;
	
}SReSendControl;

typedef struct SAppMsg
{
	char  * m_pAppLayerNo;
	
	char  * m_pAppLayerType;
	
	char  * m_pAppLayerSender;
	
	char  * m_pAppLayerReceiver;
	
	char  * m_pAppLayerMainCmd;
	
	char  * m_pAppLayerSubCmd;
	
	char  * m_pAppLayerMainPara;
	
	char  * m_pAppLayerSubPara;

}SAppMsg;

#pragma pack(1)

typedef struct SMsgHeader
{
	uchar					m_ucSynHead;
	
	uchar					m_usFrameLenH;
	
	uchar					m_usFrameLenL;
	
	uchar					m_ucAddr;
	
	uchar					m_ucTransFrameBelong;
	
	uchar					m_ucTransFrameNo;
	
	uchar					m_ucTransFrameType;

}SMsgHeader;

#pragma pack()

struct SEndpoint;

typedef struct SMsg
{
	SMsgHeader				*		m_pHeader;
	
	SAppMsg						* 	m_pAppMsg;
	
	SByteArray				* 	m_pRawFrame;
	
	SReSendControl 		* 	m_pReSendControl;
	
	struct SEndpoint	*		m_pEndpoint;
	
	bool									m_bIsInterpreted;
		
}SMsg;


void* WholeMsgMemGet(void);

void WholeMsgMemPut(void * ptr);

SMsg * CreateMsg(void);

uchar GetAddrFromMsg(SMsg * pMsg);

ushort GetAppMsgLen(SMsg * pMsg);

bool IsTransAckMsg(SMsg * pMsg);

void DeleteMsg(SMsg * pMsg);

#endif //__MSG_H__
