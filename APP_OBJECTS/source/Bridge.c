#include "Bridge.h"
#include "ZQueue.h"
#include "ZList.h"
#include "Endpoint.h"
#include "Me.h"
#include "Interpreter.h"
#include "Reply.h"

/**
  * @brief  消息处理核心函数
  * @param  pMsg: 消息结构体指针
  * @note   功能说明：
  *         1. 执行消息解析（协议解析）
  *         2. 处理传输确认消息（ACK）：
  *            a. 从重传列表移除已确认消息
  *            b. 释放消息内存
  *         3. 处理普通消息：
  *            a. 发送传输确认应答
  *            b. 根据紧急程度分发到不同执行队列
  * @warning 需确保消息指针有效性，调用后指针可能被置NULL
  */
void ProcessMsg(SMsg * pMsg)
{
	if(Interprete(pMsg))  // 协议解析
	{
		if(IsTransAckMsg(pMsg))  // 传输确认消息处理
		{
			// 清除已确认的重传消息
			EndpointRemoveAckedMsg(pMsg->m_pEndpoint, pMsg);
			// 释放消息内存
			DeleteMsg(pMsg);
			pMsg = NULL;
		}
		else  // 普通消息处理
		{
			// 发送传输层确认应答
			SendTransAck(pMsg);
		
			// 根据紧急程度分发消息
			if(Me.IsMsgUrgent2MeFun(pMsg))  // 紧急消息判断
			{
				// 压入紧急执行队列
				Push2Queue(Me.m_pUrgentExeQueue,pMsg);
			}
			else  // 普通消息处理
			{
				// 压入普通执行队列
				Push2Queue(Me.m_pNormalExeQueue,pMsg);
			}
		}
	}
};

//void CommBoardMsgRouter(SMsg * pMsg)
//{
//	if(pMsg->m_pEndpoint == &CommSocketEndpoint)
//	{
//		uchar ucAddr = GetAddrFromMsg(pMsg);
//		
//		switch(ucAddr)
//		{
//			case MAIN_1_BOARD_ADDR:
//				Push2Queue(CommSerial1Endpoint.m_pWaitSendQueue,pMsg);
//				break;
//			case MAIN_2_BOARD_ADDR:
//				Push2Queue(CommSerial2Endpoint.m_pWaitSendQueue,pMsg);
//				break;
//			case MAIN_3_BOARD_ADDR:
//				Push2Queue(CommSerial3Endpoint.m_pWaitSendQueue,pMsg);
//				break;
//			case MAIN_4_BOARD_ADDR:
//				Push2Queue(CommSerial4Endpoint.m_pWaitSendQueue,pMsg);
//				break;
//			default:
//				WholeMsgMemPut(pMsg);
//				break;
//		}
//	}
//	else
//	{
//		Push2Queue(CommSocketEndpoint.m_pWaitSendQueue,pMsg);
//	}
//}

//void MainBoardMsgRouter(SMsg * pMsg)
//{
//	if(pMsg->m_pEndpoint == &MainSerialUpEndpoint)
//	{
//		Push2Queue(MainSerialDownEndpoint.m_pWaitSendQueue,pMsg);
//	}
//	else
//	{
//		Push2Queue(MainSerialUpEndpoint.m_pWaitSendQueue,pMsg);
//	}
//}
//void TempBoardMsgRouter(SMsg * pMsg)
//{
//	WholeMsgMemPut(pMsg);
//}
