#ifndef __VIBRATE_TASK_H__
#define __VIBRATE_TASK_H__

#include "FreeRTOS.h"
#include "task.h"


#define VIBRATE_TASK_STK_SIZE  		256

#define VIBRATE_TASK_PRIO           3

extern TaskHandle_t                 VibrateTask_Handler;


void VibrateTask(void *pData);




#endif
