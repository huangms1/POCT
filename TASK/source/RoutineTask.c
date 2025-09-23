#include "RoutineTask.h"
#include "Me.h"
#include "Reply.h"
#include "AppMsgDefine.h"
#include "Endpoint.h"
#include "FreeRTOS.h"
#include "StartTask.h"
#include "InstrumentInclude.h"
#include "main.h"

#define WATCH_DOG_BARK_INTERVAL_MS 		  8000

#define FEED_WATCH_DOG_INTERVAL_MS 		  4000

#define HAND_SHAKE_INTERVAL_MS 			  10000

TaskHandle_t RoutineTask_Handler;

static void MainInit(void);

//static void SetWatchDog(uint iBarkInterval);

//static void FeedWatchDog(void);

static void HandShake(void);


void RoutineTask(void *pData)
{	
	while(1)
	{
		Me.Routine();
		
		vTaskDelay(10); 		
	}
}

void MainRoutine(void)
{
		
#define MAIN_BOARD_QUERY_ADDR_DELAY_MS 1000
	
	switch(Me.m_RoutineManager.m_eRunningState)
	{
		case eRunningState_InitBegin:
			
			MainInit();
					
			Me.m_RoutineManager.m_eRunningState = eRunningState_InitComplete;
				
			break;
		case eRunningState_InitComplete:
			
			SendFrameI(APP_MSG_UNIT_MAIN,	APP_MSG_UNIT_PC,\
								APP_MSG_MAIN_CMD_RPT,	APP_MSG_SUB_CMD_INI,"",	SUB_PARA_T,&SocketEndpoint);
			
			SendFrameI(APP_MSG_UNIT_MAIN,	APP_MSG_UNIT_PC,\
					   APP_MSG_MAIN_CMD_RPT,	APP_MSG_SUB_CMD_VER,"",	INSTRUMENTS_VERSION,&SocketEndpoint);
			
//			SetWatchDog(WATCH_DOG_BARK_INTERVAL_MS);
		
			StartExpiredTimer(&Me.m_RoutineManager.m_FeedWatchDogExpiredTimer,FEED_WATCH_DOG_INTERVAL_MS);

			StartExpiredTimer(&Me.m_RoutineManager.m_HandShakeExpiredTimer,HAND_SHAKE_INTERVAL_MS);
					
			Me.m_RoutineManager.m_eRunningState = eRunningState_Running;
				
			break;		
		case eRunningState_Running:
			
//			FeedWatchDog();
		
			HandShake();
			break;
		default:
			break;
	}
}

static void MainInit(void)
{
	BaseType_t err = pdFALSE;
	
	Globle.Mechine.State = IDLE;
	
	while(1)
	{
		if(Socket_InitSeceass_SemaphoreHandle!=NULL)
		{
			err = xSemaphoreTake(Socket_InitSeceass_SemaphoreHandle,portMAX_DELAY);	//获取网络链接信号量
			if(err==pdTRUE)									
			{
				break; //网络链接成功
			}
		}
		else if(err==pdFALSE)
		{
			vTaskDelay(5); 
		}
	}

#if IS_BOOTLOADER_PROGRAM
	
#else
	ALL_Motor_RESET();
#endif
	ALL_LED_OFF();		
}

//static void SetWatchDog(uint iBarkInterval)
//{
//	
//}

//static void FeedWatchDog(void)
//{
//	
//}

static void HandShake(void)
{
	if(IsTimerExpired(&Me.m_RoutineManager.m_HandShakeExpiredTimer))
	{
		SendFrameI(APP_MSG_UNIT_MAIN,	APP_MSG_UNIT_PC,\
								APP_MSG_MAIN_CMD_RPT,	APP_MSG_SUB_CMD_HAND,"",	"",&SocketEndpoint);
//		vTaskDelay(5);
		StartExpiredTimer(&Me.m_RoutineManager.m_HandShakeExpiredTimer,HAND_SHAKE_INTERVAL_MS);	
	}
}
