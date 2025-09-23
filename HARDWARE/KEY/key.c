/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  ������
  * @version V1.0
  * @date    2021.10.19
  * @brief   ��������
  ******************************************************************************
  * @attention
  * 
  *
  ******************************************************************************
  */  

#include "key.h"
//#include "FreeRTOS.h"
#include "delay.h"


//������ʼ������
/*���Ű���  PD7*/
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOD_CLK_ENABLE();           //����GPIODʱ��
  
    GPIO_Initure.Pin=GPIO_PIN_7;            //PD7
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
    
}

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(void)
{   
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==0)
	{
		while(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==0)//��������
		{
			delay_ms(10);
			if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==0)return 1;
		}
	}
	else
	{
		 return 0;   //�ް�������
	}
   return 0;
}
