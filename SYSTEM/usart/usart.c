#include "usart.h"
#include "delay.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32F4xx.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //os 使用	  
#endif

DMA_HandleTypeDef  DMA_Handle;      //DMA句柄
USART_HandleTypeDef Uart1_Handle;      //UART句柄

USART_HandleTypeDef FA_USART_Handle;          //UART句柄

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	HAL_USART_Transmit(&FA_USART_Handle, (uint8_t *)&ch, 1, 1000);	
	return ch;
}
#endif 

 /**
  * @brief  USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
      
  DEBUG_USART_RX_GPIO_CLK_ENABLE();
  DEBUG_USART_TX_GPIO_CLK_ENABLE();
  /* 使能 UART 时钟 */
  DEBUG_USART_CLK_ENABLE();
  
   /* 配置Tx引脚为复用功能  */
  GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
  HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* 配置Rx引脚为复用功能 */
  GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
  GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;
  HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);
    
  Uart1_Handle.Instance          = DEBUG_USART;
  Uart1_Handle.Init.BaudRate     = DEBUG_USART_BAUDRATE;
  Uart1_Handle.Init.WordLength   = UART_WORDLENGTH_8B;
  Uart1_Handle.Init.StopBits     = UART_STOPBITS_1;
  Uart1_Handle.Init.Parity       = UART_PARITY_NONE;
//  Uart1_Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  Uart1_Handle.Init.CLKPolarity = USART_POLARITY_LOW;
  Uart1_Handle.Init.CLKPhase = USART_PHASE_1EDGE;
  Uart1_Handle.Init.CLKLastBit = USART_LASTBIT_DISABLE;
  Uart1_Handle.Init.Mode         = USART_MODE_TX_RX;
  
  HAL_USART_Init(&Uart1_Handle); 
}


/**
  * @brief  USART1 TX DMA 配置，内存到外设(USART1->DR)
  * @param  无
  * @retval 无
  */
void USART_DMA_Config(void)
{
  DEBUG_USART_DMA_CLK_ENABLE();  
   
  //Tx DMA配置
  DMA_Handle.Instance=DEBUG_USART_DMA_STREAM;                  //数据流选择
  DMA_Handle.Init.Channel=DEBUG_USART_DMA_CHANNEL;            //通道选择
  DMA_Handle.Init.Direction=DMA_MEMORY_TO_PERIPH;             //存储器到外设
  DMA_Handle.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
  DMA_Handle.Init.MemInc=DMA_MINC_ENABLE;//DMA_MINC_ENABLE;                     //存储器增量模式
  DMA_Handle.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //外设数据长度:8位
  DMA_Handle.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //存储器数据长度:8位
  DMA_Handle.Init.Mode=DMA_NORMAL;                            //外设普通模式
  DMA_Handle.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级
  DMA_Handle.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              //禁用FIFO
  DMA_Handle.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
  DMA_Handle.Init.MemBurst=DMA_MBURST_SINGLE;                 //存储器突发单次传输
  DMA_Handle.Init.PeriphBurst=DMA_PBURST_SINGLE;              //外设突发单次传输
  
  HAL_DMA_Init(&DMA_Handle);
  /* Associate the DMA handle */
  __HAL_LINKDMA(&Uart1_Handle, hdmatx, DMA_Handle); 
  
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn,7,1);    //设置中断优先级，抢占优先级1，子优先级0
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);          //开启ITM3中断 
		
}


 
void DMA2_Stream7_IRQHandler()
{	
	Uart1_Handle.State = HAL_USART_STATE_READY;
	DMA_Handle.State = HAL_DMA_STATE_READY;
	__HAL_DMA_CLEAR_FLAG(&DMA_Handle, DMA_FLAG_TCIF3_7);
	__HAL_DMA_CLEAR_FLAG(&DMA_Handle, DMA_FLAG_HTIF3_7);
	__HAL_DMA_CLEAR_FLAG(&DMA_Handle, DMA_FLAG_FEIF3_7 );
	__HAL_UNLOCK(&DMA_Handle);
//	HAL_DMA_IRQHandler(&DMA_Handle);
}

/**
* @阀串口初始化.
  */
void USART_Config(void)
{
	Debug_USART_Config(); 
	/* 配置使用DMA模式 */
	USART_DMA_Config();
}


