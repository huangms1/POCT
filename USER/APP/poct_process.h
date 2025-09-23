#ifndef _POCT_PROCESS_H
#define _POCT_PROCESS_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "socket.h"
#include "Me.h"
#include <string.h>
#include <stdio.h>
#include "includes.h"
#include "InstrumentInclude.h"


// 定义一个结构体来存储 LED 名称和对应的索引
typedef struct {
    const char *name;
    uint8_t index;
} LEDMapping;

typedef enum
{
	LED_ATTO = 0U,
	LED_FAM,
	LED_HEX,
	LED_ROX,
	LED_CY5,	
	LED_CY7,
	LED_QUAS705,
	LED_AF405
}LEDEnum_TypeDef;

// 定义一个结构体来存储电机的相关信息
typedef struct {
    const char *mainPara;
    uint16_t fre;
    uint8_t motor;
    uint8_t dir;
    GPIO_TypeDef *goGpioPort;
    uint16_t goGpioPin;
} MotorInfo;

int LedNickName2Index(char *pLedNickName);

uint8_t protocol_analysis(uint8_t len);

bool User_App(SMsg * pMsg,int * pFaultCode);

void ExtractParametersToGlobal(char *str,uint8_t len);

void ControlUserMotor(SMsg * pMsg);

void SendT1Values(SMsg * pMsg,uint16_t *Tbuf);

void SendT2T3Values(SMsg * pMsg,uint16_t *Tbuf);
/*当仪器出现故障时 调用此函数*/
void SaveFaultCode(char *FaultStr);

uint32_t ExtractNumberAfterEqualSign(char *token);

bool Poct_init(void);

void TimingDisplay(SMsg * pMsg);

#endif

