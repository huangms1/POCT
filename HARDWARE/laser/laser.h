#ifndef _LASER_H
#define _LASER_H
#include "sys.h"

#define LASER_LED_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()//发射
#define LASER_LED_GPIO_PORT             GPIOF
#define LASER_LED_GPIO_PIN              GPIO_PIN_3

#define LASER_PHOTODIODE_CLK_ENABLE()        __HAL_RCC_GPIOD_CLK_ENABLE()//photodiode接收
#define LASER_PHOTODIODE_PORT                GPIOD
#define LASER_PHOTODIODE_PIN                 GPIO_PIN_1

#define LASER_LED_ON()   HAL_GPIO_WritePin(LASER_LED_GPIO_PORT, LASER_LED_GPIO_PIN, GPIO_PIN_SET)
#define LASER_LED_OFF()  HAL_GPIO_WritePin(LASER_LED_GPIO_PORT, LASER_LED_GPIO_PIN, GPIO_PIN_RESET)

void LASER_Init(void);
u8 LASER_PHOTODIDE_Scan(void);
#endif
