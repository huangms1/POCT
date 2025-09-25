#include "Endpoint.h"
#include "Msg.h"
#include "FreeRTOS.h"

SEndpoint SocketEndpoint;

/**
  * @brief  网络端点初始化函数
  * @param  pEndpoint: 端点对象指针
  * @param  pName: 端点名称字符串
  * @param  pSendFun: 底层发送函数指针
  * @param  pRecvFun: 底层接收函数指针
  * @note   功能说明：
  *         1. 初始化消息队列和重传列表
  *         2. 绑定底层收发回调函数
  *         3. 设置端点名称（自动截断超长部分）
  *         4. 启动帧接收超时定时器
  * @warning 名称长度不超过ENDPOINT_NAME_LEN定义值
  */
void EndpointInit(SEndpoint * pEndpoint,char * pName,SendFunPtr pSendFun,RecvFunPtr pRecvFun)
{
    int iCopyBytes = 0;
    
    // 初始化基本状态
    pEndpoint->m_bEnabled = true;
    
    // 创建消息队列（等待发送）
    pEndpoint->m_pWaitSendQueue = CreateQueue();
    
    // 创建重传消息链表
    pEndpoint->m_pReSendList = CreateList();
    
    // 绑定底层驱动函数
    pEndpoint->SendFun = pSendFun;
    pEndpoint->RecvFun = pRecvFun;
    
    // 清空端点名称缓冲区
    memset(pEndpoint->m_Name,0,ENDPOINT_NAME_LEN);
    
    // 安全复制端点名称（防溢出）
    iCopyBytes = strlen(pName) < ENDPOINT_NAME_LEN ? strlen(pName):ENDPOINT_NAME_LEN;
    memcpy(pEndpoint->m_Name,pName, iCopyBytes);
    
    // 启动帧接收超时定时器（FreeRTOS服务）
    StartExpiredTimer(&pEndpoint->m_FrameRecvExpiredTimer,FRAME_MAX_RECV_MS);
};
#include "LogTask.h"
/**
  * @brief  端点数据接收与消息解析函数
  * @param  pEndpoint: 网络端点对象指针
  * @note   功能说明：
  *         1. 检查端点及接收回调函数有效性
  *         2. 调用底层接收函数获取数据
  *         3. 当接收数据量>0时：
  *            a. 记录原始数据到日志系统
  *            b. 解析原始数据为结构化消息
  *         4. 接收状态机管理接收缓冲区和数据量
  */
void EndpointRecvAndSpell(SEndpoint * pEndpoint)
{
    if(NULL != pEndpoint)
    {
        // 验证接收回调函数有效性
        if(NULL != pEndpoint->RecvFun)
        {
            // 调用硬件驱动层接收函数（如W5500接收）
            pEndpoint->RecvFun(pEndpoint->m_RecvStateMachine.m_RecvBuffer,
                             &pEndpoint->m_RecvStateMachine.m_usRecvSize);
            
            // 当接收有效数据时执行消息处理
            if(pEndpoint->m_RecvStateMachine.m_usRecvSize > 0)
            {
                // 记录原始二进制数据（用于调试和故障分析）
                WRITE_DATA_LOG(pEndpoint->m_RecvStateMachine.m_RecvBuffer,
                             pEndpoint->m_RecvStateMachine.m_usRecvSize);
                
                // 解析原始字节流为应用层消息结构体
                SpellMsg(&pEndpoint->m_RecvStateMachine, pEndpoint);
            }    
        }
    }
};
/**
  * @brief  端点消息解析触发函数
  * @param  pEndpoint: 网络端点对象指针
  * @note   功能说明：
  *         1. 验证端点对象有效性
  *         2. 当接收缓冲区存在有效数据时
  *            a. 调用核心解析函数处理原始数据
  *         3. 通常用于非自动接收场景的手动解析触发
  * @warning 需确保接收状态机已正确初始化
  */
void EndpointSpell(SEndpoint * pEndpoint)
{
	if(NULL != pEndpoint)
	{
		if(pEndpoint->m_RecvStateMachine.m_usRecvSize > 0)
			SpellMsg(&pEndpoint->m_RecvStateMachine, pEndpoint);
	}
};

/**
  * @brief  端点消息发送控制函数
  * @param  pEndpoint: 网络端点对象指针
  * @note   功能说明：
  *         1. 验证端点和发送回调有效性
  *         2. 通过控制标志交替执行：
  *            a. TRUE状态：发送等待队列中的新消息
  *            b. FALSE状态：处理重传列表中的消息
  *         3. 实现新消息与重传消息的交替发送机制
  * @warning 需与EndpointSendOneFromWaitQueue/EndpointSendOneFromReSendList配合使用
  */
void EndpointSendOne(SEndpoint * pEndpoint)
{
	if(NULL != pEndpoint && NULL != pEndpoint->SendFun)
	{
		// 切换发送控制标志实现轮询机制
		if(pEndpoint->m_bSendControl)
		{
			pEndpoint->m_bSendControl = !pEndpoint->m_bSendControl;
			EndpointSendOneFromWaitQueue(pEndpoint);  // 优先发送新消息
		}
		else
		{
			pEndpoint->m_bSendControl = !pEndpoint->m_bSendControl;
			EndpointSendOneFromReSendList(pEndpoint);  // 处理需要重传的消息
		}
	}
};

/**
  * @brief  从等待队列发送单条消息
  * @param  pEndpoint: 网络端点对象指针
  * @note   功能说明：
  *         1. 检查等待队列是否有待发送消息
  *         2. 从队列头部取出消息对象
  *         3. 验证消息有效性后执行发送操作
  *         4. 根据重传控制标志：
  *            a. 启用重传：设置定时器并加入重传列表
  *            b. 禁用重传：立即释放消息内存
  * @warning 需要确保发送回调函数已正确初始化
  */
void EndpointSendOneFromWaitQueue(SEndpoint * pEndpoint)
{
	if(!IsQueueEmpty(pEndpoint->m_pWaitSendQueue))
	{
		// 从等待队列提取消息
		SMsg * pMsg = PopFromQueue(pEndpoint->m_pWaitSendQueue);
		
		if(NULL != pMsg)
		{
			// 检查消息是否已在重传列表
			if(!IsSendMsgInEndpointReSendList(pEndpoint,pMsg))
			{
				// 调用底层驱动发送原始数据帧
				pEndpoint->SendFun(pMsg->m_pRawFrame->m_pData,pMsg->m_pRawFrame->m_uiSize);
			
				// 处理消息重传逻辑
				if(pMsg->m_pReSendControl->m_bEnabled)
				{
					// 启动重传定时器
					StartExpiredTimer(&pMsg->m_pReSendControl->m_ExpiredTimer,RESEND_INTERVAL_MS);
					// 加入重传监控列表
					ListAppend(pEndpoint->m_pReSendList,pMsg);
				}
				else
				{
					// 立即释放无重传需求的消息
					DeleteMsg(pMsg);
					pMsg = NULL;
				}
			}
			else
			{
				// 发现重复消息直接释放
				DeleteMsg(pMsg);
				pMsg = NULL;
			}
		}
	}
};

void EndpointSendOneFromReSendList(SEndpoint * pEndpoint)
{
	if(!IsListEmpty(pEndpoint->m_pReSendList))
	{
		ListIterator it = ListBegin(pEndpoint->m_pReSendList);

		for(;NULL != it &&  it != ListEnd(pEndpoint->m_pReSendList); )
		{
			SMsg * pReSendMsg = it->m_pData;
			
			if(NULL != pReSendMsg \
				&& pReSendMsg->m_pReSendControl->m_uiReSendCount > 0 \
			  && IsTimerExpired(&pReSendMsg->m_pReSendControl->m_ExpiredTimer))
			{
				pEndpoint->SendFun(pReSendMsg->m_pRawFrame->m_pData,pReSendMsg->m_pRawFrame->m_uiSize);
				
				--pReSendMsg->m_pReSendControl->m_uiReSendCount;
				
				if(pReSendMsg->m_pReSendControl->m_uiReSendCount > 0)
					StartExpiredTimer(&pReSendMsg->m_pReSendControl->m_ExpiredTimer,RESEND_INTERVAL_MS);
			
				break;
			
			}
			it = it->m_pNext;
		}
		it = ListBegin(pEndpoint->m_pReSendList);
		
		for(; NULL != it && it != ListEnd(pEndpoint->m_pReSendList); )
		{
			SMsg * pReSendMsg = it->m_pData;
			
			if(NULL != pReSendMsg && pReSendMsg->m_pReSendControl->m_uiReSendCount == 0)
			{
				DeleteMsg(pReSendMsg);
				it = ListRemoveOne(pEndpoint->m_pReSendList,it);
			}
			else
			{
				it = it->m_pNext;
			}
		}
	}
};

uchar EndpointGetTransFrameNo(SEndpoint * pEndpoint)
{
	return ++ pEndpoint->m_ucTransFrameNo;
};

void EndpointRemoveAckedMsg(SEndpoint * pEndpoint, SMsg * pTransAckMsg)
{
	if(!IsListEmpty(pEndpoint->m_pReSendList))
	{
		ListIterator it = ListBegin(pEndpoint->m_pReSendList);
	
		for(;NULL != it &&  it != ListEnd(pEndpoint->m_pReSendList); )
		{
			SMsg * pReSendMsg = it->m_pData;
			
			if(NULL != pReSendMsg \
				&& pReSendMsg->m_pHeader->m_ucTransFrameBelong == pTransAckMsg->m_pHeader->m_ucTransFrameBelong \
				&& pReSendMsg->m_pHeader->m_ucTransFrameNo == pTransAckMsg->m_pHeader->m_ucTransFrameNo)
			{
				DeleteMsg(pReSendMsg);
				it = ListRemoveOne(pEndpoint->m_pReSendList,it);
			}
			else
			{
				it = it->m_pNext;
			}
		}
	}
};

bool IsSendMsgInEndpointReSendList(SEndpoint * pEndpoint,SMsg * pSendMsg)
{
	ListIterator it = ListBegin(pEndpoint->m_pReSendList);

	for(;NULL != it &&  it != ListEnd(pEndpoint->m_pReSendList); )
	{
		SMsg * pReSendMsg = it->m_pData;
		
		if(NULL != pReSendMsg \
			&& pReSendMsg->m_pHeader->m_ucTransFrameBelong == pSendMsg->m_pHeader->m_ucTransFrameBelong \
			&& pReSendMsg->m_pHeader->m_ucTransFrameNo == pSendMsg->m_pHeader->m_ucTransFrameNo \
			&& pReSendMsg->m_pHeader->m_ucAddr == pSendMsg->m_pHeader->m_ucAddr \
			&& pReSendMsg->m_pHeader->m_ucTransFrameType == pSendMsg->m_pHeader->m_ucTransFrameType)
		{
			return true;
		}
		else
		{
			it = it->m_pNext;
		}
	}
	return false;
}

bool IsTotalHeapAlmostUsedUp(void)
{
	size_t iFreeSize = xPortGetFreeHeapSize();
	
	int iTimes = configTOTAL_HEAP_SIZE/iFreeSize;
	
	return iTimes > 2 ? true:false;
}

void EndpointRemoveFirstMsgIfMemAlmostUsedUp(SEndpoint * pEndpoint)
{
	if(IsTotalHeapAlmostUsedUp())
	{
		if(!IsListEmpty(pEndpoint->m_pReSendList))
		{
			ZListNode * pMsgNode = ListTakeFirstNode(pEndpoint->m_pReSendList);
			
			if(NULL != pMsgNode)
			{
				SMsg * pMsg = (SMsg *)pMsgNode->m_pData;
				
				DeleteListNode(pMsgNode);
				
				if(NULL != pMsg)
					DeleteMsg(pMsg);
			}
		}
		else
		{
			if(!IsQueueEmpty(pEndpoint->m_pWaitSendQueue))
			{
				SMsg * pMsg = PopFromQueue(pEndpoint->m_pWaitSendQueue);
				
				if(NULL != pMsg)
					DeleteMsg(pMsg);
			}
		}
	}
};

