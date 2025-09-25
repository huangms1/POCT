//#include "main.h"
#include "mytimer.h"

u8 myTimer_flag = 0;

void vTimerCallback(TimerHandle_t xTimer) {
    myTimer_flag = 1;
}
