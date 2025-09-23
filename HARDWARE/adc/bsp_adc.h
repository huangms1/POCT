#ifndef __BSP_ADC_H
#define	__BSP_ADC_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"

//#define PT100_NOFCHANEL      5

///*=====================通道1 IO======================*/
//#define PT100_ADC_GPIO_PORT1             GPIOA
//#define PT100_ADC_GPIO_PIN1              GPIO_PIN_0
//#define PT100_ADC_GPIO_CLK1_ENABLE()     __GPIOA_CLK_ENABLE()
//#define PT100_ADC_CHANNEL1               ADC_CHANNEL_0
///*=====================通道2 IO ======================*/
//#define PT100_ADC_GPIO_PORT2             GPIOA
//#define PT100_ADC_GPIO_PIN2              GPIO_PIN_1
//#define PT100_ADC_GPIO_CLK2_ENABLE()     __GPIOA_CLK_ENABLE()
//#define PT100_ADC_CHANNEL2               ADC_CHANNEL_1
///*=====================通道3 IO ======================*/
//#define PT100_ADC_GPIO_PORT3             GPIOA
//#define PT100_ADC_GPIO_PIN3              GPIO_PIN_2
//#define PT100_ADC_GPIO_CLK3_ENABLE()     __GPIOA_CLK_ENABLE()
//#define PT100_ADC_CHANNEL3               ADC_CHANNEL_2

///*=====================通道4 IO ======================*/
//#define PT100_ADC_GPIO_PORT4             GPIOB
//#define PT100_ADC_GPIO_PIN4              GPIO_PIN_1
//#define PT100_ADC_GPIO_CLK4_ENABLE()     __GPIOB_CLK_ENABLE()
//#define PT100_ADC_CHANNEL4               ADC_CHANNEL_9


///*=====================通道5 IO ======================*/
//#define PT100_ADC_GPIO_PORT5             GPIOB
//#define PT100_ADC_GPIO_PIN5              GPIO_PIN_0
//#define PT100_ADC_GPIO_CLK5_ENABLE()     __GPIOB_CLK_ENABLE()
//#define PT100_ADC_CHANNEL5               ADC_CHANNEL_8

//   
//// ADC 序号宏定义
//#define PT100_ADC                        ADC1
//#define PT100_ADC_CLK_ENABLE()           __ADC1_CLK_ENABLE()

//// ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
//#define PT100_ADC_DR_ADDR                ((uint32_t)ADC1+0x4c)

//// ADC DMA 通道宏定义，这里我们使用DMA传输
//#define PT100_ADC_DMA_CLK_ENABLE()       __DMA2_CLK_ENABLE()
//#define PT100_ADC_DMA_CHANNEL            DMA_CHANNEL_0
//#define PT100_ADC_DMA_STREAM             DMA2_Stream0


//void PT100_Init(void);

uint16_t GetMedianNum(uint8_t ch,uint8_t len);
int GetTemp(uint8_t ch);

#endif /* __BSP_ADC_H */



