#ifndef __SOCKET_TASK_H__
#define __SOCKET_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "tcp_handler.h"

//任务优先级
#define SOCKET_TASK_PRIO		1
//任务堆栈大小	
#define SOCKET_STK_SIZE 		128  
//任务句柄
extern TaskHandle_t SocketTask_Handler;

void SocketTask(void *pData);

#endif //__SOCKET_TASK_H__
