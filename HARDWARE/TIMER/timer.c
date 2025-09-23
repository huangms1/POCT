#include "timer.h"
#include "led.h"
#include "malloc.h"
#include "usart.h"
#include "string.h"
#include "bsp_TiMbase.h"
#include "w5500_conf.h"
#include "bsp_motor_conf.h"

//////////////////////////////////////////////////////////////////////////////////	 
								  
////////////////////////////////////////////////////////////////////////////////// 	


TIM_HandleTypeDef TIM10_Handler;	//TIM3句柄 	

//FreeRTOS时间统计所用的节拍计数器
volatile unsigned long long FreeRTOSRunTimeTicks;

//初始化TIM3使其为FreeRTOS的时间统计提供时基
void ConfigureTimeForRunTimeStats(void)
{
	//定时器3初始化，定时器时钟为90M，分频系数为90-1，所以定时器3的频率
	//为90M/90=1M，自动重装载为50-1，那么定时器周期就是50us
	FreeRTOSRunTimeTicks = 0;
	TIM10_Init(50-1,90-1);	//初始化TIM3
}

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM10_Init(u16 arr,u16 psc)
{  
    TIM10_Handler.Instance = TIM10;                          //通用定时器3
    TIM10_Handler.Init.Prescaler = psc;                     //分频系数
    TIM10_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM10_Handler.Init.Period=arr;                        //自动装载值
    TIM10_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM10_Handler);    
    HAL_TIM_Base_Start_IT(&TIM10_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE   
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM10)
	{
		__HAL_RCC_TIM10_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn,1,0);    //设置中断优先级，抢占优先级1，子优先级0
		HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);          //开启ITM3中断   
	}	
}


