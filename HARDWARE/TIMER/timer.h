#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"





extern TIM_HandleTypeDef TIM10_Handler;      //��ʱ����� 

void TIM10_Init(u16 arr,u16 psc);

extern volatile unsigned long long FreeRTOSRunTimeTicks;
void ConfigureTimeForRunTimeStats(void);
#endif

