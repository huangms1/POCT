#ifndef __NORMAL_EXE_TASK_H__
#define __NORMAL_EXE_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "poct_process.h"



//�������ȼ�
#define NORMAL_EXE_TASK_PRIO		3
//�����ջ��С	
#define NORMAL_EXE_STK_SIZE 		2048  
//������
extern TaskHandle_t NormalExeTask_Handler;

void NormalExeTask(void *pData);

#endif //__NORMAL_EXE_TASK_H__

