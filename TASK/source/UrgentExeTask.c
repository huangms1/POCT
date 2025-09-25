#include "UrgentExeTask.h"
#include "delay.h"
#include "Me.h"


OS_STK UrgentExeTaskStack[URGENT_EXE_STK_SIZE];

TaskHandle_t Urgent_EXE_Task_Handler;

//紧急指令任务函数

void UrgentExeTask(void *pData)
{	 	
	while(1)
	{
		SMsg * pMsg = PopFromQueue(Me.m_pUrgentExeQueue);
		
		if(NULL != pMsg)
			ExeMsgIfNotExe(pMsg, Me.UrgentExe);
		
		delay_ms(10);
	};
}
