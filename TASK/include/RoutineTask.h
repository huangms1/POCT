#ifndef __ROUTINE_TASK_H__
#define __ROUTINE_TASK_H__

#include "FreeRTOS.h"
#include "task.h"

//#include "bsp_motor.h"
#include "poct_process.h"

//任务优先级
#define ROUTINE_TASK_PRIO		2
//任务堆栈大小	
#define ROUTINE_STK_SIZE 		128  
//任务句柄
extern TaskHandle_t RoutineTask_Handler;

void RoutineTask(void *pData);

void CommRoutine(void);

void MainRoutine(void);

void TempRoutine(void);

#endif //__ROUTINE_TASK_H__
