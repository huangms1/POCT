#include "TempConTrolTask.h"

#include "poct_process.h"

#include "LogTask.h"

#include <string.h>

TaskHandle_t TempControlTask_Handler;

//温控任务函数
void Temp_Control_task(void *pvParameters)
{
	assign_struct();
	
	memset(&Globle.TempControl,0,sizeof(TempControlTypedef));
	
	while(1)
	{
		if(Globle.TempControl.Temp_Ch1_ENABLE == SET)
		{			
			PID_Calculate(Globle.TempControl.Temp_Ch1_Set/10.0,0);
			vTaskDelay(20);
		}
		if(Globle.TempControl.Temp_Ch2_ENABLE == SET)
		{
			PID_Calculate(Globle.TempControl.Temp_Ch2_Set/10.0,1);
			vTaskDelay(20);
		}
		if(Globle.TempControl.Temp_Ch3_ENABLE == SET)
		{
			PID_Calculate(Globle.TempControl.Temp_Ch3_Set/10.0,2);
			vTaskDelay(20);
		}			
		
		/**************************************
		if(Globle.TempControl.Temp_Ch1_ENABLE == SET)
		{			
			PID_Calculate(Globle.TempControl.Temp_Ch1_Set/10.0,0);
			vTaskDelay(30);
		}
		if(Globle.TempControl.Temp_Ch2_ENABLE == SET)
		{
			PID_Calculate(Globle.TempControl.PcrTempSet/10.0,1);
			vTaskDelay(30);
		}
		if(Globle.TempControl.Temp_Ch3_ENABLE == SET)
		{
			PID_Calculate(Globle.TempControl.PcrTempSet/10.0,1);
			vTaskDelay(30);
		}
		***************************************/
		vTaskDelay(10);
	}
}

