/**
  ******************************************************************************
  * @file    bsp_TimBase.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   TIM2 1ms ��ʱӦ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ISO-MINI STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "bsp_TiMbase.h" 
#include "w5500_conf.h"

TIM_HandleTypeDef TIM5_Handler;	//TIM5��� 	


/**
*@brief		��ʼ�����綨ʱ��TIM5
*@param		��
*@return	��
*/

void TIM5_Init(uint16_t arr,uint16_t psc)
{  
    TIM5_Handler.Instance = TIM5;                          //ͨ�ö�ʱ��3
    TIM5_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM5_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM5_Handler);    
    HAL_TIM_Base_Start_IT(&TIM5_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE   
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
////      LED1_TOGGLE;  //���������˸
//    }
//}



/*********************************************END OF FILE**********************/
