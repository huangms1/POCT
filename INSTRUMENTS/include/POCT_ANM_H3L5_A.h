#ifndef __POCT_ANM_H3L5_A_H 
#define __POCT_ANM_H3L5_A_H

/********************************头文件************************************/
#include "poct_process.h"
#include "FreeRTOS.h"
#include "bsp_motor.h"
#include "utility.h"
#include "w5500_conf.h"
#include "led.h"
#include "Reply.h"
#include "TempControlTask.h"
#include "delay.h"
#include "ZStr2StrMap.h"
#include "AppMsgDefine.h"

/*********************************宏定义*****************************************/
#define TOTAL_MOTOR     13      

#define MOTOR_CONF_NUM  12        

#define HOTCAP_TEMP     1060     

#define M01_FRE        1000    
#define M02_FRE        1000     
#define M03_FRE        1000   
#define M04_FRE        1000
#define M05_FRE        1000     
#define M06_FRE        1000    
#define M07_FRE        1000     
#define M08_FRE        800    
#define M09_FRE        1000   
#define M10_FRE        1000  
#define M11_FRE        500 
#define M12_FRE        500    
#define M13_FRE        1500 
#define M14_FRE        2500
#define M15_FRE        800

#define M01_DIR        CW
#define M02_DIR        CW
#define M03_DIR        CW
#define M04_DIR        CCW
#define M05_DIR        CW
#define M06_DIR        CW
#define M07_DIR        CW
#define M08_DIR        CW
#define M09_DIR        CW
#define M10_DIR        CW
#define M11_DIR        CCW
#define M12_DIR        CW
#define M13_DIR        CW
#define M14_DIR        CW
#define M15_DIR        CW

#define MIX_VL            (Globle.POCT_Par.M10_P1/4)
#define MIX_SPEED         9000
#define MIX_TIMES         20


#define M09_M10_UP_FRE    1000 
#define M09_M10_DOWN_FRE  250  

/********************************结构体*********************************************/
typedef struct{
  uint16_t        pul_pin;
  uint16_t        dir_pin;
  uint16_t        en_pin;
  uint16_t        go_pin;
  GPIO_TypeDef  * pul_port;
  GPIO_TypeDef  * dir_port;
  GPIO_TypeDef  * en_port;
  GPIO_TypeDef  * go_port;
}Motor_Conf_TypeDef;

extern Motor_Conf_TypeDef Motor_Conf[MOTOR_CONF_NUM];

typedef struct POCT_EEPROM
{
	uint8_t rs1;
	uint8_t rs2;
	uint8_t rs3;
	uint8_t rs4;
	uint32_t LED[5];

	uint32_t M01_P1;        
	uint32_t M02_P1;        
	uint32_t M02_P2;       
	
	uint32_t M03_P[5];
	uint32_t M02_P3;
	
	uint32_t M08_LED_P[5];
	
	uint32_t M05_P1;        
	uint32_t M06_P1;        
	uint32_t M07_P1;        
	uint32_t M12_P1;       
	uint32_t M07_PumpInTolal_V;        
	uint32_t M07_dg_mix_V;        
	uint32_t M10_P1;        
	uint32_t M09_P1; 

	uint32_t M11_Pos[5];     
	     
	uint32_t M10_Loose;    
	uint32_t M10_OFF;       	 
		
	uint32_t T1_Compensate_Pesitive;
	uint32_t T1_Compensate_Negative;
	uint32_t T2_Compensate_Pesitive;
	uint32_t T2_Compensate_Negative;
	uint32_t T3_Compensate_Pesitive;
	uint32_t T3_Compensate_Negative;
	
	uint32_t M04_Y_P[4];
	uint32_t M11_P5;
	uint32_t M05_X_P[4];
	uint32_t Solution_Volume[3];
	
}POCT_EEPROMTypedef;


typedef struct BootloaderFlag
{
	uint8_t bootLoaderFlag;
}BootLoaderTypeDef;

extern  POCT_EEPROMTypedef POCT_Para;

typedef enum {
	
	IDLE = 0U,  
	
	RUNNING,     
	
	FAULT      
}MechineEnum;

typedef struct MECHINE_STATE
{
	uint8_t State;    
	char faultStr[20];
}MechineStateTypeDef;



typedef struct SavaPara
{
	uint16_t Para[10];
}SavaParaNumTypedef;

typedef struct TempControl
{
	uint8_t  Temp_Ch1_ENABLE;
	
	uint8_t  Temp_Ch2_ENABLE;
	
	uint8_t  Temp_Ch3_ENABLE;
	
	uint8_t  Temp_Ch4_ENABLE;
	
	uint8_t  Temp_Ch5_ENABLE;
	
	uint16_t Temp_Ch1_Set;
	
	uint16_t Temp_T1;
	
	uint16_t Temp_Ch2_Set;
	
	uint16_t Temp_T2;
	
	uint16_t Temp_Ch3_Set;
	
	uint16_t Temp_T3;
	
	uint16_t Temp_Ch4_Set;
	
	uint16_t Temp_T4;
	
	uint16_t Temp_Set_Ch5;
	
	uint16_t TimeCnt_T5;
	
}TempControlTypedef;

typedef struct __GLOBLE
{
	uint8_t              res;
	
	uint8_t              pcr_times;
	
	POCT_EEPROMTypedef   POCT_Par;
	
	SavaParaNumTypedef   SavaParaNum;
	
	TempControlTypedef   TempControl;
	
	MechineStateTypeDef  Mechine;
	
	BootLoaderTypeDef    BootLoader;
	
}GlobleTypedef;

extern GlobleTypedef Globle;

/***********************************函数声明******************************************/
void Motor_GPIO_Config(void);

uint8_t LED_CAM_Scan(uint8_t cam_chanel,uint8_t led_chanel);
void MagneticSet_UD(uint16_t step,uint16_t fre,uint8_t times,uint8_t WaitTime);
bool App_NZL(SMsg * pMsg,int * pFaultCode);
bool App_YBX(SMsg * pMsg,int * pFaultCode);
void App_PCR(SMsg * pMsg);

void Extraction_Tiqu(SMsg * pMsg);               
void Extraction_Clean1(void);                   
void Extraction_Clean2(SMsg * pMsg);            
void Extraction_Elution(SMsg * pMsg);           
void Extraction_Solution_Apportion(SMsg * pMsg);

uint8_t Door_Control(uint8_t sta);
void push_motor(void);

void ALL_Motor_RESET(void);
void T1_Heating(SMsg *pMsg);
void Motor_Move(SMsg * pMsg,uint16_t Rstfre,uint8_t motor_num,uint8_t dir,GPIO_TypeDef *Port,uint16_t Pin);

void VibrateTask(void *pData);

#endif //__POCT_ANM_H3L5_A_H
