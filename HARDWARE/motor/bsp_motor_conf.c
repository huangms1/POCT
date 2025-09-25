/**
  ******************************************************************************
  * @file    bsp_motor_conf.c
  * @author  hms
  * @version V1.0
  * @date    2021.11.11
  * @brief   用于电机定义
  ******************************************************************************
  * @attention
  ******************************************************************************
  */
  
#include "bsp_motor_conf.h" 
#include "bsp_motor.h"
#include "FreeRTOS.h"
#include "stm32f4xx_it.h"

TIM_HandleTypeDef  TIM4_Handler; //定时器4 PWM句柄。控制电机
TIM_HandleTypeDef  TIM1_Handler; //定时器1 PWM句柄，控制电机
TIM_HandleTypeDef  TIM2_Handler; //定时器2 PWM句柄，可不使用
TIM_HandleTypeDef  TIM3_Handler; //定时器2 PWM句柄，控制电机

#if IS_BOOTLOADER_PROGRAM

Motor_en_conf_Typedef Boot_Motor_EN[17] = {
   {GPIO_PIN_10,GPIOD},
   {GPIO_PIN_8,GPIOD},
   {GPIO_PIN_13,GPIOG},
   {GPIO_PIN_2,GPIOF},
   {GPIO_PIN_6,GPIOG},
   {GPIO_PIN_5,GPIOF},
   {GPIO_PIN_10,GPIOC},
   {GPIO_PIN_0,GPIOC},
   {GPIO_PIN_13,GPIOG},
   {GPIO_PIN_12,GPIOE},
   {GPIO_PIN_9,GPIOB},
   {GPIO_PIN_3,GPIOB},
   {GPIO_PIN_0,GPIOG},
   {GPIO_PIN_13,GPIOF},
   {GPIO_PIN_2,GPIOB},
   {GPIO_PIN_8,GPIOF},
   {GPIO_PIN_0,GPIOD},
};

//防止电机电流过大烧掉
void  BootMotorENConf(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	int i = 0;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	for(i = 0; i < 17; i++)
	{
		/*选择要控制的GPIO引脚*/															   
		GPIO_InitStruct.Pin   = Boot_Motor_EN[i].en_pin;	
		GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
		GPIO_InitStruct.Pull  = GPIO_PULLUP;  
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(Boot_Motor_EN[i].en_port, &GPIO_InitStruct);
	
		HAL_GPIO_WritePin(Boot_Motor_EN[i].en_port,Boot_Motor_EN[i].en_pin,HIGH1);
	}
}

#else

#endif


void fan_init(void)
{
	GPIO_InitTypeDef GPIO_Initure;	
	FAN_CLK_ENABLE();
	
	GPIO_Initure.Pin   = FAN_GPIO_PIN;
	GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  
    GPIO_Initure.Pull  = GPIO_PULLUP;         
    GPIO_Initure.Speed = GPIO_SPEED_HIGH; 
	HAL_GPIO_Init(FAN_GPIO_PORT,&GPIO_Initure);
	
	HAL_GPIO_WritePin(FAN_GPIO_PORT,FAN_GPIO_PIN,LOW1);	
}



//TIM4初始化   电机M01 通道2  M02-> 通道1  M03-> 通道3  M08-> 通道4
void Motor_TIM4_Init(uint16_t psc,uint16_t arr)
{
	TIM_OC_InitTypeDef TIM_OCInitStructure;
	/*使能定时器4*/
	__TIM4_CLK_ENABLE();
	
	TIM4_Handler.Instance           = TIM4; 
	TIM4_Handler.Init.Prescaler     = psc; 
	TIM4_Handler.Init.CounterMode   = TIM_COUNTERMODE_UP;
	TIM4_Handler.Init.Period        = arr;  
	TIM4_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; 
	TIM4_Handler.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&TIM4_Handler);
	
	/*PWM模式配置--这里配置为输出比较模式*/
	TIM_OCInitStructure.OCMode       = TIM_OCMODE_TOGGLE;
	/*比较输出的计数值*/
	TIM_OCInitStructure.Pulse        = 0;
	/*当定时器计数值小于CCR1_Val时为高电平*/
	TIM_OCInitStructure.OCPolarity   = TIM_OCPOLARITY_HIGH;
	/*设置互补通道输出的极性*/
	TIM_OCInitStructure.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	/*快速模式设置*/
	TIM_OCInitStructure.OCFastMode   = TIM_OCFAST_DISABLE;
	/*空闲电平*/
	TIM_OCInitStructure.OCIdleState  = TIM_OCIDLESTATE_RESET;
	/*互补通道设置*/
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	
	HAL_TIM_OC_ConfigChannel(&TIM4_Handler,&TIM_OCInitStructure,TIM_CHANNEL_1);

	/*确定定时器*/
	HAL_TIM_Base_Stop(&TIM4_Handler);
	//HAL_TIM_Base_Start(&TIM4_Handler);	
	//设置抢占优先级，子优先级
	HAL_NVIC_SetPriority(TIM4_IRQn, 2, 1);
	// 设置中断来源
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
	
}

void Decision_TIM2(TIM_HandleTypeDef*htim,SpeedCalc_TypeDef * pSpeed)
{
	if(Stepper[M01].start_flag == 1)
	{
		Speed_Decision_TIM2(htim,TIM_IT_CC1,TIM_CHANNEL_1,M01,pSpeed);
	}
	if(Stepper[M02].start_flag == 1)
	{
		Speed_Decision_TIM2(htim,TIM_IT_CC1,TIM_CHANNEL_1,M02,pSpeed);
	}
	if(Stepper[M03].start_flag == 1)
	{
		Speed_Decision_TIM2(htim,TIM_IT_CC1,TIM_CHANNEL_1,M03,pSpeed);
	}
	if(Stepper[M04].start_flag == 1)
	{
		Speed_Decision_TIM2(htim,TIM_IT_CC1,TIM_CHANNEL_1,M04,pSpeed);
	}
}

void Decision_TIM3(TIM_HandleTypeDef*htim,SpeedCalc_TypeDef * pSpeed)
{
	if(Stepper[M05].start_flag == 1)
	{
		Speed_Decision_TIM3(htim,TIM_IT_CC1,TIM_CHANNEL_1,M05,pSpeed);
	}
	if(Stepper[M06].start_flag == 1)
	{
		Speed_Decision_TIM3(htim,TIM_IT_CC1,TIM_CHANNEL_1,M06,pSpeed);
	}
	if(Stepper[M07].start_flag == 1)
	{
		Speed_Decision_TIM3(htim,TIM_IT_CC1,TIM_CHANNEL_1,M07,pSpeed);
	}
	if(Stepper[M08].start_flag == 1)
	{
		Speed_Decision_TIM3(htim,TIM_IT_CC1,TIM_CHANNEL_1,M08,pSpeed);
	}	
	if(Stepper[M11].start_flag == 1)
	{
		Speed_Decision_TIM3(htim,TIM_IT_CC1,TIM_CHANNEL_1,M11,pSpeed);
	}
	if(Stepper[M09].start_flag == 1)
	{
		Speed_Decision_TIM3(htim,TIM_IT_CC1,TIM_CHANNEL_1,M09,pSpeed);
//		Speed_Decision_TIM3(&TIM2_Handler,TIM_IT_CC1,TIM_CHANNEL_1,M09,pSpeedGloble[0]);
	}
}

void Decision_TIM4(TIM_HandleTypeDef*htim,SpeedCalc_TypeDef * pSpeed)
{
	if(Stepper[M12].start_flag == 1)
	{
		Speed_Decision_TIM4(htim,TIM_IT_CC1,TIM_CHANNEL_1,M12,pSpeed);
	}
	if(Stepper[M15].start_flag == 1)
	{
		Speed_Decision_TIM4(htim,TIM_IT_CC1,TIM_CHANNEL_1,M15,pSpeed);
	}
	if(Stepper[M10].start_flag == 1)
	{
		Speed_Decision_TIM4(htim,TIM_IT_CC1,TIM_CHANNEL_1,M10,pSpeed);
//		Speed_Decision_TIM4(&TIM3_Handler,TIM_IT_CC1,TIM_CHANNEL_1,M10,pSpeedGloble[1]);
	}
}

void Decision_leve(TIM_HandleTypeDef*htim,SpeedCalc_TypeDef * pSpeed)
{
	
}

//速度决策
void motor_Decision(TIM_HandleTypeDef*htim)
{
	SpeedCalc_TypeDef * pSpeed = NULL;
	
	if(htim == (&TIM2_Handler))
	{
		pSpeed = pSpeedGloble[0];
		Decision_TIM2(htim,pSpeed);
	}
	else if(htim == (&TIM3_Handler))
	{
		pSpeed = pSpeedGloble[1];
		Decision_TIM3(htim,pSpeed);
	}
	else if(htim == (&TIM4_Handler))
	{
		pSpeed = pSpeedGloble[2];
		Decision_TIM4(htim,pSpeed);
	}
	else
		return;
}

/**********************
*@brief定时器比较中断
*@paramhtim：定时器句柄指针
*@note无
*@retval无
**********************/
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef*htim)
{
	//判断正在运行	
	motor_Decision(htim);	
}

//TIM2初始化   电机M04 通道1  M05-> 通道2  M07-> 通道3
void Motor_TIM2_Init(uint16_t psc,uint16_t arr)
{
	TIM_OC_InitTypeDef TIM_OCInitStructure;
	/*使能定时器4*/
	__TIM2_CLK_ENABLE();
	
	TIM2_Handler.Instance        = TIM2; 	                    //定时器4  
	TIM2_Handler.Init.Prescaler  = psc;                       	//定时器分频系数  	
	TIM2_Handler.Init.CounterMode= TIM_COUNTERMODE_UP;      	//向上计数模式  
	TIM2_Handler.Init.Period     = arr;                         //自动重装载值  
	TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1; 	
	TIM2_Handler.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&TIM2_Handler);
	
	/*PWM模式配置--这里配置为输出比较模式*/
	TIM_OCInitStructure.OCMode       = TIM_OCMODE_TOGGLE;
	/*比较输出的计数值*/
	TIM_OCInitStructure.Pulse        = 20;
	/*当定时器计数值小于CCR1_Val时为高电平*/
	TIM_OCInitStructure.OCPolarity   = TIM_OCPOLARITY_HIGH;
	/*设置互补通道输出的极性*/
	TIM_OCInitStructure.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	/*快速模式设置*/
	TIM_OCInitStructure.OCFastMode   = TIM_OCFAST_DISABLE;
	/*空闲电平*/
	TIM_OCInitStructure.OCIdleState  = TIM_OCIDLESTATE_RESET;
	/*互补通道设置*/
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	
	HAL_TIM_OC_ConfigChannel(&TIM2_Handler,&TIM_OCInitStructure,TIM_CHANNEL_1);
	
	HAL_TIM_Base_Stop(&TIM2_Handler);
	//设置抢占优先级，子优先级
	HAL_NVIC_SetPriority(TIM2_IRQn, 3, 0);
	// 设置中断来源
	HAL_NVIC_EnableIRQ(TIM2_IRQn);		
}

//TIM2初始化   电机M04 通道1  M05-> 通道2  M07-> 通道3
void Motor_TIM3_Init(uint16_t psc,uint16_t arr)
{
	TIM_OC_InitTypeDef TIM_OCInitStructure;
	/*使能定时器4*/
	__TIM3_CLK_ENABLE();
	
	TIM3_Handler.Instance           =  TIM3; 	                       	
	TIM3_Handler.Init.Prescaler     =  psc;                        	
	TIM3_Handler.Init.CounterMode   =  TIM_COUNTERMODE_UP;      
	TIM3_Handler.Init.Period        =  arr;                          
	TIM3_Handler.Init.ClockDivision =  TIM_CLOCKDIVISION_DIV1; 
	TIM3_Handler.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&TIM3_Handler);
	
	/*PWM模式配置--这里配置为输出比较模式*/
	TIM_OCInitStructure.OCMode       = TIM_OCMODE_TOGGLE;
	/*比较输出的计数值*/
	TIM_OCInitStructure.Pulse        = 20;
	/*当定时器计数值小于CCR1_Val时为高电平*/
	TIM_OCInitStructure.OCPolarity   = TIM_OCPOLARITY_HIGH;
	/*设置互补通道输出的极性*/
	TIM_OCInitStructure.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	/*快速模式设置*/
	TIM_OCInitStructure.OCFastMode   = TIM_OCFAST_DISABLE;
	/*空闲电平*/
	TIM_OCInitStructure.OCIdleState  = TIM_OCIDLESTATE_RESET;
	/*互补通道设置*/
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	
	HAL_TIM_OC_ConfigChannel(&TIM3_Handler,&TIM_OCInitStructure,TIM_CHANNEL_1);
	
	HAL_TIM_Base_Stop(&TIM3_Handler);
	//设置抢占优先级，子优先级
	HAL_NVIC_SetPriority(TIM3_IRQn, 3, 1);
	// 设置中断来源
	HAL_NVIC_EnableIRQ(TIM3_IRQn);		
}

/***************************
*@brief		电机相关初始化
*@param		无
*@return	无
***************************/
void Motor_Init(void)
{
	
#if IS_BOOTLOADER_PROGRAM
	BootMotorENConf();
#else
//	min_motor_init(); 
	fan_init();
	
	Motor_GPIO_Config();
	
	Motor_TIM2_Init(TIM_PERIOD,TIM_PRESCALER); 
	
	Motor_TIM4_Init(TIM_PERIOD,TIM_PRESCALER);
	
	Motor_TIM3_Init(TIM_PERIOD,TIM_PRESCALER);
#endif
	
}




