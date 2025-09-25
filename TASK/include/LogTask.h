#ifndef ___LOG_TASK_H__
#define ___LOG_TASK_H__

void WriteTextLog(char * pTextLog);

void WriteDataLog(char * pData, int iLength);

void WriteFreeLog(const char * pFormat,...);

#ifdef LOG_ENABLE

#define WRITE_TEXT_LOG(x) WriteTextLog(x)
#define WRITE_DATA_LOG(x,y) WriteDataLog((char *)x,y)
#define WRITE_FREE_LOG(x,...) WriteFreeLog((const char *)x,##__VA_ARGS__)
	
#else
#define WRITE_TEXT_LOG(x)
#define WRITE_DATA_LOG(x,y)
#define WRITE_FREE_LOG(x,...)
#endif

#include "FreeRTOS.h"
#include "task.h"

//任务优先级
#define LOG_TASK_PRIO		6
//任务堆栈大小	
#define LOG_STK_SIZE 		256  
//任务句柄
extern TaskHandle_t LogTask_Handler;

void LogTask(void *pData);

#endif //___LOG_TASK_H__

