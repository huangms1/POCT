#include <stdio.h>
#include "StartTask.h"
#include "SocketTask.h"
#include "LogTask.h"
//#include "SerialTask.h"
//#include "UrgentExeTask.h"
#include "NormalExeTask.h"
#include "RoutineTask.h"
//#include "PublicDefine.h"
#include "Endpoint.h"
//#include "W5500.h"
#include "Me.h"
//#include "Usart.h"
#include "TempControlTask.h"

#include "UrgentExeTask.h"

#include "main.h"

#include "IAPTask.h"

#include "VibrateTask.h"

#include "InstrumentInclude.h"
#include "mytimer.h"

TaskHandle_t StartTask_Handler;

SemaphoreHandle_t Socket_InitSeceass_SemaphoreHandle;	      //网络初始化完成信号量

SemaphoreHandle_t Vibrate_SemaphoreHandle;                   //M10震荡信号量

SemaphoreHandle_t VibrateStop_SemaphoreHandle;              //M10震荡信号量

SemaphoreHandle_t myTimerFlagSemaphoreHandle = NULL;

EventGroupHandle_t ledEventGroupHandle;

#if IS_BOOTLOADER_PROGRAM
void bootloader_start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //进入临界区
	MeInit();
	EndpointInit(&SocketEndpoint,"SocketEndpoint",&NetSend,&NetRecv);
	Socket_InitSeceass_SemaphoreHandle = xSemaphoreCreateBinary();//网络初始化完成信号量
	//创建网口收发任务
    xTaskCreate((TaskFunction_t )SocketTask,             
                (const char*    )"SocketTask",           
                (uint16_t       )SOCKET_STK_SIZE,        
                (void*          )&SocketEndpoint,                  
                (UBaseType_t    )SOCKET_TASK_PRIO,        
                (TaskHandle_t*  )&SocketTask_Handler);
				
	//创建执行常规指令任务
    xTaskCreate((TaskFunction_t )IAPTask,             
                (const char*    )"IAPTask",           
                (uint16_t       )IAP_TASK_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )IAP_TASK_PRIO,        
                (TaskHandle_t*  )&IAPTask_Handler);
								
		//创建日常任务
    xTaskCreate((TaskFunction_t )RoutineTask,             
                (const char*    )"RoutineTask",           
                (uint16_t       )ROUTINE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )ROUTINE_TASK_PRIO,        
                (TaskHandle_t*  )&RoutineTask_Handler); 
				
	vTaskDelete(StartTask_Handler); //删除开始任务	
	
    taskEXIT_CRITICAL();            //退出临界区
}
#else
//开始任务任务函数
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();           //进入临界区

	MeInit();

	EndpointInit(&SocketEndpoint,"SocketEndpoint",&NetSend,&NetRecv);

	Socket_InitSeceass_SemaphoreHandle = xSemaphoreCreateBinary();//网络初始化完成信号量

	Vibrate_SemaphoreHandle = xSemaphoreCreateBinary();           //震荡信号量

	VibrateStop_SemaphoreHandle = xSemaphoreCreateBinary();       //M10震荡信号量
	
	ledEventGroupHandle = xEventGroupCreate();
		
#ifdef LOG_ENABLE
	//创建日志任务
    xTaskCreate((TaskFunction_t )LogTask,             
                (const char*    )"LogTask",           
                (uint16_t       )LOG_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )LOG_TASK_PRIO,        
                (TaskHandle_t*  )&LogTask_Handler); 
#endif	
	//创建网口收发任务
    xTaskCreate((TaskFunction_t )SocketTask,             
                (const char*    )"SocketTask",           
                (uint16_t       )SOCKET_STK_SIZE,        
                (void*          )&SocketEndpoint,                  
                (UBaseType_t    )SOCKET_TASK_PRIO,        
                (TaskHandle_t*  )&SocketTask_Handler); 
	
	//创建执行常规指令任务
    xTaskCreate((TaskFunction_t )NormalExeTask,             
                (const char*    )"NormalExeTask",           
                (uint16_t       )NORMAL_EXE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )NORMAL_EXE_TASK_PRIO,        
                (TaskHandle_t*  )&NormalExeTask_Handler); 
								
	//创建日常任务
    xTaskCreate((TaskFunction_t )RoutineTask,             
                (const char*    )"RoutineTask",           
                (uint16_t       )ROUTINE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )ROUTINE_TASK_PRIO,        
                (TaskHandle_t*  )&RoutineTask_Handler); 
								
	//创建温控任务
    xTaskCreate((TaskFunction_t )Temp_Control_task,             
                (const char*    )"SCVM_usart_task",           
                (uint16_t       )TEMP_CONTROL_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TEMP_CONTROL_TASK_PRIO,        
                (TaskHandle_t*  )&TempControlTask_Handler); 
				
	//紧急执行任务
    xTaskCreate((TaskFunction_t )UrgentExeTask,             
                (const char*    )"UrgentExeTask",           
                (uint16_t       )URGENT_EXE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )URGENT_EXE_TASK_PRIO,        
                (TaskHandle_t*  )&Urgent_EXE_Task_Handler); 
						
    xTaskCreate((TaskFunction_t )VibrateTask,             
                (const char*    )"VibrateTask",           
                (uint16_t       )VIBRATE_TASK_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )VIBRATE_TASK_PRIO,        
                (TaskHandle_t*  )&VibrateTask_Handler); 

									
    vTaskDelete(StartTask_Handler); //删除开始任务	
	
    taskEXIT_CRITICAL();            //退出临界区
}

#endif


