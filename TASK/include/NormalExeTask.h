#ifndef __NORMAL_EXE_TASK_H__
#define __NORMAL_EXE_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "poct_process.h"



//任务优先级
#define NORMAL_EXE_TASK_PRIO		1
//任务堆栈大小	
#define NORMAL_EXE_STK_SIZE 		256  
//任务句柄
extern TaskHandle_t NormalExeTask_Handler;

void NormalExeTask(void *pData);

#endif //__NORMAL_EXE_TASK_H__

