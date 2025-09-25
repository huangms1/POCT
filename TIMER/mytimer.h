#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"

extern TimerHandle_t TimerHandle;

extern u8 myTimer_flag;

void vTimerCallback(TimerHandle_t xTimer);

#endif
