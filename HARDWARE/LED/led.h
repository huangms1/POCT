#ifndef _LED_H
#define _LED_H
#include "sys.h"
#include "FreeRTOS.h"
#include "event_groups.h" 

extern EventGroupHandle_t ledEventGroupHandle;

#define POWER_LED_FLAG           (1 << 0)
#define EXTRACTION_LED_FLAG      (1 << 1)
#define PCR_LED_FLAG             (1 << 2)
#define ERROR_LED_FLAG           (1 << 3)
#define END_LED_FLAG             (1 << 4)

/****************************LED仪器状态控制***********************************/
#define POWER_LED_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE();
#define POWER_LED_PORT            GPIOF
#define POWER_LED_PIN             GPIO_PIN_12

#define EXTRACTION_LED_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE();
#define EXTRACTION_LED_PORT              GPIOF
#define EXTRACTION_LED_PIN               GPIO_PIN_13

#define PCR_LED_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE();
#define PCR_LED_PORT              GPIOF
#define PCR_LED_PIN               GPIO_PIN_14

#define ERROR_LED_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE();
#define ERROR_LED_PORT              GPIOF
#define ERROR_LED_PIN               GPIO_PIN_15

#define END_LED_CLK_ENABLE()      __HAL_RCC_GPIOE_CLK_ENABLE();
#define END_LED_PORT              GPIOE
#define END_LED_PIN               GPIO_PIN_9

#define POWER_LED                 PFout(12)
#define EXTRACTION_LED            PFout(13)
#define PCR_LED                   PFout(14)
#define ERROR_LED                 PFout(15)
#define END_LED                   PEout(9)

/****************************激光头***********************************/
#define JG_LED_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE();
#define JG_LED_PORT            GPIOA
#define JG_LED_PIN             GPIO_PIN_8

#define JG_LED             PAout(8)


/***************  LED灯亮度控制********************************
CLK	PB13	
SDI	PC3	
CS	PC2	
***************************************************************/

#define LED_SPI_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE(); 
#define LED_SPI_CLK_PORT        GPIOB
#define LED_SPI_CLK_PIN         GPIO_PIN_13

#define LED_SPI_SDI_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE(); 
#define LED_SPI_SDI_PORT        GPIOC
#define LED_SPI_SDI_PIN         GPIO_PIN_3

#define LED_SPI_CS_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE(); 
#define LED_SPI_CS_PORT        GPIOC
#define LED_SPI_CS_PIN         GPIO_PIN_2

#define LED1_SPI_CS_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE(); 
#define LED1_SPI_CS_PORT          GPIOA
#define LED1_SPI_CS_PIN           GPIO_PIN_1

#define SYS_LED_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE(); 
#define SYS_LED_PORT           GPIOA
#define SYS_LED_PIN            GPIO_PIN_3

#define SYS_LED                PAout(3)   //LED0

#define LED_SPI_CLK            PBout(13)
#define LED_SPI_SDI            PCout(3)
#define LED_SPI_CS             PCout(2)
#define LED1_SPI_CS            PAout(1)

void JG_init(void);
void LED_Init(void);
void ALL_LED_OFF(void);
void LED_SPI_SendData(uint8_t num ,uint8_t data);
void LED_StateControl(void);
void LED_EventGroupSetbit(uint16_t led_bit);
#endif
