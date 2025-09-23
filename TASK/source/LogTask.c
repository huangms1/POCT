#include "includes.h"
#include "LogTask.h"
#include "ZQueue.h"
#include "Msg.h"
#include "usart.h"

TaskHandle_t LogTask_Handler;

static ZQueue * gpLogQueue;

static bool IsLogUsartTxReady(void)
{
	return HAL_UART_STATE_READY == Uart1_Handle.State;
}

void WriteTextLog(char * pTextLog)
{
	if(NULL != pTextLog)
	{
		uchar tail[2] = {'\r','\n'};
		
		int iTextLength = 0;
		
		SByteArray * pByteArray = (SByteArray *)pvPortMalloc(sizeof(SByteArray));
		
		iTextLength = strlen(pTextLog);
		
		pByteArray->m_uiSize = iTextLength + 2;
		
		pByteArray->m_pData = (uchar *)pvPortMalloc(pByteArray->m_uiSize);
		
		memcpy(pByteArray->m_pData,pTextLog, iTextLength);
			
		memcpy(pByteArray->m_pData + iTextLength,tail, sizeof(tail));
		
		Push2Queue(gpLogQueue,pByteArray);
	}
}

void WriteDataLog(char * pData, int iLength)
{
	if(NULL != pData && iLength > 0)
	{
		uchar tail[2] = {'\r','\n'};
		
		SByteArray * pByteArray = (SByteArray *)pvPortMalloc(sizeof(SByteArray));
		
		pByteArray->m_uiSize = iLength + 2;
		
		pByteArray->m_pData = (uchar *)pvPortMalloc(pByteArray->m_uiSize);
		
		memcpy(pByteArray->m_pData,pData, iLength);
		
		memcpy(pByteArray->m_pData + iLength,tail, sizeof(tail));
		
		Push2Queue(gpLogQueue,pByteArray);
	}
}

void WriteFreeLog(const char * pFormat,...)
{
	char buf[128] = {0};
	
	int iSize = 0;
	
	SByteArray * pByteArray = NULL;
	
	va_list arg;
	
	va_start(arg,pFormat);
	
	iSize = vsprintf(buf, pFormat, arg);
	
	va_end(arg);
	
	pByteArray = (SByteArray *)pvPortMalloc(sizeof(SByteArray));
	
	pByteArray->m_uiSize = iSize;
	
	pByteArray->m_pData = (uchar *)pvPortMalloc(iSize);
	
	memcpy(pByteArray->m_pData,buf, iSize);
	
	Push2Queue(gpLogQueue,pByteArray);
}

void LogTask(void *pData)
{
	gpLogQueue = CreateQueue();

	while(1)
	{
		SByteArray * pByteArray = (SByteArray *)PopFromQueue(gpLogQueue);
		
		if(NULL != pByteArray)
		{
			if(IsLogUsartTxReady())
				HAL_USART_Transmit_DMA(&Uart1_Handle, pByteArray->m_pData,pByteArray->m_uiSize);
			
			vPortFree(pByteArray->m_pData);
		}
		vPortFree(pByteArray);
		vTaskDelay(5); 
	};
}

