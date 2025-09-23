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
#define TIM_PRESCALER               (160 - 1)      //24-1  33us
//定义定时器周期，输出比较模式周期设置为0xFFFF
#define TIM_PERIOD                   800          //1000

/************************************************************/
#define HIGH1 GPIO_PIN_SET                        //高电平
#define LOW1  GPIO_PIN_RESET                      //低电平

//#define ON  LOW                                 //开
//#define OFF HIGH                                //关

#define CW  HIGH1                                 //顺时针
#define CCW LOW1                                  //逆时针

#if IS_BOOTLOADER_PROGRAM
typedef struct{
	 uint16_t en_pin;
	 GPIO_TypeDef *en_port;
}Motor_en_conf_Typedef;

#else

#endif

/*************************震动 电机*******************************************
*震动马达：PB0 
*****************************************************************************/
//#define  MIN_MOTOR_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE();
//#define  MIN_MOTOR_GPIO_PORT     GPIOB
//#define  MIN_MOTOR_GPIO_PIN      GPIO_PIN_0

//#define  MIN_MOTOR_RUN(a)        if (a)	\
//								 HAL_GPIO_WritePin(MIN_MOTOR_GPIO_PORT,MIN_MOTOR_GPIO_PIN,HIGH1);\
//								 else		\
//								 HAL_GPIO_WritePin(MIN_MOTOR_GPIO_PORT,MIN_MOTOR_GPIO_PIN,LOW1)    // HAL_GPIO_WritePin(MIN_MOTOR_GPIO_PORT,MIN_MOTOR_GPIO_PIN,x);

//#define MinMotorToggle(p,i)		 {p->ODR ^=i;}			                                           //输出反转状态
//#define MIN_MOTOR_TOGGLE		 MinMotorToggle(MIN_MOTOR_GPIO_PORT,MIN_MOTOR_GPIO_PIN)


/*****************************风扇*******************************************
*风扇：三孔：PG13  单孔：PA11
*****************************************************************************/

#if defined POCT_ANM_H1L4_A
	#define  FAN_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE();
	#define  FAN_GPIO_PORT           GPIOA
	#define  FAN_GPIO_PIN            GPIO_PIN_11

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

/*************************M01 电机*******************************************
*M01 
*M01_STEP	   PD13	   TIM4_CH2
*M01_EN	       PD10	
*M01_DIR	   PD11	
*****************************************************************************/

#define M01_PUL_PORT                        GPIOD
#define M01_PUL_PIN                         GPIO_PIN_13
#define M01_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
	
#define M01_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOD_CLK_ENABLE();		
#define M01_EN_GPIO_PORT       				GPIOD			            	
#define M01_EN_GPIO_PIN		    			GPIO_PIN_10			       			

#define M01_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOD_CLK_ENABLE();		
#define M01_DIR_GPIO_PORT       			GPIOD			         
#define M01_DIR_GPIO_PIN		    		GPIO_PIN_11			     

#define M01_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOD_CLK_ENABLE();	
#define M01_GO_GPIO_PORT       				GPIOD	
#define M01_GO_GPIO_PIN		    			GPIO_PIN_9	

#define M01_PUL_LEVE  PDout(13)	

#define M01_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M01_EN_GPIO_PORT,M01_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M01_EN_GPIO_PORT,M01_EN_GPIO_PIN,LOW1)
										
#define M01_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M01_DIR_GPIO_PORT,M01_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M01_DIR_GPIO_PORT,M01_DIR_GPIO_PIN,LOW1)
					
#define M01_GO_STA  HAL_GPIO_ReadPin(M01_GO_GPIO_PORT,M01_GO_GPIO_PIN)			   
			   
			   
/****************************************************************************
*M02电机配置引脚
*M02_STEP	        PD12	  TIM4_CH1
*M02_EN	            PD8	
*M02_DIR	        PB15	
*M02_TEMP_QH_GD	    PB14	
*****************************************************************************/

#define M02_PUL_PORT                        GPIOD
#define M02_PUL_PIN                         GPIO_PIN_12
#define M02_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
	
#define M02_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOD_CLK_ENABLE();		
#define M02_EN_GPIO_PORT       				GPIOD			            	
#define M02_EN_GPIO_PIN		    			GPIO_PIN_8			       			

#define M02_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOB_CLK_ENABLE();		
#define M02_DIR_GPIO_PORT       			GPIOB			         
#define M02_DIR_GPIO_PIN		    		GPIO_PIN_15			     

#define M02_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOB_CLK_ENABLE();	
#define M02_GO_GPIO_PORT       				GPIOB	
#define M02_GO_GPIO_PIN		    			GPIO_PIN_14	

#define M02_PUL_LEVE  PDout(12)

#define M02_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M02_EN_GPIO_PORT,M02_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M02_EN_GPIO_PORT,M02_EN_GPIO_PIN,LOW1)
					
#define M02_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M02_DIR_GPIO_PORT,M02_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M02_DIR_GPIO_PORT,M02_DIR_GPIO_PIN,LOW1)
					
#define M02_GO_STA  HAL_GPIO_ReadPin(M02_GO_GPIO_PORT,M02_GO_GPIO_PIN)	
					
					
/****************************************************************************
*M03电机配置引脚
*M03_STEP	PD14	TIM4_CH3
*M03_EN	    PG3	
*M03_DIR	PG4	
*M03_CAM_GD	PG2		
*****************************************************************************/
#define M03_PUL_PORT                  GPIOD
#define M03_PUL_PIN                   GPIO_PIN_14
#define M03_PUL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()

#define M03_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();		
#define M03_EN_GPIO_PORT       				GPIOG			          
#define M03_EN_GPIO_PIN		    			GPIO_PIN_3		

//方向
#define M03_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	
#define M03_DIR_GPIO_PORT       			GPIOG			       
#define M03_DIR_GPIO_PIN		    		GPIO_PIN_4			 

//光耦
#define M03_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOB_CLK_ENABLE();		
#define M03_GO_GPIO_PORT       				GPIOG			        
#define M03_GO_GPIO_PIN		    			GPIO_PIN_2

#define M03_PUL_LEVE  PDout(14)

#define M03_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M03_EN_GPIO_PORT,M03_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M03_EN_GPIO_PORT,M03_EN_GPIO_PIN,LOW1)
					

#define M03_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M03_DIR_GPIO_PORT,M03_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M03_DIR_GPIO_PORT,M03_DIR_GPIO_PIN,LOW1)

#define M03_GO_STA  HAL_GPIO_ReadPin(M03_GO_GPIO_PORT,M03_GO_GPIO_PIN)
					
/****************************************************************************
*M08电机配置引脚
*M08_STEP	PD15	TIM4_CH4
*M08_EN	    PG6	
*M08_DIR	PG7	
*M08_GO	    PG5		
*****************************************************************************/	
#define M08_PUL_PORT                  GPIOD
#define M08_PUL_PIN                   GPIO_PIN_15
#define M08_PUL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()

#define M08_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();		
#define M08_EN_GPIO_PORT       				GPIOG			       
#define M08_EN_GPIO_PIN		    			GPIO_PIN_6			      

#define M08_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	
#define M08_DIR_GPIO_PORT       			GPIOG			          
#define M08_DIR_GPIO_PIN		    		GPIO_PIN_7			     

#define M08_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	
#define M08_GO_GPIO_PORT       				GPIOG			         
#define M08_GO_GPIO_PIN		    			GPIO_PIN_5			   

#define M08_PUL_LEVE  PDout(15)

#define M08_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M08_EN_GPIO_PORT,M08_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M08_EN_GPIO_PORT,M08_EN_GPIO_PIN,LOW1)
					
					
#define M08_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M08_DIR_GPIO_PORT,M08_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M08_DIR_GPIO_PORT,M08_DIR_GPIO_PIN,LOW1)
					
#define M08_GO_STA  HAL_GPIO_ReadPin(M08_GO_GPIO_PORT,M08_GO_GPIO_PIN)
					
/****************************************************************************
*M04电机配置引脚
M04_STEP	PE9	    TIM1_CH1
M04_EN	PE2	
M04_DIR	PE3	
M04_DOOR_OPPEN_GD	PF0	
M04_DOOR_CLOSE_GD	PF1	
*****************************************************************************/

#define M04_PUL_PORT                                   GPIOE
#define M04_PUL_PIN                                    GPIO_PIN_9
#define M04_PUL_GPIO_CLK_ENABLE()                      __HAL_RCC_GPIOE_CLK_ENABLE()

#define M04_EN_GPIO_CLK_ENABLE() 	                   __HAL_RCC_GPIOF_CLK_ENABLE();	
#define M04_EN_GPIO_PORT       				           GPIOF			           
#define M04_EN_GPIO_PIN		    			           GPIO_PIN_2			     

//方向
#define M04_DIR_GPIO_CLK_ENABLE() 	                   __HAL_RCC_GPIOF_CLK_ENABLE();	
#define M04_DIR_GPIO_PORT       			           GPIOF			            
#define M04_DIR_GPIO_PIN		    		           GPIO_PIN_3			       
		   
#define M04_GO_GPIO_CLK_ENABLE() 	                   __HAL_RCC_GPIOF_CLK_ENABLE();
#define M04_GO_GPIO_PORT       				           GPIOF			           
#define M04_GO_GPIO_PIN		    			           GPIO_PIN_0	

#define M04_DOOR_OPPEN_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();		
#define M04_DOOR_OPPEN_GO_GPIO_PORT       				GPIOF			           
#define M04_DOOR_OPPEN_GO_GPIO_PIN		    			GPIO_PIN_1			       

#define M04_DOOR_CLOSE_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();
#define M04_DOOR_CLOSE_GO_GPIO_PORT       				GPIOF			           
#define M04_DOOR_CLOSE_GO_GPIO_PIN		    			GPIO_PIN_0	

#define M04_CLOSE_STA  HAL_GPIO_ReadPin(M04_DOOR_CLOSE_GO_GPIO_PORT,M04_DOOR_CLOSE_GO_GPIO_PIN)

#define M04_OPPEN_STA  HAL_GPIO_ReadPin(M04_DOOR_OPPEN_GO_GPIO_PORT,M04_DOOR_OPPEN_GO_GPIO_PIN)		

#define M04_PUL_LEVE  PEout(9)

#define M04_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M04_EN_GPIO_PORT,M04_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M04_EN_GPIO_PORT,M04_EN_GPIO_PIN,LOW1)

#define M04_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M04_DIR_GPIO_PORT,M04_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M04_DIR_GPIO_PORT,M04_DIR_GPIO_PIN,LOW1)	
					
					
/****************************************************************************
*M05电机配置引脚
*M05_STEP	PE11	TIM1_CH2
*M05_EN	    PF5	
*M05_DIR	PF6	
*M05_GO	    PF4		
*****************************************************************************/
#define M05_PUL_PORT                        GPIOE
#define M05_PUL_PIN                         GPIO_PIN_11
#define M05_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOE_CLK_ENABLE()

#define M05_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();	
#define M05_EN_GPIO_PORT       				GPIOF			           
#define M05_EN_GPIO_PIN		    			GPIO_PIN_5			       

//方向
#define M05_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();	
#define M05_DIR_GPIO_PORT       			GPIOF			           
#define M05_DIR_GPIO_PIN		    		GPIO_PIN_6			       

//光耦
#define M05_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();		
#define M05_GO_GPIO_PORT       				GPIOF			          
#define M05_GO_GPIO_PIN		    			GPIO_PIN_4			    

#define M05_PUL_LEVE  PEout(11)

#define M05_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M05_EN_GPIO_PORT,M05_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M05_EN_GPIO_PORT,M05_EN_GPIO_PIN,LOW1)
					
					
#define M05_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M05_DIR_GPIO_PORT,M05_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M05_DIR_GPIO_PORT,M05_DIR_GPIO_PIN,LOW1)					

#define M05_GO_STA  HAL_GPIO_ReadPin(M05_GO_GPIO_PORT,M05_GO_GPIO_PIN)
					
					
/****************************************************************************
*M07电机配置引脚
*M07_STEP	PE13	TIM1_CH3
*M07_EN	    PC0	
*M07_DIR	PC1	
*M07_GO	    PF10	
*****************************************************************************/			
#define M07_PUL_PORT                        GPIOE
#define M07_PUL_PIN                         GPIO_PIN_10
#define M07_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOE_CLK_ENABLE()
				
#define M07_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOC_CLK_ENABLE();	
#define M07_EN_GPIO_PORT       				GPIOC			           
#define M07_EN_GPIO_PIN		    			GPIO_PIN_0			       

//方向
#define M07_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOC_CLK_ENABLE();	
#define M07_DIR_GPIO_PORT       			GPIOC			           
#define M07_DIR_GPIO_PIN		    		GPIO_PIN_1			     

//光耦
#define M07_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();	
#define M07_GO_GPIO_PORT       				GPIOF			          
#define M07_GO_GPIO_PIN		    			GPIO_PIN_10	

#define M07_PUL_LEVE  PEout(10)

#define M07_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M07_EN_GPIO_PORT,M07_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M07_EN_GPIO_PORT,M07_EN_GPIO_PIN,LOW1)
					
#define M07_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M07_DIR_GPIO_PORT,M07_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M07_DIR_GPIO_PORT,M07_DIR_GPIO_PIN,LOW1)						
	
#define M07_GO_STA  HAL_GPIO_ReadPin(M07_GO_GPIO_PORT,M07_GO_GPIO_PIN)
					
/****************************************************************************
*M09电机配置引脚
*M09_STEP	PG12
*M09_EN	    PG13	
*M09_DIR	PG11	
*M09_GO	    PG10		
*****************************************************************************/
#if defined POCT_ANM_H1L4_A //|| POCT_ANM_H3L5_A
					
	//M09 Motor 脉冲
	#define M09_PUL_PORT                         GPIOG
	#define M09_PUL_PIN                          GPIO_PIN_10
	#define M09_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()			

	#define M09_EN_GPIO_PORT       				GPIOG			          
	#define M09_EN_GPIO_PIN		    			GPIO_PIN_11		
	#define M09_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	

	//方向	
	#define M09_DIR_GPIO_PORT       			GPIOG			           
	#define M09_DIR_GPIO_PIN		    		GPIO_PIN_12		
	#define M09_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();

	//光耦
	#define M09_GO_GPIO_PORT       				GPIOG			          
	#define M09_GO_GPIO_PIN		    			GPIO_PIN_13	
	#define M09_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();

	#define M09_PUL_LEVE  PGout(10)	
	
#elif defined POCT_HMN_H2L8_A
	//M09 Motor 脉冲
	#define M09_PUL_PORT                         GPIOG
	#define M09_PUL_PIN                          GPIO_PIN_10
	#define M09_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()			

	#define M09_EN_GPIO_PORT       				GPIOG			          
	#define M09_EN_GPIO_PIN		    			GPIO_PIN_11		
	#define M09_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	

	//方向	
	#define M09_DIR_GPIO_PORT       			GPIOG			           
	#define M09_DIR_GPIO_PIN		    		GPIO_PIN_12		
	#define M09_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();

	//光耦
	#define M09_GO_GPIO_PORT       				GPIOG			          
	#define M09_GO_GPIO_PIN		    			GPIO_PIN_13	
	#define M09_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();

	#define M09_PUL_LEVE  PGout(10)	

	
#elif defined POCT_ANM_H3L5_A
    //M09 Motor 脉冲
	#define M09_PUL_PORT                         GPIOB
	#define M09_PUL_PIN                          GPIO_PIN_10
	#define M09_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()			

	#define M09_EN_GPIO_PORT       				GPIOG			          
	#define M09_EN_GPIO_PIN		    			GPIO_PIN_0		
	#define M09_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	

	//方向	
	#define M09_DIR_GPIO_PORT       			GPIOG			           
	#define M09_DIR_GPIO_PIN		    		GPIO_PIN_1		
	#define M09_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();

	//光耦
	#define M09_GO_GPIO_PORT       				GPIOE			          
	#define M09_GO_GPIO_PIN		    			GPIO_PIN_7	
	#define M09_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();

	#define M09_PUL_LEVE  PBout(10)	

					
#elif defined POCT_HMN_H5L5_A
	//M09 Motor 脉冲
	#define M09_PUL_PORT                         GPIOG
	#define M09_PUL_PIN                          GPIO_PIN_12
	#define M09_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOG_CLK_ENABLE()			

	#define M09_EN_GPIO_PORT       				GPIOG			          
	#define M09_EN_GPIO_PIN		    			GPIO_PIN_13		
	#define M09_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	
	
	#define M09_DIR_GPIO_PORT       			GPIOG			           
	#define M09_DIR_GPIO_PIN		    		GPIO_PIN_11		
	#define M09_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();

	#define M09_GO_GPIO_PORT       				GPIOG			          
	#define M09_GO_GPIO_PIN		    			GPIO_PIN_10	
	#define M09_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	

	#define M09_PUL_LEVE  PGout(12)	
#endif					

/* 使能IO*/
#define M09_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M09_EN_GPIO_PORT,M09_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M09_EN_GPIO_PORT,M09_EN_GPIO_PIN,LOW1)
					
					
/* 方向IO*/
#define M09_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M09_DIR_GPIO_PORT,M09_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M09_DIR_GPIO_PORT,M09_DIR_GPIO_PIN,LOW1)						
	
#define M09_GO_STA  HAL_GPIO_ReadPin(M09_GO_GPIO_PORT,M09_GO_GPIO_PIN)	

/****************************************************************************
*M10电机配置引脚
*M10_STEP	PE0
*M10_EN	    PB9	
*M10_DIR	PE1	
*M10_GO	    PB8	
*****************************************************************************/
#if defined POCT_ANM_H1L4_A// || POCT_ANM_H3L5_A
//M10 Motor 脉冲
#define M10_PUL_PORT                         GPIOE
#define M10_PUL_PIN                          GPIO_PIN_0
#define M10_PUL_GPIO_CLK_ENABLE()          	 __HAL_RCC_GPIOE_CLK_ENABLE()			

#define M10_EN_GPIO_PORT       				 GPIOB			          
#define M10_EN_GPIO_PIN		    			 GPIO_PIN_9		
#define M10_EN_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();	

//方向	
#define M10_DIR_GPIO_PORT       			 GPIOE			           
#define M10_DIR_GPIO_PIN		    		 GPIO_PIN_1		
#define M10_DIR_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOE_CLK_ENABLE();

//光耦
#define M10_GO_GPIO_PORT       				 GPIOB			          
#define M10_GO_GPIO_PIN		    			 GPIO_PIN_8	
#define M10_GO_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();	

#define M10_PUL_LEVE  PEout(0)

#elif defined POCT_HMN_H2L8_A

//M10 Motor 脉冲
#define M10_PUL_PORT                         GPIOE
#define M10_PUL_PIN                          GPIO_PIN_0
#define M10_PUL_GPIO_CLK_ENABLE()          	 __HAL_RCC_GPIOE_CLK_ENABLE()			

#define M10_EN_GPIO_PORT       				 GPIOB			          
#define M10_EN_GPIO_PIN		    			 GPIO_PIN_9		
#define M10_EN_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();	

//方向	
#define M10_DIR_GPIO_PORT       			 GPIOE			           
#define M10_DIR_GPIO_PIN		    		 GPIO_PIN_1		
#define M10_DIR_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOE_CLK_ENABLE();

//光耦
#define M10_GO_GPIO_PORT       				 GPIOB			          
#define M10_GO_GPIO_PIN		    			 GPIO_PIN_8	
#define M10_GO_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();	

#define M10_PUL_LEVE  PEout(0)

#elif defined POCT_ANM_H3L5_A

//M10 Motor 脉冲
#define M10_PUL_PORT                         GPIOE
#define M10_PUL_PIN                          GPIO_PIN_15
#define M10_PUL_GPIO_CLK_ENABLE()          	 __HAL_RCC_GPIOE_CLK_ENABLE()			

#define M10_EN_GPIO_PORT       				 GPIOE			          
#define M10_EN_GPIO_PIN		    			 GPIO_PIN_12		
#define M10_EN_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOE_CLK_ENABLE();	

//方向	
#define M10_DIR_GPIO_PORT       			 GPIOE			           
#define M10_DIR_GPIO_PIN		    		 GPIO_PIN_10		
#define M10_DIR_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOE_CLK_ENABLE();

//光耦
#define M10_GO_GPIO_PORT       				 GPIOD			          
#define M10_GO_GPIO_PIN		    			 GPIO_PIN_5	
#define M10_GO_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOD_CLK_ENABLE();
#define M10_PUL_LEVE  PEout(15)

#elif defined POCT_HMN_H5L5_A

//M10 Motor 脉冲
#define M10_PUL_PORT                         GPIOE
#define M10_PUL_PIN                          GPIO_PIN_0
#define M10_PUL_GPIO_CLK_ENABLE()          	 __HAL_RCC_GPIOE_CLK_ENABLE()			

#define M10_EN_GPIO_PORT       				 GPIOB			          
#define M10_EN_GPIO_PIN		    			 GPIO_PIN_9		
#define M10_EN_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();	

//方向	
#define M10_DIR_GPIO_PORT       			 GPIOE			           
#define M10_DIR_GPIO_PIN		    		 GPIO_PIN_1		
#define M10_DIR_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOE_CLK_ENABLE();

//光耦
#define M10_GO_GPIO_PORT       				 GPIOB			          
#define M10_GO_GPIO_PIN		    			 GPIO_PIN_8	
#define M10_GO_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();	

#define M10_PUL_LEVE  PEout(0)					
#endif

/* 使能IO*/
#define M10_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M10_EN_GPIO_PORT,M10_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M10_EN_GPIO_PORT,M10_EN_GPIO_PIN,LOW1)
										
/* 方向IO*/
#define M10_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M10_DIR_GPIO_PORT,M10_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M10_DIR_GPIO_PORT,M10_DIR_GPIO_PIN,LOW1)						
	
#define M10_GO_STA  HAL_GPIO_ReadPin(M10_GO_GPIO_PORT,M10_GO_GPIO_PIN)	

/****************************************************************************
*M11电机配置引脚
*M11_STEP	PB4
*M11_EN	    PB3	
*M11_DIR	PB5	
*M11_GO	    PG15	
*****************************************************************************/

#if defined POCT_ANM_H1L4_A// || POCT_ANM_H3L5_A
//M10 Motor 脉冲
#define M11_PUL_PORT                         GPIOB
#define M11_PUL_PIN                          GPIO_PIN_4
#define M11_PUL_GPIO_CLK_ENABLE()          	 __HAL_RCC_GPIOB_CLK_ENABLE()			

#define M11_EN_GPIO_PORT       				 GPIOB			          
#define M11_EN_GPIO_PIN		    			 GPIO_PIN_3		
#define M11_EN_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();	

//方向	
#define M11_DIR_GPIO_PORT       			 GPIOB			           
#define M11_DIR_GPIO_PIN		    		 GPIO_PIN_5		
#define M11_DIR_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();

//光耦
#define M11_GO_GPIO_PORT       				 GPIOG			          
#define M11_GO_GPIO_PIN		    			 GPIO_PIN_15	
#define M11_GO_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOG_CLK_ENABLE();			

#define M11_PUL_LEVE  PBout(4)

#elif defined POCT_HMN_H2L8_A

//M10 Motor 脉冲
#define M11_PUL_PORT                         GPIOB
#define M11_PUL_PIN                          GPIO_PIN_4
#define M11_PUL_GPIO_CLK_ENABLE()          	 __HAL_RCC_GPIOB_CLK_ENABLE()			

#define M11_EN_GPIO_PORT       				 GPIOB			          
#define M11_EN_GPIO_PIN		    			 GPIO_PIN_3		
#define M11_EN_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();	

//方向	
#define M11_DIR_GPIO_PORT       			 GPIOB			           
#define M11_DIR_GPIO_PIN		    		 GPIO_PIN_5		
#define M11_DIR_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();

//光耦
#define M11_GO_GPIO_PORT       				 GPIOG			          
#define M11_GO_GPIO_PIN		    			 GPIO_PIN_15	
#define M11_GO_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOG_CLK_ENABLE();			

#define M11_PUL_LEVE  PBout(4)

#elif defined POCT_ANM_H3L5_A

//M10 Motor 脉冲
#define M11_PUL_PORT                         GPIOE
#define M11_PUL_PIN                          GPIO_PIN_0
#define M11_PUL_GPIO_CLK_ENABLE()          	 __HAL_RCC_GPIOE_CLK_ENABLE()			

#define M11_EN_GPIO_PORT       				 GPIOB			          
#define M11_EN_GPIO_PIN		    			 GPIO_PIN_9		
#define M11_EN_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();	

//方向	
#define M11_DIR_GPIO_PORT       			 GPIOE			           
#define M11_DIR_GPIO_PIN		    		 GPIO_PIN_1		
#define M11_DIR_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOE_CLK_ENABLE();

//光耦
#define M11_GO_GPIO_PORT       				 GPIOB			          
#define M11_GO_GPIO_PIN		    			 GPIO_PIN_8	
#define M11_GO_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();			

#define M11_PUL_LEVE  PEout(0)


#elif defined POCT_HMN_H5L5_A


//M10 Motor 脉冲
#define M11_PUL_PORT                         GPIOB
#define M11_PUL_PIN                          GPIO_PIN_4
#define M11_PUL_GPIO_CLK_ENABLE()          	 __HAL_RCC_GPIOB_CLK_ENABLE()			

#define M11_EN_GPIO_PORT       				 GPIOB			          
#define M11_EN_GPIO_PIN		    			 GPIO_PIN_3		
#define M11_EN_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();	

//方向	
#define M11_DIR_GPIO_PORT       			 GPIOB			           
#define M11_DIR_GPIO_PIN		    		 GPIO_PIN_5		
#define M11_DIR_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOB_CLK_ENABLE();

//光耦
#define M11_GO_GPIO_PORT       				 GPIOG			          
#define M11_GO_GPIO_PIN		    			 GPIO_PIN_15	
#define M11_GO_GPIO_CLK_ENABLE() 	         __HAL_RCC_GPIOG_CLK_ENABLE();	

#define M11_PUL_LEVE  PBout(4)
#endif					
	
/* 使能IO*/
#define M11_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M11_EN_GPIO_PORT,M11_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M11_EN_GPIO_PORT,M11_EN_GPIO_PIN,LOW1)
										
/* 方向IO*/
#define M11_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M11_DIR_GPIO_PORT,M11_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M11_DIR_GPIO_PORT,M11_DIR_GPIO_PIN,LOW1)						
	
#define M11_GO_STA  HAL_GPIO_ReadPin(M11_GO_GPIO_PORT,M11_GO_GPIO_PIN)	

						
									
/***************************************************************************************
*M12电机配置引脚
*M12_STEP	PB10	TIM2_CH3
*M12_EN	    PG0	
*M12_DIR	PG1	
*M12_GO	    PE7	
***************************************************************************************/

#if defined POCT_ANM_H1L4_A// || POCT_ANM_H3L5_A
					
#define M12_PUL_PORT                         GPIOB
#define M12_PUL_PIN                          GPIO_PIN_10
#define M12_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()

#define M12_EN_GPIO_PORT       				GPIOG			          
#define M12_EN_GPIO_PIN		    			GPIO_PIN_0			        
#define M12_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	
					
//方向
#define M12_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();
#define M12_DIR_GPIO_PORT       			GPIOG		
#define M12_DIR_GPIO_PIN		    		GPIO_PIN_1	

//光耦
#define M12_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOE_CLK_ENABLE();		
#define M12_GO_GPIO_PORT       				GPIOE			           
#define M12_GO_GPIO_PIN		    			GPIO_PIN_7			     

#define M12_PUL_LEVE  PBout(10)	

#elif defined POCT_HMN_H2L8_A// || POCT_ANM_H3L5_A
					
#define M12_PUL_PORT                         GPIOB
#define M12_PUL_PIN                          GPIO_PIN_10
#define M12_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()

#define M12_EN_GPIO_PORT       				GPIOG			          
#define M12_EN_GPIO_PIN		    			GPIO_PIN_0			        
#define M12_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	
					
//方向
#define M12_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();
#define M12_DIR_GPIO_PORT       			GPIOG		
#define M12_DIR_GPIO_PIN		    		GPIO_PIN_1	

//光耦
#define M12_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOE_CLK_ENABLE();		
#define M12_GO_GPIO_PORT       				GPIOE			           
#define M12_GO_GPIO_PIN		    			GPIO_PIN_7			     

#define M12_PUL_LEVE  PBout(10)	

#elif defined POCT_ANM_H3L5_A

#define M12_PUL_PORT                         GPIOB
#define M12_PUL_PIN                          GPIO_PIN_4
#define M12_PUL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()

#define M12_EN_GPIO_PORT       				GPIOB			          
#define M12_EN_GPIO_PIN		    			GPIO_PIN_3			        
#define M12_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOB_CLK_ENABLE();	
					
//方向
#define M12_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOB_CLK_ENABLE();
#define M12_DIR_GPIO_PORT       			GPIOB		
#define M12_DIR_GPIO_PIN		    		GPIO_PIN_5	

//光耦
#define M12_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();		
#define M12_GO_GPIO_PORT       				GPIOG			           
#define M12_GO_GPIO_PIN		    			GPIO_PIN_15			     

#define M12_PUL_LEVE  PBout(4)	
					
#elif defined POCT_HMN_H5L5_A

#define M12_PUL_PORT                  GPIOB
#define M12_PUL_PIN                   GPIO_PIN_10
#define M12_PUL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define M12_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	
#define M12_EN_GPIO_PORT       				GPIOG			          
#define M12_EN_GPIO_PIN		    			GPIO_PIN_0			       


//方向
#define M12_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOG_CLK_ENABLE();	
#define M12_DIR_GPIO_PORT       			GPIOG			          
#define M12_DIR_GPIO_PIN		    		GPIO_PIN_1			      

//光耦
#define M12_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOE_CLK_ENABLE();
#define M12_GO_GPIO_PORT       				GPIOE			        
#define M12_GO_GPIO_PIN		    			GPIO_PIN_7			    

#define M12_PUL_LEVE  PBout(10)	

#endif					

#define M12_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M12_EN_GPIO_PORT,M12_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M12_EN_GPIO_PORT,M12_EN_GPIO_PIN,LOW1)
					
					
#define M12_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M12_DIR_GPIO_PORT,M12_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M12_DIR_GPIO_PORT,M12_DIR_GPIO_PIN,LOW1)

#define M12_GO_STA  HAL_GPIO_ReadPin(M12_GO_GPIO_PORT,M12_GO_GPIO_PIN)
	
								
/***************************************************************************************
*M13电机配置引脚
*M13_STEP	PB11	TIM2_CH4
*M13_EN	    PF13	
*M13_DIR	PF14	
*M13_GO	    PF15		
**************************************************************************************/
//M13 Motor 脉冲
#define M13_PUL_IRQn                  TIM2_IRQn
#define M13_PUL_IRQHandler            TIM2_IRQHandler

#define M13_PUL_TIM                   TIM2
#define M13_PUL_CLK_ENABLE()          __TIM2_CLK_ENABLE()

#define M13_PUL_PORT                  GPIOB
#define M13_PUL_PIN                   GPIO_PIN_11
#define M13_PUL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define M13_PUL_GPIO_AF               GPIO_AF1_TIM2
#define M13_PUL_CHANNEL_x             TIM_CHANNEL_4

#define M13_TIM_IT_CCx                TIM_IT_CC4
#define M13_TIM_FLAG_CCx              TIM_FLAG_CC4	

#define M13_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();	
#define M13_EN_GPIO_PORT       				GPIOF			           
#define M13_EN_GPIO_PIN		    			GPIO_PIN_13			      

//方向
#define M13_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();		
#define M13_DIR_GPIO_PORT       			GPIOF			           
#define M13_DIR_GPIO_PIN		    		GPIO_PIN_14			       

//光耦
#define M13_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();		
#define M13_GO_GPIO_PORT       				GPIOF			           
#define M13_GO_GPIO_PIN		    			GPIO_PIN_15			        


#define M13_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M13_EN_GPIO_PORT,M13_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M13_EN_GPIO_PORT,M13_EN_GPIO_PIN,LOW1)
									
#define M13_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M13_DIR_GPIO_PORT,M13_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M13_DIR_GPIO_PORT,M13_DIR_GPIO_PIN,LOW1)

#define M13_GO_STA  HAL_GPIO_ReadPin(M13_GO_GPIO_PORT,M13_GO_GPIO_PIN)		

/***************************************************************************************
*M14电机配置引脚
M14_STEP	PA15	TIM2_CH1
M14_EN	PB2	
M14_DIR	PF11	
M14_GO	PF12	
**************************************************************************************/
//M14 Motor 脉冲
#define M14_PUL_IRQn                  TIM2_IRQn
#define M14_PUL_IRQHandler            TIM2_IRQHandler

#define M14_PUL_TIM                   TIM2
#define M14_PUL_CLK_ENABLE()          __TIM2_CLK_ENABLE()

#define M14_PUL_PORT                  GPIOA
#define M14_PUL_PIN                   GPIO_PIN_15
#define M14_PUL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define M14_PUL_GPIO_AF               GPIO_AF1_TIM2
#define M14_PUL_CHANNEL_x             TIM_CHANNEL_1

#define M14_TIM_IT_CCx                TIM_IT_CC1
#define M14_TIM_FLAG_CCx              TIM_FLAG_CC1	

#define M14_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOB_CLK_ENABLE();		
#define M14_EN_GPIO_PORT       				GPIOB			           
#define M14_EN_GPIO_PIN		    			GPIO_PIN_2			      

#define M14_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();	
#define M14_DIR_GPIO_PORT       			GPIOF			         
#define M14_DIR_GPIO_PIN		    		GPIO_PIN_11			       

#define M14_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();		
#define M14_GO_GPIO_PORT       				GPIOF			          
#define M14_GO_GPIO_PIN		    			GPIO_PIN_12			       

#define M14_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M14_EN_GPIO_PORT,M14_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M14_EN_GPIO_PORT,M14_EN_GPIO_PIN,LOW1)
					
#define M14_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M14_DIR_GPIO_PORT,M14_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M14_DIR_GPIO_PORT,M14_DIR_GPIO_PIN,LOW1)										
					
#define M14_GO_STA  HAL_GPIO_ReadPin(M14_GO_GPIO_PORT,M14_GO_GPIO_PIN)

/***************************************************************************************
*M06电机配置引脚
M06_STEP	PC7	TIM3_CH2/TIM8_CH2
M06_EN	PF8	
M06_DIR	PF9	
M06_GO	PF7	
		
**************************************************************************************/	
//M06 Motor 脉冲
#define M06_PUL_PORT                  GPIOC
#define M06_PUL_PIN                   GPIO_PIN_7
#define M06_PUL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()

#define M06_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();		
#define M06_EN_GPIO_PORT       				GPIOF			           
#define M06_EN_GPIO_PIN		    			GPIO_PIN_8			       

#define M06_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();		
#define M06_DIR_GPIO_PORT       			GPIOF			          
#define M06_DIR_GPIO_PIN		    		GPIO_PIN_9			     

#define M06_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOF_CLK_ENABLE();
#define M06_GO_GPIO_PORT       				GPIOF			            
#define M06_GO_GPIO_PIN		    			GPIO_PIN_7			      

#define M06_PUL_LEVE  PCout(7)

#define M06_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M06_EN_GPIO_PORT,M06_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M06_EN_GPIO_PORT,M06_EN_GPIO_PIN,LOW1)
					
					
#define M06_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M06_DIR_GPIO_PORT,M06_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M06_DIR_GPIO_PORT,M06_DIR_GPIO_PIN,LOW1)

#define M06_GO_STA  HAL_GPIO_ReadPin(M06_GO_GPIO_PORT,M06_GO_GPIO_PIN)					
					
/***************************************************************************************
*M14电机配置引脚
M15_STEP	PC6	TIM3_CH1, TIM8_CH1,
M15_EN	PD0	
M15_DIR	PD1	
M15_GO	PC12			
**************************************************************************************/
#define M15_PUL_PORT                      GPIOC
#define M15_PUL_PIN                       GPIO_PIN_6
#define M15_PUL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()

#define M15_EN_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOD_CLK_ENABLE();		
#define M15_EN_GPIO_PORT       				GPIOD
#define M15_EN_GPIO_PIN		    			GPIO_PIN_0			       

#define M15_DIR_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOD_CLK_ENABLE();		
#define M15_DIR_GPIO_PORT       			GPIOD			          
#define M15_DIR_GPIO_PIN		    		GPIO_PIN_1			       

#define M15_GO_GPIO_CLK_ENABLE() 	        __HAL_RCC_GPIOC_CLK_ENABLE();	
#define M15_GO_GPIO_PORT       				GPIOC			           
#define M15_GO_GPIO_PIN		    			GPIO_PIN_12			       


#define M15_EN_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M15_EN_GPIO_PORT,M15_EN_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M15_EN_GPIO_PORT,M15_EN_GPIO_PIN,LOW1)
										
#define M15_DIR_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(M15_DIR_GPIO_PORT,M15_DIR_GPIO_PIN,HIGH1);\
					else		\
					HAL_GPIO_WritePin(M15_DIR_GPIO_PORT,M15_DIR_GPIO_PIN,LOW1)
							
#define M15_GO_STA  HAL_GPIO_ReadPin(M15_GO_GPIO_PORT,M15_GO_GPIO_PIN)						
#define M15_PUL_LEVE  PCout(6)	
					
void Motor_Init(void);					
					
void Motor_TIM2_Init(uint16_t psc,uint16_t arr);					
					
#endif
					
