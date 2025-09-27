#ifndef TIME_TEST_H
#define TIME_TEST_H

#include "stm32f4xx.h"


extern TIM_HandleTypeDef TIM5_Handler;	//TIM5句柄 	

void TIM5_NVIC_Configuration(void);
//void TIM5_Configuration(void);
void TIM5_Init(uint16_t arr,uint16_t psc);

#endif	/* TIME_TEST_H */
