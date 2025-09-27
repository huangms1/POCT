/**
  ******************************************************************************
  * @file    bsp_TimBase.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   TIM2 1ms 定时应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 ISO-MINI STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "bsp_TiMbase.h" 
#include "w5500_conf.h"

TIM_HandleTypeDef TIM5_Handler;	//TIM5句柄 	


/**
*@brief		初始化网络定时器TIM5
*@param		无
*@return	无
*/

void TIM5_Init(uint16_t arr,uint16_t psc)
{  
    TIM5_Handler.Instance = TIM5;                          //通用定时器3
    TIM5_Handler.Init.Prescaler=psc;                     //分频系数
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM5_Handler.Init.Period=arr;                        //自动装载值
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM5_Handler);    
    HAL_TIM_Base_Start_IT(&TIM5_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE   
}



/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */	
void  BASIC_TIM_IRQHandler (void)
{
	HAL_TIM_IRQHandler(&TIM5_Handler);	 	
}




//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//    if(htim==(&TIM5_Handler))
//    {
////      LED1_TOGGLE;  //红灯周期闪烁
//    }
//}



/*********************************************END OF FILE**********************/
