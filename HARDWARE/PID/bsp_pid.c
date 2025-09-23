/**
  ******************************************************************************
  * @file    bsp_pid_conf.c
  * @author  黄明送
  * @version V1.0
  * @date    2021-11-16
  * @brief   POCT，扩增温度控制。
  ******************************************************************************
  * @attention
  *
  * 运行平台:臻惠医疗分子诊断仪器。
  *
  ******************************************************************************
  */ 
  
#include "bsp_pid.h"
	
#include "poct_process.h"
	
#include "fuzzypid.h"
	
#include "FreeRTOS.h"
  
//PIDTypedef PID[5];
  
void PID_Init(void)
{
	assign_struct();
}

void PID_Calculate(float setTemp,uint8_t ch)
{ 	
	float currentTemp = (float)GetTemp(ch)/100.0f;
	
	COOL_Fuzzytrans(&FPID_data[ch],setTemp,currentTemp);

	if(FPID_data[ch].iError >= 35.0f)
	{
		FPID_data[ch].pwm = 999;
	}

	else if((FPID_data[ch].iError < 35)&&(FPID_data[ch].iError >= 18))
	{
		FPID_data[ch].pwm = 600;
	}
	
	else if((FPID_data[ch].iError >= 4) && (FPID_data[ch].iError < 18))
	{
		FPID_data[ch].pwm = 300;
		if(ch == 0)
		{
			FPID_data[ch].pwm = 200;
		}
	}
	
	else if(FPID_data[ch].iError < -0.2f)
	{
		FPID_data[ch].pwm = 0;
	}
			
	else      
	{		
		FPID_data[ch].pwm += FPID_data[ch].data_kp*(FPID_data[ch].iError - FPID_data[ch].lasterror) + \
		FPID_data[ch].data_ki*FPID_data[ch].iError + \
		FPID_data[ch].data_kd*(FPID_data[ch].iError - 2*FPID_data[ch].lasterror + FPID_data[ch].preerror);

		if(ch == 1)
		{
			if(FPID_data[ch].pwm >= 80) //60
				FPID_data[ch].pwm = 80;
			else if(FPID_data[ch].pwm <= 35)
				FPID_data[ch].pwm = 35;	
		}
		else if(ch == 0)
		{
			if(FPID_data[ch].pwm >= 80) //40
				FPID_data[ch].pwm = 80;
			else if(FPID_data[ch].pwm <= 20)
				FPID_data[ch].pwm = 20;	
		}
		else
		{
			if(FPID_data[ch].pwm >= 120) //80
				FPID_data[ch].pwm = 120;
			else if(FPID_data[ch].pwm <= 25)
				FPID_data[ch].pwm = 25;
		}
	}
		
	if(ch == 0)
	{		
		TIM_SetTIMCompare(TIM8,FPID_data[ch].pwm,2);
	}
	else if(ch == 1)
	{
		TIM_SetTIMCompare(TIM8,FPID_data[ch].pwm,3);
	}
	else if(ch == 2)
	{
		TIM_SetTIMCompare(TIM9,FPID_data[ch].pwm,0);
	}
	else if(ch == 3)
	{
		TIM_SetTIMCompare(TIM8,FPID_data[ch].pwm,0);
	} 		
	else if (ch == 4)
	{
		TIM_SetTIMCompare(TIM9,FPID_data[ch].pwm,1);
	} 
}
		 
HAL_StatusTypeDef PID_Stop(uint8_t ch)
{
	HAL_StatusTypeDef state;
	switch(ch)
	{
		case 0:
		{
			if(HAL_TIM_PWM_Stop(&TIM8_Handler,TIM_CHANNEL_3) == HAL_OK)
			{
				Globle.TempControl.Temp_Ch1_ENABLE = RESET;
			}			
			break;
		}
		case 1:
		{
			TIM_SetTIMCompare(TIM8,0,3);
			
			if(HAL_TIM_PWM_Stop(&TIM8_Handler,TIM_CHANNEL_4) == HAL_OK)
			{
				Globle.TempControl.Temp_Ch2_ENABLE = RESET;
			}
			break;
		}
		case 2:
		{			
			if(HAL_TIM_PWM_Stop(&TIM9_Handler,TIM_CHANNEL_1) == HAL_OK)
			{
				Globle.TempControl.Temp_Ch3_ENABLE = RESET;
			}	
			break;
		}
			
		case 3:
		{				
			if(HAL_TIM_PWM_Stop(&TIM8_Handler,TIM_CHANNEL_1) == HAL_OK)
			{
				Globle.TempControl.Temp_Ch4_ENABLE = RESET;
			}
			break;
		}
			
		case 4:
		{				
			if(HAL_TIM_PWM_Stop(&TIM9_Handler,TIM_CHANNEL_2) == HAL_OK)
			{
				Globle.TempControl.Temp_Ch5_ENABLE = RESET;
			}			
			break;
		}
					
		default:
			break;
	}
	return state;
}

//结束


