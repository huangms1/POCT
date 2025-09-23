#ifndef __TEMP_CONTROL_TASK_H__
#define __TEMP_CONTROL_TASK_H__
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_pid.h"
#include "semphr.h"
#include <string.h>


//�������ȼ�
#define TEMP_CONTROL_TASK_PRIO		2
//�����ջ��С	
#define TEMP_CONTROL_STK_SIZE 		512  
//������
extern TaskHandle_t TempControlTask_Handler;

//������
void Temp_Control_task(void *pvParameters);



#endif
