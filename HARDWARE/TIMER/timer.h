#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"





extern TIM_HandleTypeDef TIM10_Handler;      //¶¨Ê±Æ÷¾ä±ú 

void TIM10_Init(u16 arr,u16 psc);

extern volatile unsigned long long FreeRTOSRunTimeTicks;
void ConfigureTimeForRunTimeStats(void);
#endif

