#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "pid_fuzzy.h"

// 初始化结构体参数
FUZZYPID FPID = {
	.setpoint = 0,	/*设定值*/
	.kp = 1,		/*比例系数*/
	.ki = 1,		/*积分系数*/
	.kd = 1,		/*微分系数*/
	.iError = 0,	/*前一拍偏差*/
	.lasterror = 0, /*前一拍偏差*/
	.preerror = 0,	/*前两拍偏差*/
	.deadband = 0,	/*死区*/
	.output = 0,	/*输出值*/
	.result = 0,	/*物理量输出值*/
	.maximum = 4,	/*输入值的上限*/
	.minimum = 0,	/*输入值的下限*/

	.maxdKp = 0, /*Kp增量的最大限值*/
	.mindKp = 0, /*Kp增量的最小限值*/
	.qKp = 0.03,	 /*Kp增量的影响系数*/
	.maxdKi = 0, /*Ki增量的最大限值*/
	.mindKi = 0, /*Ki增量的最小限值*/
	.qKi = 0.03,	 /*Ki增量的影响系数*/
	.maxdKd = 0, /*Kd增量的最大限值*/
	.mindKd = 0, /*Kd增量的最小限值*/
	.qKd = 0.03,	 /*Kd增量的影响系数*/

	.data_kp = 0,
	.data_ki = 0,
	.data_kd = 0,
};
FUZZYPID FPID_data[MAX_CHN];

void set_qKp_qKi_qKd(FUZZYPID *vPID, float qKp, float qKi, float qKd)
{
	vPID->qKp = qKp;
	vPID->qKi = qKi;
	vPID->qKd = qKd;
}

static int assign_struct(void)
{

	for (int i = 0; i < MAX_CHN; i++)
	{
		rt_memcpy(&FPID_data[i].setpoint, &FPID.setpoint, sizeof(FUZZYPID));
	}

	return 0;
}
INIT_BOARD_EXPORT(assign_struct);
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
	if ((qv >= NB) && (qv < NM))
	{
		index[0] = 0;
		index[1] = 1;
		ms[0] = -0.5f * qv - 2.0f; // y=-0.5x-2.0
		ms[1] = 0.5f * qv + 3.0f;	 // y=0.5x+3.0
	}
	else if ((qv >= NM) && (qv < NS))
	{
		index[0] = 1;
		index[1] = 2;
		ms[0] = -0.5f * qv - 1.0f; // y=-0.5x-1.0
		ms[1] = 0.5f * qv + 2.0f;	  // y=0.5x+2.0
	}
	else if ((qv >= NS) && (qv < ZO))
	{
		index[0] = 2;
		index[1] = 3;
		ms[0] = -0.5f * qv;		// y=-0.5x
		ms[1] = 0.5f * qv + 1.0f; // y=0.5x+1.0
	}
	else if ((qv >= ZO) && (qv < PS))
	{
		index[0] = 3;
		index[1] = 4;
		ms[0] = -0.5f * qv + 1.0f; // y=-0.5x+1.0
		ms[1] = 0.5f * qv;		 // y=0.5x
	}
	else if ((qv >= PS) && (qv < PM))
	{
		index[0] = 4;
		index[1] = 5;
		ms[0] = -0.5f * qv + 2.0f; // y=-0.5x+2.0
		ms[1] = 0.5f * qv - 1.0f;	 // y=0.5x-1.0
	}
	else if ((qv >= PM) && (qv <= PB))
	{
		index[0] = 5;
		index[1] = 6;
		ms[0] = -0.5f * qv + 3.0f; // y=-0.5x+3.0
		ms[1] = 0.5f * qv - 2.0f;	 // y=0.5x-2.0
	}
	//	 printf("index[0]:%d\r\n,index[1]:%d\r\n,ms[0]:%f\r\n,ms[1]:%f\r\n",index[0],index[1],ms[0],ms[1]);
}
/******************************************************************************
函数功能：
*   @ 量化
参数描述:
*   @  *vPID：模糊PID参数,pv：读取的值,*qValue：量化得到的值
返回值描述：
*   @ 无
说明：
*   @ (输入值的模糊化)线性量化操作函数,论域{-6，-5，-4，-3，-2，-1，0，1，2，3，4，5，6}
******************************************************************************/
void LinearQuantization(FUZZYPID *vPID, float pv, float *qValue)
{
	float thisError;
	float deltaError;

	thisError = vPID->setpoint - pv;		  // 计算偏差值
	deltaError = thisError - vPID->lasterror; // 计算偏差增量

	qValue[0] = 6.0f * thisError / (vPID->maximum - vPID->minimum);	// E的量化值
	qValue[1] = 3.0f * deltaError / (vPID->maximum - vPID->minimum); // Ec的量化值

	if (qValue[0] > 6)
	{
		qValue[0] = 6;
	}
	else if (qValue[0] < -6)
	{
		qValue[0] = -6;
	}

	if (qValue[1] > 3)
	{
		qValue[1] = 3;
	}
	else if (qValue[1] < -3)
	{
		qValue[1] = -3;
	}
}

/******************************************************************************
函数功能：
*   @ 解模糊
参数描述:
*   @  *vPID：模糊PID参数,_Real_Value：读取的值
返回值描述：
*   @ 无
说明：
*   @
******************************************************************************/
void FuzzyComputation(FUZZYPID *vPID, float _Real_Value)
{
	// 量化值
	float qValue[2] = {0, 0};

	int indexE[2] = {0, 0}; // e在规则库中的索引
	float msE[2] = {0, 0};	// e的隶属度

	int indexEC[2] = {0, 0}; // ec在规则库中的索引
	float msEC[2] = {0, 0};	 // ec的隶属度

	// pid增量值
	float pidvalue[3];

	// 量化
	LinearQuantization(vPID, _Real_Value, qValue);
	// 计算e的隶属度和索引
	CalcMembership(msE, qValue[0], indexE);
	// 计算ec的隶属度和索引
	CalcMembership(msEC, qValue[1], indexEC);

	// 采用重心法计算pid增量值
	pidvalue[0] = msE[0] * (msEC[0] * ruleKp[indexE[0]][indexEC[0]] + msEC[1] * ruleKp[indexE[0]][indexEC[1]]) + msE[1] * (msEC[0] * ruleKp[indexE[1]][indexEC[0]] + msEC[1] * ruleKp[indexE[1]][indexEC[1]]);
	pidvalue[1] = msE[0] * (msEC[0] * ruleKi[indexE[0]][indexEC[0]] + msEC[1] * ruleKi[indexE[0]][indexEC[1]]) + msE[1] * (msEC[0] * ruleKi[indexE[1]][indexEC[0]] + msEC[1] * ruleKi[indexE[1]][indexEC[1]]);
	pidvalue[2] = msE[0] * (msEC[0] * ruleKd[indexE[0]][indexEC[0]] + msEC[1] * ruleKd[indexE[0]][indexEC[1]]) + msE[1] * (msEC[0] * ruleKd[indexE[1]][indexEC[0]] + msEC[1] * ruleKd[indexE[1]][indexEC[1]]);

	// pid增量修正
	vPID->data_kp = vPID->qKp * pidvalue[0];
	vPID->data_ki = vPID->qKi * pidvalue[1];
	vPID->data_kd = vPID->qKd * pidvalue[2];
}

/******************************************************************************
函数功能：
*   @
参数描述:
*   @  _Set_Vaule：设置的值,_Measure_Vaule：读取的值
返回值描述：
*   @ 无
说明：
*   @
******************************************************************************/
void COOL_Fuzzytrans(FUZZYPID *vPID, float _Set_Vaule, float _Measure_Vaule)
{
	//float dError;
	//float I_dead = 0;

	vPID->iError = _Set_Vaule - _Measure_Vaule;
	vPID->setpoint = _Set_Vaule;

	// 输入量的最大最小值
	vPID->maximum = 4;
	vPID->minimum = 0;
	FuzzyComputation(vPID, _Measure_Vaule);

	vPID->preerror = vPID->lasterror;
	vPID->lasterror = vPID->iError;

	//
	//	return (-FPID.data_kp * FPID.iError*10						//比例项
	//				+		FPID.data_ki * FPID.SumError *begin_flag			//积分项
	//				+		FPID.data_kd * dError					//微分项
	//		);
}

// float PID_Vout(short vSet, short vin)
// {
// 	//	float Vout=0;
// 	//	short vset;
// 	//	short vIn;
// 	//
// 	//	vset = vSet; 				// 设置PID目标
// 	//	vIn  = vin; 							// 读取输入
// 	//	Vout = COOL_Fuzzytrans( vset,vIn ); // 执行PID交互影响
// 	//
// 	//	return Vout;									//返回vOut

// }

