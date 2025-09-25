/**
  ******************************************************************************
  * @file    bsp_motor.c
  * @author  hms 
  * @version V1.0
  * @date    2021.10.19
  * @brief   
  ******************************************************************************
  * @attention
  * 
  *
  ******************************************************************************
  */  
  
  
/***********************************************头文件*************************************/
#include "bsp_motor.h"   

#include "bsp_motor_conf.h"

#include "poct_process.h"

#include "usart.h"

#include "types.h"

#include "delay.h"

#include "LogTask.h"

#include "Reply.h"

#include "LogTask.h"

#include <math.h>

#include <string.h>

#include <stdlib.h>

#include "includes.h"
/******************************************************************************************/

/*************************************宏定义***********************************************/
//
/******************************************************************************************/

/***********************************全局变量************************************************/
Stepper_Typedef      Stepper[TOTAL_MOTOR]; 

DOORState_TypeDef    DoorState;

SpeedCalc_TypeDef *  pSpeedGloble[3];    
/******************************************************************************************/



/**********************************
  * @brief  
  *	@note 	
  ********************************/
void Motor_Stop(uint8_t motor_num,SpeedCalc_TypeDef * pSpeed)
{	
 	Stepper[motor_num].status = STOP;
	
	Stepper[motor_num].index = 0;
	
	HAL_GPIO_WritePin(Motor_Conf[motor_num].en_port,Motor_Conf[motor_num].en_pin,HIGH1);

	HAL_TIM_OC_Stop_IT(pSpeed->htim, TIM_CHANNEL_1);
	
	Stepper[motor_num].pos = 0; 
	
	Stepper[motor_num].start_flag = 0;	
}
  

/**********************************
//速度计算
**********************************/
bool pMCalcSpeed(int32_t Vo, int32_t Vt, float T,SpeedCalc_TypeDef * pSpeed)
{
	int32_t i    = 0;
	
	float Vm     = 0.0f;    
	
	float K      = 0.0f;  
	
	float Ti     = 0.0f;  
	
	float Sumt   = 0.0f;  
	
	float DeltaV = 0.0f;  
	
	float temp   = 0.0f;
	
	pSpeed->Vo = CONVER(Vo); 	
	
	pSpeed->Vt = CONVER(Vt);

	T = T / 2.0f;       
	
	Vm = (pSpeed->Vo + pSpeed->Vt) / 2.0f;    
	
	K = fabsf((2.0f * (Vm - pSpeed->Vo)) / (T * T));    
	
	pSpeed->INC_AccelTotalStep = (int32_t)((pSpeed->Vo * T) + ((K * T * T* T) / 6.0f));
	
	pSpeed->Dec_AccelTotalStep = (int32_t)(((pSpeed->Vt + pSpeed->Vo) * T - pSpeed->INC_AccelTotalStep));
	
	pSpeed->AccelTotalStep = pSpeed->Dec_AccelTotalStep + pSpeed->INC_AccelTotalStep;       
	
	if( pSpeed->AccelTotalStep  % 2 != 0)                                                                
		pSpeed->AccelTotalStep  += 1;	

	if(FORM_LEN < pSpeed->AccelTotalStep)
	{
		return false;	
	}

	Ti = pow(6.0f * 1.0f / K, 1.0f / 3.0f); 
	
	Sumt += Ti;                             
	
	DeltaV = 0.5f * K * Sumt * Sumt;

	pSpeed->Form[0] = pSpeed->Vo + DeltaV;
	
	if (pSpeed->Form[0] <= MIN_SPEED)     
	{
		pSpeed->Form[0] = MIN_SPEED;
	}

	for (i = 1; i < pSpeed->AccelTotalStep; i++)
	{
		Ti = 1.0f / pSpeed->Form[i-1];
		if (i < pSpeed->INC_AccelTotalStep)
		{
			
			Sumt += Ti;

			DeltaV = 0.5f * K * Sumt * Sumt;

			pSpeed->Form[i] = pSpeed->Vo + DeltaV;

			if (i == pSpeed->INC_AccelTotalStep - 1)
			{Sumt  = fabsf(Sumt - T);}
		}

		else
		{

		  Sumt += Ti;

		  temp = fabsf(T - Sumt);
		  DeltaV = 0.5f * K * temp * temp;
		  pSpeed->Form[i] = pSpeed->Vt - DeltaV;
		}
	}
	return true;
}


/***************************************************
//
***************************************************/
void motor_TIM_start(TIM_HandleTypeDef *htim,uint32_t TIM_channelX,uint32_t pluse_time)
{
	__HAL_TIM_SET_COUNTER(htim, 0);
						  
	__HAL_TIM_SET_COMPARE(htim, TIM_channelX, pluse_time);
	
	HAL_TIM_OC_Start_IT(htim, TIM_channelX);	
	
	TIM_CCxChannelCmd(htim->Instance, TIM_channelX, TIM_CCx_ENABLE);
}

/**************************************************
//设置电机方向
**************************************************/
void set_motor_dir(uint8_t motor_num,uint8_t dir)
{
	if (dir == CCW) 
	{
		HAL_GPIO_WritePin(Motor_Conf[motor_num].dir_port,Motor_Conf[motor_num].dir_pin,CCW);
	}
	else 
	{
		HAL_GPIO_WritePin(Motor_Conf[motor_num].dir_port,Motor_Conf[motor_num].dir_pin,CW);
	}
}


/*********************************************
//
//
*********************************************/
void motor_enable(uint8_t motor_num)
{	
	HAL_GPIO_WritePin(Motor_Conf[motor_num].en_port,Motor_Conf[motor_num].en_pin,LOW1);
}


/**********************************************
//
***********************************************/
void motor_leve(uint8_t motor_num)
{
	switch(motor_num)
	{
		case M01: M01_PUL_LEVE = ~M01_PUL_LEVE;break;
			
		case M02: M02_PUL_LEVE = ~M02_PUL_LEVE;break;
			
		case M03: M03_PUL_LEVE = ~M03_PUL_LEVE;break;
			
		case M04: M04_PUL_LEVE = ~M04_PUL_LEVE;break;
			
		case M05: M05_PUL_LEVE = ~M05_PUL_LEVE;break;
			
		case M06: M06_PUL_LEVE = ~M06_PUL_LEVE;break;
			
		case M07: M07_PUL_LEVE = ~M07_PUL_LEVE;break;
			
		case M08: M08_PUL_LEVE = ~M08_PUL_LEVE;break;
			
		case M09: M09_PUL_LEVE = ~M09_PUL_LEVE;break;
			
		case M10: M10_PUL_LEVE = ~M10_PUL_LEVE;break;
			
		case M11: M11_PUL_LEVE = ~M11_PUL_LEVE;break;
			
		case M12: M12_PUL_LEVE = ~M12_PUL_LEVE;break;

		case M15: M15_PUL_LEVE = ~M15_PUL_LEVE;break;		
		default: break;
	}
}

/*****************************************
  * @brief  
  * @param  
  * @retval 
  *	@note 	
  ****************************************/
void Speed_Decision_TIM2(TIM_HandleTypeDef *htim,uint32_t TIM_IT_CCn,uint32_t Channel,uint8_t motor_num,SpeedCalc_TypeDef * pSpeed)
{

	static __IO uint8_t i = 0;	
	
	uint32_t tim_count = 0;	
	
	uint16_t TIM_arr = 0;
	
	uint16_t tmp = 0;
	
	if (__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_CCn) != RESET)
	{

		__HAL_TIM_CLEAR_IT(htim, TIM_IT_CCn);		
		
		motor_leve(motor_num);

		i++;	
		if(i == 2)
		{

			i = 0;

			if (pSpeed->deceleration_flag == 1)
			{
				if (Stepper[motor_num].status == UNIFORM)
				{
					if (Stepper[motor_num].pos >= (pSpeed->TotalStep - 1))
					{	
						Stepper[motor_num].index = 0;
						
						Stepper[motor_num].status = STOP;	
						
						HAL_TIM_OC_Stop_IT(htim, Channel);
						
						Motor_Stop(motor_num,pSpeed);	/* 鍏抽棴瀹氭椂鍣*/					
					}						
				}
			}
			
			else
			{
				switch (Stepper[motor_num].status)
				{
					case ACCEL:	
					{
						if (Stepper[motor_num].pos >= (pSpeed->AccelTotalStep - 1))
						{
							Stepper[motor_num].status = UNIFORM;	
							
							Stepper[motor_num].index -= 1;
							//index -= 1;
							break;
						}

						Stepper[motor_num].pluse_time = (uint16_t)(T1_FREQ / pSpeed->Form[Stepper[motor_num].index] / 2U);

						Stepper[motor_num].index++;
						break;
					}
					
					case DECEL:
					{
						if (Stepper[motor_num].pos >= (pSpeed->TotalStep - 1))
						{
							Stepper[motor_num].index = 0;
							
							Stepper[motor_num].status = STOP;
							
							HAL_TIM_OC_Stop_IT(htim, Channel);
							
							Motor_Stop(motor_num,pSpeed);
											
							break;
						}	

						Stepper[motor_num].pluse_time = (uint16_t)(T1_FREQ / pSpeed->Form[Stepper[motor_num].index] / 2U);

						Stepper[motor_num].index--;	
						break;
					}
						
					case UNIFORM:  //鍖€閫 
					{
						if (Stepper[motor_num].pos >= pSpeed->DecPoint)
						{
							Stepper[motor_num].status = DECEL;
						}
						break;
					}
				}
			}			
			Stepper[motor_num].pos++;		
		}
		
		tim_count = __HAL_TIM_GET_COUNTER(htim);

		if(pSpeed->deceleration_flag != 1)
		{
			tmp = tim_count + Stepper[motor_num].pluse_time;
			__HAL_TIM_SET_COMPARE(htim, Channel, tmp);
//			__HAL_TIM_SET_AUTORELOAD(htim, tim_count*2);
		}
		else
		{
			TIM_arr = __HAL_TIM_GET_AUTORELOAD(htim);
//			__HAL_TIM_SET_AUTORELOAD(htim, tim_count*2);
			__HAL_TIM_SET_COMPARE(htim, Channel, TIM_arr/2);
		}	
	}	
}

void Speed_Decision_TIM3(TIM_HandleTypeDef *htim,uint32_t TIM_IT_CCn,uint32_t Channel,uint8_t motor_num,SpeedCalc_TypeDef * pSpeed)
{
	static __IO uint8_t i = 0;	
	
	uint32_t tim_count = 0;
	
	uint16_t TIM_arr = 0;
	
	uint16_t tmp = 0;
	
	if (__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_CCn) != RESET)
	{

		__HAL_TIM_CLEAR_IT(htim, TIM_IT_CCn);		
		
		motor_leve(motor_num);

		i++;	
		if (i == 2)
		{
			i = 0;

			if (pSpeed->deceleration_flag == 1)
			{
				if (Stepper[motor_num].status == UNIFORM)
				{
					if (Stepper[motor_num].pos >= (pSpeed->TotalStep - 1))
					{	
						Stepper[motor_num].index = 0;	
						
						Stepper[motor_num].status = STOP;
						
						HAL_TIM_OC_Stop_IT(htim, Channel);
						
						Motor_Stop(motor_num,pSpeed);						
					}						
				}
			}
			
			else
			{
				switch (Stepper[motor_num].status)
				{
					case ACCEL:	
					{
						if (Stepper[motor_num].pos >= (pSpeed->AccelTotalStep - 1))
						{
							Stepper[motor_num].status = UNIFORM;		
							
							Stepper[motor_num].index -= 1;
							
							//index -= 1;
							break;
						}

						Stepper[motor_num].pluse_time = (uint16_t)(T1_FREQ / pSpeed->Form[Stepper[motor_num].index] / 2U);

						Stepper[motor_num].index++;
						break;
					}
					
					case DECEL:
					{
						if (Stepper[motor_num].pos >= (pSpeed->TotalStep - 1))
						{
							Stepper[motor_num].index = 0;
							
							Stepper[motor_num].status = STOP;
							
							HAL_TIM_OC_Stop_IT(htim, Channel);
							
							Motor_Stop(motor_num,pSpeed);
											
							break;
						}	

						Stepper[motor_num].pluse_time = (uint16_t)(T1_FREQ / pSpeed->Form[Stepper[motor_num].index] / 2U);

						Stepper[motor_num].index--;	
						break;
					}
						
					case UNIFORM: 
					{
						if (Stepper[motor_num].pos >= pSpeed->DecPoint)
						{
							Stepper[motor_num].status = DECEL;
						}
						break;
					}
				}
			}			
		
			Stepper[motor_num].pos++;		
		}

		tim_count = __HAL_TIM_GET_COUNTER(htim);

		if(pSpeed->deceleration_flag != 1)
		{
			tmp = tim_count + Stepper[motor_num].pluse_time;
			
			__HAL_TIM_SET_COMPARE(htim, Channel, tmp);
//			__HAL_TIM_SET_AUTORELOAD(htim, tim_count*2);
		}
		else
		{
			TIM_arr = __HAL_TIM_GET_AUTORELOAD(htim);
//			__HAL_TIM_SET_AUTORELOAD(htim, tim_count*2);
			__HAL_TIM_SET_COMPARE(htim, Channel, TIM_arr/2);
		}	
	}	
}



void Speed_Decision_TIM4(TIM_HandleTypeDef *htim,uint32_t TIM_IT_CCn,uint32_t Channel,uint8_t motor_num,SpeedCalc_TypeDef * pSpeed)
{

	static __IO uint8_t i = 0;	
	
	uint32_t tim_count = 0;	
	
	uint16_t TIM_arr = 0;
	
	uint16_t tmp = 0;
	
	if (__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_CCn) != RESET)
	{

		__HAL_TIM_CLEAR_IT(htim, TIM_IT_CCn);		
		
		motor_leve(motor_num);

		i++;	
		if (i == 2)
		{

			i = 0;
	
			if (pSpeed->deceleration_flag == 1)
			{
				if (Stepper[motor_num].status == UNIFORM)
				{
					if (Stepper[motor_num].pos >= (pSpeed->TotalStep - 1))
					{	
						Stepper[motor_num].index = 0;
						
						Stepper[motor_num].status = STOP;	
						
						HAL_TIM_OC_Stop_IT(htim, Channel);
						
						Motor_Stop(motor_num,pSpeed);				
					}						
				}
			}
			
			else
			{
				switch (Stepper[motor_num].status)
				{
					case ACCEL:	
					{
						if (Stepper[motor_num].pos >= (pSpeed->AccelTotalStep - 1))
						{
							Stepper[motor_num].status = UNIFORM;
							
							Stepper[motor_num].index -= 1;
							//index -= 1;
							break;
						}
						
						Stepper[motor_num].pluse_time = (uint16_t)(T1_FREQ / pSpeed->Form[Stepper[motor_num].index] / 2U);
				
						Stepper[motor_num].index++;
						break;
					}
					
					case DECEL:
					{
						if (Stepper[motor_num].pos >= (pSpeed->TotalStep - 1))
						{
				
							Stepper[motor_num].index = 0;
							
							Stepper[motor_num].status = STOP;
							
							HAL_TIM_OC_Stop_IT(htim, Channel);
							
							Motor_Stop(motor_num,pSpeed);
											
							break;
						}	
					
						Stepper[motor_num].pluse_time = (uint16_t)(T1_FREQ / pSpeed->Form[Stepper[motor_num].index] / 2U);
						
						Stepper[motor_num].index--;	
						break;
					}
						
					case UNIFORM:
					{
						if (Stepper[motor_num].pos >= pSpeed->DecPoint)
						{
							Stepper[motor_num].status = DECEL;
						}
						break;
					}
				}
			}			
			
			Stepper[motor_num].pos++;		
		}
		
		tim_count = __HAL_TIM_GET_COUNTER(htim);

		if(pSpeed->deceleration_flag != 1)
		{
			tmp = tim_count + Stepper[motor_num].pluse_time;
			
			__HAL_TIM_SET_COMPARE(htim, Channel, tmp);
//			__HAL_TIM_SET_AUTORELOAD(htim, tim_count*2);
		}
		else
		{
			TIM_arr = __HAL_TIM_GET_AUTORELOAD(htim);
//			__HAL_TIM_SET_AUTORELOAD(htim, tim_count*2);
			__HAL_TIM_SET_COMPARE(htim, Channel, TIM_arr/2);
		}	
	}	
}


/*****************************************************************************
* @brief  
* @param  
* @retval 
* @note   
******************************************************************************/
bool Stepper_Move_S(int16_t start_speed, int16_t end_speed, float acc_time, int32_t step,uint8_t motor_num,uint8_t dir,SpeedCalc_TypeDef * pSpeed)
{
	if (pSpeed == 0)
	{
		return false;
	}
	
	pSpeed -> deceleration_flag = 0;
	if (step <= 0)
	{
		return false;
	}

	if (Stepper[motor_num].status != STOP)
	{
		return false;
	}
	
	if (pMCalcSpeed(start_speed, end_speed,acc_time,pSpeed) != true)
	{
		return false;
	}
	
	motor_enable(motor_num);  
	
	set_motor_dir(motor_num,dir);    
  
	if (step >= pSpeed->AccelTotalStep * 2)
	{
		if (pSpeed->Form[0] == 0)
		{
			Stepper[motor_num].pluse_time = 1500;
		}
		else
		{
			pSpeed->TotalStep = step;
			
			pSpeed->DecPoint = pSpeed->TotalStep - pSpeed->AccelTotalStep;	
			
			Stepper[motor_num].status = ACCEL;	
			
			Stepper[motor_num].pluse_time = (uint16_t)(T1_FREQ / pSpeed->Form[0] / 2U);
		}
	}
	else
	{
		pSpeed->TotalStep = step;	
		
		pSpeed->deceleration_flag = 1;
		
		Stepper[motor_num].pluse_time = 1500;	
		
		Stepper[motor_num].status = UNIFORM;			
	} 

	Stepper[motor_num].pos = 0;		
	
	Stepper[motor_num].start_flag = 1;
	
	if ((motor_num == M01) || (motor_num == M02) || (motor_num == M03) || (motor_num == M04))
	{
		pSpeedGloble[0] = NULL;
		
		pSpeedGloble[0] = pSpeed;
		
		pSpeedGloble[0]->htim = &TIM2_Handler;	
		
		pSpeedGloble[0]->motor_num = motor_num;
		
		motor_TIM_start(&TIM2_Handler,TIM_CHANNEL_1,Stepper[motor_num].pluse_time);
		return true;
	}
	else if ((motor_num == M05) || (motor_num == M06) || (motor_num == M07) || (motor_num == M08)\
		|| (motor_num == M09) || (motor_num == M11))
	{
		pSpeedGloble[1] = NULL;
		
		pSpeedGloble[1] = pSpeed;
		
		pSpeedGloble[1]->htim = &TIM3_Handler;	
		
		pSpeedGloble[1]->motor_num = motor_num;
		
		motor_TIM_start(&TIM3_Handler,TIM_CHANNEL_1,Stepper[motor_num].pluse_time);
		
		return true;
	}
	else
	{
		pSpeedGloble[2] = NULL;
		
		pSpeedGloble[2] = pSpeed;
		
		pSpeedGloble[2]->htim = &TIM4_Handler;	
		
		pSpeedGloble[2]->motor_num = motor_num;
		
		motor_TIM_start(&TIM4_Handler,TIM_CHANNEL_1,Stepper[motor_num].pluse_time);
		
		return true;
	}
}

/****************************
* @brief  
* @
* @retval 
* @retval 
* @note   
*************************/
uint8_t Motor_Rst(uint16_t fre,uint8_t motor_num,uint8_t dir,\
GPIO_TypeDef *Port,uint16_t Pin,uint8_t reversalEnable)
{	
	uint32_t timeout = 0;	
	
	SpeedCalc_TypeDef * pSpeed = NULL;
	
	pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
	
	if (HAL_GPIO_ReadPin(Port,Pin) == SET)
	{
		Stepper_Move_S(MIN_FRE,fre,0.01f, RST_MAX_STEP,motor_num,dir,pSpeed);

		while (HAL_GPIO_ReadPin(Port,Pin) == SET)
		{
			if (timeout > MOTOR_RES_TIMEOUT * 1000)
			{
				Motor_Stop(motor_num,pSpeed);
				
				vPortFree(pSpeed);
				
				return 1;
			}
			
			vTaskDelay(1);
			
			timeout++;
		}
		
		timeout = 0;
		
		Motor_Stop(motor_num,pSpeed);
	}
	else
	{	
		if (dir == CCW)
		{
			Stepper_Move_S(MIN_FRE,300,0.01f, 2000,motor_num,CW,pSpeed); 
		}
		else
		{
			Stepper_Move_S(MIN_FRE,300,0.01f, 2000,motor_num,CCW,pSpeed);
		}
		
		if (reversalEnable == 1)
		{
			while(Stepper[motor_num].status != 0){vTaskDelay(1);}
		}
		else
		{
			while(HAL_GPIO_ReadPin(Port,Pin) == RESET){vTaskDelay(1);}
		}
			
		Motor_Stop(motor_num,pSpeed);
		
		if (dir == CCW)
		{
			Stepper_Move_S(MIN_FRE,300,0.01f, RST_MAX_STEP,motor_num,CCW,pSpeed);
		}
		else
		{
			Stepper_Move_S(MIN_FRE,300,0.01f, RST_MAX_STEP,motor_num,CW,pSpeed);
		}
				
		while(HAL_GPIO_ReadPin(Port,Pin) == SET){vTaskDelay(1);}
		
		Motor_Stop(motor_num,pSpeed);
	}
	
	Stepper[motor_num].position = 0;
	
	vPortFree(pSpeed);
	
	return 0;
}


/**********************************
* @brief  
* @param  
* @retval 
**********************************/
uint8_t Motor_HighLeve_Rst(uint16_t fre,uint8_t motor_num,uint8_t dir,\
GPIO_TypeDef *Port,uint16_t Pin,uint8_t reversalEnable)
{
	uint32_t timeout = 0; 
	
	SpeedCalc_TypeDef * pSpeed = NULL;
	
	pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
	
	if(HAL_GPIO_ReadPin(Port,Pin) == RESET)
	{			
		Stepper_Move_S(MIN_FRE,fre,0.01f, RST_MAX_STEP,motor_num,dir,pSpeed);
		
		while (HAL_GPIO_ReadPin(Port,Pin) == RESET)
		{
			if (timeout > MOTOR_RES_TIMEOUT*1000)
			{
				Motor_Stop(motor_num,pSpeed);
				
				vPortFree(pSpeed);
				
				return 1;
			}
			vTaskDelay(1);
			
			timeout++;
		}	
		Motor_Stop(motor_num,pSpeed);
						
	}
	else
	{	
		if (dir == CCW)
		{
			Stepper_Move_S(MIN_FRE,500,0.01f, 2500,motor_num,CW,pSpeed); 
		}
		else
		{
			Stepper_Move_S(MIN_FRE,500,0.01f, 2500,motor_num,CCW,pSpeed);
		}
		
		if (reversalEnable == 1)
		{
			while(Stepper[motor_num].status != 0){vTaskDelay(1);}
		}
		else
		{
			while(HAL_GPIO_ReadPin(Port,Pin) == SET){vTaskDelay(1);}
		}
		Motor_Stop(motor_num,pSpeed);

		if (dir == CCW)
		{
			Stepper_Move_S(MIN_FRE,500,0.01f, RST_MAX_STEP,motor_num,CCW,pSpeed);
		}
		else
		{
			Stepper_Move_S(MIN_FRE,500,0.01f, RST_MAX_STEP,motor_num,CW,pSpeed);
		}
				
		while (HAL_GPIO_ReadPin(Port,Pin) == RESET){vTaskDelay(1);}
		
		Motor_Stop(motor_num,pSpeed);
	}
	
	Stepper[motor_num].position = 0;	
	vPortFree(pSpeed);
	
	return 0;
}


/********************************************************
* @brief  
* @param  
* @retval 
******************************************************/
uint16_t Motor_Fre(uint32_t step)
{
	uint16_t fre = 0;	
	
	fre = (uint16_t)(1.5*step + 100);
	
	fre = fre > 3000u ? fre = 3000u : fre;
	
	fre = fre < 1500u ? fre = (uint16_t)(0.8*step + 200) : fre;

	return (fre);
}

/****************************
  * @brief  
  * @param  
  * @retval 
  **************************/
uint16_t Motor_Fre_Vi(uint32_t step)
{
	return MIN_FRE;	
}

/**************************
* @brief 
* @param  
* @retval 
************************/
void Motor_Abs_Run(uint32_t step,uint8_t motor_num)
{
	uint16_t fre = 0;		
	
	SpeedCalc_TypeDef * pSpeed = NULL;
	
	pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
	
	if(step >= Stepper[motor_num].position)
	{		
		step = step - Stepper[motor_num].position;
		
		fre = Motor_Fre(step);
		
		Stepper_Move_S(MIN_FRE,fre,0.01f,step,motor_num,CCW,pSpeed); 
		
		while(Stepper[motor_num].status != STOP){vTaskDelay(1);}
		
		Stepper[motor_num].position = Stepper[motor_num].position + step;
	}
	else 
	{
		step = Stepper[motor_num].position - step;	
		
		fre = Motor_Fre(step);
		
		Stepper_Move_S(MIN_FRE,fre,0.01f,step,motor_num,CW,pSpeed);
		
		while(Stepper[motor_num].status != STOP){vTaskDelay(1);}
		
		Stepper[motor_num].position = Stepper[motor_num].position - step;
	}
	vPortFree(pSpeed);
}

/************************************
* @brief  
* @param  
* @retval 
**********************************/
void MotorFre_Abs_Run(uint32_t step,uint8_t motor_num,uint8_t dir,uint16_t fre)
{	
	SpeedCalc_TypeDef * pSpeed = NULL;
	pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
	
	if(step >= Stepper[motor_num].position)                               
	{		
		step = step - Stepper[motor_num].position;
		
		if(dir ==  CCW)			
			Stepper_Move_S(MIN_FRE,fre,0.01f,step,motor_num,CW,pSpeed);
		else
			Stepper_Move_S(MIN_FRE,fre,0.01f,step,motor_num,CCW,pSpeed);
				
		while(Stepper[motor_num].status != STOP){vTaskDelay(1);}
		
		Stepper[motor_num].position = Stepper[motor_num].position + step;			
	}
	else 
	{
		step = Stepper[motor_num].position - step;
				
		if(dir ==  CCW)			
			Stepper_Move_S(MIN_FRE,fre,0.01f,step,motor_num,CCW,pSpeed);
		else
			Stepper_Move_S(MIN_FRE,fre,0.01f,step,motor_num,CW,pSpeed);
		
		while(Stepper[motor_num].status != STOP){vTaskDelay(1);}
		
		Stepper[motor_num].position = Stepper[motor_num].position - step;
	}
	vPortFree(pSpeed);
}

//震动电机
void ZD_motor_run(SMsg * pMsg)
{
//	uint16_t ZDtime,fre;
//	
//	uint32_t TimeNowCnt;
//	
//	Motor_Rst(M01_FRE,M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN,0);
//	
//	Motor_Rst(M02_FRE,M02,M02_DIR,M02_GO_GPIO_PORT,M02_GO_GPIO_PIN,0);
//	
//	fre = Motor_Fre(Globle.POCT_Par.M02_P3);
//	
//	MotorFre_Abs_Run(Globle.POCT_Par.M02_P3,M02,M02_DIR,fre);

//	fre = Motor_Fre(Globle.POCT_Par.M01_P1);
//	
//	MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M02_DIR,fre);
//	
//	MIN_MOTOR_RUN(0);
//	
//	ZDtime = atoi32(pMsg->m_pAppMsg->m_pAppLayerMainPara,10);
//	
//	TimeNowCnt  = HAL_GetTick();
//	
//	while(TimeNowCnt + ZDtime*1000 > HAL_GetTick())
//	{
//		MIN_MOTOR_TOGGLE;
//		delay_ms(1000);
//	}
//	
//	MIN_MOTOR_RUN(1);
}

void scan_module_reset(void)
{
	if(Stepper[M08].position > 4000)
		MotorFre_Abs_Run(500,M08,M08_DIR,3800);
	
	Motor_Rst(350,M08,M08_DIR,M08_GO_GPIO_PORT,M08_GO_GPIO_PIN,0);
	
	if(Stepper[M03].position > 4000)
		MotorFre_Abs_Run(500,M03,M03_DIR,3800);
			
	Motor_Rst(350,M03,M03_DIR,M03_GO_GPIO_PORT,M03_GO_GPIO_PIN,0);
}


void motor_fast_rst(uint8_t motor_num,uint8_t dir,GPIO_TypeDef *gpio_port,uint16_t gpio_pin)
{
	uint8_t err = 0;

	char *str = NULL;
	
	str = pvPortMalloc(25);
	
	memset(str,0,25);

	if(Stepper[motor_num].position > 4000)
	{
		MotorFre_Abs_Run(500,motor_num,dir,3800);
	}
	
	delay_ms(1);
	
	err = Motor_Rst(500,motor_num,dir,gpio_port,gpio_pin,0);
	
	if(err != 0)
	{
		motor_num = motor_num + 1;

		itoa(motor_num,str,10);

		strcat(str,"-MOTOR TIME OUT!");

		SaveFaultCode(str);
		
		vPortFree(str);
		return;
	}
	
	vPortFree(str);
}



/*********************************************END OF FILE**********************/
