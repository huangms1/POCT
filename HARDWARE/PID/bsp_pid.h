#ifndef __BSP_PID_H
#define	__BSP_PID_H

#include "stm32F4xx.h"

#include "bsp_adc.h"

#include "bsp_pwm_conf.h"


//PID结构体
typedef struct PID_Pre{
	
	float target_temp;  //目标值
	
	float current_temp; //当前值
	
	float err;        //e(k)  本次偏差
	
	float next_err;   // e(k-1) 下次偏差
	
	float last_err;   // e(k-2) 上次偏差
	
	float Kp, Ki, Kd;
	
	uint16_t pwm;          //计算结果输出
	
}PIDTypedef;

extern PIDTypedef PID[3];

void PID_Init(void);

void PID_Calculate(float setTemp,uint8_t ch);

HAL_StatusTypeDef PID_Stop(uint8_t ch);

#endif


