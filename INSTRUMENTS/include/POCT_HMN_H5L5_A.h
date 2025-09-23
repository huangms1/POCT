#ifndef __POCT_HMN_H5L5_A_H 
#define __POCT_HMN_H5L5_A_H

#include "poct_process.h"
#include "FreeRTOS.h"
#include "bsp_motor.h"
#include "utility.h"
#include "w5500_conf.h"
#include "bsp_motor_conf.h"
#include "led.h"
#include "Reply.h"
#include "TempControlTask.h"
#include "delay.h"
#include "ZStr2StrMap.h"
#include "AppMsgDefine.h"
#include "StartTask.h"

#define TOTAL_MOTOR     13       //电机总个数

#define DOOR_FRE        5000     //开关门频率 G12  6000

#define MOTOR_CONF_NUM  13

#define HOTCAP_TEMP     1060  

/* 步进电机结构体 */
typedef struct{
	uint16_t pul_pin;
	uint16_t dir_pin;
	uint16_t en_pin;
	uint16_t go_pin;
	GPIO_TypeDef *pul_port;
	GPIO_TypeDef *dir_port;
	GPIO_TypeDef *en_port;
	GPIO_TypeDef *go_port;
}Motor_Conf_TypeDef;

extern Motor_Conf_TypeDef Motor_Conf[MOTOR_CONF_NUM];

/*电机复位及移动频率*/

#define M01_FRE        1000    
#define M02_FRE        1000     
#define M03_FRE        500   
#define M04_FRE        700
#define M05_FRE        500     
#define M06_FRE        500    
#define M07_FRE        1000     
#define M08_FRE        500    
#define M09_FRE        1000   
#define M10_FRE        800  
#define M11_FRE        800 
#define M12_FRE        500    
#define M13_FRE        1500 
#define M14_FRE        1500
#define M15_FRE        800

#define M01_DIR        CW
#define M02_DIR        CW
#define M03_DIR        CW
#define M04_DIR        CW
#define M05_DIR        CCW
#define M06_DIR        CW
#define M07_DIR        CW
#define M08_DIR        CW
#define M09_DIR        CW
#define M10_DIR        CCW
#define M11_DIR        CW
#define M12_DIR        CCW
#define M13_DIR        CW
#define M14_DIR        CW
#define M15_DIR        CW


#define MIX_TEMP_V1   27500

#define MIX_TEMP_V2   30000

#define MIX_TEMP_V3   30000

#define MIX_TEMP_V4   30000

typedef struct{
	uint8_t FaState[12];
}FaStateTypeDef;

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
	
	uint32_t M03_Cam_P[5];
	
	uint32_t M08_LED_P[5];
	
	uint32_t M05_P1;
	
	uint32_t M06_P1;
    
	uint32_t M07_P1; 
	
	uint32_t M12_P1;
	
	uint32_t M09_P[2];

	uint32_t M10_Fa_P[4];
	
    uint32_t M10_Fa_offset;
	
	uint32_t M11_P1;
		
	//温度补
	uint32_t T1_Compensate_Pesitive;

	uint32_t T1_Compensate_Negative;

	uint32_t T2_Compensate_Pesitive;

	uint32_t T2_Compensate_Negative;

	uint32_t T3_Compensate_Pesitive;
	
	uint32_t T3_Compensate_Negative;
	
	//各孔分液量
	uint32_t Solution_Volume[5];
}POCT_EEPROMTypedef;

typedef struct BootloaderFlag
{
	uint8_t bootLoaderFlag;
}BootLoaderTypeDef;

extern  POCT_EEPROMTypedef POCT_Para;

typedef enum {
	
	IDLE = 0U,   //空闲
	
	RUNNING,     //运行
	
	FAULT        //故障	
}MechineEnum;


//存放
typedef struct MECHINE_STATE
{
	uint8_t State;    //0：空闲，1：运行 2：故障；
	char faultStr[20];
}MechineStateTypeDef;


/*存放子参整形数字*/
typedef struct SavaPara
{
	uint16_t Para[10];
}SavaParaNumTypedef;


/*存放温控信息,接受温控信息时赋值，在温控任务中使用*/
typedef struct TempControl
{
	uint8_t Temp_Ch1_ENABLE;
	
	uint8_t Temp_Ch2_ENABLE;
	
	uint8_t Temp_Ch3_ENABLE;
	
	uint8_t Temp_Ch4_ENABLE;
	
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




/*全局变量*/
typedef struct __GLOBLE
{
	uint8_t res;
	
	uint8_t pcr_times;
	
	uint16_t washTime;
	
	uint8_t FaNum;
	
	uint8_t LastFaNum;
	
	uint32_t Fa1LastPst;      //记录旋转盘最后一个位置

	POCT_EEPROMTypedef POCT_Par;
	
	SavaParaNumTypedef SavaParaNum;
	
	TempControlTypedef TempControl;
	
	MechineStateTypeDef Mechine;
	
	BootLoaderTypeDef BootLoader;
	
}GlobleTypedef;

extern GlobleTypedef Globle;

void M09ToM15_GPIO_Config(void);

uint8_t LED_CAM_Scan(uint8_t cam_chanel,uint8_t led_chanel);

bool App_NZL(SMsg * pMsg,int * pFaultCode);

bool App_YBX(SMsg * pMsg,int * pFaultCode);

void Extraction_Tiqu(SMsg * pMsg);                //核酸提取步骤

void Extraction_Clean1(void);                     //核酸清洗第一次

void Extraction_Clean2(SMsg * pMsg);              //核酸清洗第二次

void Extraction_Elution(SMsg * pMsg);             //核酸洗脱步骤

void Extraction_Solution_Apportion(SMsg * pMsg);  //核酸溶液分配

void App_PCR(SMsg * pMsg);

uint8_t Door_Control(uint8_t sta);

void ALL_Motor_RESET(void);

void push_motor(void);

void Motor_Move(SMsg * pMsg,uint16_t Rstfre,uint8_t motor_num,uint8_t dir,GPIO_TypeDef *Port,uint16_t Pin);

void VibrateTask(void *pData);

#endif //__POCT_HMN_H5L5_A_H


