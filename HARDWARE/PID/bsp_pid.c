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
}

// 定义温度误差阈值宏
#define TEMP_ERROR_HIGH       35.0f
#define TEMP_ERROR_MEDIUM     18.0f
#define TEMP_ERROR_LOW        4.0f
#define TEMP_ERROR_DEADBAND   -1.0f

// PWM输出限制参数数组 [min, max]
static const int16_t pwm_limits[5][4] = {
    {20, 150, 600, 300},  // 通道0
    {35, 155, 500, 200},   // 通道1
    {20, 80, 300, 200},   // 通道2
    {20, 70, 300, 200},   // 通道3
    {35, 60, 300, 200}    // 通道4
};

void PID_Calculate(float setTemp, uint8_t ch)
{
    // 检查通道有效性
    if (ch >= 5) return;  // 假设最大通道数为4
    
    float currentTemp = (float)GetTemp(ch) / 100.0f;
    FUZZYPID *pid = &FPID_data[ch];  // 缓存结构体指针
    
    // 调用模糊PID计算
    COOL_Fuzzytrans(pid, setTemp, currentTemp);
    
    // 根据温度误差设置PWM输出
    if (pid->iError >= TEMP_ERROR_HIGH)
    {
        pid->pwm = 999;
    }
    else if (pid->iError >= TEMP_ERROR_MEDIUM)
    {
        pid->pwm = pwm_limits[ch][2];
    }
    else if (pid->iError >= TEMP_ERROR_LOW)
    {
        pid->pwm = pwm_limits[ch][3];
    }
    else if (pid->iError < -0.1)
    {
        pid->pwm = 0;
    }
    else
    {	
				float _kp = pid->kp + pid->data_kp;
				float _ki = pid->ki + pid->data_ki;
				float _kd = pid->kd + pid->data_kd;

				float integral = _ki * (pid->iError + pid->lasterror) / 2; // 梯形积分		
				float dError = pid->iError - pid->lasterror;		
				pid->pwm += _kp*pid->iError + integral + _kd*dError;
			
				 if (pid->pwm > pwm_limits[ch][1])
					pid->pwm = pwm_limits[ch][1];
				else if (pid->pwm < pwm_limits[ch][0])
					pid->pwm = pwm_limits[ch][0];  
    }
    
    // 设置定时器比较值
    switch (ch)
    {
        case 0: TIM_SetTIMCompare(TIM8, pid->pwm, 2); break;
        case 1: TIM_SetTIMCompare(TIM8, pid->pwm, 3); break;
        case 2: TIM_SetTIMCompare(TIM9, pid->pwm, 0); break;
        case 3: TIM_SetTIMCompare(TIM8, pid->pwm, 0); break;
        case 4: TIM_SetTIMCompare(TIM9, pid->pwm, 1); break;
        default: break;
    }
    
    // 统一风扇控制逻辑
	if(ch == 2){
		PCR_FAN_RUN((pid->iError  <= TEMP_ERROR_DEADBAND) ? 0 : 1);
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


