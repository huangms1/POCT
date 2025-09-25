/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  黄明送
  * @version V1.0
  * @date    2021.10.19
  * @brief   led驱动文件，
  ******************************************************************************
  * @attention
  * 
  *
  ******************************************************************************
  */  

#include "led.h"

#include "delay.h"

#define MAX_LED  8

void MyDelay(uint16_t num);

/**
 * @brief  Initializes the system state LEDs.
 *
 * This function initializes the GPIO pins for the POWER, EXTRACTION, PCR, ERROR, and END LEDs.
 * It sets the mode, pull-up/pull-down, and speed for each pin, and then writes a default state to each pin.
 *
 * @param  None
 * @retval None
 */
void sysStateLEDInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	// Enable the clock for each LED
	POWER_LED_CLK_ENABLE();
	EXTRACTION_LED_CLK_ENABLE();
	PCR_LED_CLK_ENABLE();
	ERROR_LED_CLK_ENABLE();
	END_LED_CLK_ENABLE();
	
	// Configure the GPIO pin for the PCR LED
	GPIO_Initure.Pin = POWER_LED_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(POWER_LED_PORT,&GPIO_Initure);
	HAL_GPIO_WritePin(POWER_LED_PORT,POWER_LED_PIN,GPIO_PIN_RESET);
	
	// Configure the GPIO pin for the EXTRACTION LED
	GPIO_Initure.Pin = EXTRACTION_LED_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(EXTRACTION_LED_PORT,&GPIO_Initure);
	HAL_GPIO_WritePin(EXTRACTION_LED_PORT,EXTRACTION_LED_PIN,GPIO_PIN_SET);
	
	// Configure the GPIO pin for the PCR LED
	GPIO_Initure.Pin = PCR_LED_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(PCR_LED_PORT,&GPIO_Initure);
	HAL_GPIO_WritePin(PCR_LED_PORT,PCR_LED_PIN,GPIO_PIN_SET);
	
	// Configure the GPIO pin for the ERROR LED
	GPIO_Initure.Pin = ERROR_LED_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(ERROR_LED_PORT,&GPIO_Initure);
	HAL_GPIO_WritePin(ERROR_LED_PORT,ERROR_LED_PIN,GPIO_PIN_SET);
	
	 // Configure the GPIO pin for the END LED
	GPIO_Initure.Pin = END_LED_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(END_LED_PORT,&GPIO_Initure);
	HAL_GPIO_WritePin(END_LED_PORT,END_LED_PIN,GPIO_PIN_SET);
}
			  
//初始化PB1为输出.并使能时钟	    
//LED IO初始化
/*
CLK	PB13	
SDI	PC3	
CS	PC2	
*/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    SYS_LED_CLK_ENABLE();                      //开启GPIOB时钟
	LED_SPI_CLK_ENABLE();
	LED_SPI_SDI_ENABLE();
	LED_SPI_CS_ENABLE();
	LED1_SPI_CS_ENABLE();
	
	//系统指示灯
    GPIO_Initure.Pin = SYS_LED_PIN;            //PA3
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;   //推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;           //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;      //高速
    HAL_GPIO_Init(SYS_LED_PORT,&GPIO_Initure);	
    	
	GPIO_Initure.Pin = LED_SPI_CLK_PIN; 
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;   //推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;           //上拉
	 GPIO_Initure.Speed = GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(LED_SPI_CLK_PORT,&GPIO_Initure);	
	
	GPIO_Initure.Pin=LED_SPI_SDI_PIN; 
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;   //推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;           //上拉
	 GPIO_Initure.Speed = GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(LED_SPI_SDI_PORT,&GPIO_Initure);
	
	GPIO_Initure.Pin = LED_SPI_CS_PIN; 
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;   //推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;           //上拉
	 GPIO_Initure.Speed = GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(LED_SPI_CS_PORT,&GPIO_Initure);
	
	GPIO_Initure.Pin = LED1_SPI_CS_PIN; 
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;   //推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;           //上拉
	 GPIO_Initure.Speed = GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(LED1_SPI_CS_PORT,&GPIO_Initure);
	
//	HAL_GPIO_WritePin(LED_SPI_CS_PORT,LED_SPI_CS_PIN,GPIO_PIN_SET);	//PB0置1，默认初始化后灯灭
	LED_SPI_CS = 1;
	LED1_SPI_CS = 1;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);	            //PB0置1，默认初始化后灯灭
	
	sysStateLEDInit();
	
	// JG_init();
}

void JG_init(void)
{
	GPIO_InitTypeDef GPIO_Initure;	
	
	JG_LED_CLK_ENABLE();
	
	GPIO_Initure.Pin   = JG_LED_PIN;

	GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;  

    GPIO_Initure.Pull  = GPIO_PULLUP;        

    GPIO_Initure.Speed = GPIO_SPEED_HIGH; 

	HAL_GPIO_Init(JG_LED_PORT,&GPIO_Initure);
	
	HAL_GPIO_WritePin(JG_LED_PORT,JG_LED_PIN,0);	
}


/**
 * @brief  Sends data to LEDs using SPI.
 *
 * This function sends data to LEDs using SPI. It takes two parameters:
 * - num: Specifies the LED number to which data is sent. If num is greater than 5,
 *         it sends data to LED1, otherwise it sends data to LED.
 * - data: Specifies the data to be sent to the LED.
 *
 * @param  num: The LED number to which data is sent.
 * @param  data: The data to be sent to the LED.
 * @retval None
 */
void LED_SPI_SendData(uint8_t num ,uint8_t data)
{
	uint8_t temp1 = 0,temp2 = 0;
	
	if(num > 5)
	{
		LED1_SPI_CS = 0;
		num = num - 6;
	}
	else
	{
		LED_SPI_CS = 0;
	}
	
	for(int i=0;i<3;i++)
	{
		temp1 = num&0x04;//按位与
		if(temp1)
		{
			LED_SPI_SDI = 1;
		}
		else 
		{
			LED_SPI_SDI =0;
		}		
		delay_us(4);
		LED_SPI_CLK = 1;
		delay_us(4);
		LED_SPI_CLK = 0;
		num<<=1; 
	}

	for(int i=0;i<8;i++)
	{
		temp2=data&0x80;//按位与
		if(temp2)
			LED_SPI_SDI = 1;
		else 
			LED_SPI_SDI = 0;
		delay_us(4);
		LED_SPI_CLK = 1;
		delay_us(4);
		LED_SPI_CLK = 0;
		data<<=1; 		
	}	
	LED_SPI_CS = 1;
	LED1_SPI_CS = 1;
}

/**
 * @brief  Generates a delay based on the input number.
 *
 * This function generates a delay by repeatedly decrementing the input number until it reaches zero.
 *
 * @param  num: The number of iterations to delay.
 * @retval None
 */
void MyDelay(uint16_t num)
{
	while(1)
	{
		if(num == 0)
			break;
		num--;
	}
}

void ALL_LED_OFF(void)
{
    for (int i = 0; i < MAX_LED; i++)
    {
        LED_SPI_SendData(i, 0);
        delay_ms(5); 
    }
}

void LED_StateControl(void)
{
	EventBits_t ledVelue;
		
	if(ledEventGroupHandle != NULL)
	{
		ledVelue = xEventGroupGetBits(ledEventGroupHandle);
		
		if(ledVelue&EXTRACTION_LED_FLAG)
		{
			EXTRACTION_LED = 0; 
			POWER_LED = 1;			
		}else
		{
			EXTRACTION_LED = 1;
		}
		
		if(ledVelue&PCR_LED_FLAG)
		{
			PCR_LED = 0;
			POWER_LED = 1;
		}
		else
		{
			PCR_LED = 1;
		}
		
		if(ledVelue&ERROR_LED_FLAG)
		{
			ERROR_LED = 0;
			PCR_LED = 1;
			EXTRACTION_LED = 1;
		}
		else
		{
			ERROR_LED = 1;			
		}
		
		if(ledVelue&END_LED_FLAG)
		{
			END_LED = 0;
			POWER_LED = 1;
		}
		else
		{
			END_LED = 1;
		}
	}	
}

/**
 * @brief  Sets a specific bit in the event group for LED control.
 *
 * This function takes a bit value as input and sets the corresponding bit in the event group for LED control.
 * Based on the input bit value, it performs specific actions such as setting or clearing bits in the event group.
 *
 * @param  led_bit: The bit value to be set in the event group.
 * @retval None
 */
void LED_EventGroupSetbit(uint16_t led_bit)
{
	switch (led_bit){
		
		case POWER_LED_FLAG:
			// Perform actions for POWER_LED_FLAG
			break;
		case EXTRACTION_LED_FLAG:
			xEventGroupSetBits(ledEventGroupHandle,EXTRACTION_LED_FLAG); //指示灯
			xEventGroupClearBits(ledEventGroupHandle,END_LED_FLAG);
			break;
		
		case PCR_LED_FLAG:
			xEventGroupSetBits(ledEventGroupHandle,PCR_LED_FLAG);
			xEventGroupClearBits(ledEventGroupHandle,EXTRACTION_LED_FLAG);
			xEventGroupClearBits(ledEventGroupHandle,END_LED_FLAG);
			break;
		
		case ERROR_LED_FLAG:
			// Perform actions for ERROR_LED_FLAG
			break;
		case END_LED_FLAG:
			
			break;
		
		default: break;
		}
}




