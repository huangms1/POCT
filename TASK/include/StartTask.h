#ifndef __START_TASK_H__
#define __START_TASK_H__
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

//任务优先级
#define START_TASK_PRIO		4
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
extern TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

extern SemaphoreHandle_t Socket_InitSeceass_SemaphoreHandle;	   //网络初始化完成信号量
extern SemaphoreHandle_t Vibrate_SemaphoreHandle;                  //M10震荡信号量
extern SemaphoreHandle_t VibrateStop_SemaphoreHandle;              //M10震荡信号量

void bootloader_start_task(void *pvParameters);

#endif //__START_TASK_H__
