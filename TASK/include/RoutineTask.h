#ifndef __ROUTINE_TASK_H__
#define __ROUTINE_TASK_H__

#include "FreeRTOS.h"
#include "task.h"

//#include "bsp_motor.h"
#include "poct_process.h"

//�������ȼ�
#define ROUTINE_TASK_PRIO		2
//�����ջ��С	
#define ROUTINE_STK_SIZE 		128  
//������
extern TaskHandle_t RoutineTask_Handler;

void RoutineTask(void *pData);

void CommRoutine(void);

void MainRoutine(void);

void TempRoutine(void);

#endif //__ROUTINE_TASK_H__
