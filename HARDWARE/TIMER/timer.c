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


TIM_HandleTypeDef TIM10_Handler;	//TIM3��� 	

//FreeRTOSʱ��ͳ�����õĽ��ļ�����
volatile unsigned long long FreeRTOSRunTimeTicks;

//��ʼ��TIM3ʹ��ΪFreeRTOS��ʱ��ͳ���ṩʱ��
void ConfigureTimeForRunTimeStats(void)
{
	//��ʱ��3��ʼ������ʱ��ʱ��Ϊ90M����Ƶϵ��Ϊ90-1�����Զ�ʱ��3��Ƶ��
	//Ϊ90M/90=1M���Զ���װ��Ϊ50-1����ô��ʱ�����ھ���50us
	FreeRTOSRunTimeTicks = 0;
	TIM10_Init(50-1,90-1);	//��ʼ��TIM3
}

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!(��ʱ��3����APB1�ϣ�ʱ��ΪHCLK/2)
void TIM10_Init(u16 arr,u16 psc)
{  
    TIM10_Handler.Instance = TIM10;                          //ͨ�ö�ʱ��3
    TIM10_Handler.Init.Prescaler = psc;                     //��Ƶϵ��
    TIM10_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM10_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM10_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM10_Handler);    
    HAL_TIM_Base_Start_IT(&TIM10_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE   
}

//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM10)
	{
		__HAL_RCC_TIM10_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn,1,0);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�0
		HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);          //����ITM3�ж�   
	}	
}


