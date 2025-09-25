/*
  * @file    bsp_motor.c
  * @author  hms 
  * @version V1.0
  * @date    2021.10.19
  * @brief   
  ***********************
  * @attention
  * 
  *
  ***********************
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

/*************************************宏定义***********************************************/


/***********************************全局变量************************************************/
Stepper_Typedef      Stepper[TOTAL_MOTOR]; 

DOORState_TypeDef    DoorState;

SpeedCalc_TypeDef *  pSpeedGloble[3];    
/******************************************************************************************/

/**
 * @brief 停止指定电机的运行
 * @param motor_num: 电机编号（0-based索引）
 * @param pSpeed: 速度计算相关结构体指针（包含定时器句柄等信息）
 * @note 此函数会关闭电机驱动、停止脉冲输出并重置相关状态变量
 */
void Motor_Stop(uint8_t motor_num,SpeedCalc_TypeDef * pSpeed)
{
    // 设置电机状态为停止状态（状态机关键标志）
    Stepper[motor_num].status = STOP;

    // 重置电机索引（用于步数计数或状态切换的循环索引）
    Stepper[motor_num].index = 0;

    // 关闭电机驱动：拉高使能引脚（根据硬件设计，HIGH1表示禁用驱动）
    HAL_GPIO_WritePin(Motor_Conf[motor_num].en_port, Motor_Conf[motor_num].en_pin, HIGH1);

    // 停止定时器输出比较中断：终止PWM脉冲输出（核心停止控制操作）
    HAL_TIM_OC_Stop_IT(pSpeed->htim, TIM_CHANNEL_1);

    // 重置位置计数器（用于绝对位置控制场景，清零当前位置记录）
    Stepper[motor_num].pos = 0;

    // 清除启动标志：防止因状态残留导致电机误启动
    Stepper[motor_num].start_flag = 0;
}
  
/**
 * @brief 计算电机加速和减速过程中的速度曲线
 *
 * 该函数根据给定的初始速度、最终速度和加速时间，计算电机在加速和减速过程中的速度曲线，并将结果存储在 pSpeed 结构体中。
 *
 * @param Vo 初始速度
 * @param Vt 最终速度
 * @param T 加速时间
 * @param pSpeed 存储速度计算结果的结构体指针
 * @return 如果计算成功，返回 true；如果加速总步数超过 FORM_LEN，返回 false
 */
bool pMCalcSpeed(int32_t Vo, int32_t Vt, float T, SpeedCalc_TypeDef * pSpeed)
{

    int32_t i = 0;
    float Vm = 0.0f;
    float K = 0.0f;
    float Ti = 0.0f;
    float Sumt = 0.0f;
    float DeltaV = 0.0f;
    float temp = 0.0f;
	
	    // 检查输入参数的有效性
    if (pSpeed == NULL) {
        return false;
    }

    // 转换初始速度和最终速度
    pSpeed->Vo = CONVER(Vo);
    pSpeed->Vt = CONVER(Vt);

    // 计算加速时间的一半
    T = T / 2.0f;

    // 计算平均速度
    Vm = (pSpeed->Vo + pSpeed->Vt) / 2.0f;

    // 计算加速度系数 K
    K = fabsf((2.0f * (Vm - pSpeed->Vo)) / (T * T));

    // 计算加速阶段的总步数
    pSpeed->INC_AccelTotalStep = (int32_t)((pSpeed->Vo * T) + ((K * T * T * T) / 6.0f));

    // 计算减速阶段的总步数
    pSpeed->Dec_AccelTotalStep = (int32_t)(((pSpeed->Vt + pSpeed->Vo) * T - pSpeed->INC_AccelTotalStep));

    // 计算加速和减速阶段的总步数
    pSpeed->AccelTotalStep = pSpeed->Dec_AccelTotalStep + pSpeed->INC_AccelTotalStep;

    // 确保总步数为偶数
    if (pSpeed->AccelTotalStep % 2 != 0) {
        pSpeed->AccelTotalStep += 1;
    }

    // 检查总步数是否超过 FORM_LEN
    if (FORM_LEN < pSpeed->AccelTotalStep) {
        return false;
    }

    // 计算初始时间间隔 Ti
    Ti = pow(6.0f * 1.0f / K, 1.0f / 3.0f);

    // 累加时间总和
    Sumt += Ti;

    // 计算速度增量
    DeltaV = 0.5f * K * Sumt * Sumt;

    // 计算初始速度
    pSpeed->Form[0] = pSpeed->Vo + DeltaV;

    // 确保初始速度不低于最小速度
    if (pSpeed->Form[0] <= MIN_SPEED) {
        pSpeed->Form[0] = MIN_SPEED;
    }

    // 计算加速和减速过程中的速度曲线
    for (i = 1; i < pSpeed->AccelTotalStep; i++) {
        // 计算当前时间间隔
        Ti = 1.0f / pSpeed->Form[i - 1];

        if (i < pSpeed->INC_AccelTotalStep) {
            // 加速阶段
            Sumt += Ti;
            DeltaV = 0.5f * K * Sumt * Sumt;
            pSpeed->Form[i] = pSpeed->Vo + DeltaV;

            // 调整时间总和
            if (i == pSpeed->INC_AccelTotalStep - 1) {
                Sumt = fabsf(Sumt - T);
            }
        } else {
            // 减速阶段
            Sumt += Ti;
            temp = fabsf(T - Sumt);
            DeltaV = 0.5f * K * temp * temp;
            pSpeed->Form[i] = pSpeed->Vt - DeltaV;
        }
    }

    return true;
}


void motor_TIM_start(TIM_HandleTypeDef *htim,uint32_t TIM_channelX,uint32_t pluse_time)
{
	__HAL_TIM_SET_COUNTER(htim, 0);
						  
	__HAL_TIM_SET_COMPARE(htim, TIM_channelX, pluse_time);
	
	HAL_TIM_OC_Start_IT(htim, TIM_channelX);	
	
	TIM_CCxChannelCmd(htim->Instance, TIM_channelX, TIM_CCx_ENABLE);
}

/**
 * @brief 设置电机方向
 * @param motor_num 电机编号
 * @param dir 方向(CCW或CW)
 */
void set_motor_dir(uint8_t motor_num, uint8_t dir)
{
    GPIO_PinState pin_state = (dir == CCW) ? CCW : CW;
    HAL_GPIO_WritePin(Motor_Conf[motor_num].dir_port, 
                     Motor_Conf[motor_num].dir_pin, 
                     pin_state);
}


/*********************************************
//  电机使能。
//  电机编号。
*********************************************/
void motor_enable(uint8_t motor_num)
{	
	HAL_GPIO_WritePin(Motor_Conf[motor_num].en_port,Motor_Conf[motor_num].en_pin,LOW1);
}


/**********************************************
//   电机脉冲电平翻转。
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
		
		default: break;
	}
}

// ... existing code ...

static void handle_motor_status(uint8_t motor_num, SpeedCalc_TypeDef *pSpeed, TIM_HandleTypeDef *htim, uint32_t Channel) {
    if (pSpeed->deceleration_flag == 1) {
        if (Stepper[motor_num].status == UNIFORM) {
            if (Stepper[motor_num].pos >= (pSpeed->TotalStep - 1)) {
                Stepper[motor_num].index = 0;
                Stepper[motor_num].status = STOP;
                HAL_TIM_OC_Stop_IT(htim, Channel);
                Motor_Stop(motor_num, pSpeed);
            }
        }
    } else {
        switch (Stepper[motor_num].status) {
            case ACCEL:
                if (Stepper[motor_num].pos >= (pSpeed->AccelTotalStep - 1)) {
                    Stepper[motor_num].status = UNIFORM;
                    Stepper[motor_num].index -= 1;
                } else {
                    Stepper[motor_num].pluse_time = (uint16_t)(T1_FREQ / pSpeed->Form[Stepper[motor_num].index] / 2U);
                    Stepper[motor_num].index++;
                }
                break;
            case DECEL:
                if (Stepper[motor_num].pos >= (pSpeed->TotalStep - 1)) {
                    Stepper[motor_num].index = 0;
                    Stepper[motor_num].status = STOP;
                    HAL_TIM_OC_Stop_IT(htim, Channel);
                    Motor_Stop(motor_num, pSpeed);
                } else {
                    Stepper[motor_num].pluse_time = (uint16_t)(T1_FREQ / pSpeed->Form[Stepper[motor_num].index] / 2U);
                    Stepper[motor_num].index--;
                }
                break;
            case UNIFORM:
                if (Stepper[motor_num].pos >= pSpeed->DecPoint) {
                    Stepper[motor_num].status = DECEL;
                }
                break;
        }
    }
}

// 提取公共逻辑到新函数
static void common_speed_decision_logic(TIM_HandleTypeDef *htim, uint32_t TIM_IT_CCn, uint32_t Channel, uint8_t motor_num, SpeedCalc_TypeDef *pSpeed) {
    static __IO uint8_t i = 0;
    uint32_t tim_count = 0;
    uint16_t TIM_arr = 0;
    uint16_t tmp = 0;

    if (__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_CCn) != RESET) {
        __HAL_TIM_CLEAR_IT(htim, TIM_IT_CCn);
        motor_leve(motor_num);

        i++;
        if (i == 2) {
            i = 0;
            handle_motor_status(motor_num, pSpeed, htim, Channel);
            Stepper[motor_num].pos++;
        }

        tim_count = __HAL_TIM_GET_COUNTER(htim);

        if (pSpeed->deceleration_flag != 1) {
            tmp = tim_count + Stepper[motor_num].pluse_time;
            __HAL_TIM_SET_COMPARE(htim, Channel, tmp);
        } else {
            TIM_arr = __HAL_TIM_GET_AUTORELOAD(htim);
            __HAL_TIM_SET_COMPARE(htim, Channel, TIM_arr / 2);
        }
    }
}

/**
 * @brief 处理 TIM2 定时器的电机速度决策逻辑
 * @param htim 定时器句柄
 * @param TIM_IT_CCn 定时器中断源
 * @param Channel 定时器通道
 * @param motor_num 电机编号
 * @param pSpeed 速度计算结构体指针
 */
void Speed_Decision_TIM2(TIM_HandleTypeDef *htim, uint32_t TIM_IT_CCn, uint32_t Channel, uint8_t motor_num, SpeedCalc_TypeDef *pSpeed) {
    common_speed_decision_logic(htim, TIM_IT_CCn, Channel, motor_num, pSpeed);
}

/**
 * @brief 处理 TIM3 定时器的电机速度决策逻辑
 * @param htim 定时器句柄
 * @param TIM_IT_CCn 定时器中断源
 * @param Channel 定时器通道
 * @param motor_num 电机编号
 * @param pSpeed 速度计算结构体指针
 */
void Speed_Decision_TIM3(TIM_HandleTypeDef *htim, uint32_t TIM_IT_CCn, uint32_t Channel, uint8_t motor_num, SpeedCalc_TypeDef *pSpeed) {
    common_speed_decision_logic(htim, TIM_IT_CCn, Channel, motor_num, pSpeed);
}

/**
 * @brief 处理 TIM4 定时器的电机速度决策逻辑
 * @param htim 定时器句柄
 * @param TIM_IT_CCn 定时器中断源
 * @param Channel 定时器通道
 * @param motor_num 电机编号
 * @param pSpeed 速度计算结构体指针
 */
void Speed_Decision_TIM4(TIM_HandleTypeDef *htim, uint32_t TIM_IT_CCn, uint32_t Channel, uint8_t motor_num, SpeedCalc_TypeDef *pSpeed) {
    common_speed_decision_logic(htim, TIM_IT_CCn, Channel, motor_num, pSpeed);
}

// ... existing code ...

/**
 * @brief 根据电机编号选择合适的定时器和通道
 * @param motor_num 电机编号
 * @param pSpeed 速度计算结构体指针
 * @return 定时器句柄指针
 */
TIM_HandleTypeDef* select_timer_and_channel(uint8_t motor_num, SpeedCalc_TypeDef * pSpeed) {
    TIM_HandleTypeDef* htim = NULL;
    if ((motor_num == M01) || (motor_num == M02) || (motor_num == M03) || (motor_num == M04)) {
        pSpeedGloble[0] = pSpeed;
        pSpeedGloble[0]->htim = &TIM2_Handler;
        pSpeedGloble[0]->motor_num = motor_num;
        htim = &TIM2_Handler;
    } else if ((motor_num == M05) || (motor_num == M06) || (motor_num == M07) || (motor_num == M08)
               || (motor_num == M09) || (motor_num == M11)) {
        pSpeedGloble[1] = pSpeed;
        pSpeedGloble[1]->htim = &TIM3_Handler;
        pSpeedGloble[1]->motor_num = motor_num;
        htim = &TIM3_Handler;
    } else {
        pSpeedGloble[2] = pSpeed;
        pSpeedGloble[2]->htim = &TIM4_Handler;
        pSpeedGloble[2]->motor_num = motor_num;
        htim = &TIM4_Handler;
    }
    return htim;
}

/**
 * @brief 步进电机运动控制函数
 * @param start_speed 起始速度
 * @param end_speed 结束速度
 * @param acc_time 加速时间
 * @param step 步数
 * @param motor_num 电机编号
 * @param dir 电机方向
 * @param pSpeed 速度计算结构体指针
 * @return 操作是否成功，成功返回 true，失败返回 false
 */
bool Stepper_Move_S(int16_t start_speed, int16_t end_speed, float acc_time, int32_t step, uint8_t motor_num, uint8_t dir, SpeedCalc_TypeDef * pSpeed) {
	TIM_HandleTypeDef*  htim = NULL;
    // 检查输入参数的有效性
    if (pSpeed == NULL || step <= 0 || Stepper[motor_num].status != STOP) {
        return false;
    }

    // 计算速度曲线
    if (!pMCalcSpeed(start_speed, end_speed, acc_time, pSpeed)) {
        return false;
    }

    // 使能电机并设置方向
    motor_enable(motor_num);
    set_motor_dir(motor_num, dir);

    // 根据步数设置电机状态和脉冲时间
    if (step >= pSpeed->AccelTotalStep * 2) {
        if (pSpeed->Form[0] == 0) {
            Stepper[motor_num].pluse_time = 1500;
        } else {
            pSpeed->TotalStep = step;
            pSpeed->DecPoint = pSpeed->TotalStep - pSpeed->AccelTotalStep;
            Stepper[motor_num].status = ACCEL;
            Stepper[motor_num].pluse_time = (uint16_t)(T1_FREQ / pSpeed->Form[0] / 2U);
        }
    } else {
        pSpeed->TotalStep = step;
        pSpeed->deceleration_flag = 1;
        Stepper[motor_num].pluse_time = 1500;
        Stepper[motor_num].status = UNIFORM;
    }

    // 初始化电机位置和启动标志
    Stepper[motor_num].pos = 0;
    Stepper[motor_num].start_flag = 1;

    // 选择合适的定时器和通道
    htim = select_timer_and_channel(motor_num, pSpeed);
    if (htim != NULL) {
        motor_TIM_start(htim, TIM_CHANNEL_1, Stepper[motor_num].pluse_time);
        return true;
    }

    return false;
}

//// 电机复位运行子函数
//static uint8_t motor_rst_run(uint8_t motor_num, uint16_t fre, uint8_t dir, 
//                           GPIO_TypeDef *Port, uint16_t Pin, 
//                           SpeedCalc_TypeDef *pSpeed, uint32_t *timeout)
//{
//    Stepper_Move_S(MIN_FRE, fre, 0.01f, RST_MAX_STEP, motor_num, dir, pSpeed);
//    
//    while (HAL_GPIO_ReadPin(Port, Pin) == (dir == CCW ? RESET : SET)) {
//        if ((*timeout)++ > MOTOR_RES_TIMEOUT * 1000) {
//            Motor_Stop(motor_num, pSpeed);
//            return 1;
//        }
//        vTaskDelay(1);
//    }
//    Motor_Stop(motor_num, pSpeed);
//    *timeout = 0;
//    return 0;
//}

///**
// * @brief 电机复位函数(优化版)
// * @param fre 电机运行频率
// * @param motor_num 电机编号
// * @param dir 电机运行方向
// * @param Port GPIO端口
// * @param Pin GPIO引脚 
// * @param reversalEnable 是否启用反转
// * @return 0-成功 1-超时失败
// */
//uint8_t Motor_Rst(uint16_t fre, uint8_t motor_num, uint8_t dir, 
//                 GPIO_TypeDef *Port, uint16_t Pin, uint8_t reversalEnable) 
//{
//    uint32_t timeout = 0;
//    SpeedCalc_TypeDef *pSpeed = pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
//    
//    if (pSpeed == NULL) {
//        SaveFaultCode("F^MOTOR_RST_MEM_ALLOC_FAILED");
//        return 1;
//    }

//    // 根据GPIO状态执行不同复位流程
//    if (HAL_GPIO_ReadPin(Port, Pin) == SET) {
//        // 正向复位流程
//        if (motor_rst_run(motor_num, fre, dir, Port, Pin, pSpeed, &timeout)) {
//            vPortFree(pSpeed);
//            return 1;
//        }
//    } else {
//        // 反向复位流程
//        uint8_t reverse_dir = (dir == CCW) ? CW : CCW;
//        
//        // 第一阶段复位
//        if (motor_rst_run(motor_num, 300, reverse_dir, Port, Pin, pSpeed, &timeout)) {
//            vPortFree(pSpeed);
//            return 1;
//        }
//        
//        // 第二阶段短距离移动
//        Stepper_Move_S(MIN_FRE, 300, 0.01f, 2000, motor_num, reverse_dir, pSpeed);
//        
//        // 等待条件满足
//        if (reversalEnable) {
//            while (Stepper[motor_num].status != STOP) vTaskDelay(1);
//        } else {
//            while (HAL_GPIO_ReadPin(Port, Pin) == RESET) vTaskDelay(1);
//        }
//        Motor_Stop(motor_num, pSpeed);
//        
//        // 最终复位
//        if (motor_rst_run(motor_num, 300, dir, Port, Pin, pSpeed, &timeout)) {
//            vPortFree(pSpeed);
//            return 1;
//        }
//    }

//    Stepper[motor_num].position = 0;
//    vPortFree(pSpeed);
//    return 0;
//}

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
			Stepper_Move_S(MIN_FRE,500,0.01f, RST_MAX_STEP,motor_num,CW,pSpeed); 
			
			while (HAL_GPIO_ReadPin(Port,Pin) != SET)
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
			Motor_Stop(motor_num,pSpeed);
		
			Stepper_Move_S(MIN_FRE,500,0.01f, 3000,motor_num,CW,pSpeed); 
			
		}
		else
		{
			Stepper_Move_S(MIN_FRE,500,0.01f, RST_MAX_STEP,motor_num,CCW,pSpeed); 
			
			while (HAL_GPIO_ReadPin(Port,Pin) != SET)
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
			Motor_Stop(motor_num,pSpeed);
		
			Stepper_Move_S(MIN_FRE,500,0.01f, 3000,motor_num,CCW,pSpeed);			
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
			Stepper_Move_S(MIN_FRE,500,0.01f, RST_MAX_STEP,motor_num,CCW,pSpeed);
		}
		else
		{
			Stepper_Move_S(MIN_FRE,500,0.01f, RST_MAX_STEP,motor_num,CW,pSpeed);
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
			Stepper_Move_S(MIN_FRE,500,0.01f, 3000,motor_num,CW,pSpeed); 
		}
		else
		{
			Stepper_Move_S(MIN_FRE,500,0.01f, 3000,motor_num,CCW,pSpeed);
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

/**
 * @brief 根据步数计算电机运行频率
 * @param step 电机需要运行的步数
 * @return 计算得到的电机运行频率(Hz)
 */
uint16_t Motor_Fre(uint32_t step) 
{
    // 基础频率计算
    uint16_t frequency = (uint16_t)(1.5f * step + 100);
    
    // 频率上限限制(3500Hz)
    if (frequency > 3500u) {
        frequency = 3500u;
    }
    // 频率下限优化(1500Hz以下使用不同斜率)
    else if (frequency < 1500u) {
        frequency = (uint16_t)(0.8f * step + 200);
    }
    
    return frequency;
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

/**
 * @brief 电机绝对位置运行函数
 *
 * 该函数用于控制电机运行到指定的绝对位置。根据目标位置和当前位置的差值，
 * 计算电机需要运行的步数，并根据步数计算电机运行频率，然后调用 Stepper_Move_S 函数控制电机运行。
 * 最后更新电机的当前位置。
 *
 * @param step 目标绝对位置
 * @param motor_num 电机编号
 */
void Motor_Abs_Run(uint32_t step, uint8_t motor_num)
{
    uint16_t fre = 0;
    uint32_t steps_to_move = 0;
    uint8_t direction = 0;
    SpeedCalc_TypeDef * pSpeed = NULL;

    // 分配内存
    pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
    if (pSpeed == NULL) {
        // 内存分配失败，保存错误代码并返回
        SaveFaultCode("F^MOTOR_ABS_RUN_MEM_ALLOC_FAILED");
        return;
    }

    // 计算需要移动的步数和方向
    if (step >= Stepper[motor_num].position) {
        steps_to_move = step - Stepper[motor_num].position;
        direction = CCW;
    } else {
        steps_to_move = Stepper[motor_num].position - step;
        direction = CW;
    }

    // 计算电机运行频率
    fre = Motor_Fre(steps_to_move);

    // 调用步进电机运动控制函数
    Stepper_Move_S(MIN_FRE, fre, 0.01f, steps_to_move, motor_num, direction, pSpeed);

    // 等待电机停止
    while (Stepper[motor_num].status != STOP) {
        vTaskDelay(1);
    }

    // 更新电机位置
    if (direction == CCW) {
        Stepper[motor_num].position += steps_to_move;
    } else {
        Stepper[motor_num].position -= steps_to_move;
    }

    // 释放内存
    vPortFree(pSpeed);
}

//电机相对运动
/**
 * @brief 电机相对位置运行函数
 *
 * 该函数用于控制电机相对当前位置移动指定的步数。根据目标相对位置和当前位置，
 * 计算电机需要移动的步数和方向，然后调用 Stepper_Move_S 函数控制电机运行。
 * 最后更新电机的当前位置。
 *
 * @param step 目标相对位置
 * @param motor_num 电机编号
 */
void Motor_Rel_Run(int32_t step, uint8_t motor_num) {
	uint16_t fre = 0;
    uint32_t steps_to_move = 0;
    uint8_t direction = 0;
    SpeedCalc_TypeDef * pSpeed = NULL;

    // 分配内存
    pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
    if (pSpeed == NULL) {
        // 内存分配失败，保存错误代码并返回
        SaveFaultCode("F^MOTOR_REL_RUN_MEM_ALLOC_FAILED");
        return;
    }

    // 计算需要移动的步数和方向
    if (step >= 0) {
    	steps_to_move = step;
        direction = CCW;
    }
    else {
    	steps_to_move = -step;
        direction = CW;
    }

    // 计算电机运行频率
    fre = Motor_Fre(steps_to_move);

    // 调用步进电机运动控制函数
    Stepper_Move_S(MIN_FRE, fre, 0.01f, steps_to_move, motor_num, direction, pSpeed);

    // 等待电机停止
    while (Stepper[motor_num].status!= STOP) {
        vTaskDelay(1);
    }

    // 更新电机位置
    if (direction == CCW) {
        Stepper[motor_num].position += steps_to_move;
    } else {
        Stepper[motor_num].position -= steps_to_move;
    }

    // 释放内存
    vPortFree(pSpeed);   
}


/**
 * @brief 电机以指定频率运行到绝对位置
 * @param step 目标绝对位置
 * @param motor_num 电机编号
 * @param dir 电机方向
 * @param fre 电机运行频率
 */
void MotorFre_Abs_Run(uint32_t step, uint8_t motor_num, uint8_t dir, uint16_t fre) {
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

// ... existing code ...

/**
 * @brief 控制电机运行指定步数
 *
 * 该函数用于控制指定电机按照给定的步数、方向和频率运行。
 * 它会分配内存用于速度计算，调用 Stepper_Move_S 函数启动电机，
 * 并等待电机运行结束，最后释放分配的内存。
 *
 * @param step 电机需要运行的步数
 * @param motor_num 电机编号
 * @param dir 电机运行方向
 * @param fre 电机运行频率
 */
void Motor_Run(uint32_t step, uint8_t motor_num, uint8_t dir, uint16_t fre)
{
    SpeedCalc_TypeDef * pSpeed = NULL;
    uint32_t timeout = 0;
    const uint32_t MAX_TIMEOUT = 20000; // 最大超时时间，可根据实际情况调整

    // 分配内存
    pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
    if (pSpeed == NULL) {
        // 内存分配失败，保存错误代码并返回
        SaveFaultCode("F^MOTOR_RUN_MEM_ALLOC_FAILED");
        return;
    }

    // 调用步进电机运动控制函数
    if (!Stepper_Move_S(MIN_FRE, fre, 0.01f, step, motor_num, dir, pSpeed)) {
        // 电机启动失败，释放内存并返回
        vPortFree(pSpeed);
        SaveFaultCode("F^MOTOR_RUN_START_FAILED");
        return;
    }

    // 等待电机停止，添加超时机制
    while (Stepper[motor_num].status != STOP) {
        if (timeout++ > MAX_TIMEOUT) {
            // 超时，停止电机，释放内存并保存错误代码
            Motor_Stop(motor_num, pSpeed);
            vPortFree(pSpeed);
            SaveFaultCode("F^MOTOR_RUN_TIMEOUT");
            return;
        }
        vTaskDelay(1);
    }

    // 释放内存
    vPortFree(pSpeed);
}


void scan_module_reset(void)
{
    // 重置M08电机
    motor_fast_rst(M08, M08_DIR, M08_GO_GPIO_PORT, M08_GO_GPIO_PIN);
    // reset_single_motor(M08, M08_DIR, M08_GO_GPIO_PORT, M08_GO_GPIO_PIN);
    
    // 重置M03电机
    motor_fast_rst(M03, M03_DIR, M03_GO_GPIO_PORT, M03_GO_GPIO_PIN);
    // reset_single_motor(M03, M03_DIR, M03_GO_GPIO_PORT, M03_GO_GPIO_PIN);
}

// 辅助函数：构造错误信息
static void build_error_message(char* str, uint8_t motor_num) {
    motor_num = motor_num + 1;
    itoa(motor_num, (uint8_t*)str, 10);
    strcat(str, "-MOTOR TIME OUT!");
}

/**
 * @brief 快速重置电机位置(优化版)
 * 
 * 该函数用于快速重置指定电机的位置，包含错误处理和日志记录功能。
 * 
 * @param motor_num 电机编号
 * @param dir 电机运行方向
 * @param gpio_port GPIO端口
 * @param gpio_pin GPIO引脚
 * @return 复位是否成功，成功返回true，失败返回false
 */
bool motor_fast_rst(uint8_t motor_num, uint8_t dir, GPIO_TypeDef *gpio_port, uint16_t gpio_pin)
{
    uint8_t err = 0;
    char *str = pvPortMalloc(25);
    if (str == NULL) {
        SaveFaultCode("F^MOTOR_FAST_RST_MEM_ALLOC_FAILED");
        return false;
    }
    memset(str, 0, 25);

    // 如果当前位置超过3800，先移动到500位置
    if (Stepper[motor_num].position > 3800) {
        MotorFre_Abs_Run(500, motor_num, dir, 3800);    
    }

    delay_ms(1);

    // 执行电机复位
    err = Motor_Rst(800, motor_num, dir, gpio_port, gpio_pin, 0);
    if (err != 0) {
        build_error_message(str, motor_num);
        SaveFaultCode(str);
        vPortFree(str);
        return false;
    }

    vPortFree(str);
    return true;
}

/*********************************************END OF FILE**********************/
