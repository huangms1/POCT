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

/**
  * @brief  主系统初始化函数
  * @note   功能说明：
  *         1. 设置全局设备状态为IDLE
  *         2. 循环等待网络连接就绪信号量
  *         3. 非Bootloader模式下执行电机复位
  *         4. 关闭所有LED指示灯
  * @warning 本函数包含阻塞操作，会持续等待网络连接信号量
  */
static void MainInit(void)
{
	BaseType_t err = pdFALSE;
	
	Globle.Mechine.State = IDLE;
	
	// 等待网络连接信号量（最长等待时间portMAX_DELAY）
	while(1)
	{
		if(Socket_InitSeceass_SemaphoreHandle != NULL)
		{
			// 尝试获取网络连接成功信号量
			err = xSemaphoreTake(Socket_InitSeceass_SemaphoreHandle,portMAX_DELAY);	
			if(err == pdTRUE)									
			{
				break; // 成功获取信号量，网络连接建立
			}
			vTaskDelay(5); // 短暂延时避免CPU占用过高
		}
		else if(err == pdFALSE)
		{
			vTaskDelay(5); 
		}
	}

#if IS_BOOTLOADER_PROGRAM
    // Bootloader模式下跳过硬件初始化
#else
    ALL_Motor_RESET(); // 复位所有电机驱动器
#endif
    ALL_LED_OFF();     // 关闭所有状态指示灯
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
