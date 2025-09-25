#ifndef __BSP_PWM_CONF_H
#define	__BSP_PWM_CONF_H

#include "STM32F4xx.h"
#include "InstrumentInclude.h"


extern TIM_HandleTypeDef TIM8_Handler; //定时器8 PWM句柄
extern TIM_HandleTypeDef TIM9_Handler; //定时器9 PWM句柄


/*=====================PWM IO======================*/
// PC8  TIM8_CH3
#define TEMP_PWM_GPIO_PORT1             GPIOC
#define TEMP_PWM_GPIO_PIN1              GPIO_PIN_8
#define TEMP_PWM_GPIO_CLK1_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define TEMP_PWM_TIM_CLK1_ENABLE        __HAL_RCC_TIM8_CLK_ENABLE()

// PC9  TIM8_CH4
#define TEMP_PWM_GPIO_PORT2             GPIOC
#define TEMP_PWM_GPIO_PIN2              GPIO_PIN_9
#define TEMP_PWM_GPIO_CLK2_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define TEMP_PWM_TIM_CLK2_ENABLE        __HAL_RCC_TIM8_CLK_ENABLE()

// PE5  TIM9_CH1
#define TEMP_PWM_GPIO_PORT3             GPIOE
#define TEMP_PWM_GPIO_PIN3              GPIO_PIN_5
#define TEMP_PWM_GPIO_CLK3_ENABLE()     __HAL_RCC_GPIOE_CLK_ENABLE()
#define TEMP_PWM_TIM_CLK3_ENABLE        __HAL_RCC_TIM9_CLK_ENABLE()

#ifdef POCT_ANM_H1L4_A

// PE5  TIM9_CH1
#define TEMP_PWM_GPIO_PORT4             GPIOC
#define TEMP_PWM_GPIO_PIN4              GPIO_PIN_6
#define TEMP_PWM_GPIO_CLK4_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define TEMP_PWM_TIM_CLK4_ENABLE        __HAL_RCC_TIM8_CLK_ENABLE()

#endif 

#ifdef POCT_ANM_H3L5_A

// PE5  TIM9_CH1
#define TEMP_PWM_GPIO_PORT4             GPIOC
#define TEMP_PWM_GPIO_PIN4              GPIO_PIN_6
#define TEMP_PWM_GPIO_CLK4_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define TEMP_PWM_TIM_CLK4_ENABLE        __HAL_RCC_TIM8_CLK_ENABLE()

#endif 

#define HOT5_GPIO_PORT             GPIOE
#define HOT5_GPIO_PIN              GPIO_PIN_6
#define HOT5_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOE_CLK_ENABLE()
#define HOT5_TIM_CLK_ENABLE        __HAL_RCC_TIM9_CLK_ENABLE()

void TIM_SetTIMCompare(TIM_TypeDef *TIM,uint32_t compare,uint8_t ch);
void TEMP_TIM_PWM_Init(void);


#endif

