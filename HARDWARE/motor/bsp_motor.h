#ifndef __BSP_MOTOR_H
#define	__BSP_MOTOR_H

// 标准库头文件
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "includes.h"
#include <stdbool.h>
#include "InstrumentInclude.h"

/* 电机参数配置 */
#define TOTAL_MOTOR         13       // 电机总数
#define FORM_LEN            5000     // 速度曲线数组长度
#define MIN_FRE             50       // 最小频率(Hz)
#define RST_MAX_STEP        0xFFFFFF // 复位最大步数
#define MOTOR_RES_TIMEOUT   25       // 复位超时时间(s)

/* 步进电机计算参数 */
#define T1_FREQ            (SystemCoreClock / TIM_PRESCALER)  // 定时器频率
#define STEP_ANGLE         1.8f      // 步距角(度)
#define FSPR               (360.0f / STEP_ANGLE) // 每转步数
#define MICRO_STEP         16        // 微步数
#define SPR                (FSPR * MICRO_STEP) // 每转微步数
#define CONVER(speed)      ((float)(speed * SPR / 60.0f)) // 速度转换
#define MIN_SPEED          ((float)(T1_FREQ / TIM_PERIOD)) // 最小速度
 	
/* 速度计算结构体 */
typedef struct {
    __IO int32_t Vo;                // 初始速度
    __IO int32_t Vt;                // 目标速度
    __IO int32_t AccelTotalStep;    // 总加速步数
    __IO int32_t DecPoint;          // 减速点
    __IO int32_t TotalStep;         // 总步数
    __IO int32_t INC_AccelTotalStep;// 加速阶段步数
    __IO int32_t Dec_AccelTotalStep;// 减速阶段步数
    __IO float   Form[FORM_LEN];    // 速度曲线数组
    __IO uint8_t deceleration_flag; // 减速标志
    TIM_HandleTypeDef *htim;        // 定时器句柄
    __IO uint8_t motor_num;         // 电机编号
} SpeedCalc_TypeDef;

extern SpeedCalc_TypeDef * pSpeedGloble[3];

/* 电机状态结构体 */
typedef struct {
    __IO uint8_t  status;           // 运行状态
    __IO uint8_t  dir;              // 方向
    __IO uint32_t pos;              // 当前位置
    __IO uint32_t position;         // 绝对位置
    __IO uint32_t pluse_time;       // 脉冲时间
    __IO uint16_t start_flag;       // 启动标志
    __IO uint32_t index;            // 索引
} Stepper_Typedef;

extern Stepper_Typedef Stepper[TOTAL_MOTOR];

typedef enum {
  STOP = 0,                        
  ACCEL,                           
  UNIFORM,                         
  DECEL,                           
}StateEnum_TypeDef;

typedef enum {
  CLOSE = 0U,                       
  OPPEN,                          
}DOOREnum_TypeDef;

typedef struct DOOR{
	__IO uint8_t Door_sta;
}DOORState_TypeDef;

extern DOORState_TypeDef DoorState;

typedef enum MOTOR_NUM
{	
    M01 = 0U, 
	M02, 
	M03, 
	M04, 
	M05, 
	M06, 
	M07,
	M08,
	M09,
	M10,
	M11,
	M12,
//	M13,
//	M14,
	M15	
}MOTOR_NUM;

void Motor_Stop(uint8_t motor_num,SpeedCalc_TypeDef * pSpeed);

void Speed_Decision_TIM2(TIM_HandleTypeDef *htim,uint32_t TIM_IT_CCn,uint32_t Channel,uint8_t motor_num,SpeedCalc_TypeDef * pSpeed);

void Speed_Decision_TIM3(TIM_HandleTypeDef *htim,uint32_t TIM_IT_CCn,uint32_t Channel,uint8_t motor_num,SpeedCalc_TypeDef * pSpeed);

void Speed_Decision_TIM4(TIM_HandleTypeDef *htim,uint32_t TIM_IT_CCn,uint32_t Channel,uint8_t motor_num,SpeedCalc_TypeDef * pSpeed);

bool Stepper_Move_S(int16_t start_speed, int16_t end_speed, float acc_time, int32_t step,uint8_t motor_num,uint8_t dir,SpeedCalc_TypeDef * pSpeed);

uint16_t Motor_Fre(uint32_t step);

uint16_t Motor_Fre_Vi(uint32_t step);

void Motor_Abs_Run(uint32_t step,uint8_t motor_num);

void MotorFre_Abs_Run(uint32_t step,uint8_t motor_num,uint8_t dir,uint16_t fre);

void Motor_Run(uint32_t step,uint8_t motor_num,uint8_t dir,uint16_t fre);

void PUMP_Mix_Func(uint8_t times,uint32_t temp_v,uint8_t wTime,uint16_t fre);

void PUMP_FP_Func(uint32_t step);

uint8_t Motor_Rst(uint16_t fre,uint8_t motor_num,uint8_t dir,\
GPIO_TypeDef *Port,uint16_t Pin,uint8_t tl);

uint8_t Motor_HighLeve_Rst(uint16_t fre,uint8_t motor_num,uint8_t dir,\
GPIO_TypeDef *Port,uint16_t Pin,uint8_t tl);

//void ZD_motor_run(SMsg * pMsg);

void scan_module_reset(void);

bool motor_fast_rst(uint8_t motor_num,uint8_t dir,GPIO_TypeDef *gpio_port,uint16_t gpio_pin);


#endif /*__BSP_MOTOR_H */
