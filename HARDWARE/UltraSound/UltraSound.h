#ifndef _ULTRA_SOUND_H
#define _ULTRA_SOUND_H
#include "sys.h"

#define ULTRA_SOUND_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()
#define ULTRA_SOUND_PORT            GPIOD
#define ULTRA_SOUND_PIN             GPIO_PIN_0

#define ULTRA_SOUND_ENABLE          PDout(0)

#define ULTRA_SOUND_ON()   HAL_GPIO_WritePin(ULTRA_SOUND_PORT, ULTRA_SOUND_PIN, GPIO_PIN_RESET)
#define ULTRA_SOUND_OFF()  HAL_GPIO_WritePin(ULTRA_SOUND_PORT, ULTRA_SOUND_PIN, GPIO_PIN_SET)

void UltraSound_Init(void);
void UltraSound_PeriodicOutput(uint32_t duration_ms, uint16_t pulse_width);
#endif
