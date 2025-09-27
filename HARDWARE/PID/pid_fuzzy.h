
#ifndef __PID_FUZZY_H__
#define __PID_FUZZY_H__

#include "stdint.h"
#include "stdbool.h"

#if defined(__CC_ARM)
#pragma anon_unions
#elif defined(__ICCARM__)
#endif


#define NB   -6
#define NM	 -4
#define NS	 -2   //-1
#define ZO	 0
#define ZE	 0
#define PS	 2    //1
#define PM	 4
#define PB	 6

#define MAX_CHN   14


#if 1
//�����
static const float ruleKp[7][7]={
	PB,	PB,	PM,	PM,	PS,	PS,	ZO,
	PB,	PB,	PM,	PM,	PS,	ZO,	ZO,
	PM,	PM,	PM,	PS,	ZO,	NS,	NM,
	PM,	PS,	PS,	ZO,	NS,	NM,	NM,
	PS,	PS,	ZO,	NS,	NS,	NM,	NM,
	ZO,	ZO,	NS,	NM,	NM,	NM,	NB,
	ZO,	NS,	NS,	NM,	NM,	NB,	NB
};
static const float ruleKi[7][7]={
	NB,	NB,	NB,	NM,	NM,	ZO,	ZO,
	NB,	NB,	NM,	NM,	NS,	ZO,	ZO,
	NM,	NM,	NS,	NS,	ZO,	PS,	PS,
	NM,	NS,	NS,	ZO,	PS,	PS,	PM,
	NS,	NS,	ZO,	PS,	PS,	PM,	PM,
	ZO,	ZO,	PS,	PM,	PM,	PB,	PB,
	ZO,	ZO,	PS,	PM,	PB,	PB,	PB
};

static const float ruleKd[7][7]={
	PS,	PS,	ZO,	ZO,	ZO,	PB,	PB,
	NS,	NS,	NS,	NS,	ZO,	NS,	PM,
	NB,	NB,	NM,	NS,	ZO,	PS,	PM,
	NB,	NM,	NM,	NS,	ZO,	PS,	PM,
	NB,	NM,	NS,	NS,	ZO,	PS,	PS,
	NM,	NS,	NS,	NS,	ZO,	PS,	PS,
	PS,	ZO,	ZO,	ZO,	ZO,	PB,	PB
};

#else
static const float ruleKp[7][7]={
	PB,	PB,	PM,	PM,	PS,	ZE,	ZE,
    PB,	PB,	PM,	PS,	ZE,	NS,	NS,
    PM,	PM,	PS,	ZE,	NS,	NM,	NM,
    PS,	PS,	ZE,	ZE,	NS,	NM,	NM,
    ZE,	ZE,	NS,	NM,	NM,	NM,	NS,
    ZE,	NS,	NM,	NM,	NM,	NS,	PS,
    ZE,	NS,	NM,	NM,	NS,	PS,	PB

};

static const float ruleKi[7][7]={
	NB,NB,NM,NM,NS,ZE,ZE ,
    NB,NB,NM,NS,ZE,PS,ZE ,
    NM,NM,NS,ZE,PS,PM,PS ,
    NM,NS,ZE,ZE,PS,PM,PM ,
    NS,ZE,PS,PS,PM,PB,PB ,
    ZE,ZE,PS,PM,PB,PB,PB ,
    ZE,ZE,ZE,PB,PB,PB,PB 

};


static const float ruleKd[7][7]={
    ZE,	ZE,	ZE,	ZE,	ZE,	NS,	NS,
    ZE,	ZE,	ZE,	ZE,	NS,	NS,	PS,
    ZE,	ZE,	NS,	NS,	NS,	PS,	PM,
    ZE,	NS,	NS,	NS,	PS,	PM,	PB,
    NS,	NS,	PS,	PS,	PS,	PM,	PB,
    NS,	PS,	PS,	PM,	PM,	PB,	PB,
    PS,	PS,	PM,	PB,	PB,	PB,	PB
};
#endif
/*����ṹ��͹�����*/
typedef struct
{
  float setpoint;               /*�趨ֵ*/
  float kp;                     /*����ϵ��*/
  float ki;                     /*����ϵ��*/
  float kd;                     /*΢��ϵ��*/
  float	iError;                 /*ǰһ��ƫ��*/
  float lasterror;              /*ǰһ��ƫ��*/
  float preerror;               /*ǰ����ƫ��*/
  float deadband;               /*����*/
  float output;                 /*���ֵ*/
  float result;                 /*���������ֵ*/
  float maximum;                /*���ֵ������(�룿)*/ 
  float minimum;                /*���ֵ������*/
 
  float maxdKp;                 /*Kp�����������ֵ*/
  float mindKp;                 /*Kp��������С��ֵ*/
  float qKp;                    /*Kp������Ӱ��ϵ��*/
  float maxdKi;                 /*Ki�����������ֵ*/
  float mindKi;                 /*Ki��������С��ֵ*/
  float qKi;                    /*Ki������Ӱ��ϵ��*/
  float maxdKd;                 /*Kd�����������ֵ*/
  float mindKd;                 /*Kd��������С��ֵ*/
  float qKd;                    /*Kd������Ӱ��ϵ��*/
  
  float data_kp;
  float data_ki;
  float data_kd;
}FUZZYPID;

//��ʼ���ṹ�����
extern FUZZYPID FPID;
extern FUZZYPID FPID_data[MAX_CHN];

//�����ȼ��㺯��
void CalcMembership(float *ms,float qv,int * index);

//����ֵ����������(-6->6)
 void LinearQuantization(FUZZYPID *vPID,float pv,float *qValue);

//��ģ��
 void FuzzyComputation (FUZZYPID *vPID,float _Real_Value);

/*�������������*/
void COOL_Fuzzytrans(FUZZYPID *vPID,float _Set_Vaule,float _Measure_Vaule) ;

float PID_Vout(short vSet, short vin);

void set_qKp_qKi_qKd(FUZZYPID *vPID,float qKp,float qKi, float qKd);

#endif
