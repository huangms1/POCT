/**
  ******************************************************************************
  * @file    bsp_pid_conf.c
  * @author  黄明送
  * @version V1.0
  * @date    2021-11-16
  * @brief   POCT，扩增温度控制。
  ******************************************************************************
  * @attention
  *
  * 实验平台:臻惠医疗分子诊断仪器。
  *
  ******************************************************************************
  */ 
  
  
#include "bsp_pwm_conf.h"
#include "InstrumentInclude.h"

TIM_HandleTypeDef TIM8_Handler;     

TIM_OC_InitTypeDef TIM8_CH1Handler; 
TIM_OC_InitTypeDef TIM8_CH3Handler; 
TIM_OC_InitTypeDef TIM8_CH4Handler; 

TIM_HandleTypeDef TIM9_Handler;     
TIM_OC_InitTypeDef TIM9_CH1Handler; 
TIM_OC_InitTypeDef TIM9_CH2Handler;


//定时器底层驱动，时钟使能，引脚配置 
//此函数会被HAL_TIM_PWM_Init()调用 
//htim:定时器句柄 
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) 
{
	int i = 0;
	GPIO_InitTypeDef GPIO_Initure;
	
	// 通用GPIO配置数组
    const struct {
        uint16_t pin;
        GPIO_TypeDef* port;
        uint32_t alternate;
    } gpio_config[] = {
        {TEMP_PWM_GPIO_PIN1, TEMP_PWM_GPIO_PORT1, GPIO_AF3_TIM8},
        {TEMP_PWM_GPIO_PIN2, TEMP_PWM_GPIO_PORT2, GPIO_AF3_TIM8},
        {TEMP_PWM_GPIO_PIN3, TEMP_PWM_GPIO_PORT3, GPIO_AF3_TIM9}
    };
		
    if(htim == NULL) return;
      
    // 启用时钟
    TEMP_PWM_TIM_CLK1_ENABLE;
    TEMP_PWM_TIM_CLK3_ENABLE;
    TEMP_PWM_GPIO_CLK1_ENABLE();
    TEMP_PWM_GPIO_CLK2_ENABLE();
    TEMP_PWM_GPIO_CLK3_ENABLE();
				
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;

    // 配置通用GPIO
    for(i = 0; i < sizeof(gpio_config)/sizeof(gpio_config[0]); i++) {
        GPIO_Initure.Pin = gpio_config[i].pin;
        GPIO_Initure.Alternate = gpio_config[i].alternate;
        HAL_GPIO_Init(gpio_config[i].port, &GPIO_Initure);
    }

    // 特殊配置
#ifdef POCT_ANM_H1L4_A
    GPIO_Initure.Pin = TEMP_PWM_GPIO_PIN4;
    GPIO_Initure.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(TEMP_PWM_GPIO_PORT4, &GPIO_Initure);
#elif defined POCT_ANM_H3L5_A
    GPIO_Initure.Pin = TEMP_PWM_GPIO_PIN4;
    GPIO_Initure.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(TEMP_PWM_GPIO_PORT4, &GPIO_Initure);
    
    HOT5_TIM_CLK_ENABLE;
    HOT5_GPIO_CLK_ENABLE();
    
    GPIO_Initure.Pin = HOT5_GPIO_PIN;
    GPIO_Initure.Alternate = GPIO_AF3_TIM9;
    HAL_GPIO_Init(HOT5_GPIO_PORT, &GPIO_Initure);
#elif defined POCT_HMN_H5L5_A
    HOT5_TIM_CLK_ENABLE;
    HOT5_GPIO_CLK_ENABLE();
    
    GPIO_Initure.Pin = HOT5_GPIO_PIN;
    GPIO_Initure.Alternate = GPIO_AF3_TIM9;
    HAL_GPIO_Init(HOT5_GPIO_PORT, &GPIO_Initure);
#endif
} 


//PWM (也就是TIM8初始化) 
//PWM输出初始化 
//arr：自动重装值 
//psc：时钟预分频数 
void TIM8_PWM_Init(uint16_t arr,uint16_t psc) 
{
	TIM8_Handler.Instance = TIM8;                     //定时器9  
	TIM8_Handler.Init.Prescaler   = psc;              //定时器分频系数  
	TIM8_Handler.Init.CounterMode =TIM_COUNTERMODE_UP;//向上计数模式  
	TIM8_Handler.Init.Period=arr;                     //自动重装载值  
	TIM8_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;  
	HAL_TIM_PWM_Init(&TIM8_Handler);                  //初始化PWM 
	
#ifdef POCT_ANM_H1L4_A
	TIM8_CH1Handler.OCMode = TIM_OCMODE_PWM2;        //模式选择PWM2  
	TIM8_CH1Handler.Pulse=0;                         //设置比较值用来确定占空比为50%  
	TIM8_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH;  //输出比较极性为高 
#endif	
	
#ifdef POCT_ANM_H3L5_A
	TIM8_CH1Handler.OCMode = TIM_OCMODE_PWM2;        //模式选择PWM2  
	TIM8_CH1Handler.Pulse=0;                         //设置比较值用来确定占空比为50%  
	TIM8_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH;  //输出比较极性为高 
#endif	
	
	TIM8_CH3Handler.OCMode = TIM_OCMODE_PWM2;        //模式选择PWM2  
	TIM8_CH3Handler.Pulse  = 0;                         //设置比较值用来确定占空比为50%  
	TIM8_CH3Handler.OCPolarity=TIM_OCPOLARITY_HIGH;  //输出比较极性为高 	
	
	TIM8_CH4Handler.OCMode = TIM_OCMODE_PWM2;        //模式选择PWM2  
	TIM8_CH4Handler.Pulse = 0;                         //设置比较值用来确定占空比为50%  
	TIM8_CH4Handler.OCPolarity=TIM_OCPOLARITY_HIGH;  //输出比较极性为高 
	
#ifdef POCT_ANM_H1L4_A
	HAL_TIM_PWM_ConfigChannel(&TIM8_Handler,&TIM8_CH1Handler, TIM_CHANNEL_1);//配置TIM8通道3	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_1);                          //开启PWM通道2
#endif	
	
#ifdef POCT_ANM_H3L5_A
	HAL_TIM_PWM_ConfigChannel(&TIM8_Handler,&TIM8_CH1Handler, TIM_CHANNEL_1);//配置TIM8通道3	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_1);                          //开启PWM通道2
#endif	

	HAL_TIM_PWM_ConfigChannel(&TIM8_Handler,&TIM8_CH3Handler, TIM_CHANNEL_3);//配置TIM8通道3	
	HAL_TIM_PWM_ConfigChannel(&TIM8_Handler,&TIM8_CH4Handler, TIM_CHANNEL_4);//配置TIM8通道4 
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_3);//开启PWM通道2
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_4);//开启PWM通道2
}

//PWM (也就是TIM9初始化) 
//PWM输出初始化 
//arr：自动重装值 
//psc：时钟预分频数 
void TIM9_PWM_Init(uint16_t arr,uint16_t psc) 
{
	TIM9_Handler.Instance = TIM9;                                            //定时器9  
	TIM9_Handler.Init.Prescaler=psc;                                         //定时器分频系数  
	TIM9_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;                        //向上计数模式  
	TIM9_Handler.Init.Period=arr;                                            //自动重装载值  
	TIM9_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;  
	HAL_TIM_PWM_Init(&TIM9_Handler);                                         //初始化PWM 
	
	TIM9_CH1Handler.OCMode = TIM_OCMODE_PWM2;                                  //模式选择PWM2  
	TIM9_CH1Handler.Pulse = 0;                                                 //设置比较值用来确定占空比为50%  
	TIM9_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH;                           //输出比较极性为高  	
	HAL_TIM_PWM_ConfigChannel(&TIM9_Handler,&TIM9_CH1Handler, TIM_CHANNEL_1);//配置TIM9通道1
	
	TIM9_CH2Handler.OCMode     = TIM_OCMODE_PWM2;                                  //模式选择PWM2  
	TIM9_CH2Handler.Pulse      = 0;                                                 //设置比较值用来确定占空比为50%  
	TIM9_CH2Handler.OCPolarity = TIM_OCPOLARITY_HIGH;                          //输出比较极性为高  
	HAL_TIM_PWM_ConfigChannel(&TIM9_Handler,&TIM9_CH2Handler, TIM_CHANNEL_2);//配置TIM9通道2
	
	HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_1);                          //开启PWM通道2
	
	HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_2);                          //开启PWM通道2			
}

//设置TIM通道的占空比 
//TIM_TypeDef:定时器 (1~14)
//compare:比较值 
//ch:通道号 
void TIM_SetTIMCompare(TIM_TypeDef *TIM,uint32_t compare,uint8_t ch)
{	
	if (ch == 0)
	{
	  TIM->CCR1 = compare;
	}
	if (ch == 1)
	{
	  TIM->CCR2 = compare;
	}
	if (ch == 2)
	{
	  TIM->CCR3 = compare;	
	}
	if (ch == 3)
	{
	  TIM->CCR4 = compare;	
	}
}

//初始化PWM 

void TEMP_TIM_PWM_Init(void)
{
//	HAL_TIM_PWM_MspInit();	
	TIM8_PWM_Init(999,260-1); //第一路和第二路温控。
	
	TIM9_PWM_Init(999,260-1); //第三路温控。	
}






