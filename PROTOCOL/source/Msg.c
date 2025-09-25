#include <string.h>
#include <stdlib.h>
#include "Msg.h"
#include "FreeRTOS.h"
#include "Me.h"
#include "Reply.h"
#include "LogTask.h"

void* WholeMsgMemGet(void)
{
	SMsg * pWholeMsg = (SMsg *)pvPortMalloc(sizeof(SMsg));
	
	pWholeMsg->m_pHeader = (SMsgHeader *)pvPortMalloc(sizeof(SMsgHeader));
	
	memset(pWholeMsg->m_pHeader,0, sizeof(SMsgHeader));
	
	pWholeMsg->m_pReSendControl = (SReSendControl *)pvPortMalloc(sizeof(SReSendControl));
	
	memset(pWholeMsg->m_pReSendControl,0, sizeof(SReSendControl));
	
	pWholeMsg->m_pAppMsg = (SAppMsg *)pvPortMalloc(sizeof(SAppMsg));
	
	memset(pWholeMsg->m_pAppMsg,0, sizeof(SAppMsg));

	pWholeMsg->m_pRawFrame = (SByteArray *)pvPortMalloc(sizeof(SByteArray));
	
	pWholeMsg->m_pRawFrame->m_uiSize = 0;
	
	pWholeMsg->m_pRawFrame->m_pData = (uchar *)pvPortMalloc(MAX_MSG_SIZE);
	
	memset(pWholeMsg->m_pRawFrame->m_pData,0, MAX_MSG_SIZE);
	
//	WRITE_TEXT_LOG("MSG MEM GET");

//  WRITE_FREE_LOG("%s:%d\n","msg mem get",10);
//  WRITE_FREE_LOG("%s");

	return pWholeMsg;
};

void WholeMsgMemPut(void * ptr)
{
	SMsg * pWholeMsg = NULL;
	
	if(NULL == ptr)
		return;
	
	pWholeMsg = (SMsg * )ptr;
	
	if(NULL != pWholeMsg->m_pHeader)
		vPortFree(pWholeMsg->m_pHeader);
	
	if(NULL != pWholeMsg->m_pReSendControl)
		vPortFree(pWholeMsg->m_pReSendControl);
	
	if(NULL != pWholeMsg->m_pRawFrame)
	{
		if(NULL != pWholeMsg->m_pRawFrame->m_pData)
			vPortFree(pWholeMsg->m_pRawFrame->m_pData);
		
		vPortFree(pWholeMsg->m_pRawFrame);
	}
	
	if(NULL != pWholeMsg->m_pAppMsg)
	{
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerNo)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerNo);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerType)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerType);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerSender)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerSender);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerReceiver)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerReceiver);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerMainCmd)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerMainCmd);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerSubCmd)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerSubCmd);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerMainPara)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerMainPara);
		
		if(NULL != pWholeMsg->m_pAppMsg->m_pAppLayerSubPara)
			vPortFree(pWholeMsg->m_pAppMsg->m_pAppLayerSubPara);
		
		vPortFree(pWholeMsg->m_pAppMsg);
	}
	vPortFree(ptr);
	
//	WRITE_TEXT_LOG("MSG MEM PUT");
};

SMsg * CreateMsg(void)
{
	SMsg * pMsg = (SMsg *)WholeMsgMemGet();
	
	return pMsg;
};

/**
  * @brief  获取消息目标地址
  * @param  pMsg: 消息结构体指针
  * @retval uchar: 设备地址字节
  * @note   功能说明：
  *         1. 当消息已解析时，从协议头获取地址
  *         2. 未解析时直接从原始数据帧的固定位置读取
  *         3. 地址位置 = 同步头长度 + 长度字段长度
  * @warning 未解析时直接访问原始数据需确保协议格式稳定
  */
uchar GetAddrFromMsg(SMsg * pMsg)
{
    if(pMsg->m_bIsInterpreted)
    {
        return pMsg->m_pHeader->m_ucAddr;
    }
    else
    {
        return pMsg->m_pRawFrame->m_pData[SYN_HEAD_BYTES + LENGTH_FIELD_BYTES];
    }
};

/**
  * @brief  计算应用层消息总长度
  * @param  pMsg: 消息结构体指针
  * @retval ushort: 消息总字节数（包含分隔符）
  * @note   功能说明：
  *         1. 累加各字段字符串长度（包含：层级编号、类型、发送方、接收方、主命令、子命令等）
  *         2. 包含7个字段分隔逗号和1个结尾逗号，共8个逗号
  *         3. 自动跳过空指针参数字段
  * @warning 需确保各字段指针有效（除MainPara/SubPara外）
  */
ushort GetAppMsgLen(SMsg * pMsg)
{
#define COMMA_COUNT_AMONG_APP_MSG 8  // 字段间分隔符总数
	
	ushort usAppMsgLen = 0;
	
	// 参数有效性检查
	if(NULL == pMsg || IsTransAckMsg(pMsg))
		return 0;
	
	// 累加各字段长度（按协议顺序）
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerNo);        // 层级编号
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerType);     // 消息类型
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerSender);    // 发送方标识
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerReceiver); // 接收方标识
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerMainCmd);   // 主命令码
	usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerSubCmd);    // 子命令码
	
	// 可选参数处理
	if(NULL != pMsg->m_pAppMsg->m_pAppLayerMainPara)
		usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerMainPara); // 主参数
	if(NULL != pMsg->m_pAppMsg->m_pAppLayerSubPara)
		usAppMsgLen += strlen((char *)pMsg->m_pAppMsg->m_pAppLayerSubPara);  // 子参数
	
	// 添加字段分隔符总长度
	usAppMsgLen += COMMA_COUNT_AMONG_APP_MSG;
	
	return usAppMsgLen;
};

/**
  * @brief  判断是否为传输确认消息
  * @param  pMsg: 消息结构体指针
  * @retval bool: 确认结果
  *         true - 是自动回复的确认消息
  *         false - 非确认消息
  * @note   通过检查消息头中的传输帧类型字段实现
  *         协议常量TYPE_FIELD_AUTO_REPLY_VALUE表示自动回复类型
  */
bool IsTransAckMsg(SMsg * pMsg)
{
	return TYPE_FIELD_AUTO_REPLY_VALUE == pMsg->m_pHeader->m_ucTransFrameType;
};

void DeleteMsg(SMsg * pMsg)
{
	if(NULL != pMsg)
	{
		WholeMsgMemPut(pMsg);
	}
};

