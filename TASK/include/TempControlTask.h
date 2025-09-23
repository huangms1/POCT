#ifndef __TEMP_CONTROL_TASK_H__
#define __TEMP_CONTROL_TASK_H__
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_pid.h"
#include "semphr.h"
#include <string.h>


//任务优先级
#define TEMP_CONTROL_TASK_PRIO		2
//任务堆栈大小	
#define TEMP_CONTROL_STK_SIZE 		512  
//任务句柄
extern TaskHandle_t TempControlTask_Handler;

//任务函数
void Temp_Control_task(void *pvParameters);



#endif
