#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	
#include <stdio.h>

#define PRINTF  1    //打印标记



extern USART_HandleTypeDef Uart1_Handle; 
extern DMA_HandleTypeDef DMA_Handle;

#define BUFFER_SIZE_U6              20
//extern uint8_t usart6_rx_buffer[BUFFER_SIZE_U6];

//串口波特率
#define DEBUG_USART_BAUDRATE                     115200
//串口波特率
#define FA_USART_BAUDRATE                        115200

//引脚定义
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK_ENABLE()                __USART1_CLK_ENABLE();

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_10
#define DEBUG_USART_RX_AF                       GPIO_AF7_USART1

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_9
#define DEBUG_USART_TX_AF                       GPIO_AF7_USART1

#define DEBUG_USART_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART_IRQ                 		USART1_IRQn
/************************************************************/
//DMA
//#define SENDBUFF_SIZE                     		10//发送的数据量
#define DEBUG_USART_DMA_CLK_ENABLE()      		__DMA2_CLK_ENABLE()	
#define DEBUG_USART_DMA_CHANNEL           		DMA_CHANNEL_4
#define DEBUG_USART_DMA_STREAM            		DMA2_Stream7


void Debug_USART_Config(void);
void USART_DMA_Config(void);
void USART_Config(void);

////如果想串口中断接收，请不要注释以下宏定义
//void uart_init(u32 bound);


#endif
