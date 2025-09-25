/**
  ******************************************************************************
  * @file    bsp_pid_conf.c
  * @author  ������
  * @version V1.0
  * @date    2021-11-16
  * @brief   POCT�������¶ȿ��ơ�
  ******************************************************************************
  * @attention
  *
  * ����ƽ̨:���ҽ�Ʒ������������
  *
  ******************************************************************************
  */ 
  
  #include "bsp_pid.h"
  #include "poct_process.h"
  
  PIDTypedef PID[5];
  
  
/*******************************************************************************
* Function Name  : PID_Init
* Description    : PID������ʼ��

*******************************************************************************/
void PID_Init(void)
{
	/*�����ڴ�*/
	int i = 0;
	for(i = 0;i < 5;i++)
	{		
		PID[i].err = 0;        //e(k)
		PID[i].next_err = 0;   //e(k-1)
		PID[i].last_err = 0;   //e(k-2)
		PID[i].target_temp = 0;
		PID[i].current_temp = 0;	
		PID[i].pwm = 0;
	}
	
	PID[0].Kp = 2;
	PID[0].Ki = 15;
	PID[0].Kd = 0.2;
	
	PID[1].Kp = 2;
	PID[1].Ki = 15;
	PID[1].Kd = 0.2;
	
	PID[2].Kp = 2;
	PID[2].Ki = 15;
	PID[2].Kd = 0.2;
	
	PID[3].Kp = 2;
	PID[3].Ki = 15;
	PID[3].Kd = 0.01;
	
	PID[4].Kp = 2;
	PID[4].Ki = 15;
	PID[4].Kd = 0.01;
}

/* ģ������ */
double fuzzy_rule(double error) {
    if (error <= -50.0) {
        return -10.0;
    } else if (error > -50.0 && error <= -25) {
        return -5;
    } else if (error > -25 && error <= 0.0) {
        return 0.0;
    } else if (error > 0.0 && error <= 25) {
        return 5;
    } else {
        return 10;
    }
}

/*******************************************************************************
* Function Name  : PID_Calculate
* Description    : PID����
* Input          : u16 Ŀ���¶�
* Return         : u8 �¶�״̬ 0 δ���� 1 ����
* Author         : Daguang
* Date           : 2021 - 11 - 16
*******************************************************************************/
void PID_Calculate(uint16_t target,uint8_t ch)
{ 	
		PID[ch].current_temp = GetTemp(ch); //�˴���Ҫ���һ��ʱ��   ʵʱ�¶�
	
//		PID[ch].target_temp = target;//Ŀ���¶�
	
		/*2.�������*/

		PID[ch].target_temp = (int)target;

		PID[ch].err = PID[ch].target_temp - PID[ch].current_temp; 

		if(PID[ch].err >= 120)
		{
			PID[ch].pwm = 999;
		}
		else if((PID[ch].err >= 50) && (PID[ch].err < 120))
		{
			PID[ch].pwm = 500;
		}
		else if(PID[ch].err < 0)  // ��ǰ�¶ȳ����趨�¶� 1�ȡ��ر�PID
		{
			PID[ch].pwm = 0;
		}
		else      
		{
			PID[ch].pwm += PID[ch].Kp*(PID[ch].err - PID[ch].next_err) + PID[ch].Ki*PID[ch].err + PID[ch].Kd*(PID[ch].err - 2*PID[ch].next_err + PID[ch].last_err);//����ʽPID�㷨 
			
			if(PID[ch].target_temp > 700)
			{
				if(PID[ch].pwm >= 400)
					PID[ch].pwm = 400;
			}
			else
			{
				if(PID[ch].pwm >= 150)
					PID[ch].pwm = 150;
//				else if(PID[ch].pwm <= 10)
//					PID[ch].pwm = 10;
			}
		}
		
		if(ch == 0)
		{		
			TIM_SetTIMCompare(TIM8,PID[ch].pwm,2);
		}
		else if(ch == 1)
		{
			TIM_SetTIMCompare(TIM8,PID[ch].pwm,3);
		}
		else if(ch == 2)
		{
			TIM_SetTIMCompare(TIM9,PID[ch].pwm,0);
		}
		else if(ch == 3)
		{
			TIM_SetTIMCompare(TIM8,PID[ch].pwm,0);
		} 		
		else if (ch == 4)
		{
			TIM_SetTIMCompare(TIM9,PID[ch].pwm,1);
		} 

		PID[ch].last_err = PID[ch].next_err;

		PID[ch].next_err = PID[ch].err;						
}
		 

/*******************************************************************************
* Function Name  : PID_Stop
* Description    : 

*******************************************************************************/
HAL_StatusTypeDef PID_Stop(uint8_t ch)
{
	HAL_StatusTypeDef state;
	switch(ch)
	{
		case 0:
		{
			if(HAL_TIM_PWM_Stop(&TIM8_Handler,TIM_CHANNEL_3) == HAL_OK)
			{
				Globle.TempControl.Temp_Ch1_ENABLE = RESET;
			}			
			break;
		}
		case 1:
		{
			TIM_SetTIMCompare(TIM8,0,3);
			
			if(HAL_TIM_PWM_Stop(&TIM8_Handler,TIM_CHANNEL_4) == HAL_OK)
			{
				Globle.TempControl.Temp_Ch2_ENABLE = RESET;
			}
			break;
		}
		case 2:
		{			
			if(HAL_TIM_PWM_Stop(&TIM9_Handler,TIM_CHANNEL_1) == HAL_OK)
			{
				Globle.TempControl.Temp_Ch3_ENABLE = RESET;
			}	
			break;
		}
			
		case 3:
		{				
			if(HAL_TIM_PWM_Stop(&TIM8_Handler,TIM_CHANNEL_1) == HAL_OK)
			{
				Globle.TempControl.Temp_Ch4_ENABLE = RESET;
			}
			break;
		}
			
		case 4:
		{				
			if(HAL_TIM_PWM_Stop(&TIM9_Handler,TIM_CHANNEL_2) == HAL_OK)
			{
				Globle.TempControl.Temp_Ch5_ENABLE = RESET;
			}			
			break;
		}
					
		default:
			break;
	}
	return state;
}

//����


