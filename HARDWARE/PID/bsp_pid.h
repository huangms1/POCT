#ifndef __BSP_PID_H
#define	__BSP_PID_H

#include "stm32F4xx.h"

#include "bsp_adc.h"

#include "bsp_pwm_conf.h"


//PID�ṹ��
typedef struct PID_Pre{
	
	int target_temp;  //Ŀ��ֵ
	
	int current_temp; //��ǰֵ
	
	float err;        //e(k)  ����ƫ��
	
	float next_err;   // e(k-1) �´�ƫ��
	
	float last_err;   // e(k-2) �ϴ�ƫ��
	
	float Kp, Ki, Kd;
	
	int pwm;          //���������
	
}PIDTypedef;

extern PIDTypedef PID[5];

void PID_Init(void);

void PID_Calculate(float setTemp,uint8_t ch);

HAL_StatusTypeDef PID_Stop(uint8_t ch);

#endif


