#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"


void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();               //开启GPIOH时钟
    
    GPIO_Initure.Pin=GPIO_PIN_3;               	//PH3
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    //中断线3-PH3
    HAL_NVIC_SetPriority(EXTI3_IRQn,6,0);       //抢占优先级为6，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //使能中断线3
}

//任务句柄
extern TaskHandle_t Task2Task_Handler;

//中断服务函数
void EXTI3_IRQHandler(void)
{
	BaseType_t YieldRequired;
	
	delay_xms(50);						//消抖
	if(KEY0==0)
	{
		YieldRequired=xTaskResumeFromISR(Task2Task_Handler);//恢复任务2
		printf("恢复任务2的运行!\r\n");
		if(YieldRequired==pdTRUE)
		{
			/*如果函数xTaskResumeFromISR()返回值为pdTRUE，那么说明要恢复的这个
			任务的任务优先级等于或者高于正在运行的任务(被中断打断的任务),所以在
			退出中断的时候一定要进行上下文切换！*/
			portYIELD_FROM_ISR(YieldRequired);
		}
	}
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);	//清除中断标志位
}
