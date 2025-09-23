#ifndef __IAP_TASK_H__
#define __IAP_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "Me.h"
#include "poct_process.h"
//#include "semphr.h"
//#include "event_groups.h"

//#include "includes.h"

#define IAP_TASK_STK_SIZE  		128

#define IAP_TASK_PRIO           1

extern TaskHandle_t IAPTask_Handler;

extern uint32_t flash_app_addr;



void IAPTask(void *pData);


#endif //__URGENT_EXE_TASK_H__
