#ifndef __BSP_MOTOR_H
#define	__BSP_MOTOR_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "includes.h"
#include <stdbool.h>
#include "InstrumentInclude.h"


//#include "sys.h"

#define TOTAL_MOTOR    13        

#define FORM_LEN 	   5000

#define MIN_FRE        50         

#define RST_MAX_STEP   0xFFFFFF


#define MOTOR_RES_TIMEOUT  25

#define T1_FREQ               (SystemCoreClock / TIM_PRESCALER)

#define STEP_ANGLE			   1.8f					            

#define FSPR              	   (360.0f / 1.8f)                  
			
#define MICRO_STEP        	   16          				        

#define SPR               	  (FSPR * MICRO_STEP)               

#define CONVER(speed)         (float)(speed * SPR / 60.0f)      
  
#define MIN_SPEED             (float)(T1_FREQ / TIM_PERIOD)    
	
typedef struct speedCale{
	 __IO int32_t Vo;                  
	 __IO int32_t Vt;                  
	 __IO int32_t AccelTotalStep;     
	 __IO int32_t DecPoint;           
	 __IO int32_t TotalStep;          
	 __IO int32_t INC_AccelTotalStep; 
	 __IO int32_t Dec_AccelTotalStep; 
	 __IO float Form[FORM_LEN];      
	 __IO uint8_t  deceleration_flag;
	 TIM_HandleTypeDef * htim;
	 __IO uint8_t motor_num;
}SpeedCalc_TypeDef;

extern SpeedCalc_TypeDef * pSpeedGloble[3];

typedef struct {
	 __IO uint8_t  status;            
	 __IO uint8_t  dir;                
	 __IO uint32_t pos;               
	 __IO uint32_t position;           
	 __IO uint32_t pluse_time;         
	 __IO uint16_t start_flag;       
	 __IO uint32_t index;
}Stepper_Typedef;

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

void PUMP_Mix_Func(uint8_t times,uint32_t temp_v,uint8_t wTime);

void PUMP_FP_Func(uint32_t step);

uint8_t Motor_Rst(uint16_t fre,uint8_t motor_num,uint8_t dir,\
GPIO_TypeDef *Port,uint16_t Pin,uint8_t tl);

uint8_t Motor_HighLeve_Rst(uint16_t fre,uint8_t motor_num,uint8_t dir,\
GPIO_TypeDef *Port,uint16_t Pin,uint8_t tl);

void ZD_motor_run(SMsg * pMsg);

void scan_module_reset(void);

void motor_fast_rst(uint8_t motor_num,uint8_t dir,GPIO_TypeDef *gpio_port,uint16_t gpio_pin);


#endif /*__BSP_MOTOR_H */
