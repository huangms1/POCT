/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  黄明送
  * @version V1.0
  * @date    2021.10.19
  * @brief   按键处理
  ******************************************************************************
  * @attention
  * 
  *
  ******************************************************************************
  */  

#include "key.h"
//#include "FreeRTOS.h"
#include "delay.h"


//按键初始化函数
/*舱门按键  PD7*/
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOD_CLK_ENABLE();           //开启GPIOD时钟
  
    GPIO_Initure.Pin=GPIO_PIN_7;            //PD7
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
    
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(void)
{   
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==0)
	{
		while(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==0)//按键按下
		{
			delay_ms(10);
			if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)==0)return 1;
		}
	}
	else
	{
		 return 0;   //无按键按下
	}
   return 0;
}
