#ifndef __BSP_MOTOR_CONF_H
#define	__BSP_MOTOR_CONF_H

/**
  ******************************************************************************
  * @file    bsp_motor_conf.h
  * @author  hms
  * @version V1.0
  * @date    2021.11.11
  * @brief   步进点击端口配置
  ******************************************************************************
  * @attention
  * 
  *
  ******************************************************************************
  */
  
#include "stm32F4xx.h"
#include "includes.h"
#include "InstrumentInclude.h"
#include "main.h"

extern TIM_HandleTypeDef TIM4_Handler; 
extern TIM_HandleTypeDef TIM1_Handler; 
extern TIM_HandleTypeDef TIM2_Handler; 
extern TIM_HandleTypeDef TIM3_Handler; 

//定时器实际时钟频率为：168MHz/(TIM_PRESCALER+1)
//其中 高级定时器的 频率为168MHz,其他定时器为84MHz
//168/(12)=2Mhz
//具体需要的频率可以自己计算
#define TIM_PRESCALER               (160 - 1)      
//定义定时器周期，输出比较模式周期设置为0xFFFF
#define TIM_PERIOD                   800           

/************************************************************/
#define HIGH1 GPIO_PIN_SET                         //高电平
#define LOW1  GPIO_PIN_RESET                       //低电平

//#define ON  LOW                                  //开
//#define OFF HIGH                                 //关

#define CW  HIGH1                                  //顺时针
#define CCW LOW1                                   //逆时针

#if IS_BOOTLOADER_PROGRAM
typedef struct{
	 uint16_t en_pin;
	 GPIO_TypeDef *en_port;
}Motor_en_conf_Typedef;

#else

#endif

/*****************************风扇*******************************************
*风扇：三孔：PG13  单孔：PA11
*****************************************************************************/

#if defined POCT_ANM_H1L4_A
	#define  FAN_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE();
	#define  FAN_GPIO_PORT           GPIOA
	#define  FAN_GPIO_PIN            GPIO_PIN_11
	
	#define  PCR_FAN_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE();
	#define  PCR_FAN_GPIO_PORT       GPIOB
	#define  PCR_FAN_GPIO_PIN         GPIO_PIN_0
	
	

#elif defined POCT_HMN_H2L8_A
	#define  FAN_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE();
	#define  FAN_GPIO_PORT           GPIOA
	#define  FAN_GPIO_PIN            GPIO_PIN_11

#else
	#define  FAN_CLK_ENABLE()        __HAL_RCC_GPIOG_CLK_ENABLE();
	#define  FAN_GPIO_PORT           GPIOG
	#define  FAN_GPIO_PIN            GPIO_PIN_13
	
#endif

#define  FAN_RUN(a)              if (a)	\
								 HAL_GPIO_WritePin(FAN_GPIO_PORT,FAN_GPIO_PIN,HIGH1);\
								 else		\
								 HAL_GPIO_WritePin(FAN_GPIO_PORT,FAN_GPIO_PIN,LOW1)    // HAL_GPIO_WritePin(MIN_MOTOR_GPIO_PORT,MIN_MOTOR_GPIO_PIN,x);
								 
#define  PCR_FAN_RUN(a)          if (a)	\
								 HAL_GPIO_WritePin(PCR_FAN_GPIO_PORT,PCR_FAN_GPIO_PIN,HIGH1);\
								 else		\
								 HAL_GPIO_WritePin(PCR_FAN_GPIO_PORT,PCR_FAN_GPIO_PIN,LOW1)    // HAL_GPIO_WritePin(MIN_MOTOR_GPIO_PORT,MIN_MOTOR_GPIO_PIN,x);

/*************************M01 电机*******************************************
*M01电机：
*M01_STEP	PD13	TIM4_CH2
*M01_EN	    PD10
*M01_DIR	PD11
*M01_GO	    PD9
*****************************************************************************/
// M01 Motor 脉冲信号配置（TIM4_CH2）
#define M01_PUL_PORT                        GPIOD       // 脉冲输出端口
#define M01_PUL_PIN                         GPIO_PIN_13 // 脉冲输出引脚
#define M01_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE() // 使能GPIOD时钟

// 使能信号配置
#define M01_EN_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOD_CLK_ENABLE() // 使能GPIOD时钟
#define M01_EN_GPIO_PORT       				GPIOD        // 使能信号端口
#define M01_EN_GPIO_PIN		    			GPIO_PIN_10  // 使能信号引脚

// 方向控制配置
#define M01_DIR_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE() // 使能GPIOD时钟
#define M01_DIR_GPIO_PORT       			GPIOD         // 方向控制端口
#define M01_DIR_GPIO_PIN		    	GPIO_PIN_11   // 方向控制引脚

// 光耦传感器配置（位置检测）
#define M01_GO_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOD_CLK_ENABLE() // 使能GPIOD时钟
#define M01_GO_GPIO_PORT       				GPIOD         // 光耦输入端口
#define M01_GO_GPIO_PIN		    			GPIO_PIN_9    // 光耦输入引脚

// 脉冲电平控制宏（直接端口操作）
#define M01_PUL_LEVE  PDout(13)  // 使用位操作控制PD13引脚电平

// #define M01_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M01_EN_GPIO_PORT,M01_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M01_EN_GPIO_PORT,M01_EN_GPIO_PIN,LOW1)
										
// #define M01_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M01_DIR_GPIO_PORT,M01_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M01_DIR_GPIO_PORT,M01_DIR_GPIO_PIN,LOW1)
					
// #define M01_GO_STA  HAL_GPIO_ReadPin(M01_GO_GPIO_PORT,M01_GO_GPIO_PIN)			   
			   
			   
/****************************************************************************
*M02电机配置引脚
*M02_STEP	PD12	TIM4_CH1
*M02_EN	    PD8
*M02_DIR	PD15
*M02_GO	    PD14
*****************************************************************************/
// M02 Motor 脉冲信号配置（TIM4_CH1）
#define M02_PUL_PORT                        GPIOD        // 脉冲输出端口
#define M02_PUL_PIN                         GPIO_PIN_12  // 脉冲输出引脚
#define M02_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE() // 使能GPIOD时钟

// 使能信号配置
#define M02_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOD_CLK_ENABLE() // 使能GPIOD时钟	
#define M02_EN_GPIO_PORT       				GPIOD         // 使能信号端口
#define M02_EN_GPIO_PIN		    			GPIO_PIN_8    // 使能信号引脚

// 方向控制配置
#define M02_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOB_CLK_ENABLE() // 使能GPIOB时钟	
#define M02_DIR_GPIO_PORT       			GPIOB          // 方向控制端口
#define M02_DIR_GPIO_PIN		    		GPIO_PIN_15    // 方向控制引脚

// 光耦传感器配置（位置检测）
#define M02_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOB_CLK_ENABLE() // 使能GPIOB时钟
#define M02_GO_GPIO_PORT       				GPIOB          // 光耦输入端口
#define M02_GO_GPIO_PIN		    			GPIO_PIN_14    // 光耦输入引脚

// 脉冲电平控制宏（直接端口操作）
#define M02_PUL_LEVE  PDout(12)  // 使用位操作控制PD12引脚电平

// #define M02_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M02_EN_GPIO_PORT,M02_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M02_EN_GPIO_PORT,M02_EN_GPIO_PIN,LOW1)
					
// #define M02_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M02_DIR_GPIO_PORT,M02_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M02_DIR_GPIO_PORT,M02_DIR_GPIO_PIN,LOW1)
					
// #define M02_GO_STA  HAL_GPIO_ReadPin(M02_GO_GPIO_PORT,M02_GO_GPIO_PIN)	
					
					
/****************************************************************************
*M03电机配置引脚
*M03_STEP	PD14	TIM4_CH3
*M03_EN	    PG3
*M03_DIR	PG4
*M03_GO	    PG2
*****************************************************************************/
// M03 Motor 脉冲信号配置（TIM4_CH3）
#define M03_PUL_PORT                  GPIOD       // 脉冲输出端口
#define M03_PUL_PIN                   GPIO_PIN_14  // 脉冲输出引脚
#define M03_PUL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE() // 使能GPIOD时钟

// 使能信号配置
#define M03_EN_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟
#define M03_EN_GPIO_PORT       				GPIOG        // 使能信号端口
#define M03_EN_GPIO_PIN		    			GPIO_PIN_3    // 使能信号引脚

// 方向控制配置
#define M03_DIR_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟
#define M03_DIR_GPIO_PORT       			GPIOG        // 方向控制端口
#define M03_DIR_GPIO_PIN		    		GPIO_PIN_4    // 方向控制引脚

// 光耦传感器配置（位置检测）
#define M03_GO_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOB_CLK_ENABLE() // 使能GPIOB时钟
#define M03_GO_GPIO_PORT       				GPIOG        // 光耦输入端口
#define M03_GO_GPIO_PIN		    			GPIO_PIN_2    // 光耦输入引脚

// 脉冲电平控制宏（直接端口操作）
#define M03_PUL_LEVE  PDout(14)  // 使用位操作控制PD14引脚电平

// #define M03_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M03_EN_GPIO_PORT,M03_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M03_EN_GPIO_PORT,M03_EN_GPIO_PIN,LOW1)
					

// #define M03_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M03_DIR_GPIO_PORT,M03_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M03_DIR_GPIO_PORT,M03_DIR_GPIO_PIN,LOW1)

// #define M03_GO_STA  HAL_GPIO_ReadPin(M03_GO_GPIO_PORT,M03_GO_GPIO_PIN)
					
/****************************************************************************
*M08电机配置引脚
*M08_STEP	PD15	TIM4_CH4
*M08_EN	    PG6	
*M08_DIR	PG7	
*M08_GO	    PG5		
*****************************************************************************/	
// M08 Motor 脉冲信号配置（TIM4_CH4）
#define M08_PUL_PORT                  GPIOD       // 脉冲输出端口
#define M08_PUL_PIN                   GPIO_PIN_15  // 脉冲输出引脚
#define M08_PUL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE() // 使能GPIOD时钟

// 使能信号配置
#define M08_EN_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟
#define M08_EN_GPIO_PORT       		   GPIOG        // 使能信号端口
#define M08_EN_GPIO_PIN		    	   GPIO_PIN_6   // 使能信号引脚

// 方向控制配置
#define M08_DIR_GPIO_CLK_ENABLE() 	   __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟
#define M08_DIR_GPIO_PORT       	   GPIOG        // 方向控制端口
#define M08_DIR_GPIO_PIN		       GPIO_PIN_7    // 方向控制引脚

// 光耦传感器配置（位置检测）
#define M08_GO_GPIO_CLK_ENABLE() 	   __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟
#define M08_GO_GPIO_PORT       		   GPIOG         // 光耦输入端口
#define M08_GO_GPIO_PIN		           GPIO_PIN_5     // 光耦输入引脚

// 脉冲电平控制宏（直接端口操作）
#define M08_PUL_LEVE  PDout(15)  // 使用位操作控制PD15引脚电平

// #define M08_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M08_EN_GPIO_PORT,M08_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M08_EN_GPIO_PORT,M08_EN_GPIO_PIN,LOW1)
					
					
// #define M08_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M08_DIR_GPIO_PORT,M08_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M08_DIR_GPIO_PORT,M08_DIR_GPIO_PIN,LOW1)
					
// #define M08_GO_STA  HAL_GPIO_ReadPin(M08_GO_GPIO_PORT,M08_GO_GPIO_PIN)
					
/****************************************************************************
*M04电机配置引脚
*M04_STEP	PE8	TIM1_CH1
*M04_EN	    PF2
*M04_DIR	PF1
*M04_GO	    PF0
*****************************************************************************/
// M04 Motor 脉冲信号配置（TIM1_CH1）
#define M04_PUL_PORT                        GPIOE        // 脉冲输出端口
#define M04_PUL_PIN                         GPIO_PIN_8   // 脉冲输出引脚
#define M04_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOE_CLK_ENABLE() // 使能GPIOE时钟

// 使能信号配置
#define M04_EN_GPIO_PORT                    GPIOF        // 使能信号端口
#define M04_EN_GPIO_PIN                     GPIO_PIN_2    // 使能信号引脚
#define M04_EN_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOF_CLK_ENABLE() // 使能GPIOF时钟

// 方向控制配置  
#define M04_DIR_GPIO_PORT                   GPIOF        // 方向控制端口
#define M04_DIR_GPIO_PIN                    GPIO_PIN_1    // 方向控制引脚
#define M04_DIR_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOF_CLK_ENABLE() // 使能GPIOF时钟

// 光耦传感器配置（位置检测）
#define M04_GO_GPIO_PORT                    GPIOF         // 光耦输入端口
#define M04_GO_GPIO_PIN                     GPIO_PIN_0    // 光耦输入引脚
#define M04_GO_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOF_CLK_ENABLE() // 使能GPIOF时钟

// 脉冲电平控制宏（直接端口操作）
#define M04_PUL_LEVE                        PEout(8)      // 使用位操作控制

// #define M04_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M04_EN_GPIO_PORT,M04_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M04_EN_GPIO_PORT,M04_EN_GPIO_PIN,LOW1)

// #define M04_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M04_DIR_GPIO_PORT,M04_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M04_DIR_GPIO_PORT,M04_DIR_GPIO_PIN,LOW1)	
					
					
/****************************************************************************
*M05电机配置引脚
*M05_STEP	PE11	TIM1_CH2
*M05_EN	    PF5
*M05_DIR	PF6
*M05_GO	    PF4	
*****************************************************************************/
// M05 Motor 脉冲信号配置（TIM1_CH2）
#define M05_PUL_PORT                        GPIOE       // 脉冲输出端口
#define M05_PUL_PIN                         GPIO_PIN_11 // 脉冲输出引脚
#define M05_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOE_CLK_ENABLE() // 使能GPIOE时钟

// 使能信号配置
#define M05_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();	
#define M05_EN_GPIO_PORT       				GPIOF        // 使能信号端口
#define M05_EN_GPIO_PIN		    			GPIO_PIN_5    // 使能信号引脚

// 方向控制配置
#define M05_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();	
#define M05_DIR_GPIO_PORT       			GPIOF        // 方向控制端口
#define M05_DIR_GPIO_PIN		    		GPIO_PIN_6    // 方向控制引脚

// 光耦传感器配置（位置检测）
#define M05_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();		
#define M05_GO_GPIO_PORT       				GPIOF        // 光耦输入端口
#define M05_GO_GPIO_PIN		    			GPIO_PIN_4    // 光耦输入引脚

// 脉冲电平控制宏（直接端口操作）
#define M05_PUL_LEVE  PEout(11)  // 使用位操作控制PE11引脚电平

// #define M05_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M05_EN_GPIO_PORT,M05_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M05_EN_GPIO_PORT,M05_EN_GPIO_PIN,LOW1)
					
					
// #define M05_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M05_DIR_GPIO_PORT,M05_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M05_DIR_GPIO_PORT,M05_DIR_GPIO_PIN,LOW1)					

// #define M05_GO_STA  HAL_GPIO_ReadPin(M05_GO_GPIO_PORT,M05_GO_GPIO_PIN)
					
					
/****************************************************************************
*M07电机配置引脚
*M07_STEP	PE10	TIM1_CH3
*M07_EN	    PC0
*M07_DIR	PC1
*M07_GO	    PF10
*****************************************************************************/			
// M07 Motor 脉冲信号配置（TIM1_CH3）
#define M07_PUL_PORT                        GPIOE       // 脉冲输出端口
#define M07_PUL_PIN                         GPIO_PIN_10 // 脉冲输出引脚
#define M07_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOE_CLK_ENABLE() // 使能GPIOE时钟

// 使能信号配置
#define M07_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOC_CLK_ENABLE();	
#define M07_EN_GPIO_PORT       				GPIOC        // 使能信号端口
#define M07_EN_GPIO_PIN		    			GPIO_PIN_0    // 使能信号引脚

// 方向控制配置
#define M07_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOC_CLK_ENABLE();	
#define M07_DIR_GPIO_PORT       			GPIOC        // 方向控制端口
#define M07_DIR_GPIO_PIN		    		GPIO_PIN_1    // 方向控制引脚

// 光耦传感器配置（位置检测）
#define M07_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();	
#define M07_GO_GPIO_PORT       				GPIOF        // 光耦输入端口
#define M07_GO_GPIO_PIN		    			GPIO_PIN_10   // 光耦输入引脚

// 脉冲电平控制宏（直接端口操作）
#define M07_PUL_LEVE  PEout(10)  // 使用位操作控制PE10引脚电平

// #define M07_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M07_EN_GPIO_PORT,M07_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M07_EN_GPIO_PORT,M07_EN_GPIO_PIN,LOW1)
					
// #define M07_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M07_DIR_GPIO_PORT,M07_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M07_DIR_GPIO_PORT,M07_DIR_GPIO_PIN,LOW1)						
	
// #define M07_GO_STA  HAL_GPIO_ReadPin(M07_GO_GPIO_PORT,M07_GO_GPIO_PIN)
					
/****************************************************************************
*M09电机配置引脚
*M09_STEP	PG10
*M09_EN	    PG11
*M09_DIR	PG12
*M09_GO	    PG13		
*****************************************************************************/
#if defined POCT_ANM_H1L4_A //|| POCT_ANM_H3L5_A
// M09 Motor 脉冲信号配置
#define M09_PUL_PORT                        GPIOG       // 脉冲输出端口
#define M09_PUL_PIN                         GPIO_PIN_10  // 脉冲输出引脚
#define M09_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE() // 使能GPIOB时钟

// 使能信号配置
#define M09_EN_GPIO_PORT       				GPIOG        // 使能信号端口
#define M09_EN_GPIO_PIN		    			GPIO_PIN_11   // 使能信号引脚
#define M09_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟

// 方向控制配置
#define M09_DIR_GPIO_PORT       			GPIOG        // 方向控制端口
#define M09_DIR_GPIO_PIN		    		GPIO_PIN_12   // 方向控制引脚
#define M09_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟

// 光耦传感器配置（位置检测）
#define M09_GO_GPIO_PORT       				GPIOG        // 光耦输入端口
#define M09_GO_GPIO_PIN		    			GPIO_PIN_13   // 光耦输入引脚
#define M09_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟

// 脉冲电平控制宏（直接端口操作）
#define M09_PUL_LEVE  PGout(10)  // 使用位操作控制PG10引脚电平
	
#endif					

/* 使能IO*/
// #define M09_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M09_EN_GPIO_PORT,M09_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M09_EN_GPIO_PORT,M09_EN_GPIO_PIN,LOW1)
					
					
// /* 方向IO*/
// #define M09_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M09_DIR_GPIO_PORT,M09_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M09_DIR_GPIO_PORT,M09_DIR_GPIO_PIN,LOW1)						
	
// #define M09_GO_STA  HAL_GPIO_ReadPin(M09_GO_GPIO_PORT,M09_GO_GPIO_PIN)	

/****************************************************************************
*M10电机配置引脚
*M10_STEP	PE0
*M10_EN	    PB9
*M10_DIR	PE1
*M10_GO	    PB8
*****************************************************************************/
#if defined POCT_ANM_H1L4_A// || POCT_ANM_H3L5_A
// M10 Motor 脉冲信号配置
#define M10_PUL_PORT                         GPIOE       // 脉冲输出端口
#define M10_PUL_PIN                          GPIO_PIN_0  // 脉冲输出引脚
#define M10_PUL_GPIO_CLK_ENABLE()          	 __HAL_RCC_GPIOE_CLK_ENABLE() // 使能GPIOE时钟

// 使能信号配置
#define M10_EN_GPIO_PORT       				 GPIOB        // 使能信号端口
#define M10_EN_GPIO_PIN		    			 GPIO_PIN_9    // 使能信号引脚
#define M10_EN_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE() // 使能GPIOB时钟

// 方向控制配置
#define M10_DIR_GPIO_PORT       			 GPIOE         // 方向控制端口
#define M10_DIR_GPIO_PIN		    		 GPIO_PIN_1    // 方向控制引脚
#define M10_DIR_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOE_CLK_ENABLE() // 使能GPIOE时钟

// 光耦传感器配置（位置检测）
#define M10_GO_GPIO_PORT       				 GPIOB         // 光耦输入端口
#define M10_GO_GPIO_PIN		    			 GPIO_PIN_8    // 光耦输入引脚
#define M10_GO_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE() // 使能GPIOB时钟

// 脉冲电平控制宏（直接端口操作）
#define M10_PUL_LEVE  PEout(0)  // 使用位操作控制PE0引脚电平		
#endif

/* 使能IO*/
// #define M10_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M10_EN_GPIO_PORT,M10_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M10_EN_GPIO_PORT,M10_EN_GPIO_PIN,LOW1)
										
// /* 方向IO*/
// #define M10_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M10_DIR_GPIO_PORT,M10_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M10_DIR_GPIO_PORT,M10_DIR_GPIO_PIN,LOW1)						
	
// #define M10_GO_STA  HAL_GPIO_ReadPin(M10_GO_GPIO_PORT,M10_GO_GPIO_PIN)	

/****************************************************************************
*M11电机配置引脚
*M11_STEP	PB4
*M11_EN	    PB3	
*M11_DIR	PB5	
*M11_GO	    PG15	
*****************************************************************************/

#if defined POCT_ANM_H1L4_A// || POCT_ANM_H3L5_A
// M11 Motor 脉冲信号配置
#define M11_PUL_PORT                         GPIOB        // 脉冲输出端口
#define M11_PUL_PIN                          GPIO_PIN_4   // 脉冲输出引脚
#define M11_PUL_GPIO_CLK_ENABLE()          	 __HAL_RCC_GPIOB_CLK_ENABLE() // 使能GPIOB时钟

// 使能信号配置
#define M11_EN_GPIO_PORT       				 GPIOB         // 使能信号端口
#define M11_EN_GPIO_PIN		    			 GPIO_PIN_3    // 使能信号引脚
#define M11_EN_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE() // 使能GPIOB时钟

// 方向控制配置
#define M11_DIR_GPIO_PORT       			 GPIOB         // 方向控制端口
#define M11_DIR_GPIO_PIN		    		 GPIO_PIN_5    // 方向控制引脚
#define M11_DIR_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE() // 使能GPIOB时钟

// 光耦传感器配置（位置检测）
#define M11_GO_GPIO_PORT       				 GPIOG          // 光耦输入端口
#define M11_GO_GPIO_PIN		    			 GPIO_PIN_15    // 光耦输入引脚
#define M11_GO_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟

// 脉冲电平控制宏（直接端口操作）
#define M11_PUL_LEVE  PBout(4)  // 使用位操作控制PB4引脚电平
#endif					
	
// /* 使能IO*/
// #define M11_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M11_EN_GPIO_PORT,M11_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M11_EN_GPIO_PORT,M11_EN_GPIO_PIN,LOW1)
										
// /* 方向IO*/
// #define M11_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M11_DIR_GPIO_PORT,M11_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M11_DIR_GPIO_PORT,M11_DIR_GPIO_PIN,LOW1)						
	
// #define M11_GO_STA  HAL_GPIO_ReadPin(M11_GO_GPIO_PORT,M11_GO_GPIO_PIN)	

						
									
/***************************************************************************************
*M12电机配置引脚
*M12_STEP	PB10	TIM2_CH3
*M12_EN	    PG0	
*M12_DIR	PG1	
*M12_GO	    PE7	
***************************************************************************************/

#if defined POCT_ANM_H1L4_A// || POCT_ANM_H3L5_A

// 脉冲信号配置（TIM2_CH3）
#define M12_PUL_PORT                        GPIOB       // 脉冲输出端口
#define M12_PUL_PIN                         GPIO_PIN_10 // 脉冲输出引脚
#define M12_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE() // 使能GPIOB时钟

// 使能信号配置
#define M12_EN_GPIO_PORT       				GPIOG       // 使能信号端口  
#define M12_EN_GPIO_PIN		    			GPIO_PIN_0   // 使能信号引脚
#define M12_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟

// 方向控制配置
#define M12_DIR_GPIO_PORT       			GPIOG       // 方向控制端口
#define M12_DIR_GPIO_PIN		    		GPIO_PIN_1   // 方向控制引脚
#define M12_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE() // 使能GPIOG时钟

// 光耦传感器配置（位置检测）
#define M12_GO_GPIO_PORT       				GPIOE       // 光耦输入端口  
#define M12_GO_GPIO_PIN		    			GPIO_PIN_7   // 光耦输入引脚
#define M12_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOE_CLK_ENABLE() // 使能GPIOE时钟			     

#define M12_PUL_LEVE  PBout(10)	
#endif					

// #define M12_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M12_EN_GPIO_PORT,M12_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M12_EN_GPIO_PORT,M12_EN_GPIO_PIN,LOW1)
										
// #define M12_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M12_DIR_GPIO_PORT,M12_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M12_DIR_GPIO_PORT,M12_DIR_GPIO_PIN,LOW1)

// #define M12_GO_STA  HAL_GPIO_ReadPin(M12_GO_GPIO_PORT,M12_GO_GPIO_PIN)
	
/***************************************************************************************
*M06电机配置引脚
M06_STEP	PC7	TIM3_CH2/TIM8_CH2
M06_EN	PF8	
M06_DIR	PF9	
M06_GO	PF7	
		
**************************************************************************************/	
//M06 Motor 脉冲信号配置
#define M06_PUL_PORT                  GPIOC       // 脉冲输出端口
#define M06_PUL_PIN                   GPIO_PIN_7   // 脉冲输出引脚
#define M06_PUL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE() // 使能GPIOC时钟

// 使能信号配置
#define M06_EN_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOF_CLK_ENABLE() // 使能GPIOF时钟		
#define M06_EN_GPIO_PORT       		GPIOF        // 使能信号端口
#define M06_EN_GPIO_PIN		    	GPIO_PIN_8   // 使能信号引脚

// 方向控制配置
#define M06_DIR_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOF_CLK_ENABLE() // 使能GPIOF时钟		
#define M06_DIR_GPIO_PORT       		GPIOF        // 方向控制端口
#define M06_DIR_GPIO_PIN		    	GPIO_PIN_9   // 方向控制引脚

// 光耦传感器配置（位置检测）
#define M06_GO_GPIO_CLK_ENABLE() 	    __HAL_RCC_GPIOF_CLK_ENABLE() // 使能GPIOF时钟
#define M06_GO_GPIO_PORT       		GPIOF         // 光耦输入端口
#define M06_GO_GPIO_PIN		    	GPIO_PIN_7    // 光耦输入引脚

// 脉冲电平控制宏（直接端口操作）
#define M06_PUL_LEVE  PCout(7)  // 使用位操作控制PC7引脚电平

// #define M06_EN_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M06_EN_GPIO_PORT,M06_EN_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M06_EN_GPIO_PORT,M06_EN_GPIO_PIN,LOW1)
					
					
// #define M06_DIR_OUT(a)	if (a)	\
// 					HAL_GPIO_WritePin(M06_DIR_GPIO_PORT,M06_DIR_GPIO_PIN,HIGH1);\
// 					else		\
// 					HAL_GPIO_WritePin(M06_DIR_GPIO_PORT,M06_DIR_GPIO_PIN,LOW1)

// #define M06_GO_STA  HAL_GPIO_ReadPin(M06_GO_GPIO_PORT,M06_GO_GPIO_PIN)	
void fan_init(void);
			
void Motor_Init(void);					
					
void Motor_TIM2_Init(uint16_t psc,uint16_t arr);					
					
#endif
					
