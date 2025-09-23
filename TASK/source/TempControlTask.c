#include "TempConTrolTask.h"

#include "poct_process.h"

TaskHandle_t TempControlTask_Handler;

//ÎÂ¿ØÈÎÎñº¯Êý
void Temp_Control_task(void *pvParameters)
{
	
	memset(&Globle.TempControl,0,sizeof(TempControlTypedef));
	
	while(1)
	{
		if(Globle.TempControl.Temp_Ch1_ENABLE == SET)
		{			
			PID_Calculate(Globle.TempControl.Temp_Ch1_Set/10.0,0);
			vTaskDelay(10);
		}
		if(Globle.TempControl.Temp_Ch2_ENABLE == SET)
		{
			PID_Calculate(Globle.TempControl.Temp_Ch2_Set/10.0,1);
			vTaskDelay(10);
		}
		if(Globle.TempControl.Temp_Ch3_ENABLE == SET)
		{
			PID_Calculate(Globle.TempControl.Temp_Ch3_Set/10.0,2);
			vTaskDelay(10);
		}	
		if(Globle.TempControl.Temp_Ch4_ENABLE == SET)
		{
			PID_Calculate(Globle.TempControl.Temp_Ch4_Set/10.0,3);
			vTaskDelay(10);
		}			
		if(Globle.TempControl.Temp_Ch5_ENABLE == SET)
		{
			PID_Calculate(Globle.TempControl.Temp_Set_Ch5/10.0,4);
			vTaskDelay(10);
		}		
		vTaskDelay(20);
	}
}

