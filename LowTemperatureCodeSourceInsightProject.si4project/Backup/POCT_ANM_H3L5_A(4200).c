
/*************************************头文件****************************************/
#include "InstrumentInclude.h"
#include "FreeRTOS.h"
#include "bsp_motor.h"
#include "utility.h"
#include "w5500_conf.h"
#include "bsp_motor_conf.h"
#include "led.h"
#include "Reply.h"
#include "TempControlTask.h"
#include "delay.h"
#include "poct_process.h"


#ifdef POCT_ANM_H3L5_A

/*************************************宏定义****************************************/
#define PUMP_IN     0

#define PUMP_OUT    1

#define DISPLAY_TEMP0   0       //显示57度

#define DISPLAY_TEMP1   2


#define	TEMP2_SEND  0 //温度2 97

#define	TIME2_SEND  1 //时间2

#define	TEMP3_SEND  2 //温度3 57

#define	TIME3_SEND  3 //时间3

/*************************************函数声明****************************************/
void MagneticSet_UD(uint16_t step,uint16_t fre,uint8_t times,uint8_t WaitTime);


/*************************************函数****************************************/
Motor_Conf_TypeDef Motor_Conf[MOTOR_CONF_NUM] = {
	{M01_PUL_PIN,M01_DIR_GPIO_PIN,M01_EN_GPIO_PIN,M01_GO_GPIO_PIN,\
	M01_PUL_PORT,M01_DIR_GPIO_PORT,M01_EN_GPIO_PORT,M01_GO_GPIO_PORT},\
	
	{M02_PUL_PIN,M02_DIR_GPIO_PIN,M02_EN_GPIO_PIN,M02_GO_GPIO_PIN,\
	M02_PUL_PORT,M02_DIR_GPIO_PORT,M02_EN_GPIO_PORT,M02_GO_GPIO_PORT},\
	
	{M03_PUL_PIN,M03_DIR_GPIO_PIN,M03_EN_GPIO_PIN,M03_GO_GPIO_PIN,\
	M03_PUL_PORT,M03_DIR_GPIO_PORT,M03_EN_GPIO_PORT,M03_GO_GPIO_PORT},\
	
	{M04_PUL_PIN,M04_DIR_GPIO_PIN,M04_EN_GPIO_PIN,M04_GO_GPIO_PIN,\
	M04_PUL_PORT,M04_DIR_GPIO_PORT,M04_EN_GPIO_PORT,M04_GO_GPIO_PORT},\
	
	{M05_PUL_PIN,M05_DIR_GPIO_PIN,M05_EN_GPIO_PIN,M05_GO_GPIO_PIN,\
	M05_PUL_PORT,M05_DIR_GPIO_PORT,M05_EN_GPIO_PORT,M05_GO_GPIO_PORT},\
	
	{M06_PUL_PIN,M06_DIR_GPIO_PIN,M06_EN_GPIO_PIN,M06_GO_GPIO_PIN,\
	M06_PUL_PORT,M06_DIR_GPIO_PORT,M06_EN_GPIO_PORT,M06_GO_GPIO_PORT},\
	
	{M07_PUL_PIN,M07_DIR_GPIO_PIN,M07_EN_GPIO_PIN,M07_GO_GPIO_PIN,\
	M07_PUL_PORT,M07_DIR_GPIO_PORT,M07_EN_GPIO_PORT,M07_GO_GPIO_PORT},\
	
	{M08_PUL_PIN,M08_DIR_GPIO_PIN,M08_EN_GPIO_PIN,M08_GO_GPIO_PIN,\
	M08_PUL_PORT,M08_DIR_GPIO_PORT,M08_EN_GPIO_PORT,M08_GO_GPIO_PORT},\
	
	{M09_PUL_PIN,M09_DIR_GPIO_PIN,M09_EN_GPIO_PIN,M09_GO_GPIO_PIN,\
	M09_PUL_PORT,M09_DIR_GPIO_PORT,M09_EN_GPIO_PORT,M09_GO_GPIO_PORT},\
	
	{M10_PUL_PIN,M10_DIR_GPIO_PIN,M10_EN_GPIO_PIN,M10_GO_GPIO_PIN,\
	M10_PUL_PORT,M10_DIR_GPIO_PORT,M10_EN_GPIO_PORT,M10_GO_GPIO_PORT},\
	
	{M11_PUL_PIN,M11_DIR_GPIO_PIN,M11_EN_GPIO_PIN,M11_GO_GPIO_PIN,\
	M11_PUL_PORT,M11_DIR_GPIO_PORT,M11_EN_GPIO_PORT,M11_GO_GPIO_PORT},\
	
	{M12_PUL_PIN,M12_DIR_GPIO_PIN,M12_EN_GPIO_PIN,M12_GO_GPIO_PIN,\
	M12_PUL_PORT,M12_DIR_GPIO_PORT,M12_EN_GPIO_PORT,M12_GO_GPIO_PORT}
};


/*********************************
  * @brief  
  * @param  
  * @retval 
  ***********************************/
void Motor_GPIO_Config(void)
{
	uint8_t i = 0;
	
	GPIO_InitTypeDef GPIO_InitStruct;
  
	uint8_t member_count = MOTOR_CONF_NUM;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	for(i = 0; i < member_count; i++)
	{															   
		GPIO_InitStruct.Pin = Motor_Conf[i].dir_pin;	
		GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
		GPIO_InitStruct.Pull =GPIO_PULLUP;  
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(Motor_Conf[i].dir_port, &GPIO_InitStruct);
		  
		GPIO_InitStruct.Pin = Motor_Conf[i].en_pin;
		HAL_GPIO_Init(Motor_Conf[i].en_port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(Motor_Conf[i].en_port,Motor_Conf[i].en_pin,HIGH1);
		  
		GPIO_InitStruct.Pin  = Motor_Conf[i].go_pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;                   
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;             
		HAL_GPIO_Init(Motor_Conf[i].go_port, &GPIO_InitStruct); 

		GPIO_InitStruct.Pin = Motor_Conf[i].pul_pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;                  
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;             
		HAL_GPIO_Init(Motor_Conf[i].pul_port,&GPIO_InitStruct);	  
	}
}

/***************************
*@ 	T1加热	
****************************/
void T1_Heating(SMsg * pMsg)
{	
//	int i = 0;
	
	uint16_t temp_up_cnt = 0 , countdown = 0,mixTime = 0;
	
	uint32_t TimeNowCnt = 0;
	
	uint16_t SendT1buf[2] = {0};
	
	memset(SendT1buf,0,sizeof(SendT1buf));
		
	Globle.TempControl.Temp_Ch1_ENABLE  = SET;
	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_3);
	
	while(1)
	{
		temp_up_cnt++;
		
		if(temp_up_cnt >= 100)
		{
			SendT1buf[0] = GetTemp(0);
			
			Send_T1(pMsg,SendT1buf);
			
			temp_up_cnt = 0;
		}
		
		vTaskDelay(10);
			
		if(SendT1buf[0] >= Globle.TempControl.Temp_Ch1_Set - 10)
		{			
			TimeNowCnt = HAL_GetTick();

			mixTime = Globle.TempControl.Temp_T1;
			
			while(TimeNowCnt + mixTime*1000 > HAL_GetTick())
			{
				if(temp_up_cnt >= 100)
				{
					SendT1buf[0] = GetTemp(0);
												
					countdown++;
					
					SendT1buf[1] = countdown;
									
					Send_T1(pMsg,SendT1buf);
					
					temp_up_cnt = 0;
				}					
				vTaskDelay(10);
						
				temp_up_cnt++;				
			}

			break;
		}
	}	
}


/************
*@T4 加热
*************/
void T4_Heating(SMsg * pMsg)
{	
	uint16_t temp_up_cnt = 0 , countdown = 0;
	
	uint32_t TimeNowCnt = 0;
	
	uint16_t SendT4buf[2] = {0};
	
	memset(SendT4buf,0,sizeof(SendT4buf));
		
	Globle.TempControl.Temp_Ch4_ENABLE  = SET;
	
	Get_Para_Num(pMsg->m_pAppMsg->m_pAppLayerSubPara,2);
	
	Globle.TempControl.Temp_Ch4_Set = Globle.SavaParaNum.Para[0];
	
	Globle.TempControl.Temp_T4 = Globle.SavaParaNum.Para[1];
			
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_1);
	
	while(1)
	{		
		temp_up_cnt++;
		if(temp_up_cnt >= 100)
		{			
			SendT4buf[0] = GetTemp(3); 
			
			Send_T1(pMsg,SendT4buf);
			
			temp_up_cnt = 0;
		}
		
		vTaskDelay(10);
				
		if(SendT4buf[0] >= Globle.TempControl.Temp_Ch4_Set - 10)
		{
			
			TimeNowCnt = HAL_GetTick();
		
			while(TimeNowCnt + Globle.TempControl.Temp_T4*1000 > HAL_GetTick())
			{
				if(temp_up_cnt >= 100)
				{
					SendT4buf[0] = GetTemp(3); 
												
					countdown++;
					
					SendT4buf[1] = countdown;
									
					Send_T1(pMsg,SendT4buf);
					
					temp_up_cnt = 0;
				}
											
				vTaskDelay(10);	
				
				temp_up_cnt++;				
			}
			break;
		}
	}		
}

/*****************************************
//热盖
*****************************************/
void hotCap(SMsg * pMsg,uint16_t Temp)
{
	uint16_t temp_up_cnt = 0;
	
	uint16_t SendT2T3buf[4] = {0};
	
	uint32_t err_cnt = 0;
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
	
	Globle.TempControl.Temp_Ch5_ENABLE  = SET;
	
	Globle.TempControl.Temp_Set_Ch5 = Temp;
			
	HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_2);
	
	while(GetTemp(4) >= (Temp+10))
	{
		
		err_cnt ++;
		
		temp_up_cnt++;
		
		if(temp_up_cnt >= 100)
		{		
			SendT2T3buf[DISPLAY_TEMP0] = GetTemp(1); 
					
			if((SendT2T3buf[DISPLAY_TEMP0] > Globle.TempControl.Temp_Ch2_Set - 5)&&(SendT2T3buf[DISPLAY_TEMP0] < Globle.TempControl.Temp_Ch2_Set + 5))
			{
				SendT2T3buf[DISPLAY_TEMP0] =  Globle.TempControl.Temp_Ch2_Set;
			}
			
			SendT2T3buf[DISPLAY_TEMP1] = GetTemp(2); 
			
			if((SendT2T3buf[DISPLAY_TEMP1] > Globle.TempControl.Temp_Ch3_Set - 5)&&(SendT2T3buf[DISPLAY_TEMP1] < Globle.TempControl.Temp_Ch3_Set + 5))
			{
				SendT2T3buf[DISPLAY_TEMP1] =  Globle.TempControl.Temp_Ch3_Set;
			}
			
			Send_T2T3(pMsg,SendT2T3buf);
			temp_up_cnt = 0;
		}
		
		if(err_cnt > 8*60*100)
		{
			SaveFaultCode("HOTCAP_TIMEOUT");//
		}
		
		delay_ms(10);
	}
}


/****************************************************
*@阀旋转到孔位
*
*
****************************************************/
void Fa_To_Hole(uint8_t hole_num)
{
	if(hole_num > 5)
		return;

	MotorFre_Abs_Run(Globle.POCT_Par.M11_Pos[hole_num],M11,M11_DIR,400);	
}

/****************************************************
*@函数名：PUMP_Func 
*@功能说明：泵运动，将液体吸推。
*@参数：step:泵运动的行程，dir:运动方向，u16Fre:泵运动的速度。
*@返回值：无
****************************************************/
void PUMP_Func(uint32_t step,uint8_t dir,uint16_t u16Fre)
{
	SpeedCalc_TypeDef * pSpeed = NULL;
	uint16_t fre = 0;
	fre = Motor_Fre(step);
	pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef));
	
	if (fre >  u16Fre)
		fre = u16Fre;
	
	if (dir == PUMP_IN)
	{	
		if (M07_DIR == CCW)
			Stepper_Move_S(MIN_FRE,fre,0.01f,step,M07,CCW,pSpeed);
		else 
			Stepper_Move_S(MIN_FRE,fre,0.01f,step,M07,CW,pSpeed);
		Stepper[M07].position = Stepper[M07].position - step;
	}
	else
	{
		if (M07_DIR == CCW)
			Stepper_Move_S(MIN_FRE,fre,0.01f,step,M07,CW,pSpeed);
		else 
			Stepper_Move_S(MIN_FRE,fre,0.01f,step,M07,CCW,pSpeed);
		Stepper[M07].position = Stepper[M07].position + step;
	}	
	while (Stepper[M07].status != STOP){vTaskDelay(1);}
	
	vPortFree (pSpeed);
}

/***************************************
  * @brief  
  * @param 
  * @param 
  * @retval
  ************************************/
void MagneticBead_FB(uint8_t hole)
{
	uint16_t fre = 0;	
	
	uint32_t x_step = 0,y_step = 0,X_offset = 0,Y_offset0 = 0,Y_offset1 = 0;
	
	X_offset = (Globle.POCT_Par.M05_X_P[1] - Globle.POCT_Par.M05_X_P[0])/2;
	
	Y_offset0 = (Globle.POCT_Par.M04_Y_P[1] - Globle.POCT_Par.M04_Y_P[0])/2;
	
	Y_offset1 = Globle.POCT_Par.M04_Y_P[2] - Globle.POCT_Par.M04_Y_P[0];
	//计算孔位
	
	if(hole == 1)
	{
		x_step = Globle.POCT_Par.M05_X_P[0];
		y_step = Globle.POCT_Par.M04_Y_P[0];
	}
	else
	{
		if(hole < 4)
		{
			x_step = X_offset*((hole%4)-1) + Globle.POCT_Par.M05_X_P[0];
			y_step = Y_offset0*((hole%4)-1) + Globle.POCT_Par.M04_Y_P[0];
		}
		else
		{
			x_step = X_offset*(hole%4) + Globle.POCT_Par.M05_X_P[0];
			y_step = Y_offset0*((hole%4)-1) + Globle.POCT_Par.M04_Y_P[0] + Y_offset1;
		}
	}
	
	fre = Motor_Fre(x_step);
	
	MotorFre_Abs_Run(x_step,M05,M05_DIR,fre);
	
	fre = Motor_Fre(y_step);
	
	MotorFre_Abs_Run(y_step,M04,M04_DIR,fre);
}

/**********************************************
* @brief 
* @param  
* @param  
* @retval 
***********************************************/
void MagneticSet_UD(uint16_t step,uint16_t fre,uint8_t times,uint8_t WaitTime)
{
	int i=0;
	
	MotorFre_Abs_Run(Globle.POCT_Par.M10_P1,M10,M10_DIR,3000);
	
	for(i=0;i<times;i++)
	{
		MotorFre_Abs_Run(Globle.POCT_Par.M10_P1,M10,M10_DIR,fre);
		
		Wait_Time(WaitTime);
		
		MotorFre_Abs_Run(Globle.POCT_Par.M10_P1 - step,M10,M10_DIR,fre);
	}
	Motor_Rst(M10_FRE,M10,M10_DIR,M10_GO_GPIO_PORT,M10_GO_GPIO_PIN,0);
	
}

void M09_M10_UP(uint32_t step,uint16_t fre)
{
	SpeedCalc_TypeDef * pSpeed_M09 = NULL;
	
	SpeedCalc_TypeDef * pSpeed_M10 = NULL;
	
	pSpeed_M09 = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef)+1);
	
	pSpeed_M10 = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef)+1);
	
	Stepper_Move_S(MIN_FRE,fre,0.01f,step,M09,M09_DIR,pSpeed_M09);
	
	Stepper_Move_S(MIN_FRE,fre,0.01f,step,M10,M10_DIR,pSpeed_M10);	
	
	while(M10_GO_STA == 1){delay_ms(1);}
	
	Motor_Stop(M09,pSpeed_M09);
	
	Motor_Stop(M10,pSpeed_M10);
	
	Stepper[M09].position = Stepper[M09].position - Globle.POCT_Par.M10_P1;
	
	Stepper[M10].position = 0;
	
	vPortFree(pSpeed_M09);
	
	vPortFree(pSpeed_M10);
}


void M09_M10_DOWN(uint32_t step,uint16_t fre)
{
	uint8_t dir = 0;
	
	SpeedCalc_TypeDef * pSpeed_M09 = NULL;
	
	SpeedCalc_TypeDef * pSpeed_M10 = NULL;
	
	pSpeed_M09 = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef)+1);
	
	pSpeed_M10 = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef)+1);
	
	dir = M09_DIR;

	if(dir == CCW)
	{
		Stepper_Move_S(MIN_FRE,fre,0.01f,step,M10,CW,pSpeed_M10);		
		
		Stepper_Move_S(MIN_FRE,fre,0.01f,step,M09,CW,pSpeed_M09);		
	}
	else
	{
		Stepper_Move_S(MIN_FRE,fre,0.01f,step,M10,CCW,pSpeed_M10);
		
		Stepper_Move_S(MIN_FRE,fre,0.01f,step,M09,CCW,pSpeed_M09);
	}
	
	while(Stepper[M10].status != STOP){delay_ms(1);}	
	
	Motor_Stop(M10,pSpeed_M10);
	
	Motor_Stop(M09,pSpeed_M09);
		
	vPortFree(pSpeed_M09);
	
	vPortFree(pSpeed_M10);
	
	Stepper[M09].position = Stepper[M09].position + step;	
	
	Stepper[M10].position = Stepper[M10].position + step;	
}


//
void led_scan(uint8_t led_channel)
{
	u8 num = 0;
	num = led_channel - 1;
	Motor_Abs_Run(Globle.POCT_Par.M08_LED_P[num],M08);
	LED_SPI_SendData(num ,(uint8_t)Globle.POCT_Par.LED[num]);
//	switch(led_channel)
//	{
//		case 1:                                              
//			Motor_Abs_Run(Globle.POCT_Par.M08_LED_P1,M08);
//			
//			LED_SPI_SendData(0 ,(uint8_t)Globle.POCT_Par.LED1);			
//			
//			break;
//		case 2:
//			Motor_Abs_Run(Globle.POCT_Par.M08_LED_P2,M08);
//			
//			LED_SPI_SendData(1 ,(uint8_t)Globle.POCT_Par.LED2);		
//			
//			break;
//		case 3:
//			
//			Motor_Abs_Run(Globle.POCT_Par.M08_LED_P3,M08);
//			
//			LED_SPI_SendData(2 ,(uint8_t)Globle.POCT_Par.LED3);
//			
//			break;
//		case 4:
//			Motor_Abs_Run(Globle.POCT_Par.M08_LED_P4,M08);
//			
//			LED_SPI_SendData(3 ,(uint8_t)Globle.POCT_Par.LED4);	
//			
//			break;
//		case 5:
//			
//			Motor_Abs_Run(Globle.POCT_Par.M08_LED_P5,M08);
//			
//			LED_SPI_SendData(4 ,(uint8_t)Globle.POCT_Par.LED5);
//			break;
//		default: break;
//	}
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
uint8_t LED_CAM_Scan(uint8_t cam_chanel,uint8_t led_chanel)
{	
	uint8_t faulCode = 0;	
	
	ALL_LED_OFF();
	
	if(cam_chanel == 0)
	{
		scan_module_reset();
	}
	else
	{
		Motor_Abs_Run(Globle.POCT_Par.M03_P[cam_chanel - 1],M03);           
		led_scan(led_chanel);
	}
	
	
//	switch(cam_chanel)
//	{		
//		case 0:
//		{
//			scan_module_reset();
//			break;
//		}
//		
//		case 1:                                                
//			Motor_Abs_Run(Globle.POCT_Par.M03_P1,M03);           
//			led_scan(led_chanel);
//			break;
//		
//		case 2:
//			Motor_Abs_Run(Globle.POCT_Par.M03_P2,M03);
//			led_scan(led_chanel);
//			break;
//		
//		case 3:
//			Motor_Abs_Run(Globle.POCT_Par.M03_P3,M03);
//			led_scan(led_chanel);
//			break;
//		
//		case 4:
//			Motor_Abs_Run(Globle.POCT_Par.M03_P4,M03);
//			led_scan(led_chanel);
//			break;
//		
//		case 5:
//			Motor_Abs_Run(Globle.POCT_Par.M03_P5,M03);
//			led_scan(led_chanel);
//			break;
//			
//		    default: 
//				break;
//	}
	delay_ms(10);
	
	return faulCode;	
}


void temp_filtering(SMsg * pMsg,uint16_t temp2_set,uint16_t temp3_set,uint16_t *SendT2T3buf)
{
//	uint16_t SendT2T3buf[4] = {0};
	if((SendT2T3buf[TEMP2_SEND] > temp2_set - 5)&&(SendT2T3buf[TEMP2_SEND] < temp2_set + 5))
	{
		SendT2T3buf[TEMP2_SEND] =  temp2_set;
	}
	else
	{
		SendT2T3buf[TEMP2_SEND] = GetTemp(1); 
	}
	
	if((SendT2T3buf[TEMP3_SEND] > temp3_set - 5)&&(SendT2T3buf[TEMP3_SEND] < temp3_set + 5))
	{
		SendT2T3buf[TEMP3_SEND] =  temp3_set;
	}
	else
	{
		SendT2T3buf[TEMP3_SEND] = GetTemp(2); 
	}
	
//	Send_T2T3(pMsg,SendT2T3buf);
	
}


/*******************************
  * @brief  dfsag =你说是
  * @param  
  * @param  
  * @retval 
  *****************************/
bool App_NZL(SMsg * pMsg,int * pFaultCode)
{
	uint32_t temp_up_cnt = 0;
	
	uint16_t SendT2T3buf[4] = {0}, countdown = 0,fre;
	
	uint32_t TimeNowCnt = 0;
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
	
	Motor_Rst(M01_FRE,M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN,0);
	
	Motor_Rst(M02_FRE,M02,M02_DIR,M02_GO_GPIO_PORT,M02_GO_GPIO_PIN,0);
	
	fre = Motor_Fre(Globle.POCT_Par.M02_P2);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M02_P2,M02,M02_DIR,fre);	
	
	fre = Motor_Fre(Globle.POCT_Par.M01_P1);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,fre);	
	
	hotCap(pMsg,HOTCAP_TEMP);
	
	while(1)
	{		
		temp_up_cnt++;		
		if(temp_up_cnt >= 100)
		{
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);
			
			Send_T2T3(pMsg,SendT2T3buf);
			
			temp_up_cnt = 0;
		}	
		
		vTaskDelay(10);				
		if(SendT2T3buf[TEMP3_SEND] >= Globle.TempControl.Temp_Ch3_Set - 2)
		{
		  TimeNowCnt  = HAL_GetTick();
			
			countdown = 0;	
			
			temp_up_cnt = 0;
				
			while(TimeNowCnt + Globle.TempControl.Temp_T3*1000 > HAL_GetTick())
			{						
				if(temp_up_cnt >= 100)
				{
					temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);
										
					countdown++;
					
					SendT2T3buf[TIME3_SEND] = countdown;
									
					Send_T2T3(pMsg,SendT2T3buf);

					temp_up_cnt = 0;
				}
											
				vTaskDelay(10);				
				temp_up_cnt++;
			}
			
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);
			countdown++;
				
			SendT2T3buf[TIME3_SEND] = countdown;
							
			Send_T2T3(pMsg,SendT2T3buf);	
			
			temp_up_cnt = 0;
			
			break;	
		}
	}
	return true;
}

/*******************************************
  * @brief  
  * @param  
  * @param  
  * @retval 
  *****************************************/
bool App_YBX(SMsg * pMsg,int * pFaultCode)
{
	uint32_t temp_up_cnt = 0;
	
	uint16_t fre;
	
	uint16_t SendT2T3buf[4] = {0}, countdown = 0;
	
	uint32_t TimeNowCnt = 0;
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
	
	motor_fast_rst(M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN);
	
	motor_fast_rst(M02,M02_DIR,M02_GO_GPIO_PORT,M02_GO_GPIO_PIN);
	
	fre = Motor_Fre(Globle.POCT_Par.M02_P1);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M02_P1,M02,M02_DIR,fre);
	
	fre = Motor_Fre(Globle.POCT_Par.M01_P1);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,fre);
	
	hotCap(pMsg,HOTCAP_TEMP);
	
	while(1)
	{		
		temp_up_cnt++;
		
		if(temp_up_cnt >= 100)
		{		
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);
			
			Send_T2T3(pMsg,SendT2T3buf);
			temp_up_cnt = 0;
		}
		
		delay_ms(10);				
		if(SendT2T3buf[TEMP2_SEND] >= Globle.TempControl.Temp_Ch2_Set - 2)
		{
		    TimeNowCnt  = HAL_GetTick();
			countdown = 0;	
			temp_up_cnt = 0;
				
			while(TimeNowCnt + Globle.TempControl.Temp_T2*1000 > HAL_GetTick())
			{							
				if(temp_up_cnt >= 100)
				{
					temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);
										
					countdown++;
					
					SendT2T3buf[TIME2_SEND] = countdown;
									
					Send_T2T3(pMsg,SendT2T3buf);
					
					temp_up_cnt = 0;
				}
											
				vTaskDelay(10);			
				temp_up_cnt++;
			}
			
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);
			
			countdown ++;
			
			SendT2T3buf[TIME2_SEND] = countdown;	
			
			Send_T2T3(pMsg,SendT2T3buf);
			
			temp_up_cnt = 0;
			
			break;			
		}		
	}
	return true;
}


/*********************************
  * @brief  PCR
  * @param  
  * @retval 
  ******************************/
void App_PCR(SMsg * pMsg)
{
	uint16_t fre = 0;
	
	uint32_t temp_up_cnt = 0;
	
	uint16_t SendT2T3buf[4] = {0}, countdown = 0;
	
	uint32_t TimeNowCnt = 0; 
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
	
	motor_fast_rst(M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN);
	
	motor_fast_rst(M02,M02_DIR,M02_GO_GPIO_PORT,M02_GO_GPIO_PIN);
	
	hotCap(pMsg,HOTCAP_TEMP);
	
	while(1)
	{		
		temp_up_cnt++;		
		if(temp_up_cnt >= 100)
		{
			SendT2T3buf[TEMP2_SEND] = GetTemp(1);
			
			SendT2T3buf[TEMP3_SEND] = GetTemp(2); 
						
			Send_T2T3(pMsg,SendT2T3buf);
			
			temp_up_cnt = 0;
			
		}
		
		vTaskDelay(10);	
		
		if(SendT2T3buf[TEMP2_SEND] >= Globle.TempControl.Temp_Ch2_Set - 10)
		{
			fre =  Motor_Fre(Globle.POCT_Par.M02_P1);
			
			MotorFre_Abs_Run(Globle.POCT_Par.M02_P1,M02,M02_DIR,fre);
			
			fre =  Motor_Fre(Globle.POCT_Par.M01_P1);
			
			MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,fre);
			
			TimeNowCnt  = HAL_GetTick();
			
			countdown = 0;
			
			while(TimeNowCnt + Globle.TempControl.Temp_T2 * 1000 > HAL_GetTick())
			{
				
				if(temp_up_cnt>=100)
				{
					temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);

					countdown++;		
					
					SendT2T3buf[TIME2_SEND] = countdown;
					
					Send_T2T3(pMsg,SendT2T3buf);
					
					temp_up_cnt = 0;		
				}			
				vTaskDelay(10);
				
				temp_up_cnt++;
			}
			countdown++;		
			
			temp_up_cnt = 0;
					
			SendT2T3buf[TIME2_SEND] = countdown;
					
			Send_T2T3(pMsg,SendT2T3buf);
								
			break;		
		}		
	}
	
	motor_fast_rst(M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN);
	
	while(1)
	{			
		temp_up_cnt++;
		
		if(temp_up_cnt > 100)
		{	
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);			
			Send_T2T3(pMsg,SendT2T3buf);
			temp_up_cnt = 0;
		}
		
		vTaskDelay(10);
		
		fre =  Motor_Fre(Globle.POCT_Par.M02_P2);
			
		MotorFre_Abs_Run(Globle.POCT_Par.M02_P2,M02,M02_DIR,fre);
		
		fre =  Motor_Fre(Globle.POCT_Par.M01_P1);
				
		MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,fre);
	
		if(SendT2T3buf[TEMP3_SEND] >= Globle.TempControl.Temp_Ch3_Set - 10)
		{	
					
		  TimeNowCnt  = HAL_GetTick();
			
			temp_up_cnt = 0;
			
			countdown = 0;
			
			while(TimeNowCnt + Globle.TempControl.Temp_T3*1000 > HAL_GetTick())
			{		
				if(temp_up_cnt >= 100)
				{				
					temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);
					
					countdown++;	
					
					SendT2T3buf[TIME3_SEND] = countdown;	
					
					Send_T2T3(pMsg,SendT2T3buf);
					
					temp_up_cnt=0;
				}
					
				vTaskDelay(10);		
				
				temp_up_cnt++;
				
			}
			
			countdown++;
															
			SendT2T3buf[TIME3_SEND] = countdown;
										
			Send_T2T3(pMsg,SendT2T3buf);
	
			break;			
		}		
	}
	
}

/****************************************************************
//裂解细胞
******************************************************************/
#include "StartTask.h"

void mix_time(uint16_t time)
{
	BaseType_t err = pdFALSE;
	
	xSemaphoreGive(Vibrate_SemaphoreHandle);//释放信号量
	
	Wait_Time(time);
	
	xSemaphoreGive(Vibrate_SemaphoreHandle);//释放信号量
	
	while(1)
	{		
		if(VibrateStop_SemaphoreHandle!=NULL)
		{
			err = xSemaphoreTake(VibrateStop_SemaphoreHandle,portMAX_DELAY);
			if(err==pdTRUE)
			{
				break;
			}
		}
		vTaskDelay(1);
	}
}


void Extraction_Tiqu(SMsg * pMsg)
{
	BaseType_t err = pdFALSE;
	
	MotorFre_Abs_Run(Globle.POCT_Par.M11_Pos[0],M11,M11_DIR,400);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,500);
	
	MagneticBead_FB(1);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M07_P1,M07,M07_DIR,800);	 
	
	MotorFre_Abs_Run(Globle.POCT_Par.M06_P1,M06,M06_DIR,2500); 
	
	xSemaphoreGive(Vibrate_SemaphoreHandle);//释放信号量
	
	T1_Heating(pMsg);
	
	xSemaphoreGive(Vibrate_SemaphoreHandle);//释放信号量
	
	while(1)
	{		
		if(VibrateStop_SemaphoreHandle != NULL)
		{

			err = xSemaphoreTake(VibrateStop_SemaphoreHandle,portMAX_DELAY);

			if(err==pdTRUE)
			{
				break;
			}
		}
		vTaskDelay(1);
	}
	
//	Motor_Rst(M06_FRE,M06,M06_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN,0);

	MotorFre_Abs_Run(Globle.POCT_Par.M09_P1,M09,M09_DIR,2500); 
	
	M09_M10_DOWN(Globle.POCT_Par.M10_P1/2,M09_M10_UP_FRE);
	
	delay_ms(50);
	
	M09_M10_DOWN(Globle.POCT_Par.M10_P1/2,M09_M10_DOWN_FRE);
	
	Wait_Time(10);
	
	M09_M10_UP(0xfffff,M09_M10_UP_FRE);
	
	PID_Stop(0);

	Globle.TempControl.Temp_Ch4_Set = 600;
			
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_1);

	Globle.TempControl.Temp_Ch4_ENABLE = SET;
	
}

/***********************
//第一次清洗
************************/
void Extraction_Clean1(void) 
{
	MagneticBead_FB(2);    

//	Motor_Rst(M09_FRE,M09,M09_DIR,M09_GO_GPIO_PORT,M09_GO_GPIO_PIN,0);		
	
	mix_time(50);
	
//	Wait_Time(4);
	
	Motor_Rst(M10_FRE,M10,M10_DIR,M10_GO_GPIO_PORT,M10_GO_GPIO_PIN,0);

	MotorFre_Abs_Run(Globle.POCT_Par.M09_P1,M09,M09_DIR,2500); 
	
	M09_M10_DOWN(Globle.POCT_Par.M10_P1/2,M09_M10_UP_FRE);	
	
	delay_ms(100);
	
	M09_M10_DOWN(Globle.POCT_Par.M10_P1/2,M09_M10_DOWN_FRE);
	
	Wait_Time(10);
	
	M09_M10_UP(0xfffff,M09_M10_UP_FRE);
	
}

/***********************
//第二次清洗
************************/
void Extraction_Clean2(SMsg * pMsg)
{
//	uint32_t size;
	ZStr2StrMap *pMap = ParseStr2Map(pMsg->m_pAppMsg->m_pAppLayerSubPara, TOP_SEPARATE_STRING, EQUAL_SEPARAT_STRING);
	
	char * pTime = GetValueFromStr2StrMap(pMap,"t1");
	
	uint16_t uTime = atoi(pTime);
	
	MagneticBead_FB(3); 

//	Motor_Rst(M09_FRE,M09,M09_DIR,M09_GO_GPIO_PORT,M09_GO_GPIO_PIN,0);	
	
	mix_time(50);

	Motor_Rst(M10_FRE,M10,M10_DIR,M10_GO_GPIO_PORT,M10_GO_GPIO_PIN,0);	
//	MagneticSet_UD(MIX_VL,MIX_SPEED,MIX_TIMES,0);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M09_P1,M09,M09_DIR,2500);
	
	M09_M10_DOWN(Globle.POCT_Par.M10_P1/2,M09_M10_UP_FRE);
	
	delay_ms(50);
	
	M09_M10_DOWN(Globle.POCT_Par.M10_P1/2,M09_M10_DOWN_FRE);
	
	Wait_Time(10);
	
	M09_M10_UP(0xfffff,M09_M10_UP_FRE);
	
	TimingDisplay(pMsg); 

}
	
/***********************
//
************************/
void Extraction_Elution(SMsg * pMsg)           
{
	BaseType_t err = pdFALSE;
	
	MotorFre_Abs_Run(Globle.POCT_Par.M06_P1,M06,M06_DIR,3000);
	
	MagneticBead_FB(6);                         
	
//	MagneticSet_UD(MIX_VL,MIX_SPEED,MIX_TIMES,0);
	
	Motor_Rst(M09_FRE,M09,M09_DIR,M09_GO_GPIO_PORT,M09_GO_GPIO_PIN,0);
	
	xSemaphoreGive(Vibrate_SemaphoreHandle);//释放信号量
	
	T4_Heating(pMsg); 

	xSemaphoreGive(Vibrate_SemaphoreHandle);//释放信号量	
	
	while(1)
	{		
		if(VibrateStop_SemaphoreHandle!=NULL)
		{
			err = xSemaphoreTake(VibrateStop_SemaphoreHandle,portMAX_DELAY);
			if(err==pdTRUE)
			{
				break;
			}
		}
		vTaskDelay(1);
	}
	
//	Wait_Time(5);
	
	Motor_Rst(M10_FRE,M10,M10_DIR,M10_GO_GPIO_PORT,M10_GO_GPIO_PIN,0);	
	
	MotorFre_Abs_Run(Globle.POCT_Par.M09_P1,M09,M09_DIR,2500);
	
	M09_M10_DOWN(Globle.POCT_Par.M10_P1/2,M09_M10_UP_FRE);
	
	delay_ms(50);
	
	M09_M10_DOWN(Globle.POCT_Par.M10_P1/2,M09_M10_DOWN_FRE);
	
	Wait_Time(10);
	
	M09_M10_UP(0xfffff,M09_M10_UP_FRE);
	
//	Motor_Rst(M06_FRE,M06,M06_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN,0);
	
	PID_Stop(3);
	
	//预升温  95摄氏度
    Globle.TempControl.Temp_Ch3_Set = 550;
	
	Globle.TempControl.Temp_Ch2_Set = 940;
	
	HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_1);
	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_4);
	
	Globle.TempControl.Temp_Ch3_ENABLE = SET;
	
	Globle.TempControl.Temp_Ch2_ENABLE = SET;
}

/************************************************
//
//
//
************************************************/
void Extraction_Solution_Apportion(SMsg * pMsg)
{
	uint32_t aTemp_V[3];
	
	uint32_t recrementVolume[3]; 
	
	uint32_t pumbInTotalV[2];
	
	int i = 0;
	
	char *pValue = NULL;
	
	ZStr2StrMap *pStr2StrMap = NULL;
	
	char * pKey = NULL;

	memset(aTemp_V,0,sizeof(aTemp_V));
	
	MotorFre_Abs_Run(Globle.POCT_Par.M11_Pos[0],M11,M11_DIR,500);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,500);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M07_P1,M07,M07_DIR,1500);
	
	delay_ms(100);
	
	if (Globle.POCT_Par.M07_PumpInTolal_V <= 8000)
	{
		pumbInTotalV[0] = 8000;
		
		pumbInTotalV[1] = 8000;
	}
	else
	{
		pumbInTotalV[0] = Globle.POCT_Par.M07_PumpInTolal_V;
		
		pumbInTotalV[1] = Globle.POCT_Par.M07_dg_mix_V;
	}
	
	PUMP_Func(pumbInTotalV[0],PUMP_IN,250);
	
	Wait_Time(2);
	
//	Wait_Time(120);

	Fa_To_Hole(1);	
	
    MotorFre_Abs_Run(Globle.POCT_Par.M07_P1,M07,M07_DIR,250);
	
	Wait_Time(2);	

	for (i = 0;i < 3;i++)
	{
		PUMP_Func(pumbInTotalV[1] + 5000,PUMP_IN,500);

		Wait_Time(1);

		PUMP_Func(pumbInTotalV[1] + 5000,PUMP_OUT,500);
	}
	Wait_Time(1);
	
	PUMP_Func(pumbInTotalV[1],PUMP_IN,250);
	
	Wait_Time(3);
	
	PUMP_Func(800,PUMP_OUT,500);           //推气泡 
	
	Wait_Time(1);
	
	if ((Globle.POCT_Par.Solution_Volume[0] > 5000) || (Globle.POCT_Par.Solution_Volume[1] > 5000)
		|| (Globle.POCT_Par.Solution_Volume[2] > 5000))
	{
		aTemp_V[0] = 2300;
		
		aTemp_V[1] = 2300;
		
		aTemp_V[2] = 2300;
	}
	else
	{
		aTemp_V[0] = Globle.POCT_Par.Solution_Volume[0];
		
		aTemp_V[1] = Globle.POCT_Par.Solution_Volume[1];
		
		aTemp_V[2] = Globle.POCT_Par.Solution_Volume[2];
	}
	
	recrementVolume[0] = 1100;
	
	recrementVolume[1] = 1100;
	
	recrementVolume[2] = 2000;
	
	pStr2StrMap = ParseStr2Map(pMsg->m_pAppMsg->m_pAppLayerSubPara,TOP_SEPARATE_STRING,EQUAL_SEPARAT_STRING);
	
	while(1)
	{
		pKey = GetNextKeyFromStr2StrMap(pStr2StrMap);
		
		if (pKey != NULL)
		{
			pValue = GetValueFromStr2StrMap(pStr2StrMap,pKey); 
			
			if (!strcmp(pValue,"1"))
			{
				int iIndex = atoi32(pKey + 1,10);
				
				Fa_To_Hole(iIndex + 1);
				
				PUMP_Func(aTemp_V[iIndex - 1],PUMP_OUT,1500); 
				Wait_Time(2);     
				PUMP_Func(recrementVolume[iIndex - 1],PUMP_IN,500);	
				
				Wait_Time(1);                          			
			}
		}
		else
		{
			break;
		}
	}
	
	MotorFre_Abs_Run(Globle.POCT_Par.M11_Pos[4] + 2200,M11,M11_DIR,400);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M11_Pos[4] + 500,M11,M11_DIR,400);
}

/***********************
//
************************/
void ALL_Motor_RESET(void)	
{
	int i = 0;	
	uint8_t Err = 0;
	
	for(i = 0;i < 12;i++)
	{
		memset(&Stepper[i], 0, sizeof(Stepper_Typedef));
	}
	
	Err = Motor_Rst(M01_FRE,M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN,0);
	if(Err != 0)
	{
		SaveFaultCode("F^M01_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M02_FRE,M02,M02_DIR,M02_GO_GPIO_PORT,M02_GO_GPIO_PIN,0);
	if(Err != 0)
	{
		SaveFaultCode("F^M02_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M03_FRE,M03,M03_DIR,M03_GO_GPIO_PORT,M03_GO_GPIO_PIN,1);
	if(Err != 0)
	{
		SaveFaultCode("F^M03_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M08_FRE,M08,M08_DIR,M08_GO_GPIO_PORT,M08_GO_GPIO_PIN,1);
	if(Err != 0)
	{
		SaveFaultCode("F^M08_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M06_FRE,M06,M05_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN,0);
	if(Err != 0)
	{
		SaveFaultCode("F^M06_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M07_FRE,M07,M07_DIR,M07_GO_GPIO_PORT,M07_GO_GPIO_PIN,0);
	if(Err != 0)
	{
		SaveFaultCode("F^M07_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M09_FRE,M09,M09_DIR,M09_GO_GPIO_PORT,M09_GO_GPIO_PIN,0);
	if(Err != 0)
	{
		SaveFaultCode("F^M09_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M10_FRE,M10,M10_DIR,M10_GO_GPIO_PORT,M10_GO_GPIO_PIN,0);
	if(Err != 0)
	{
		SaveFaultCode("F^M10_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M05_FRE,M05,M05_DIR,M05_GO_GPIO_PORT,M05_GO_GPIO_PIN,0);
	if(Err != 0)
	{
		SaveFaultCode("F^M05_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M04_FRE,M04,M04_DIR,M04_GO_GPIO_PORT,M04_GO_GPIO_PIN,0);
	if(Err != 0)
	{
		SaveFaultCode("F^M04_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0);
	if(Err != 0)
	{
		SaveFaultCode("F^M12_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M11_FRE,M11,M11_DIR,M11_GO_GPIO_PORT,M11_GO_GPIO_PIN,0);
	if(Err != 0)
	{
		SaveFaultCode("F^M11_TIMEOUT");
		return;
	}
}

/*************************
  * @brief  
  * @param 
  * @retval
  *************************/
void Motor_Move(SMsg * pMsg,uint16_t Rstfre,uint8_t motor_num,uint8_t dir,GPIO_TypeDef *Port,uint16_t Pin)
{
//	uint8_t Err = 0;
	uint32_t step = 0;	
	
	uint16_t fre = 0;
	
	SpeedCalc_TypeDef * pSpeed = NULL;
	
	pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef));
	
	step = PARA_Get_num(pMsg->m_pAppMsg->m_pAppLayerSubPara);
		
	fre = Motor_Fre(step);
	
	if((motor_num == M12)||(motor_num == M11)||(motor_num == M05)||(motor_num == M06)||(motor_num == M07)||(motor_num == M10))
	{
		if (fre > 500)
		{
			fre = 500;
		}
	}
	
	if(pMsg->m_pAppMsg->m_pAppLayerSubPara[1] == '+')                   //CW
	{		
		if(dir == CCW)			
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CW,pSpeed);
		else
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CCW,pSpeed);
		
		while(Stepper[motor_num].status != STOP){vTaskDelay(1);}
			Stepper[motor_num].position = step + Stepper[motor_num].position;
	}
	else if(pMsg->m_pAppMsg->m_pAppLayerSubPara[1] == '-')
	{			
		if(dir == CCW)
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CCW,pSpeed);
		else
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CW,pSpeed);
		
		while(Stepper[motor_num].status != STOP){vTaskDelay(1);}
		
		Stepper[motor_num].position = Stepper[motor_num].position - step;
	}
	else
	{
		if((motor_num == M12)||(motor_num == M11)||(motor_num == M05)||(motor_num == M06)||(motor_num == M07)||(motor_num == M10))
		{
			
		}
		else
		{
			if(Stepper[motor_num].position > 4000)
				MotorFre_Abs_Run(1500,motor_num,dir,4000);
		}
		Motor_Rst(500,motor_num,dir,Port,Pin,1);
	}	
	vPortFree(pSpeed);
}

void push_motor(void)
{
//	MotorFre_Abs_Run(Globle.POCT_Par.M05_P1,M05,M05_DIR,1000); 
}

uint8_t Door_Control(uint8_t sta)
{	
	return 0;
}


TaskHandle_t VibrateTask_Handler;

void VibrateTask(void *pData)
{
	uint16_t TimeOut = 0;
	
	BaseType_t err = pdFALSE;

	while(1)
	{
		
		if(Vibrate_SemaphoreHandle!=NULL)
		{
			err = xSemaphoreTake(Vibrate_SemaphoreHandle,portMAX_DELAY);	  //获取信号量
			if(err==pdTRUE)										              //获取信号量成功
			{
				MotorFre_Abs_Run(Globle.POCT_Par.M10_P1,M10,M10_DIR,8000);
				while(1)
				{

					MotorFre_Abs_Run(Globle.POCT_Par.M10_P1,M10,M10_DIR,MIX_SPEED);
										
					MotorFre_Abs_Run(Globle.POCT_Par.M10_P1 - MIX_VL,M10,M10_DIR,MIX_SPEED);
					
					err = xSemaphoreTake(Vibrate_SemaphoreHandle,0);	//获取信号量
					if(err==pdTRUE)										//获取信号量成功
					{
						xSemaphoreGive(VibrateStop_SemaphoreHandle);
						break;
					}

					vTaskDelay(10);   //延时10ms，也就是10个时钟节拍						
				}
				
				Motor_Rst(M10_FRE,M10,M10_DIR,M10_GO_GPIO_PORT,M10_GO_GPIO_PIN,0);
			}
		}
		else if(err==pdFALSE)
		{
			vTaskDelay(10);            //延时10ms，也就是10个时钟节拍	
		}		
	}
}

#endif


