/**
  ******************************************************************************
  * @file    bsp_motor_conf.c
  * @author  hms
  * @version V1.0
  * @date    2021.11.11
  * @brief   ���ڵ������
  ******************************************************************************
  * @attention
  ******************************************************************************
  */
  
#include "bsp_motor_conf.h" 
#include "bsp_motor.h"
#include "FreeRTOS.h"
#include "stm32f4xx_it.h"

TIM_HandleTypeDef  TIM4_Handler; //��ʱ��4 PWM��������Ƶ��
TIM_HandleTypeDef  TIM1_Handler; //��ʱ��1 PWM��������Ƶ��
TIM_HandleTypeDef  TIM2_Handler; //��ʱ��2 PWM������ɲ�ʹ��
TIM_HandleTypeDef  TIM3_Handler; //��ʱ��2 PWM��������Ƶ��

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

//��ֹ������������յ�
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
		/*ѡ��Ҫ���Ƶ�GPIO����*/															   
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



//TIM4��ʼ��   ���M01 ͨ��2  M02-> ͨ��1  M03-> ͨ��3  M08-> ͨ��4
void Motor_TIM4_Init(uint16_t psc,uint16_t arr)
{
	TIM_OC_InitTypeDef TIM_OCInitStructure;
	/*ʹ�ܶ�ʱ��4*/
	__TIM4_CLK_ENABLE();
	
	TIM4_Handler.Instance           = TIM4; 
	TIM4_Handler.Init.Prescaler     = psc; 
	TIM4_Handler.Init.CounterMode   = TIM_COUNTERMODE_UP;
	TIM4_Handler.Init.Period        = arr;  
	TIM4_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; 
	TIM4_Handler.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&TIM4_Handler);
	
	/*PWMģʽ����--��������Ϊ����Ƚ�ģʽ*/
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE;
	/*�Ƚ�����ļ���ֵ*/
	TIM_OCInitStructure.Pulse = 0;
	/*����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ*/
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	/*���û���ͨ������ļ���*/
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_LOW;
	/*����ģʽ����*/
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
	/*���е�ƽ*/
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;
	/*����ͨ������*/
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	
	HAL_TIM_OC_ConfigChannel(&TIM4_Handler,&TIM_OCInitStructure,TIM_CHANNEL_1);

	/*ȷ����ʱ��*/
	HAL_TIM_Base_Stop(&TIM4_Handler);
	//HAL_TIM_Base_Start(&TIM4_Handler);	
	//������ռ���ȼ��������ȼ�
	HAL_NVIC_SetPriority(TIM4_IRQn, 2, 1);
	// �����ж���Դ
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


//�ٶȾ���
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
*@brief��ʱ���Ƚ��ж�
*@paramhtim����ʱ�����ָ��
*@note��
*@retval��
**********************/
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef*htim)
{
	//�ж���������	
	motor_Decision(htim);	
}

//TIM2��ʼ��   ���M04 ͨ��1  M05-> ͨ��2  M07-> ͨ��3
void Motor_TIM2_Init(uint16_t psc,uint16_t arr)
{
	TIM_OC_InitTypeDef TIM_OCInitStructure;
	/*ʹ�ܶ�ʱ��4*/
	__TIM2_CLK_ENABLE();
	
	TIM2_Handler.Instance=TIM2; 	                       //��ʱ��4  
	TIM2_Handler.Init.Prescaler=psc;                       //��ʱ����Ƶϵ��  	
	TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;      //���ϼ���ģʽ  
	TIM2_Handler.Init.Period=arr;                          //�Զ���װ��ֵ  
	TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1; 	
	TIM2_Handler.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&TIM2_Handler);
	
	/*PWMģʽ����--��������Ϊ����Ƚ�ģʽ*/
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE;
	/*�Ƚ�����ļ���ֵ*/
	TIM_OCInitStructure.Pulse=20;
	/*����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ*/
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	/*���û���ͨ������ļ���*/
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_LOW;
	/*����ģʽ����*/
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
	/*���е�ƽ*/
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;
	/*����ͨ������*/
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	
	HAL_TIM_OC_ConfigChannel(&TIM2_Handler,&TIM_OCInitStructure,TIM_CHANNEL_1);
	
	HAL_TIM_Base_Stop(&TIM2_Handler);
	//������ռ���ȼ��������ȼ�
	HAL_NVIC_SetPriority(TIM2_IRQn, 3, 0);
	// �����ж���Դ
	HAL_NVIC_EnableIRQ(TIM2_IRQn);		
}

//TIM2��ʼ��   ���M04 ͨ��1  M05-> ͨ��2  M07-> ͨ��3
void Motor_TIM3_Init(uint16_t psc,uint16_t arr)
{
	TIM_OC_InitTypeDef TIM_OCInitStructure;
	/*ʹ�ܶ�ʱ��4*/
	__TIM3_CLK_ENABLE();
	
	TIM3_Handler.Instance=TIM3; 	                       	
	TIM3_Handler.Init.Prescaler=psc;                        	
	TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;      
	TIM3_Handler.Init.Period=arr;                          
	TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1; 
	TIM3_Handler.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&TIM3_Handler);
	
	/*PWMģʽ����--��������Ϊ����Ƚ�ģʽ*/
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE;
	/*�Ƚ�����ļ���ֵ*/
	TIM_OCInitStructure.Pulse=20;
	/*����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ*/
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	/*���û���ͨ������ļ���*/
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_LOW;
	/*����ģʽ����*/
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
	/*���е�ƽ*/
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;
	/*����ͨ������*/
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	
	HAL_TIM_OC_ConfigChannel(&TIM3_Handler,&TIM_OCInitStructure,TIM_CHANNEL_1);
	
	HAL_TIM_Base_Stop(&TIM3_Handler);
	//������ռ���ȼ��������ȼ�
	HAL_NVIC_SetPriority(TIM3_IRQn, 3, 1);
	// �����ж���Դ
	HAL_NVIC_EnableIRQ(TIM3_IRQn);		
}

/***************************
*@brief		�����س�ʼ��
*@param		��
*@return	��
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




