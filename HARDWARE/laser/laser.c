/**
  ******************************************************************************
  * @file    laser.c
  * @author  黄明送
  * @version V1.0
  * @date    2025.3.7
  * @brief   激光分液检测
  ******************************************************************************
  * @attention
  * 
  *
  ******************************************************************************
  */  

#include "laser.h"
#include "delay.h"

void LASER_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    LASER_LED_GPIO_CLK_ENABLE(); 
    LASER_PHOTODIODE_CLK_ENABLE();

    GPIO_Initure.Pin  = LASER_PHOTODIODE_PIN;            
    GPIO_Initure.Mode = GPIO_MODE_INPUT;                
    GPIO_Initure.Pull = GPIO_PULLDOWN;                  
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;                 
    HAL_GPIO_Init(LASER_PHOTODIODE_PORT,&GPIO_Initure);   

    GPIO_Initure.Pin = LASER_LED_GPIO_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(LASER_LED_GPIO_PORT,&GPIO_Initure);

    LASER_LED_OFF();
}

/**
 * @brief 激光光电二极管检测
 * @return 1-检测到激光被遮挡 0-未检测到遮挡
 */
//uint8_t LASER_PHOTODIDE_Scan(void)
//{
//    // 检测光电二极管状态
//    if (HAL_GPIO_ReadPin(LASER_PHOTODIODE_PORT, LASER_PHOTODIODE_PIN) == GPIO_PIN_RESET) 
//    {
//        // 持续检测一段时间确认状态
//        uint8_t retry = 3;  // 重试次数
//        while (retry--) 
//        {
//            delay_ms(5);
//            if (HAL_GPIO_ReadPin(LASER_PHOTODIODE_PORT, LASER_PHOTODIODE_PIN) == GPIO_PIN_SET) 
//            {
//                break;
//            }
//        }
//        // 确认被遮挡
//        if (retry == 0) 
//        {
//            return 1;
//        }
//    }
//    return 0;
//}
 u8 LASER_PHOTODIDE_Scan(void)
 {   
     if(HAL_GPIO_ReadPin(LASER_PHOTODIODE_PORT,LASER_PHOTODIODE_PIN) == 0)
     {
       while(HAL_GPIO_ReadPin(LASER_PHOTODIODE_PORT,LASER_PHOTODIODE_PIN) == 0)
       {
         delay_ms(10);
         if(HAL_GPIO_ReadPin(LASER_PHOTODIODE_PORT,LASER_PHOTODIODE_PIN) == 0)
			 return 1;
       }
     }
     else
     {
       return 0;   
     }
     return 0;
 }
