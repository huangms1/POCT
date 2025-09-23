#include <stdlib.h>
#include "NormalExeTask.h"
#include "Me.h"
#include "key.h"
#include "bsp_motor.h"
#include "InstrumentInclude.h"

TaskHandle_t NormalExeTask_Handler;

void NormalExeTask(void *pData)
{
		
	while(1)
	{
		SMsg * pMsg = PopFromQueue(Me.m_pNormalExeQueue);
		
		if(NULL != pMsg)
			ExeMsgIfNotExe(pMsg, Me.NormalExe);
	
		vTaskDelay(5);
		
		/*按键开关门*/
		if(KEY_Scan() == 1)
		{
			Door_Control(DoorState.Door_sta);
		}
	};
	
}

