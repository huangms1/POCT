#ifndef __URGENT_EXE_TASK_H__
#define __URGENT_EXE_TASK_H__

#include "includes.h"
#include "FreeRTOS.h"
#include "task.h"

#define URGENT_EXE_STK_SIZE  					  512

//任务优先级
#define URGENT_EXE_TASK_PRIO		              7

extern TaskHandle_t Urgent_EXE_Task_Handler;

extern OS_STK UrgentExeTaskStack[URGENT_EXE_STK_SIZE];

void UrgentExeTask(void *pData);

#endif //__URGENT_EXE_TASK_H__
