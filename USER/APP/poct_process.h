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

//LED序号
#define LED_ATTO     0

#define LED_FAM      1

#define LED_HEX      2

#define LED_ROX      3

#define LED_CY5      4

#define LED_CY7      5

#define LED_CY5DOT5  6

#define LED_AMCA     7

int LedNickName2Index(char *pLedNickName);

uint8_t protocol_analysis(uint8_t len);

bool User_App(SMsg * pMsg,int * pFaultCode);

void Get_Para_Num(char *str,uint8_t len);

void User_Motor_Control(SMsg * pMsg);

void Send_T1(SMsg * pMsg,uint16_t *Tbuf);

void Send_T2T3(SMsg * pMsg,uint16_t *Tbuf);
/*当仪器出现故障时 调用此函数*/
void SaveFaultCode(char *FaultStr);

uint32_t PARA_Get_num(char *token);

bool Poct_init(void);

void TimingDisplay(SMsg * pMsg);

#endif

