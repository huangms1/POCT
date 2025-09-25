#include "fuzzypid.h"
#include "bsp_pid.h"
#include "FreeRTOS.h"
#include <math.h>


// 确保MAX_CHN已定义，若未定义则设置默认值为4
#ifndef MAX_CHN
#define MAX_CHN 4
#endif

/**
 * @brief 模糊PID控制器结构体数组
 * @details 包含MAX_CHN个模糊PID控制器实例，每个实例可独立配置
 *          用于多通道温度、压力等参数的精确控制
 */
FUZZYPID FPID_data[MAX_CHN] = {
    // 通道0: 默认通用配置
    {
        .setpoint = 0,               /*设定值*/
        .kp = 5,                     /*比例系数 - 适中值*/
        .ki = 14,                    /*积分系数 - 较小值避免超调*/
        .kd = 4,                     /*微分系数 - 适中值*/
        .iError = 0,                 /*前一拍偏差*/
        .lasterror = 0,              /*前一拍偏差*/
        .preerror = 0,               /*前两拍偏差*/
        .deadband = 0.1,             /*死区 - 避免微小波动*/
        .output = 0,                 /*输出值*/
        .result = 0,                 /*物理量输出值*/
        .maximum = 4,                /*输入值的上限 - 调整为常见范围*/
        .minimum = 0,                /*输入值的下限*/
        
        .maxdKp = 0,                 /*Kp增量的最大限值*/
        .mindKp = 0,                 /*Kp增量的最小限值*/
        .qKp = 0.03,                 /*Kp增量的影响系数*/
        .maxdKi = 0,                 /*Ki增量的最大限值*/
        .mindKi = 0,                 /*Ki增量的最小限值*/
        .qKi = 0.03,                 /*Ki增量的影响系数*/
        .maxdKd = 0,                 /*Kd增量的最大限值*/
        .mindKd = 0,                 /*Kd增量的最小限值*/
        .qKd = 0.03,                 /*Kd增量的影响系数*/
        
        .data_kp = 0,
        .data_ki = 0,
        .data_kd = 0
    },
    // 通道1: 快速响应配置
    {
        .setpoint = 0,
        .kp = 70,                /*较大比例系数，提高响应速度 8.5*/  //6.81
        .ki = 60,                /*较小积分系数，避免超调 6.2*/         //6.3
        .kd = 100,                /*较大微分系数，抑制震荡 4*/          //7.11
        .deadband = 0,
        .maximum = 4,
        .minimum = 0,
        .maxdKp = 0,
        .mindKp = 0,
        .qKp = 0.03,
        .maxdKi = 0,
        .mindKi = 0,
        .qKi = 0.03,
        .maxdKd = 0,
        .mindKd = 0,
        .qKd = 0.03
        // 其他成员未指定，自动初始化为0
    },
    // 通道2: 稳定优先配置
    {
        .setpoint = 0,
        .kp = 5,                   /*较小比例系数，稳定性优先*/
        .ki = 12,                   /*较大积分系数，消除稳态误差*/
        .kd = 4,                   /*较小微分系数*/
        .deadband = 0,
        .maximum = 4,
        .minimum = 0,
        .maxdKp = 0,
        .mindKp = 0,
        .qKp = 0.03,
        .maxdKi = 0,
        .mindKi = 0,
        .qKi = 0.03,
        .maxdKd = 0,
        .mindKd = 0,
        .qKd = 0.03
    },
    // 通道3及以后: 默认使用通道0的配置
#if MAX_CHN > 3
    // 可根据需要添加更多通道配置
#endif
};

//FUZZYPID FPID;

//FUZZYPID FPID_data[MAX_CHN];

void set_qKp_qKi_qKd(FUZZYPID *vPID,float qKp,float qKi, float qKd)
{
	vPID->qKp = qKp;
	vPID->qKi = qKi;
	vPID->qKd = qKd;
}


int assign_struct(void) {

//	int i = 0;	
//	FPID.setpoint = 0;               /*设定值*/
//	FPID.kp = 5;                     /*比例系数*/
//	FPID.ki = 14;                     /*积分系数 12*/
//	FPID.kd = 4;                     /*微分系数*/
//	FPID.iError = 0;                 /*前一拍偏差*/
//	FPID.lasterror = 0;              /*前一拍偏差*/
//	FPID.preerror = 0;               /*前两拍偏差*/
//	FPID.deadband = 0;               /*死区*/
//	FPID.output = 0;                 /*输出值*/
//	FPID.result = 0;                 /*物理量输出值*/
//	FPID.maximum = 4;                /*输入值的上限*/
//	FPID.minimum = 0;                /*输入值的下限*/
//	
//	FPID.maxdKp = 0;                 /*Kp增量的最大限值*/
//	FPID.mindKp = 0;                 /*Kp增量的最小限值*/
//	FPID.qKp = 0.03f;                /*Kp增量的影响系数，从0.03提高以增强响应*/
//	FPID.maxdKi = 0;                 /*Ki增量的最大限值*/
//	FPID.mindKi = 0;                 /*Ki增量的最小限值*/
//	FPID.qKi = 0.03f;                /*Ki增量的影响系数*/
//	FPID.maxdKd = 0;                 /*Kd增量的最大限值*/
//	FPID.mindKd = 0;                 /*Kd增量的最小限值*/
//	FPID.qKd = 0.03f;                /*Kd增量的影响系数*/
//	FPID.data_kp = 0;
//	FPID.data_ki = 0;
//	FPID.data_kd = 0;
//	
//	for(i = 0; i< MAX_CHN;i++)
//	{
//		memcpy(&FPID_data[i].setpoint,&FPID.setpoint,sizeof(FUZZYPID));
//	}
//	
	return 0;
}

//INIT_BOARD_EXPORT(assign_struct);
/******************************************************************************
函数功能：
*   @ 隶属度计算函数
参数描述:
*   @  *ms：隶属度,qv：量化的值,*index：规则库中的索引
返回值描述：
*   @ 无
说明：
*   @ (输入值的模糊化)线性量化操作函数,论域{-6，-5，-4，-3，-2，-1，0，1，2，3，4，5，6}
******************************************************************************/
void CalcMembership(float *ms, float qv, int *index)
{
    // 输入参数有效性检查
    if (ms == NULL || index == NULL)
        return;

    // 边界处理：确保输入值在论域范围内
    float clamped_qv = qv;
    if (clamped_qv < NB) clamped_qv = NB;
    if (clamped_qv > PB) clamped_qv = PB;

    // 模糊区间参数表：[下限, 上限, 左索引, 右索引, 左斜率, 左截距, 右斜率, 右截距]
    const float intervals[6][8] = {
        {NB, NM, 0, 1, -0.5f, -2.0f, 0.5f, 3.0f},  // NB-NM区间
        {NM, NS, 1, 2, -0.5f, -1.0f, 0.5f, 2.0f},  // NM-NS区间
        {NS, ZO, 2, 3, -0.5f,  0.0f, 0.5f, 1.0f},  // NS-ZO区间
        {ZO, PS, 3, 4, -0.5f,  1.0f, 0.5f, 0.0f},  // ZO-PS区间
        {PS, PM, 4, 5, -0.5f,  2.0f, 0.5f, -1.0f}, // PS-PM区间
        {PM, PB, 5, 6, -0.5f,  3.0f, 0.5f, -2.0f}  // PM-PB区间
    };

    // 遍历区间表查找匹配区间（替代冗长的if-else链）
    for (uint8_t i = 0; i < 6; i++)
    {
        if (clamped_qv >= intervals[i][0] && clamped_qv < intervals[i][1])
        {
            // 设置模糊集合索引
            index[0] = (int)intervals[i][2];
            index[1] = (int)intervals[i][3];

            // 计算隶属度（使用统一公式：y = 斜率*x + 截距）
            ms[0] = intervals[i][4] * clamped_qv + intervals[i][5];
            ms[1] = intervals[i][6] * clamped_qv + intervals[i][7];

            // 隶属度范围限制（确保在[0,1]区间内）
            ms[0] = (ms[0] < 0.0f) ? 0.0f : (ms[0] > 1.0f) ? 1.0f : ms[0];
            ms[1] = (ms[1] < 0.0f) ? 0.0f : (ms[1] > 1.0f) ? 1.0f : ms[1];
            return;
        }
    }

    // 处理边界情况（等于PB时）
    index[0] = 5; index[1] = 6;
    ms[0] = -0.5f * clamped_qv + 3.0f;
    ms[1] = 0.5f * clamped_qv - 2.0f;
}

void LinearQuantization(FUZZYPID *vPID,float pv,float *qValue)
{
  float thisError;
  float deltaError;
 
  thisError=vPID->setpoint-pv;                            //计算偏差值
  deltaError=thisError-vPID->lasterror;                   //计算偏差增量
 
  qValue[0]=6.0f*thisError/(vPID->maximum-vPID->minimum);  //E的量化值
  qValue[1]=3.0f*deltaError/(vPID->maximum-vPID->minimum); //Ec的量化值
    
  if(qValue[0] > 6)
  {
      qValue[0] = 6;
  }else if(qValue[0] < -6)
  {
      qValue[0] = -6;
  }
  
  if(qValue[1] > 3)
  {
      qValue[1] = 3;
  }else if(qValue[1] < -3)
  {
      qValue[1] = -3;
  }
}

void FuzzyComputation(FUZZYPID *vPID,float _Real_Value)
{
	 //量化值
	 float qValue[2]={0,0};  

	 int indexE[2]={0,0};     //e在规则库中的索引
	 float msE[2]={0,0};      //e的隶属度
	 
	 int indexEC[2]={0,0};    //ec在规则库中的索引
	 float msEC[2]={0,0};      //ec的隶属度
	 
	 //pid增量值
	 float pidvalue[3];
	 
	 //量化
	 LinearQuantization(vPID,_Real_Value,qValue);
	 //计算e的隶属度和索引
	 CalcMembership(msE,qValue[0],indexE);
	 //计算ec的隶属度和索引
	 CalcMembership(msEC,qValue[1],indexEC);
	 
	 //采用重心法计算pid增量值
	 pidvalue[0]=msE[0]*(msEC[0]*ruleKp[indexE[0]][indexEC[0]]+msEC[1]*ruleKp[indexE[0]][indexEC[1]]) 
				+msE[1]*(msEC[0]*ruleKp[indexE[1]][indexEC[0]]+msEC[1]*ruleKp[indexE[1]][indexEC[1]]);
	 pidvalue[1]=msE[0]*(msEC[0]*ruleKi[indexE[0]][indexEC[0]]+msEC[1]*ruleKi[indexE[0]][indexEC[1]])
				+msE[1]*(msEC[0]*ruleKi[indexE[1]][indexEC[0]]+msEC[1]*ruleKi[indexE[1]][indexEC[1]]);
	 pidvalue[2]=msE[0]*(msEC[0]*ruleKd[indexE[0]][indexEC[0]]+msEC[1]*ruleKd[indexE[0]][indexEC[1]])
				+msE[1]*(msEC[0]*ruleKd[indexE[1]][indexEC[0]]+msEC[1]*ruleKd[indexE[1]][indexEC[1]]);

	//pid增量修正
	 vPID->data_kp=vPID->qKp*pidvalue[0];
	 vPID->data_ki=vPID->qKi*pidvalue[1];
	 vPID->data_kd=vPID->qKd*pidvalue[2];
}


float COOL_Fuzzytrans(FUZZYPID *vPID,float _Set_Vaule,float _Measure_Vaule) 
{
//    float dError;
//    float I_dead=0;
    
    vPID->iError    = _Set_Vaule - _Measure_Vaule;
    vPID->setpoint  = _Set_Vaule;  
    
    // 输入量的最大最小值
    vPID->maximum = 4; 
    vPID->minimum = 0;
    FuzzyComputation(vPID,_Measure_Vaule);

    // 死区处理
   if(fabs(vPID->iError) <= vPID->deadband){
       vPID->iError = 0; // 误差在死区内，不积累积分
   }
    
      // 计算微分项 (当前误差-上一次误差)
//    dError = vPID->iError - vPID->lasterror;
    
//    // 积分项计算 (加入抗积分饱和)
//    float integral = vPID->data_ki * (vPID->iError + vPID->lasterror) / 2; // 梯形积分
//    if((vPID->output + integral) > vPID->maximum || (vPID->output + integral) < vPID->minimum){
//        integral = 0; // 即将超限时停止积分
//    }
//    
//    // PID计算 (比例+积分+微分)
//    vPID->output = vPID->data_kp * vPID->iError + integral + vPID->data_kd * dError;
//    
//    // 输出限幅
//    if(vPID->output > vPID->maximum){
//        vPID->output = vPID->maximum;
//    }else if(vPID->output < vPID->minimum){
//        vPID->output = vPID->minimum;
//    }
    
    vPID->preerror  = vPID->lasterror;
    vPID->lasterror = vPID->iError;
    
    return vPID->output;
}


float PID_Vout(short vSet, short vin)
{
	float Vout = 0;
//	short vset;
//	short vIn;
//	
//	vset = vSet; 				        // 设置PID目标
//	vIn  = vin; 					    // 读取输入
//	Vout = COOL_Fuzzytrans( vset,vIn ); // 执行PID交互影响	 
//	
	return Vout;						//返回vOut
	
}


