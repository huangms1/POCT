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
  
PIDTypedef PID[3];
  
void PID_Init(void)
{
	assign_struct();
//#define CH0_KP  5
//#define CH0_KI  1
//#define CH0_KD  0.01
//	
//	PID[0].Kp = CH0_KP;
//	PID[0].Ki = CH0_KI;
//	PID[0].Kd = CH0_KD;
//	
//	PID[1].Kp = 20;
//	PID[1].Ki = 18;
//	PID[1].Kd = 6;
//	
//	PID[2].Kp = 20;
//	PID[2].Ki = 18;
//	PID[2].Kd = 6;
}

void PID_Calculate(float setTemp,uint8_t ch)
{ 	
	float currentTemp = (float)GetTemp(ch)/100.0f;
	FUZZYPID *pid = &FPID_data[ch];
	
	COOL_Fuzzytrans(pid,setTemp,currentTemp);
//	PID[ch].err = setTemp - currentTemp;
	
//	if(PID[ch].err >= 35.0f)
//	{
//		FPID_data[ch].pwm = 999;
//		
//	}

//	else if((PID[ch].err < 35)&&(PID[ch].err >= 18))
//	{
//		FPID_data[ch].pwm = 400;
//	}
//	
//	else if((PID[ch].err >= 4) && (PID[ch].err < 18))
//	{
//		FPID_data[ch].pwm = 150;
//	}
//	
//	else if(PID[ch].err < 0)
//	{		
//		FPID_data[ch].pwm = 0;
//	}
		
	if(pid->iError >= 35.0f)
	{
		pid->pwm = 999;
	}

	else if((pid->iError < 35)&&(pid->iError >= 18))
	{
		pid->pwm = 500;
	}
	
	else if((pid->iError >= 4) && (pid->iError < 18))
	{
		pid->pwm = 200;
	}
	
	else if(pid->iError < -0.1)
	{
//		FAN_RUN(0);
		pid->pwm = 0;
	}
			
	else      
	{		
//		FPID_data[ch].pwm += FPID_data[ch].data_kp*(FPID_data[ch].iError - FPID_data[ch].lasterror) + \
//		FPID_data[ch].data_ki*FPID_data[ch].iError + \
//		FPID_data[ch].data_kd*(FPID_data[ch].iError - 2*FPID_data[ch].lasterror + FPID_data[ch].preerror);
		
//		FPID_data[ch].pwm += PID[ch].Kp*(PID[ch].err - PID[ch].next_err) + PID[ch].Ki*PID[ch].err + PID[ch].Kd*(PID[ch].err - 2*PID[ch].next_err + PID[ch].last_err);
		float _kp = pid->kp + pid->data_kp;
		float _ki = pid->ki + pid->data_ki;
		float _kd = pid->kd + pid->data_kd;
		
		float integral = _ki * (pid->iError + pid->lasterror) / 2; // 梯形积分		
		float dError = pid->iError - pid->lasterror;		
		pid->pwm += _kp*pid->iError + integral + _kd*dError;		
		
		if(ch == 1)
		{
			if(pid->pwm >= 130)    //60
				pid->pwm = 130;
			else if(pid->pwm <= 20)
				pid->pwm = 20;	
		}
		else if(ch == 0)
		{
			if(pid->pwm >= 120) //40
				pid->pwm = 120;
			else if(pid->pwm <= 20)
				pid->pwm = 20;	
		}
		else
		{
			if(pid->pwm >= 120) //80
				pid->pwm = 120;
			else if(pid->pwm <= 30)
				pid->pwm = 30;
		}
	}
		
	if(ch == 0)
	{		
		TIM_SetTIMCompare(TIM8,pid->pwm,2);
	}
	else if(ch == 1)
	{
		TIM_SetTIMCompare(TIM8,pid->pwm,3);
	}
	else if(ch == 2)
	{
		TIM_SetTIMCompare(TIM9,pid->pwm,0);
	}
	else if(ch == 3)
	{
		TIM_SetTIMCompare(TIM8,pid->pwm,0);
	} 		
	else if (ch == 4)
	{
		TIM_SetTIMCompare(TIM9,pid->pwm,1);
	} 
	
//	if(PID[ch].err < -0.5) {
//		FAN_RUN(0);
//	}else{
//		FAN_RUN(1);
//	}
	
	
//	PID[ch].last_err = PID[ch].next_err;

//	PID[ch].next_err = PID[ch].err;
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


