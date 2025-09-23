#include "InstrumentInclude.h"

#ifdef POCT_ANM_H1L4_A

#include "bsp_motor_conf.h" 

//泵吸打控制
#define PUMP_IN      0

#define PUMP_OUT     1

#define	TEMP2_SEND   0 
#define	TIME2_SEND   1 
#define	TEMP3_SEND   2 
#define	TIME3_SEND   3 

#define MIX_V        25000
#define MIX_FRE      1300

#define PUBLIC_HOLE  1 
#define LQD_HOLE     2    //液体分配孔
#define LYSTATE      1  
#define EFFLUENT_1   9  
#define WASHING_1    8  
#define WASHING_2    7  
#define WASHING_3    6 
#define ELUTION      5
#define DRYING       4

TaskHandle_t VibrateTask_Handler;

//void MagneticSet_UD(uint16_t step,uint16_t fre,uint8_t times,uint8_t WaitTime);

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

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
void Motor_GPIO_Config(void)
{
	uint8_t i = 0;
	GPIO_InitTypeDef GPIO_InitStruct;
  
	/* 获取数组元素个数 */
	uint8_t member_count = MOTOR_CONF_NUM;
  
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	__HAL_RCC_GPIOF_CLK_ENABLE();

	Globle.LastFaNum = 0;
	Globle.FaNum = 0;
	
	for(i = 0; i < member_count; i++)
	{
		/*选择要控制的GPIO引脚*/															   
		GPIO_InitStruct.Pin = Motor_Conf[i].dir_pin;	
		/*设置引脚的输出类型为推挽输出*/
		GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
		GPIO_InitStruct.Pull =GPIO_PULLUP;
		/*设置引脚速率为高速 */   
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		/*Motor 方向引脚 初始化*/
		HAL_GPIO_Init(Motor_Conf[i].dir_port, &GPIO_InitStruct);
		  
		/*Motor 使能引脚 初始化*/
		GPIO_InitStruct.Pin = Motor_Conf[i].en_pin;
		HAL_GPIO_Init(Motor_Conf[i].en_port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(Motor_Conf[i].en_port,Motor_Conf[i].en_pin,HIGH1);
		  
		/*Motor光耦引脚 初始化*/
		GPIO_InitStruct.Pin  = Motor_Conf[i].go_pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;          //上拉
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;     //高速
		HAL_GPIO_Init(Motor_Conf[i].go_port, &GPIO_InitStruct); 

		/*配置电机STEP引脚 */
		GPIO_InitStruct.Pin = Motor_Conf[i].pul_pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;          //上拉
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;     //高速 
		HAL_GPIO_Init(Motor_Conf[i].pul_port,&GPIO_InitStruct);	  	
	}
}

uint32_t Fa_Count(uint8_t FaNum)
{
	uint32_t step = 0;
	
	uint32_t offset0 = 0,offset1 = 0,offset2 = 0;

	offset1 = (Globle.POCT_Par.M10_Fa_P[2] - Globle.POCT_Par.M10_Fa_P[1])/2; 
		
	offset0 = (Globle.POCT_Par.M10_Fa_P[1] - Globle.POCT_Par.M10_Fa_P[0]) - offset1;
	
	offset2 = (Globle.POCT_Par.M10_Fa_P[2] - Globle.POCT_Par.M10_Fa_P[3]) - offset1*2;

	if(Globle.FaNum == FaNum)
	{
		return 0;
	}
	
	if(Globle.FaNum < FaNum)
	{
		if(FaNum == 1)
		{
			step = Globle.POCT_Par.M10_Fa_P[0];
		}
		else
		{
			if(Globle.LastFaNum > Globle.FaNum)
			{
//				step = Globle.POCT_Par.M10_Fa_P[0] + (offset2 - offset0) + offset1*(FaNum-1);
				step = Globle.POCT_Par.M10_Fa_P[0] + offset2 + offset1*(FaNum-1);
			}
			else
			{
				if(Globle.LastFaNum == 0)
					step = Globle.POCT_Par.M10_Fa_P[0] + offset2 + offset1*(FaNum-1);
				else
					step = Globle.POCT_Par.M10_Fa_P[0] + offset0 + offset1*(FaNum-1);
			}
		}
	}
	else
	{
			step = (Globle.POCT_Par.M10_Fa_P[0] + offset0 + offset1*(Globle.FaNum - 1) - offset2) - (offset1*(Globle.FaNum - FaNum));
	}
	Globle.LastFaNum = Globle.FaNum;

	Globle.FaNum = FaNum;
	
	return (step);
}


uint8_t Fa_Function(uint8_t FaNum)
{
	uint32_t M10_step = 0;

	M10_step = Fa_Count(FaNum);
	
	if(FaNum == 1)
	{	
		MotorFre_Abs_Run(M10_step,M10,M10_DIR,400);

		MotorFre_Abs_Run(Globle.POCT_Par.M11_P1,M11,M11_DIR,500);
	}
	else
	{
		MotorFre_Abs_Run(Globle.POCT_Par.M11_P1,M11,M11_DIR,500);

		MotorFre_Abs_Run(M10_step,M10,M10_DIR,400);			
	}
	return 0;
}


void hotCap(SMsg * pMsg,uint16_t Temp)
{
	uint16_t temp_up_cnt = 0;
	
	uint16_t SendT2T3buf[4] = {0};
	
	uint32_t err_cnt = 0;
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
	
	Globle.TempControl.Temp_Ch4_ENABLE  = SET;
	
	Globle.TempControl.Temp_Ch4_Set = Temp;
	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_1);
	
	while(GetTemp(4) >= (Temp+10))
	{
		err_cnt ++;
		
		temp_up_cnt++;
		
		if(temp_up_cnt >= 100)
		{		
			SendT2T3buf[TEMP2_SEND] = GetTemp(1); 
					
			if((SendT2T3buf[TEMP2_SEND] > Globle.TempControl.Temp_Ch2_Set - 5)&&(SendT2T3buf[TEMP2_SEND] < Globle.TempControl.Temp_Ch2_Set + 5))
			{
				SendT2T3buf[TEMP2_SEND] =  Globle.TempControl.Temp_Ch2_Set;
			}
			
			SendT2T3buf[TEMP3_SEND] = GetTemp(2); 
			
			if((SendT2T3buf[TEMP3_SEND] > Globle.TempControl.Temp_Ch3_Set - 5)&&(SendT2T3buf[TEMP3_SEND] < Globle.TempControl.Temp_Ch3_Set + 5))
			{
				SendT2T3buf[TEMP3_SEND] =  Globle.TempControl.Temp_Ch3_Set;
			}
			
			Send_T2T3(pMsg,SendT2T3buf);
			temp_up_cnt = 0;
		}
		
		if(err_cnt > 8*60*100)
		{
			SaveFaultCode("HOTCAP_TIMEOUT");
		}
		
		delay_ms(5);
	}
}

void PUMP_Func(uint32_t step,uint8_t dir,uint16_t u16Fre)
{
	bool err = true;

	SpeedCalc_TypeDef * pSpeed = NULL;

	uint16_t fre = 0;

	fre = Motor_Fre(step);

	pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef)+1);
	
	if (fre >  u16Fre)
		fre = u16Fre;
	
	if (dir == PUMP_IN)
	{	
		if (M07_DIR == CCW)
			err = Stepper_Move_S(MIN_FRE,fre,0.01f,step,M07,CCW,pSpeed);
		else 
			err = Stepper_Move_S(MIN_FRE,fre,0.01f,step,M07,CW,pSpeed);
		Stepper[M07].position = Stepper[M07].position - step;
	}
	else
	{
		if (M07_DIR == CCW)
			 err = Stepper_Move_S(MIN_FRE,fre,0.01f,step,M07,CW,pSpeed);
		else 
			 err = Stepper_Move_S(MIN_FRE,fre,0.01f,step,M07,CCW,pSpeed);
		Stepper[M07].position = Stepper[M07].position + step;
	}
	
	if(err == false)
	{
		SaveFaultCode("F^M07_TIMEOUT");
		return;
	}		
	
	while (Stepper[M07].status != STOP){vTaskDelay(1);}
	
	vPortFree(pSpeed);
}

void PUMP_Mix_Func(uint8_t times,uint32_t volume,uint8_t wTime)
{
	int i = 0;

	for(i = 0;i < times;i++)
	{		
		MotorFre_Abs_Run(Globle.POCT_Par.M07_P1 - volume,M07,M07_DIR,1000);	
		
		MotorFre_Abs_Run(Globle.POCT_Par.M07_P1,M07,M07_DIR,1000);	

		Wait_Time(wTime);
	}
}


void led_scan(uint8_t led_channel)
{
	u8 num = 0;
	
	num = led_channel - 1;
	
//	Motor_Abs_Run(Globle.POCT_Par.M08_LED_P[num],M08);
	if(num == 0)
		Motor_Abs_Run(Globle.POCT_Par.M08_LED_P[num],M08);
	else
		MotorFre_Abs_Run(Globle.POCT_Par.M08_LED_P[num],M08,M08_DIR,3800);
	
	LED_SPI_SendData(num ,(uint8_t)Globle.POCT_Par.LED[num]);	
}

/*********************************************
  * @brief  设置相机和LED的电机运动组合 扫描各孔；
  * @param  cam_chanel：孔号 ，led_chanel：灯号
  * @param  0：成功 ，1：失败
  * @retval 无
*******************************************/
uint8_t LED_CAM_Scan(uint8_t cam_chanel,uint8_t led_chanel)
{	
	ALL_LED_OFF();
	
	led_scan(led_chanel);
	
	delay_ms(5);
	
	return 0;	
}


/***************************
*@T1温度启动
****************************/
void T1_Heating(SMsg * pMsg)
{	
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


bool temperatureTrend(SMsg * pMsg,u8 channel)
{
	u16 temp[3],SendT2T3buf[4] = {0},temp_up_cnt = 0;

	u8 TempCnt = 0;
	
	memset(temp,0,sizeof(temp));
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
	
	temp[0] = GetTemp(channel);

	temp[1] = 0xffff;
	
	if(channel == 0)
		temp[2] = Globle.TempControl.Temp_Ch1_Set;
	else if(channel == 1)
		temp[2] = Globle.TempControl.Temp_Ch2_Set;
	else if(channel == 2)
		temp[2] = Globle.TempControl.Temp_Ch3_Set;
	else if(channel == 3)
		temp[2] = Globle.TempControl.Temp_Ch4_Set;

	while (temp[2] - 5 < GetTemp(channel))
	{
		
		temp_up_cnt++;
		
		if(temp_up_cnt >= 100)
		{		
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);	
			
			Send_T2T3(pMsg,SendT2T3buf);
			
			temp_up_cnt = 0;
		}
		
		delay_ms(10);
	}
	
	while (1)
	{
		temp_up_cnt++;
		
		if(temp_up_cnt >= 100)
		{		
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);	
			
			Send_T2T3(pMsg,SendT2T3buf);
			
			temp_up_cnt = 0;

			temp[TempCnt] =  GetTemp(channel);

			TempCnt++;

			if(TempCnt>1)
				TempCnt=0;
		}

		if(temp[1]<=temp[0])
			break;
		
		delay_ms(10);
	}

	return true;
	
}

/**
  * @brief  逆转录
  * @param  pMsg:传输信息帧
  * @param  0：成功 ，1：失败
  * @retval 无
  */
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
	
//	hotCap(pMsg,HOTCAP_TEMP);
	
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



/***********************************************
  * @brief  预变性
  * @param  
  * @param  0：成功 ，1：失败
  * @retval 无
  *********************************************/
bool App_YBX(SMsg * pMsg,int * pFaultCode)
{
	uint32_t temp_up_cnt = 0;
	
	uint16_t fre;
	
	uint16_t SendT2T3buf[4] = {0}, countdown = 0;
	
	uint32_t TimeNowCnt = 0;
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));

	motor_fast_rst(M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN);
	
	motor_fast_rst(M02,M02_DIR,M02_GO_GPIO_PORT,M02_GO_GPIO_PIN);

	temperatureTrend(pMsg,1);
	
	fre = Motor_Fre(Globle.POCT_Par.M02_P1);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M02_P1,M02,M02_DIR,fre);
	
	fre = Motor_Fre(Globle.POCT_Par.M01_P1);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,fre);
	
//	hotCap(pMsg,HOTCAP_TEMP);
	
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
	
	motor_fast_rst(M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN);
	
	return true;
}


/*************************************
* @brief  温度槽移动到相应孔位
* @param  0：成功 ，1：失败
* @retval 无 
************************************/
void App_PCR(SMsg * pMsg)
{
	uint16_t fre = 0;
	
	uint32_t temp_up_cnt = 0;
	
	uint16_t SendT2T3buf[4] = {0}, countdown = 0;
	
	uint32_t TimeNowCnt = 0; 
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
		
	MotorFre_Abs_Run(0,M01,M01_DIR,3700);
//	motor_fast_rst(M02,M02_DIR,M02_GO_GPIO_PORT,M02_GO_GPIO_PIN);
	
//	hotCap(pMsg,HOTCAP_TEMP);
	
	while(1)
	{		
		temp_up_cnt++;
		
		if(temp_up_cnt >= 1000)
		{
			SendT2T3buf[TEMP2_SEND] = GetTemp(1);
			
			SendT2T3buf[TEMP3_SEND] = GetTemp(2); 
						
			Send_T2T3(pMsg,SendT2T3buf);
			
			temp_up_cnt = 0;
			
		}
		
		vTaskDelay(1);	
		
		if(SendT2T3buf[TEMP2_SEND] >= Globle.TempControl.Temp_Ch2_Set - 20)
		{
			if(Stepper[M02].position < 3000)
			{
					fre =  Motor_Fre(Globle.POCT_Par.M02_P1);
					MotorFre_Abs_Run(Globle.POCT_Par.M02_P1,M02,M02_DIR,fre);
			}
			else
				MotorFre_Abs_Run(Globle.POCT_Par.M02_P1,M02,M02_DIR,4000);
			
//			fre =  Motor_Fre(Globle.POCT_Par.M01_P1);
			
			MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,3500);
			
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
	
//	motor_fast_rst(M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN);
	MotorFre_Abs_Run(0,M01,M01_DIR,3800);
	while(1)
	{			
		temp_up_cnt++;
		
		if(temp_up_cnt > 1000)
		{	
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);			
			Send_T2T3(pMsg,SendT2T3buf);
			temp_up_cnt = 0;
		}
		
		vTaskDelay(1);
		
		if(SendT2T3buf[TEMP3_SEND] >= Globle.TempControl.Temp_Ch3_Set - 20)
		{	
			
//		fre =  Motor_Fre(Globle.POCT_Par.M02_P2);
				
			MotorFre_Abs_Run(Globle.POCT_Par.M02_P2,M02,M02_DIR,3800);
			
//		fre =  Motor_Fre(Globle.POCT_Par.M01_P1);
			
			MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,3500);
					
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

/*****************************裂解细胞*****************************
*函数名称：Extraction_Tiqu
*pMsg:消息队列
*
*说明：裂解细胞。
*返回：无
******************************************************************/
void Extraction_Tiqu(SMsg * pMsg)
{
	BaseType_t err = pdFALSE;
	
//	MotorFre_Abs_Run(0xffff,M15,M15_DIR,500);		

//	Fa_Function(DRYING);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M07_P1,M07,M07_DIR,1000);	            

	Fa_Function(PUBLIC_HOLE);

	Globle.washTime = 4;
   	
	MotorFre_Abs_Run(Globle.POCT_Par.M06_P1,M06,M06_DIR,800);

	Wait_Time(3);
	
	xSemaphoreGive(Vibrate_SemaphoreHandle);                                  

	T1_Heating(pMsg);                                                     

	xSemaphoreGive(Vibrate_SemaphoreHandle); 

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
	                                                        
	Motor_Rst(M06_FRE,M06,M06_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN,0);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,800);  
	
	Wait_Time(5);                
	
	PUMP_Func(30000,PUMP_IN,700);	//4
	
	PUMP_Func(30000,PUMP_OUT,700);
	
	Wait_Time(3);
	
	PUMP_Func(30000,PUMP_IN,700);
	
	PUMP_Func(30000,PUMP_OUT,700);
	
	Wait_Time(3);
	
	PUMP_Func(30000,PUMP_IN,700);

	Fa_Function(EFFLUENT_1);
	
	PUMP_Func(30000,PUMP_OUT,700);	
	
	Fa_Function(PUBLIC_HOLE);
	
	PUMP_Func(18000,PUMP_IN,700);
	
	Fa_Function(EFFLUENT_1);
	
	PUMP_Func(18000,PUMP_OUT,700);

	
 	Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0);       

}

/*****************************第一次清洗*****************************
*函数名称：Extraction_Clean1
*
*
*说明：首次清洗。
*返回：无
******************************************************************/
void Extraction_Clean1(void)      
{
	Fa_Function(WASHING_1);  

	PUMP_Func(25000,PUMP_IN,800);
	
	Wait_Time(2);

	Fa_Function(PUBLIC_HOLE);

	PUMP_Func(25000,PUMP_OUT,800);                                         

 	PUMP_Mix_Func(5,25000,1); 
	
	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,800);               
	
	Wait_Time(5);
	
	PUMP_Func(25000,PUMP_IN,700);	
	
	PUMP_Func(25000,PUMP_OUT,700);

	Wait_Time(3);

	PUMP_Func(25000,PUMP_IN,700);
	
	PUMP_Func(25000,PUMP_OUT,700);

	Wait_Time(3);

	PUMP_Func(25000,PUMP_IN,600);
	
//	Wait_Time(20);  
//	
//	PUMP_Func(30000,PUMP_IN,1000);    

	Fa_Function(WASHING_1);  
	
 	PUMP_Func(25000,PUMP_OUT,600); 

	Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0);      
}


/*****************************************************************
*函数名称：Extraction_Clean2
*说明：第二次清洗
*返回：无
******************************************************************/
void Extraction_Clean2(SMsg * pMsg) 
{
	Fa_Function(WASHING_2);  

	PUMP_Func(30000,PUMP_IN,800);	

	Wait_Time(2);

	Fa_Function(PUBLIC_HOLE);

	PUMP_Func(30000,PUMP_OUT,800);                                        

 	PUMP_Mix_Func(5,30000,1);                                         

	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,800);               

    Wait_Time(5);                
	
	PUMP_Func(30000,PUMP_IN,700);	
	
	PUMP_Func(30000,PUMP_OUT,700);

	Wait_Time(3);

	PUMP_Func(30000,PUMP_IN,700);
	
	PUMP_Func(30000,PUMP_OUT,700);

	Wait_Time(3);

	PUMP_Func(32000,PUMP_IN,700);
	                                                                                         
	Fa_Function(WASHING_2);  
	
 	PUMP_Func(32000,PUMP_OUT,700); 

//	Fa_Function(LQD_HOLE);
//    
//	PUMP_Func(6000,PUMP_IN,500);
//
//	Wait_Time(4);
//
//	Fa_Function(WASHING_2);  
//
//	PUMP_Func(6000,PUMP_OUT,500);
//
//	Wait_Time(4);
    
	Fa_Function(WASHING_3);
	
	PUMP_Func(3000,PUMP_IN,600);
	
	Wait_Time(2);
	
	Fa_Function(PUBLIC_HOLE);
	
	PUMP_Func(2500,PUMP_OUT,600);
	
	PUMP_Func(5500,PUMP_IN,600);
	
	Fa_Function(WASHING_3); 

//	PUMP_Func(8000,PUMP_OUT,600);

//	Fa_Function(WASHING_3);

//	PUMP_Func(3000,PUMP_IN,600);
//	
//	Wait_Time(2);
//	
//	Fa_Function(PUBLIC_HOLE);
//	
//	PUMP_Func(3000,PUMP_OUT,600);
//
//	PUMP_Func(8000,PUMP_IN,600);
//	
//	Fa_Function(WASHING_2); 
//
//	PUMP_Func(8000,PUMP_OUT,600);
//
//	Fa_Function(WASHING_3);

	PUMP_Func(14000,PUMP_IN,600);

	PUMP_Func(20000,PUMP_OUT,600);

//	PUMP_Func(20000,PUMP_IN,600);
//
//	PUMP_Func(20000,PUMP_OUT,600);
	
	Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0);    
		
	MotorFre_Abs_Run(Globle.POCT_Par.M06_P1,M06,M06_DIR,800);               

	T1_Heating(pMsg);                                                     

	Motor_Rst(M06_FRE,M06,M06_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN,0);        
	
}

/*****************************核酸洗脱*****************************
*函数名称：Extraction_Elution
*说明：核酸洗脱
*返回：无
******************************************************************/
void Extraction_Elution(SMsg * pMsg)          
{
	BaseType_t err = pdFALSE;

	Fa_Function(ELUTION); 

	
	PUMP_Func(15000,PUMP_IN,600);
	
	Wait_Time(2);
	
	// Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0);      
	
	Fa_Function(PUBLIC_HOLE);
	
	PUMP_Func(15000,PUMP_OUT,600);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M06_P1,M06,M06_DIR,800);                
	
	Globle.washTime = 3;
	
	Wait_Time(3);
	
	xSemaphoreGive(Vibrate_SemaphoreHandle);                               
	
	T1_Heating(pMsg);                                                       
	
	xSemaphoreGive(Vibrate_SemaphoreHandle);                                 
	
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
	}

	Motor_Rst(M06_FRE,M06,M06_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN,0);     
	
	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,800);             
	
	PID_Stop(0);

	//预升温  95摄氏度
    Globle.TempControl.Temp_Ch3_Set = 550u;
	
	Globle.TempControl.Temp_Ch2_Set = 940u;
	
	HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_1);
	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_4);
	
	Globle.TempControl.Temp_Ch3_ENABLE = SET;
	
	Globle.TempControl.Temp_Ch2_ENABLE = SET;
	                                                         
	Wait_Time(5); 
	
	PUMP_Func(15000,PUMP_IN,600);	
	
	PUMP_Func(15000,PUMP_OUT,600);
	
	Wait_Time(3);
	
	PUMP_Func(15000,PUMP_IN,600);
	
	PUMP_Func(15000,PUMP_OUT,600);
	
	Wait_Time(3);
	
	PUMP_Func(15000,PUMP_IN,600);
	
	Wait_Time(1);
}

/*****************************溶液分配*****************************
*函数名称：Extraction_Solution_Apportion
*
*
*说明：溶液分配
*返回：无
******************************************************************/
void Extraction_Solution_Apportion(SMsg * pMsg)
{	
	uint32_t temp_V = 0;

	if(Globle.POCT_Par.Solution_Volume > 5000)
		temp_V = 2800;
	else 
		temp_V = Globle.POCT_Par.Solution_Volume;

	Fa_Function(DRYING);
	
	PUMP_Func(15000,PUMP_OUT,700); 
	
	PUMP_Mix_Func(2,15000,1);
	
	PUMP_Func(15000,PUMP_IN,500);

	Wait_Time(1);
	
	PUMP_Func(2500,PUMP_OUT,700); 

//	Wait_Time(1);

	Fa_Function(LQD_HOLE);

	PUMP_Func(temp_V,PUMP_OUT,700);
	
	Wait_Time(1);
	
	MotorFre_Abs_Run(Stepper[M10].position - 3200,M10,M10_DIR,400);
	
}

void ALL_Motor_RESET(void)	
{
	int i = 0;
	
	uint8_t Err = 0;
	
	for(i = 0;i<12;i++)
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
		
	Err = Motor_Rst(M08_FRE,M08,M08_DIR,M08_GO_GPIO_PORT,M08_GO_GPIO_PIN,1);
	
	if(Err != 0)
	{
		SaveFaultCode("F^M08_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M05_FRE,M05,M05_DIR,M05_GO_GPIO_PORT,M05_GO_GPIO_PIN,0);
	
	if(Err != 0)
	{
		SaveFaultCode("F^M05_TIMEOUT");
		return;
	}
	
	Err = Motor_Rst(M06_FRE,M06,M06_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN,0);
	
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

	Err = Motor_Rst(M11_FRE,M11,M11_DIR,M11_GO_GPIO_PORT,M11_GO_GPIO_PIN,1);

	if(Err != 0)
	{
		SaveFaultCode("F^M11_TIMEOUT");
		return;
	}

	Err = Motor_Rst(M10_FRE,M10,M10_DIR,M10_GO_GPIO_PORT,M10_GO_GPIO_PIN,1);
	
	if(Err != 0)
	{
		SaveFaultCode("F^M10_TIMEOUT");
		return;
	}

	Err = Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0);	
	if(Err != 0)
	{
		SaveFaultCode("F^M12_TIMEOUT");
		return;
	}		
	
}


/*************************
  * @brief  电机移动
	* @param  str:主参数
  * @retval 无
  *************************/
void Motor_Move(SMsg * pMsg,uint16_t Rstfre,uint8_t motor_num,uint8_t dir,GPIO_TypeDef *Port,uint16_t Pin)
{
	SpeedCalc_TypeDef * pSpeed = NULL;
	
	uint32_t step = 0;
	
	uint16_t fre = 0;
	
	pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
	
	step = ExtractNumberAfterEqualSign(pMsg->m_pAppMsg->m_pAppLayerSubPara);
		
	fre = Motor_Fre(step);
	
	if((motor_num == M12)||(motor_num==M11)||(motor_num==M10)||(motor_num==M05)||(motor_num==M06))
	{
		if(fre > 800)
			fre = 800;
	}
		
	if(pMsg->m_pAppMsg->m_pAppLayerSubPara[1] == '+') //CW
	{		
		if(dir == CCW)
			
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CW,pSpeed);
		
		else
			
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CCW,pSpeed);
		
		while(Stepper[motor_num].status != STOP){vTaskDelay(1);}
		
	}
	else if(pMsg->m_pAppMsg->m_pAppLayerSubPara[1] == '-')
	{	
		
		if(dir == CCW)
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CCW,pSpeed);
		else
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CW,pSpeed);
		
		while(Stepper[motor_num].status != STOP){vTaskDelay(1);}
		
	}
	else
	{
		Motor_Rst(Rstfre,motor_num,dir,Port,Pin,1);
	}
	vPortFree(pSpeed);
}

uint8_t Door_Control(uint8_t sta)
{	
	return 0;
}


void push_motor(void)
{
	MotorFre_Abs_Run(Globle.POCT_Par.M05_P1,M05,M05_DIR,800);
}

#include "StartTask.h"
TaskHandle_t VibrateTask_Handler;

void VibrateTask(void *pData)
{

	BaseType_t err = pdFALSE;

	while(1)
	{		
		if(Vibrate_SemaphoreHandle!=NULL)
		{
			err = xSemaphoreTake(Vibrate_SemaphoreHandle,portMAX_DELAY);
			
			if(err==pdTRUE)										              
			{
				MotorFre_Abs_Run(Globle.POCT_Par.M07_P1,M07,M07_DIR,MIX_FRE);
				vTaskDelay(1000);
				while(1)
				{
					MotorFre_Abs_Run(Globle.POCT_Par.M07_P1 - MIX_V,M07,M07_DIR,MIX_FRE);
					
					MotorFre_Abs_Run(Globle.POCT_Par.M07_P1,M07,M07_DIR,MIX_FRE);
					
					Wait_Time(Globle.washTime);
					
					vTaskDelay(5);
					
					if(xSemaphoreTake(Vibrate_SemaphoreHandle,0)==pdTRUE)									
					{							
						xSemaphoreGive(VibrateStop_SemaphoreHandle);
						
						Stepper[M07].status = STOP;
						
						break;
					}
				}				
			}
		}
		
		else if(err==pdFALSE)
		{
			vTaskDelay(10);           
		}	
		
		vTaskDelay(5);
		
	}
}


#endif 

	
	
