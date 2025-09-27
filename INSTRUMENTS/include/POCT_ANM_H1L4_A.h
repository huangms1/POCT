#ifndef __POCT_ANM_H1L4_A_H
#define __POCT_ANM_H1L4_A_H

#include <math.h>

#include "poct_process.h"
#include "FreeRTOS.h"
#include "usart.h"
#include "bsp_motor.h"
#include "utility.h"
#include "w5500_conf.h"
#include "bsp_i2c_ee.h"
#include "bsp_motor_conf.h"
#include "led.h"
#include "Reply.h"
#include "TempControlTask.h"
#include "delay.h"
#include "ZStr2StrMap.h"
#include "AppMsgDefine.h"
#include "StartTask.h"
#include "mytimer.h"
#include "laser.h"

//#define DOOR_FRE       500     //开关门频率  6000

#define MOTOR_CONF_NUM  12

//#define HOTCAP_TEMP     1060  

/* 步进电机结构体 */
typedef struct{
	uint16_t pul_pin;
	uint16_t dir_pin;
	uint16_t en_pin;
	uint16_t go_pin;
	GPIO_TypeDef * pul_port;
	GPIO_TypeDef * dir_port;
	GPIO_TypeDef * en_port;
	GPIO_TypeDef * go_port;
}Motor_Conf_TypeDef;

extern Motor_Conf_TypeDef Motor_Conf[MOTOR_CONF_NUM];


/*电机复位及移动频率*/

#define M01_FRE        1500    
#define M02_FRE        1500     
#define M03_FRE        1500   
#define M04_FRE        900
#define M05_FRE        900     //500
#define M06_FRE        900    
#define M07_FRE        1800     
#define M08_FRE        800    
#define M09_FRE        1000   
#define M10_FRE        1200  
#define M11_FRE        900 
#define M12_FRE        900    
#define M13_FRE        1500 
#define M14_FRE        2000
#define M15_FRE        800

#define M01_DIR        CW
#define M02_DIR        CW
#define M03_DIR        CW
#define M04_DIR        CCW
#define M05_DIR        CW
#define M06_DIR        CW
#define M07_DIR        CW
#define M08_DIR        CW
#define M09_DIR        CCW
#define M10_DIR        CW
#define M11_DIR        CCW
#define M12_DIR        CW
#define M13_DIR        CW
#define M14_DIR        CW
#define M15_DIR        CW

//混匀
#define MIX_VL            20000UL
#define MIX_SPEED         3000
#define MIX_TIMES         10

#define M09_M10_UP_FRE    2000 
#define M09_M10_DOWN_FRE  300  //缓慢磁吸速度

//溶液分配
#define ALC_VL            1800    //推液体积

typedef struct POCT_EEPROM
{
	uint8_t rs1;
	uint8_t rs2;
	uint8_t rs3;
	uint8_t rs4;
	
	uint32_t LED[8];
	
	uint32_t M01_P1;    //温控上下位置	
	uint32_t M02_P1;    //温控前进1位置
	uint32_t M02_P2;    //温控前进2位置
	
	uint32_t M01_P2;
	uint32_t M03_P[1];

	uint32_t M08_LED_P[8];    //M08LED上下位置

	uint32_t M05_P1;        //下压电机位置
	uint32_t M06_P1;        //提取电机位置
	uint32_t M07_P1;        //泵电机位置
	uint32_t M12_P1;        //磁铁电机位置
	
	uint32_t M10_Fa_P[5];
	uint32_t M11_P1;        //动物阀旋转180位置	
		
	//温度补
	uint32_t T1_Compensate_Pesitive;
	uint32_t T1_Compensate_Negative;
	uint32_t T2_Compensate_Pesitive;
	uint32_t T2_Compensate_Negative;
	uint32_t T3_Compensate_Pesitive;
	uint32_t T3_Compensate_Negative;
	
	uint32_t Liquid_v[1];
		
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

typedef struct MECHINE_STATE
{
	__IO uint8_t State;    //0：空闲，1：运行 2：故障；
	char faultStr[20];
}MechineStateTypeDef;

typedef struct SavaPara
{
	uint16_t Para[10];
}SavaParaNumTypedef;

typedef struct TempControl
{
	uint8_t Temp_Ch1_ENABLE;
	
	uint8_t Temp_Ch2_ENABLE;
	
	uint8_t Temp_Ch3_ENABLE;
	
	uint8_t Temp_Ch4_ENABLE;
	
	uint8_t  Temp_Ch5_ENABLE;
	
	__IO uint16_t Temp_Ch1_Set;
	
	__IO uint16_t Temp_T1;
	
	__IO uint16_t Temp_Ch2_Set;
	
	__IO uint16_t PcrTempSet;
	
	__IO uint16_t Temp_T2;
	
	__IO uint16_t Temp_Ch3_Set;
	
	__IO uint16_t Temp_T3;
	
	__IO uint16_t Temp_Ch4_Set;
	
	__IO uint16_t Temp_T4;
	
	__IO uint16_t Temp_Set_Ch5;
	
	__IO uint16_t TimeCnt_T5;
	
}TempControlTypedef;

typedef struct __GLOBLE
{
	__IO uint8_t res;
	
	__IO uint8_t pcr_times;
	
	__IO uint16_t washTime;
	
	__IO uint8_t CurrentValvePos;
	
	__IO uint8_t LastValvePos;
	
	__IO uint8_t LastDirection;
	
	POCT_EEPROMTypedef POCT_Par;
	
	SavaParaNumTypedef SavaParaNum;
	
	TempControlTypedef TempControl;
	
	MechineStateTypeDef Mechine;
	
	BootLoaderTypeDef BootLoader;
	
}GlobleTypedef;

/**
 * @brief 缓慢升温控制结构体
 */
typedef struct {
    uint16_t targetTemp;        // 目标温度
    uint16_t currentTemp;       // 当前温度
    uint16_t startTemp;         // 起始温度
    uint8_t  stage;             // 升温阶段
    uint32_t startTime;         // 开始时间
    uint32_t stageStartTime;    // 阶段开始时间
    uint16_t stageTarget;       // 阶段目标温度
    uint8_t  stabilityCount;    // 温度稳定计数
    bool     isHeating;         // 是否正在升温
	float    lastSetTemp;
	bool     needSlowHeating;
} SlowHeatCtrl_t;

extern SlowHeatCtrl_t heatCtrl;

extern GlobleTypedef Globle;

bool Slow_Heating_Linear(uint16_t targetTemp, int *pFaultCode);

void Motor_GPIO_Config(void);

void App_PCR(SMsg * pMsg);;

uint8_t LED_CAM_Scan(uint8_t cam_chanel,uint8_t led_chanel);

bool App_NZL(SMsg * pMsg,int * pFaultCode);

bool App_YBX(SMsg * pMsg,int * pFaultCode);

void Extraction_Tiqu(SMsg * pMsg);

void Extraction_Clean1(void);                    //核酸清洗第一次

void Extraction_Clean2(SMsg * pMsg);             //核酸清洗第二次	
	
void Extraction_Elution(SMsg * pMsg);            //核酸洗脱步骤

void Extraction_Solution_Apportion(SMsg * pMsg); //核酸溶液分配

void ALL_Motor_RESET(void);               

uint8_t Door_Control(uint8_t sta);

void push_motor(void);

void T1_Heating(SMsg *pMsg);

void Motor_Move(SMsg * pMsg,uint16_t Rstfre,uint8_t motor_num,uint8_t dir,GPIO_TypeDef *Port,uint16_t Pin);

#endif//





