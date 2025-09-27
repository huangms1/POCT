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
  * ʵ��ƽ̨:���ҽ�Ʒ������������
  *
  ******************************************************************************
  */ 
  
  
#include "bsp_pwm_conf.h"
#include "InstrumentInclude.h"

TIM_HandleTypeDef TIM8_Handler;     

TIM_OC_InitTypeDef TIM8_CH1Handler; 
TIM_OC_InitTypeDef TIM8_CH3Handler; 
TIM_OC_InitTypeDef TIM8_CH4Handler; 

TIM_HandleTypeDef TIM9_Handler;     
TIM_OC_InitTypeDef TIM9_CH1Handler; 
TIM_OC_InitTypeDef TIM9_CH2Handler;


//��ʱ���ײ�������ʱ��ʹ�ܣ��������� 
//�˺����ᱻHAL_TIM_PWM_Init()���� 
//htim:��ʱ����� 
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) 
{
	int i = 0;
	GPIO_InitTypeDef GPIO_Initure;
	
	// ͨ��GPIO��������
    const struct {
        uint16_t pin;
        GPIO_TypeDef* port;
        uint32_t alternate;
    } gpio_config[] = {
        {TEMP_PWM_GPIO_PIN1, TEMP_PWM_GPIO_PORT1, GPIO_AF3_TIM8},
        {TEMP_PWM_GPIO_PIN2, TEMP_PWM_GPIO_PORT2, GPIO_AF3_TIM8},
        {TEMP_PWM_GPIO_PIN3, TEMP_PWM_GPIO_PORT3, GPIO_AF3_TIM9}
    };
		
    if(htim == NULL) return;
      
    // ����ʱ��
    TEMP_PWM_TIM_CLK1_ENABLE;
    TEMP_PWM_TIM_CLK3_ENABLE;
    TEMP_PWM_GPIO_CLK1_ENABLE();
    TEMP_PWM_GPIO_CLK2_ENABLE();
    TEMP_PWM_GPIO_CLK3_ENABLE();
				
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;

    // ����ͨ��GPIO
    for(i = 0; i < sizeof(gpio_config)/sizeof(gpio_config[0]); i++) {
        GPIO_Initure.Pin = gpio_config[i].pin;
        GPIO_Initure.Alternate = gpio_config[i].alternate;
        HAL_GPIO_Init(gpio_config[i].port, &GPIO_Initure);
    }

    // ��������
#ifdef POCT_ANM_H1L4_A
    GPIO_Initure.Pin = TEMP_PWM_GPIO_PIN4;
    GPIO_Initure.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(TEMP_PWM_GPIO_PORT4, &GPIO_Initure);
#elif defined POCT_ANM_H3L5_A
    GPIO_Initure.Pin = TEMP_PWM_GPIO_PIN4;
    GPIO_Initure.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(TEMP_PWM_GPIO_PORT4, &GPIO_Initure);
    
    HOT5_TIM_CLK_ENABLE;
    HOT5_GPIO_CLK_ENABLE();
    
    GPIO_Initure.Pin = HOT5_GPIO_PIN;
    GPIO_Initure.Alternate = GPIO_AF3_TIM9;
    HAL_GPIO_Init(HOT5_GPIO_PORT, &GPIO_Initure);
#elif defined POCT_HMN_H5L5_A
    HOT5_TIM_CLK_ENABLE;
    HOT5_GPIO_CLK_ENABLE();
    
    GPIO_Initure.Pin = HOT5_GPIO_PIN;
    GPIO_Initure.Alternate = GPIO_AF3_TIM9;
    HAL_GPIO_Init(HOT5_GPIO_PORT, &GPIO_Initure);
#endif
} 


//PWM (Ҳ����TIM8��ʼ��) 
//PWM�����ʼ�� 
//arr���Զ���װֵ 
//psc��ʱ��Ԥ��Ƶ�� 
void TIM8_PWM_Init(uint16_t arr,uint16_t psc) 
{
	TIM8_Handler.Instance = TIM8;                     //��ʱ��9  
	TIM8_Handler.Init.Prescaler   = psc;              //��ʱ����Ƶϵ��  
	TIM8_Handler.Init.CounterMode =TIM_COUNTERMODE_UP;//���ϼ���ģʽ  
	TIM8_Handler.Init.Period=arr;                     //�Զ���װ��ֵ  
	TIM8_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;  
	HAL_TIM_PWM_Init(&TIM8_Handler);                  //��ʼ��PWM 
	
#ifdef POCT_ANM_H1L4_A
	TIM8_CH1Handler.OCMode = TIM_OCMODE_PWM2;        //ģʽѡ��PWM2  
	TIM8_CH1Handler.Pulse=0;                         //���ñȽ�ֵ����ȷ��ռ�ձ�Ϊ50%  
	TIM8_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH;  //����Ƚϼ���Ϊ�� 
#endif	
	
#ifdef POCT_ANM_H3L5_A
	TIM8_CH1Handler.OCMode = TIM_OCMODE_PWM2;        //ģʽѡ��PWM2  
	TIM8_CH1Handler.Pulse=0;                         //���ñȽ�ֵ����ȷ��ռ�ձ�Ϊ50%  
	TIM8_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH;  //����Ƚϼ���Ϊ�� 
#endif	
	
	TIM8_CH3Handler.OCMode = TIM_OCMODE_PWM2;        //ģʽѡ��PWM2  
	TIM8_CH3Handler.Pulse  = 0;                         //���ñȽ�ֵ����ȷ��ռ�ձ�Ϊ50%  
	TIM8_CH3Handler.OCPolarity=TIM_OCPOLARITY_HIGH;  //����Ƚϼ���Ϊ�� 	
	
	TIM8_CH4Handler.OCMode = TIM_OCMODE_PWM2;        //ģʽѡ��PWM2  
	TIM8_CH4Handler.Pulse = 0;                         //���ñȽ�ֵ����ȷ��ռ�ձ�Ϊ50%  
	TIM8_CH4Handler.OCPolarity=TIM_OCPOLARITY_HIGH;  //����Ƚϼ���Ϊ�� 
	
#ifdef POCT_ANM_H1L4_A
	HAL_TIM_PWM_ConfigChannel(&TIM8_Handler,&TIM8_CH1Handler, TIM_CHANNEL_1);//����TIM8ͨ��3	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_1);                          //����PWMͨ��2
#endif	
	
#ifdef POCT_ANM_H3L5_A
	HAL_TIM_PWM_ConfigChannel(&TIM8_Handler,&TIM8_CH1Handler, TIM_CHANNEL_1);//����TIM8ͨ��3	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_1);                          //����PWMͨ��2
#endif	

	HAL_TIM_PWM_ConfigChannel(&TIM8_Handler,&TIM8_CH3Handler, TIM_CHANNEL_3);//����TIM8ͨ��3	
	HAL_TIM_PWM_ConfigChannel(&TIM8_Handler,&TIM8_CH4Handler, TIM_CHANNEL_4);//����TIM8ͨ��4 
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_3);//����PWMͨ��2
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_4);//����PWMͨ��2
}

//PWM (Ҳ����TIM9��ʼ��) 
//PWM�����ʼ�� 
//arr���Զ���װֵ 
//psc��ʱ��Ԥ��Ƶ�� 
void TIM9_PWM_Init(uint16_t arr,uint16_t psc) 
{
	TIM9_Handler.Instance = TIM9;                                            //��ʱ��9  
	TIM9_Handler.Init.Prescaler=psc;                                         //��ʱ����Ƶϵ��  
	TIM9_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;                        //���ϼ���ģʽ  
	TIM9_Handler.Init.Period=arr;                                            //�Զ���װ��ֵ  
	TIM9_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;  
	HAL_TIM_PWM_Init(&TIM9_Handler);                                         //��ʼ��PWM 
	
	TIM9_CH1Handler.OCMode = TIM_OCMODE_PWM2;                                  //ģʽѡ��PWM2  
	TIM9_CH1Handler.Pulse = 0;                                                 //���ñȽ�ֵ����ȷ��ռ�ձ�Ϊ50%  
	TIM9_CH1Handler.OCPolarity=TIM_OCPOLARITY_HIGH;                           //����Ƚϼ���Ϊ��  	
	HAL_TIM_PWM_ConfigChannel(&TIM9_Handler,&TIM9_CH1Handler, TIM_CHANNEL_1);//����TIM9ͨ��1
	
	TIM9_CH2Handler.OCMode     = TIM_OCMODE_PWM2;                                  //ģʽѡ��PWM2  
	TIM9_CH2Handler.Pulse      = 0;                                                 //���ñȽ�ֵ����ȷ��ռ�ձ�Ϊ50%  
	TIM9_CH2Handler.OCPolarity = TIM_OCPOLARITY_HIGH;                          //����Ƚϼ���Ϊ��  
	HAL_TIM_PWM_ConfigChannel(&TIM9_Handler,&TIM9_CH2Handler, TIM_CHANNEL_2);//����TIM9ͨ��2
	
	HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_1);                          //����PWMͨ��2
	
	HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_2);                          //����PWMͨ��2			
}

//����TIMͨ����ռ�ձ� 
//TIM_TypeDef:��ʱ�� (1~14)
//compare:�Ƚ�ֵ 
//ch:ͨ���� 
void TIM_SetTIMCompare(TIM_TypeDef *TIM,uint32_t compare,uint8_t ch)
{	
	if (ch == 0)
	{
	  TIM->CCR1 = compare;
	}
	if (ch == 1)
	{
	  TIM->CCR2 = compare;
	}
	if (ch == 2)
	{
	  TIM->CCR3 = compare;	
	}
	if (ch == 3)
	{
	  TIM->CCR4 = compare;	
	}
}

//��ʼ��PWM 

void TEMP_TIM_PWM_Init(void)
{
//	HAL_TIM_PWM_MspInit();	
	TIM8_PWM_Init(999,260-1); //��һ·�͵ڶ�·�¿ء�
	
	TIM9_PWM_Init(999,260-1); //����·�¿ء�	
}






