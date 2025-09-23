#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "bsp_pwm_conf.h"
#include "bsp_adc.h"
#include "bsp_pid.h"
#include "bsp_motor_conf.h"
#include "bsp_motor.h"
#include "utility.h"
#include "poct_process.h"
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "string.h"
#include "malloc.h"
#include "FreeRTOS.h"
#include "StartTask.h"
#include "pac9535.h"
#include "DIV_Control.h"
#include "main.h"
#include "mytimer.h"
#include "fuzzypid.h"

#if IS_BOOTLOADER_PROGRAM

#include "iap.h"
int main(void)
{		
	HAL_Init();                     //初始化HAL库
	
	HAL_NVIC_SetPriorityGrouping(0);

	Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	
    delay_init(180);                //初始化延时函数	
	
	LED_Init();                     //初始化LED 
	
	PAC9535_IIC_Init();             //初始化PAC9535
	
	motor_div_init();

	i2c_CheckDevice(0xA0);
	
	ee_ReadBytes(&Globle.BootLoader.bootLoaderFlag,EEPROM_BOOTLOAD_FLAG_ADDR,sizeof(BootLoaderTypeDef)); //读取EEPROM 参数
	
	if(Globle.BootLoader.bootLoaderFlag == 0)
	{
		iap_load_app(0x08010000);
	}
	
	Motor_Init();                   //电机初始化
		
	ALL_LED_OFF();                  //关闭LED
			
	//创建开始任务
    xTaskCreate((TaskFunction_t )bootloader_start_task,            //任务函数
                (const char*    )"bootloader_start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,                   //任务堆栈大小
                (void*          )NULL,                             //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,                  //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);              //任务句柄       
				
    vTaskStartScheduler();          //开启任务调度
}

#else 


TimerHandle_t TimerHandle = NULL;

int main(void)
{
	SCB->VTOR = FLASH_BASE | 0x10000;	

	HAL_Init();                     //初始化HAL库
	
	HAL_NVIC_SetPriorityGrouping(0);
	
	Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	
	TEMP_TIM_PWM_Init();            //初始化温控PWM	
	
	delay_init(180);                //初始化延时函数	

	USART_Config();
	
	PAC9535_IIC_Init();             //初始化PAC9535
	
	delay_ms(50);
	
	motor_div_init();               //设置电机初始细分

	LED_Init();                     //初始化LED 
	
	KEY_Init();                     //初始化按键
	
	i2c_CheckDevice(0xA0);
	
//	PT100_Init();                   //初始化AD采样
	
	PID_Init();                     //PID参数初始化
	
	ADS8688_GPIO_Init();
	
	MX_SPI4_Init();
		
	Motor_Init();                   
		
	ALL_LED_OFF();    
	
	ADS8688_Init(CH0_EN | CH1_EN | CH2_EN);
	
	Set_CH_Range(CHIR_0, ADS8688_IR_1_25V);
	
	Set_CH_Range(CHIR_1, ADS8688_IR_1_25V);
	
	Set_CH_Range(CHIR_2, ADS8688_IR_1_25V);
			
	ee_ReadBytes(&Globle.POCT_Par.rs1,EEPROM_PARA_ADDR,sizeof(POCT_EEPROMTypedef));
	
	Globle.BootLoader.bootLoaderFlag = 0;
	
	ee_WriteBytes(&Globle.BootLoader.bootLoaderFlag,EEPROM_BOOTLOAD_FLAG_ADDR,sizeof(BootLoaderTypeDef));
		
    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄  

	//创建软件定时器 周期为1秒			
	TimerHandle = xTimerCreate("Timer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, vTimerCallback);
				
    vTaskStartScheduler();                              //开启任务调度
}
#endif






