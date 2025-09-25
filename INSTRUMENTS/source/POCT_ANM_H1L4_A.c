/**
  * @file    POCT_ANM_H1L4_A.c
  * @brief   全自动核酸分析仪运动控制模块
  * @author  仪器开发组
  * @date    2023-08-20
  * @version 1.2
  * @note    包含电机驱动配置、GPIO初始化、步进控制算法等核心功能
  */
#include "InstrumentInclude.h"

#ifdef POCT_ANM_H1L4_A

#include "bsp_motor_conf.h" 

//泵吸打控制
#define PUMP_IN      0
#define PUMP_OUT     1

#define	TEMP2_SEND   0 
#define	TIME2_SEND   1 
#define	TEMP3_SEND   2 
#define	TIME3_SEND   3 

#define MIX_V        30000
#define MIX_FRE      2500

//#define PUBLIC_HOLE  1 
//#define LQD_HOLE     9    //液体分配孔
//#define LYSTATE      1  
//#define EFFLUENT_1   2  
//#define WASHING_1    3  
//#define WASHING_2    4  
//#define WASHING_3    5 
//#define ELUTION      6
//#define DRYING       7

#define PUBLIC_HOLE  7 
#define LQD_HOLE     1    //液体分配孔
#define SAMPLE       8  
#define EFFLUENT_1   9    //废液磁珠 
#define WASHING_1    10    //4
#define WASHING_2    11  
#define WASHING_3    6    //2
#define ELUTION      5
#define DRYING       3

static uint8_t runStep = 0;

TaskHandle_t VibrateTask_Handler;

//void MagneticSet_UD(uint16_t step,uint16_t fre,uint8_t times,uint8_t WaitTime);

#define MOTOR_CONFIG(n) \
    { \
        n##_PUL_PIN, n##_DIR_GPIO_PIN, n##_EN_GPIO_PIN, n##_GO_GPIO_PIN, \
        n##_PUL_PORT, n##_DIR_GPIO_PORT, n##_EN_GPIO_PORT, n##_GO_GPIO_PORT \
    }

Motor_Conf_TypeDef Motor_Conf[MOTOR_CONF_NUM] = {
    MOTOR_CONFIG(M01),
    MOTOR_CONFIG(M02),
    MOTOR_CONFIG(M03),
    MOTOR_CONFIG(M04),
    MOTOR_CONFIG(M05),
    MOTOR_CONFIG(M06),
    MOTOR_CONFIG(M07),
    MOTOR_CONFIG(M08),
    MOTOR_CONFIG(M09),
    MOTOR_CONFIG(M10),
    MOTOR_CONFIG(M11),
    MOTOR_CONFIG(M12)
};

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
void Motor_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();	
	__HAL_RCC_GPIOE_CLK_ENABLE();	
	__HAL_RCC_GPIOF_CLK_ENABLE();

	Globle.LastValvePos = 0;
	Globle.CurrentValvePos = 0;
	
	for(int i = 0; i < MOTOR_CONF_NUM; i++)
	{																	   
		GPIO_InitStruct.Pin   = Motor_Conf[i].dir_pin;	
		GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
		GPIO_InitStruct.Pull  = GPIO_PULLUP;	  
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(Motor_Conf[i].dir_port, &GPIO_InitStruct);
		  
		GPIO_InitStruct.Pin = Motor_Conf[i].en_pin;
		HAL_GPIO_Init(Motor_Conf[i].en_port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(Motor_Conf[i].en_port,Motor_Conf[i].en_pin,HIGH1);
		  
		GPIO_InitStruct.Pin   = Motor_Conf[i].go_pin;
		GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull  = GPIO_PULLUP;          
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;     
		HAL_GPIO_Init(Motor_Conf[i].go_port, &GPIO_InitStruct); 

		GPIO_InitStruct.Pin   = Motor_Conf[i].pul_pin;
		GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull  = GPIO_PULLUP;          
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;    
		HAL_GPIO_Init(Motor_Conf[i].pul_port,&GPIO_InitStruct);	  	
	}
}

// 阀旋转控制函数（带空程角补偿）
uint32_t Valve_Rotation(int TargetPos, uint8_t Direction)
{
    uint32_t step = 0;
//    static uint8_t LastDirection = 0;  // 记录上一次旋转方向
	
	uint32_t StepsPerDegree = (Globle.POCT_Par.M10_Fa_P[2] - Globle.POCT_Par.M10_Fa_P[1]); 
		
//	uint32_t offset0 = (Globle.POCT_Par.M10_Fa_P[1] - Globle.POCT_Par.M10_Fa_P[0]) - StepsPerDegree;
	
    uint32_t backlash_compensation = (Globle.POCT_Par.M10_Fa_P[2] - Globle.POCT_Par.M10_Fa_P[3]) - StepsPerDegree;  // 空程角补偿值

    // 计算基础步数
    uint32_t base_step = abs(TargetPos - (int)Globle.CurrentValvePos) * StepsPerDegree;

    // 如果目标位置与当前位置相同，不执行旋转
    if (TargetPos == Globle.CurrentValvePos)
    {
        return 0;
    }

    // 根据旋转方向计算实际步数
    if (Direction == CCW)  // 顺时针
    {
        step = Stepper[M10].position + base_step;

        // 如果旋转方向改变，添加空程角补偿
        if (Globle.LastDirection != Direction)
        {
            step += backlash_compensation;
        }
    }
    else  // 逆时针
    {
        step = Stepper[M10].position - base_step;

        // 如果旋转方向改变，添加空程角补偿
        if (Globle.LastDirection != Direction)
        {
            step -= backlash_compensation;
        }
    }
	
//	Motor_Run(step, M10, Direction, 800);
	MotorFre_Abs_Run(step,M10,M10_DIR,500);

    // 更新状态变量
    Globle.LastDirection = Direction;
    Globle.LastValvePos = Globle.CurrentValvePos;
    Globle.CurrentValvePos = TargetPos;
			   
    return step;
}

void hotCap(SMsg * pMsg,uint16_t Temp)
{
	uint16_t temp_up_cnt = 0;
	
	uint16_t SendT2T3buf[4] = {0};
	
	uint32_t err_cnt = 0;
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
	
	Globle.TempControl.Temp_Ch4_ENABLE  = SET;
	
	Globle.TempControl.Temp_Ch4_Set = Temp;
	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_1);
	
	while(GetTemp(4) >= (Temp+10))
	{
		err_cnt ++;		
		
		temp_up_cnt++;
		
		if(temp_up_cnt >= 100)
		{		
			SendT2T3buf[TEMP2_SEND] = GetTemp(1); 
					
			if((SendT2T3buf[TEMP2_SEND] > Globle.TempControl.Temp_Ch2_Set - 5)&&(SendT2T3buf[TEMP2_SEND] < Globle.TempControl.Temp_Ch2_Set + 5))
			{
				SendT2T3buf[TEMP2_SEND] =  Globle.TempControl.Temp_Ch2_Set;
			}
			
			SendT2T3buf[TEMP3_SEND] = GetTemp(2); 
			
			if((SendT2T3buf[TEMP3_SEND] > Globle.TempControl.Temp_Ch3_Set - 5)&&(SendT2T3buf[TEMP3_SEND] < Globle.TempControl.Temp_Ch3_Set + 5))
			{
				SendT2T3buf[TEMP3_SEND] =  Globle.TempControl.Temp_Ch3_Set;
			}
			
			SendT2T3Values(pMsg,SendT2T3buf);
			temp_up_cnt = 0;
		}
		
		if(err_cnt > 8*60*100)
		{
			SaveFaultCode("HOTCAP_TIMEOUT");
		}
				
		delay_ms(5);
	}
}

/**********************************************************
 * @brief 液体分离操作函数
 *
 * 该函数根据传入的步数、方向和频率控制电机进行液体分离操作。
 * 同时会进行超时检测，若超时则停止电机并保存错误代码。
 *
 * @param step 电机运行的步数
 * @param dir 电机运行的方向，PUMP_IN 或 PUMP_OUT
 * @param u16Fre 电机运行的最大频率
 * @return 操作结果，0 表示成功，1 表示失败
 **********************************************************/
uint8_t Liquid_seperation(uint32_t step, uint8_t dir, uint16_t u16Fre)
{
    uint16_t TimeOut_t = 0;
    SpeedCalc_TypeDef * pSpeed = NULL;
	uint8_t motor_dir = 0;

    // 计算电机运行频率
    uint16_t fre = Motor_Fre(step);
    // 限制频率不超过最大频率
    if (fre > u16Fre) {
        fre = u16Fre;
    }

    // 分配内存
    pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
    if (pSpeed == NULL) {
        // 内存分配失败，保存错误代码并返回错误
        SaveFaultCode("F^LIQUID_SEPARATION_MEM_ALLOC_FAILED");
        return 1;
    }

    // 根据方向和电机默认方向确定电机实际运行方向
    motor_dir = (dir == PUMP_IN) ? ((M07_DIR == CCW) ? CCW : CW) : ((M07_DIR == CCW) ? CW : CCW);

    // 调用电机移动函数
    bool err = Stepper_Move_S(MIN_FRE, fre, 0.01f, step, M07, motor_dir, pSpeed);
    if (!err) {
        // 电机移动失败，保存错误代码并返回错误
        Motor_Stop(M07, pSpeed);
        vPortFree(pSpeed);
        SaveFaultCode("F^LIQUID_SEPARATION_MOVE_FAILED");
        return 1;
    }
    // 更新电机位置
    if (dir == PUMP_IN) {
        Stepper[M07].position -= step;
    } else {
        Stepper[M07].position += step;
    }

    // 进行超时检测
    while (1) {
        if (LASER_PHOTODIDE_Scan() == 1) {
            break;
        }

        if (Stepper[M07].status == STOP) {
            // 电机停止，保存错误代码并返回错误
            Motor_Stop(M07, pSpeed);
            vPortFree(pSpeed);
            SaveFaultCode("F^FY_TIMEOUT");
            return 1;
        }

        if (TimeOut_t > 10 * 1000) {
            // 超时，保存错误代码并返回错误
            Motor_Stop(M07, pSpeed);
            vPortFree(pSpeed);
            SaveFaultCode("F^FY_TIMEOUT");
            return 1;
        }

        vTaskDelay(10);
        TimeOut_t++;
    }

    // 操作完成，停止电机并释放内存
    Motor_Stop(M07, pSpeed);
    vPortFree(pSpeed);

    return 0;
}

/**********************************************************
 * @brief 泵操作函数
 *
 * 该函数根据传入的步数、方向和频率控制电机 M07 进行泵操作。
 * 会根据电机默认方向和传入的方向确定电机实际运行方向，
 * 并更新电机的位置。若电机运行失败，会保存错误代码。
 *
 * @param step 电机运行的步数
 * @param dir 电机运行的方向，PUMP_IN 或 PUMP_OUT
 * @param u16Fre 电机运行的最大频率
 *********************************************************/
void PUMP_Func(uint32_t step, uint8_t dir, uint16_t u16Fre)
{
    bool err = true;
    SpeedCalc_TypeDef * pSpeed = NULL;
    uint16_t fre = 0;
    uint8_t motor_dir;

    // 计算电机运行频率
    fre = Motor_Fre(step);
    // 限制频率不超过最大频率
    if (fre > u16Fre) {
        fre = u16Fre;
    }

    // 分配内存
    pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
    if (pSpeed == NULL) {
        // 内存分配失败，保存错误代码并返回
        SaveFaultCode("F^PUMP_FUNC_MEM_ALLOC_FAILED");
        return;
    }

    // 根据方向和电机默认方向确定电机实际运行方向
    motor_dir = (dir == PUMP_IN) ? ((M07_DIR == CCW) ? CCW : CW) : ((M07_DIR == CCW) ? CW : CCW);

    // 调用电机移动函数
    err = Stepper_Move_S(MIN_FRE, fre, 0.01f, step, M07, motor_dir, pSpeed);

    // 更新电机位置
    if (dir == PUMP_IN) {
        Stepper[M07].position -= step;
    } else {
        Stepper[M07].position += step;
    }

    // 检查电机运行是否失败
    if (!err) {
        SaveFaultCode("F^M07_TIMEOUT");
        vPortFree(pSpeed);
        return;
    }

    // 等待电机停止
    while (Stepper[M07].status != STOP) {
        vTaskDelay(1);
    }

    // 释放内存
    vPortFree(pSpeed);
}

/**
 * @brief 泵混合控制函数
 * @param times 混合次数
 * @param volume 混合体积
 * @param waitTime 每次混合后的等待时间
 * @param frequency 电机运行频率
 * @note 该函数控制泵进行指定次数的混合操作，每次操作包含抽取和释放指定体积的液体
 */
void PumpMixControl(uint8_t times, uint32_t volume, uint8_t waitTime, uint16_t frequency)
{
    // 参数验证
    if (times == 0) return;  // 混合次数不能为0
    if (frequency == 0) return;  // 频率不能为0

    uint32_t targetPosition = Globle.POCT_Par.M07_P1 - volume;
    uint32_t homePosition = Globle.POCT_Par.M07_P1;

    for (uint8_t i = 0; i < times; i++)
    {
        // 抽取液体
        MotorFre_Abs_Run(targetPosition, M07, M07_DIR, frequency);
        
        // 释放液体
        MotorFre_Abs_Run(homePosition, M07, M07_DIR, frequency);

        // 等待指定时间
        Wait_Time(waitTime);
    }
}

/***********************************************************************************
 * @brief 控制 LED 扫描到指定通道
 *
 * 该函数根据传入的 LED 通道号，控制电机移动到相应位置，并通过 SPI 发送数据点亮 LED。
 * 若传入的通道号无效，会保存错误代码并直接返回。
 *
 * @param led_channel 要扫描的 LED 通道号
 ***********************************************************************************/
void led_scan(uint8_t led_channel)
{
    // 计算数组索引
   uint8_t num = led_channel - 1;

    // 根据索引选择合适的电机运行函数
    if (num == 0) {
        Motor_Abs_Run(Globle.POCT_Par.M08_LED_P[num], M08);
    } else {
        MotorFre_Abs_Run(Globle.POCT_Par.M08_LED_P[num], M08, M08_DIR, 3500);
    }

    // 通过 SPI 发送数据点亮 LED
    LED_SPI_SendData(num, (uint8_t)Globle.POCT_Par.LED[num]);

    // 延时 10ms
    delay_ms(10);
}

/*********************************************
  * @brief  设置相机和LED的电机运动组合 扫描各孔；
  * @param  cam_chanel：孔号 ，led_chanel：灯号
  * @param  0：成功 ，1：失败
  * @retval 无
**********************************************/
uint8_t LED_CAM_Scan(uint8_t cam_chanel,uint8_t led_chanel)
{	
	ALL_LED_OFF();
	
	led_scan(led_chanel);
	
	return 0;	
}

/*****************************************************
 * @brief 发送T1温度数据
 *
 * 该函数用于获取指定通道的温度，并将温度数据发送出去。
 *
 * @param pMsg 指向消息结构体的指针
 * @param SendT1buf 用于存储温度数据的数组
 * @param channel 温度通道号
 ****************************************************/
void send_T1_temp(SMsg * pMsg, uint16_t *SendT1buf, uint8_t channel) {
    SendT1buf[0] = GetTemp(channel) / 10;
    SendT1Values(pMsg, SendT1buf);
}

/**
 * @brief 发送T1温度和倒计时数据
 *
 * 该函数用于获取指定通道的温度，并将温度和倒计时数据发送出去。
 *
 * @param pMsg 指向消息结构体的指针
 * @param SendT1buf 用于存储温度和倒计时数据的数组
 * @param channel 温度通道号
 * @param countdown 倒计时值
 */
void send_T1_temp_and_countdown(SMsg * pMsg, uint16_t *SendT1buf, uint8_t channel, uint16_t countdown) {
    SendT1buf[0] = GetTemp(channel) / 10;
    SendT1buf[1] = countdown;
    SendT1Values(pMsg, SendT1buf);
}

/********************************************************************************************
 * @brief 温度加热函数
 *
 * 该函数用于控制温度加热过程，包括升温阶段和保持温度阶段。
 *
 * @param pMsg 指向消息结构体的指针
 * @param SendT1buf 用于存储温度数据的数组
 ********************************************************************************************/
void T1_Heating(SMsg * pMsg)
{
    uint16_t temp_up_cnt = 0, countdown = 0, mixTime = 0;
    uint32_t TimeNowCnt = 0;
    uint16_t SendT1buf[2] = {0};
    // 初始化发送缓冲区
    memset(SendT1buf, 0, sizeof(SendT1buf));
    // 启用温度通道1
    Globle.TempControl.Temp_Ch1_ENABLE = SET;
    // 启动PWM定时器
    HAL_TIM_PWM_Start(&TIM8_Handler, TIM_CHANNEL_3);
    // 升温阶段        
    while (1) {
        temp_up_cnt++;
        // 每100次循环发送一次温度数据
        if (temp_up_cnt >= 100) {
            send_T1_temp(pMsg, SendT1buf, 0);
            temp_up_cnt = 0;
        }
        vTaskDelay(10);
        // 检查是否达到设定温度
        if (SendT1buf[0] >= Globle.TempControl.Temp_Ch1_Set - 40) {
            TimeNowCnt = HAL_GetTick();
            mixTime = Globle.TempControl.Temp_T1;
            // 保持温度阶段，进行倒计时
            while (TimeNowCnt + mixTime * 1000 > HAL_GetTick()) {
                if (temp_up_cnt >= 100) {
                    send_T1_temp_and_countdown(pMsg, SendT1buf, 0, countdown);
                    countdown++;
                    temp_up_cnt = 0;
                }
                vTaskDelay(10);
                temp_up_cnt++;
            }
            break;
        }
    }
}

/****************************************************************************
 * @brief 对温度值进行过滤处理
 *
 * 该函数根据设定的温度范围对实际温度值进行过滤，如果实际温度值在设定温度的一定范围内，
 * 则将其替换为设定温度值。
 *
 * @param temp 实际温度值
 * @param temp_set 设定温度值
 * @param range 允许的温度范围
 * @return 过滤后的温度值
 **************************************************************************/
uint16_t filter_temperature(uint16_t temp, uint16_t temp_set, uint16_t range)
{
    if ((temp > temp_set - range) && (temp < temp_set + range))
    {
        return temp_set;
    }
    return temp;
}

/**
 * @brief 温度过滤函数
 *
 * 该函数用于获取指定通道的温度，并根据设定的温度范围对温度值进行过滤处理，
 * 最后将过滤后的温度值存储在 SendT2T3buf 数组中。
 *
 * @param pMsg 指向消息结构体的指针
 * @param temp2_set 通道 2 的设定温度值
 * @param temp3_set 通道 3 的设定温度值
 * @param SendT2T3buf 用于存储过滤后温度值的数组
 */
void temp_filtering(SMsg * pMsg, uint16_t temp2_set, uint16_t temp3_set, uint16_t *SendT2T3buf)
{
    // 检查传入的指针是否有效
    if (pMsg == NULL || SendT2T3buf == NULL)
    {
        return;
    }

    // 获取通道 3 的温度并进行过滤处理
    SendT2T3buf[TEMP3_SEND] = filter_temperature(GetTemp(2) / 10, temp3_set, 3);

    // 延时 5 个时间单位
    vTaskDelay(5);

    // 获取通道 2 的温度并进行过滤处理
    SendT2T3buf[TEMP2_SEND] = filter_temperature(GetTemp(1) / 10, temp2_set, 3);

    // 注释掉的代码，可根据实际需求决定是否保留
    // Send_T2T3(pMsg, SendT2T3buf);
}
/**********************************************************
 * @brief 温度上升趋势
 *
 * 该函数用于检测指定通道的温度是否达到或超过设定温度。
 * 如果温度未达到设定温度，则等待一段时间后再次检测。
 *
 * @param pMsg 指向消息结构体的指针
 * @param channel 温度通道号
 * @return true 如果温度达到或超过设定温度，否则返回 false
 **********************************************************/
bool temperatureTrend(SMsg * pMsg,u8 channel)
{
	u16 temp[3],SendT2T3buf[4] = {0},temp_up_cnt = 0;

	u8 TempCnt = 0;
	
	memset(temp,0,sizeof(temp));
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
	
	temp[0] = GetTemp(channel);

	temp[1] = 0xffff;
	
	if(channel == 0)
		temp[2] = Globle.TempControl.Temp_Ch1_Set;
	else if(channel == 1)
		temp[2] = Globle.TempControl.Temp_Ch2_Set;
	else if(channel == 2)
		temp[2] = Globle.TempControl.Temp_Ch3_Set;
	else if(channel == 3)
		temp[2] = Globle.TempControl.Temp_Ch4_Set;

	while (temp[2] - 5 < GetTemp(channel))
	{		
		temp_up_cnt++;
		
		if(temp_up_cnt >= 100)
		{		
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);	
			
			SendT2T3Values(pMsg,SendT2T3buf);
			
			temp_up_cnt = 0;
		}
		
		delay_ms(10);
	}
	
	while (1)
	{
		temp_up_cnt++;
		
		if(temp_up_cnt >= 100)
		{		
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);	
			
			SendT2T3Values(pMsg,SendT2T3buf);
			
			temp_up_cnt = 0;

			temp[TempCnt] =  GetTemp(channel);

			TempCnt++;

			if(TempCnt>1)
				TempCnt=0;
		}

		if(temp[1]<=temp[0])
			break;
		
		delay_ms(10);
	}

	return true;	
}

/**
 * @brief 发送温度数据并处理定时器标志
 *
 * 该函数用于过滤温度数据，将其存储在 SendT2T3buf 中，并通过 Send_T2T3 函数发送出去。
 * 同时，将 myTimer_flag 标志置为 0，表示定时器事件已处理。
 *
 * @param pMsg 指向消息结构体的指针
 * @param SendT2T3buf 用于存储温度数据的数组
 */
void send_temperature_data(SMsg * pMsg, uint16_t *SendT2T3buf)
{
    temp_filtering(pMsg, Globle.TempControl.Temp_Ch2_Set, Globle.TempControl.Temp_Ch3_Set, SendT2T3buf);
    SendT2T3Values(pMsg, SendT2T3buf);
    myTimer_flag = 0;
}

 // 辅助函数：处理温度数据发送和倒计时
static void handle_temperature_data(SMsg *pMsg, uint16_t *SendT2T3buf, uint16_t *countdown, uint8_t temp_channel) 
{
    temp_filtering(pMsg, Globle.TempControl.Temp_Ch2_Set, Globle.TempControl.Temp_Ch3_Set, SendT2T3buf);
    if (countdown) {
        (*countdown)++;
        SendT2T3buf[temp_channel == 2 ? TIME2_SEND : TIME3_SEND] = *countdown;
    }
    SendT2T3Values(pMsg, SendT2T3buf);
    myTimer_flag = 0;
}


// 线性升温参数配置 - 0.03度/秒
#define HEAT_LINEAR_RATE      0.5f   // 每秒升温0.03℃
#define TEMP_CHECK_INTERVAL   100     // 温度检查间隔100ms
#define TEMP_STABILITY_TIME   0    // 稳定时间5秒
#define TEMP_STABILITY_MARGIN 0.1f    // 温度稳定容差0.1℃

SlowHeatCtrl_t heatCtrl = {0};

/**
 * @brief 线性缓慢升温函数 - 0.03℃/秒
 * @param targetTemp 目标温度(℃)
 * @param pMsg 消息指针
 * @param pFaultCode 故障码指针
 * @return 升温完成状态 true=完成 false=进行中/失败
 */
bool Slow_Heating_Linear(uint16_t targetTemp, int *pFaultCode)
{  
    uint16_t SendT2T3buf[4] = {0};
    uint32_t currentTime = HAL_GetTick();
        
    // 获取当前温度
//    if (myTimer_flag) {		
//        handle_temperature_data(pMsg, SendT2T3buf, NULL, 3);
		SendT2T3buf[TEMP3_SEND] = GetTemp(1) / 10;		
//		SendT2T3Values(pMsg, SendT2T3buf);
//		myTimer_flag = 0;
//    }
    
    // 初始化升温控制
    if (!heatCtrl.isHeating) {
        heatCtrl.targetTemp = targetTemp;
        heatCtrl.startTemp = SendT2T3buf[TEMP3_SEND];
        heatCtrl.startTime = currentTime;
        heatCtrl.isHeating = true;
        heatCtrl.lastSetTemp = (float)heatCtrl.startTemp;
		
        
        // 启动加热系统
        Globle.TempControl.PcrTempSet = heatCtrl.startTemp;
        HAL_TIM_PWM_Start(&TIM8_Handler, TIM_CHANNEL_4);
        Globle.TempControl.Temp_Ch2_ENABLE = SET;
        
//        printf("[Heat] Linear heating start: %.1f℃ → %d℃ (%.2f℃/s)\r\n", 
//               heatCtrl.startTemp, heatCtrl.targetTemp, HEAT_LINEAR_RATE);
    }
    
    // 计算当前应该达到的温度
    float elapsedSeconds = (currentTime - heatCtrl.startTime) / 1000.0f;
    float currentTarget = heatCtrl.startTemp + (elapsedSeconds * HEAT_LINEAR_RATE);
    
    // 限制不超过目标温度
    if (currentTarget > heatCtrl.targetTemp) {
        currentTarget = heatCtrl.targetTemp;
    }
    
    // 更新设定温度（平滑过渡）
    if (fabs(currentTarget - heatCtrl.lastSetTemp) >= 0.01f) {
        heatCtrl.lastSetTemp = currentTarget;
        Globle.TempControl.PcrTempSet = (uint16_t)(currentTarget + 0.5f);
    }
    
    // 检查是否达到目标温度
    uint16_t currentTemp = SendT2T3buf[TEMP3_SEND];
	
	static uint32_t reachTime = 0;
    if (currentTemp >= heatCtrl.targetTemp - TEMP_STABILITY_MARGIN) {
        // 保持稳定5秒       
        if (reachTime == 0) {
            reachTime = currentTime;
        }
        
        if ((currentTime - reachTime) >= TEMP_STABILITY_TIME) {
            heatCtrl.isHeating = false;
            reachTime = 0;
//            printf("[Heat] Linear heating completed: %.1f℃ reached\r\n", 
//                   currentTemp);
            return true;
        }
    } else {
        // 重置稳定计时
        reachTime = 0;
    }
    
//    // 超时检查（30分钟）
//    if (currentTime - heatCtrl.startTime > 1800000) {
//        SaveFaultCode("F^pMsg Is NULL");
//        heatCtrl.isHeating = false;
//        return false;
//    }
    
//    // 打印进度信息（每10秒）
//    static uint32_t lastPrintTime = 0;
//    if (currentTime - lastPrintTime >= 10000) {
//        printf("[Heat] Progress: %.1f℃/%.1f℃ (%.1f%%)\r\n", 
//               currentTemp, currentTarget, 
//               (currentTarget - heatCtrl.startTemp) * 100.0f / (heatCtrl.targetTemp - heatCtrl.startTemp));
//        lastPrintTime = currentTime;
//    }
    
    return false;
}

/**
 * @brief 停止线性升温
 */
void Slow_Heating_Linear_Stop(void)
{
//    extern struct heatCtrl;
    heatCtrl.isHeating = false;
    
    HAL_TIM_PWM_Stop(&TIM8_Handler, TIM_CHANNEL_4);
    Globle.TempControl.Temp_Ch2_ENABLE = RESET;
    
//    printf("[Heat] Linear heating stopped\r\n");
}

/**
 * @brief 获取升温进度百分比
 * @return 进度百分比 0-100
 */
uint8_t Slow_Heating_Progress(void)
{
    if (!heatCtrl.isHeating) return 100;
    
    uint16_t totalRange = heatCtrl.targetTemp - heatCtrl.startTemp;
    uint16_t currentProgress = heatCtrl.currentTemp - heatCtrl.startTemp;
    
    if (totalRange == 0) return 100;
    
    return (currentProgress * 100) / totalRange;
}


/*******************************************************************
  * @brief  逆转录
  * @param  pMsg:传输信息帧
  * @param  0：成功 ，1：失败
  * @retval 无
  *****************************************************************/
bool App_NZL(SMsg *pMsg, int *pFaultCode)
{
	uint16_t SendT2T3buf[4] = {0}, countdown = 0,fre;
	
	uint32_t TimeNowCnt = 0;
	
	FAN_RUN(1);
	
	LED_EventGroupSetbit(PCR_LED_FLAG);
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
	
	Motor_Rst(M01_FRE,M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN,0);
	
	Motor_Rst(M02_FRE,M02,M02_DIR,M02_GO_GPIO_PORT,M02_GO_GPIO_PIN,0);
	
	fre = Motor_Fre(Globle.POCT_Par.M02_P2);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M02_P2,M02,M02_DIR,fre);	
	
	MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,3500);	
	
	if (TimerHandle != NULL) {	
		xTimerStart(TimerHandle, 0);
	}

	while(1)
	{		
		if(myTimer_flag)
		{
			temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);
			
			SendT2T3Values(pMsg,SendT2T3buf);
			
			myTimer_flag = 0;
		}
		
		vTaskDelay(5);

		if(SendT2T3buf[TEMP3_SEND] >= Globle.TempControl.Temp_Ch3_Set - 2)
		{
			xTimerStop(TimerHandle, 0);

			xTimerStart(TimerHandle, 0);

			TimeNowCnt  = HAL_GetTick();
			
			countdown = 0;	

			while(TimeNowCnt + Globle.TempControl.Temp_T3*1000 > HAL_GetTick())
			{						
				if(myTimer_flag)
				{
					temp_filtering(pMsg,Globle.TempControl.Temp_Ch2_Set,Globle.TempControl.Temp_Ch3_Set,&SendT2T3buf[0]);
					
					countdown++;
					
					SendT2T3buf[TIME3_SEND] = countdown;
					
					SendT2T3Values(pMsg,SendT2T3buf);
					
					myTimer_flag = 0;
				}
											
				vTaskDelay(5);				
			}

			countdown++;
			
			SendT2T3buf[TIME3_SEND] = countdown;
			
			SendT2T3Values(pMsg,SendT2T3buf);
			
			break;	
		}
	}	
	FAN_RUN(0);
	
	xTimerStop(TimerHandle, 0);
	
	return true;	
	
	
/****************************************************	
	uint16_t SendT2T3buf[4] = {0}, countdown = 0,fre;
	
	uint32_t TimeNowCnt = 0;
	
//	FAN_RUN(1);
	
//	Globle.TempControl.PcrTempSet = Globle.TempControl.Temp_Ch3_Set;
	
//	// 启动 TIM8 和 TIM9 的 PWM 输出
//    HAL_TIM_PWM_Start(&TIM8_Handler, TIM_CHANNEL_4);

//  // 启用温度控制通道
//    Globle.TempControl.Temp_Ch2_ENABLE = SET;
	
	LED_EventGroupSetbit(PCR_LED_FLAG);
	
	memset(SendT2T3buf,0,sizeof(SendT2T3buf));
	
	Motor_Rst(M01_FRE,M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN,0);
	
	Motor_Rst(M02_FRE,M02,M02_DIR,M02_GO_GPIO_PORT,M02_GO_GPIO_PIN,0);
	
	fre = Motor_Fre(Globle.POCT_Par.M02_P2);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M02_P2,M02,M02_DIR,fre);	
	
	MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,3500);	
	
	if (TimerHandle != NULL) {	
		xTimerStart(TimerHandle, 0);
	}
	
	Globle.TempControl.PcrTempSet = 400;         
	
	// 启动 TIM8 和 TIM9 的 PWM 输出
    HAL_TIM_PWM_Start(&TIM8_Handler, TIM_CHANNEL_4);

    // 启用温度控制通道
    Globle.TempControl.Temp_Ch2_ENABLE = SET;
					
	while (1) {
        if (myTimer_flag) {
            handle_temperature_data(pMsg, SendT2T3buf, NULL, 3);
        }

        vTaskDelay(10);


        // 温度达到设定值
        if ((SendT2T3buf[TEMP3_SEND] >= Globle.TempControl.PcrTempSet - 2)&&(SendT2T3buf[TEMP3_SEND] <= Globle.TempControl.PcrTempSet+1)) {
            if (TimerHandle != NULL) {
                xTimerStop(TimerHandle, 0);
                xTimerStart(TimerHandle, 0);
            }

            // 倒计时阶段
            TimeNowCnt = HAL_GetTick();
            while (TimeNowCnt + Globle.TempControl.Temp_T3 * 1000 > HAL_GetTick()) {
                if (myTimer_flag) {
                    handle_temperature_data(pMsg, SendT2T3buf, &countdown, 3);
                }
                vTaskDelay(1);
            }

            // 最终发送
            handle_temperature_data(pMsg, SendT2T3buf, &countdown, 3);
            break;
        }
    }
	
//	Globle.TempControl.PcrTempSet = Globle.TempControl.Temp_Ch3_Set;  

	heatCtrl.needSlowHeating = true;
	
	// 启动 TIM8 和 TIM9 的 PWM 输出
    HAL_TIM_PWM_Start(&TIM8_Handler, TIM_CHANNEL_4);

    // 启用温度控制通道
    Globle.TempControl.Temp_Ch2_ENABLE = SET;
	
	while(1){
		 if (myTimer_flag) {
            handle_temperature_data(pMsg, SendT2T3buf, NULL, 3);
        }
		vTaskDelay(100);
		
		Slow_Heating_Linear(Globle.TempControl.Temp_Ch3_Set,NULL);
		
        // 温度达到设定值
        if ((SendT2T3buf[TEMP3_SEND] >= Globle.TempControl.Temp_Ch3_Set - 1)&&(SendT2T3buf[TEMP3_SEND] <= Globle.TempControl.Temp_Ch3_Set + 1)) {
            if (TimerHandle != NULL) {
                xTimerStop(TimerHandle, 0);
                xTimerStart(TimerHandle, 0);
            }
			break;
		}

//            // 倒计时阶段
//            TimeNowCnt = HAL_GetTick();
//            while (TimeNowCnt + Globle.TempControl.Temp_T3 * 1000 > HAL_GetTick()) {
//                if (myTimer_flag) {
//                    handle_temperature_data(pMsg, SendT2T3buf, &countdown, 3);
//                }
//                vTaskDelay(1);
//            }

//            // 最终发送
//            handle_temperature_data(pMsg, SendT2T3buf, &countdown, 3);
//            break;
//        }
	
	}
	

    // 清理阶段
//    motor_fast_rst(M01, M01_DIR, M01_GO_GPIO_PORT, M01_GO_GPIO_PIN);
//    FAN_RUN(0);
    if (TimerHandle != NULL) {
        xTimerStop(TimerHandle, 0);
    }

    return true;
**************************************/
}

/***********************************************
  * @brief  预变性
  * @param  
  * @param  0：成功 ，1：失败
  * @retval 无
  *********************************************/
bool App_YBX(SMsg *pMsg, int *pFaultCode)
{	
	uint16_t SendT2T3buf[4] = {0};
    uint16_t countdown = 0;
    uint32_t TimeNowCnt = 0;
    
    // 初始化阶段
    FAN_RUN(1);
    LED_EventGroupSetbit(PCR_LED_FLAG);
    memset(SendT2T3buf, 0, sizeof(SendT2T3buf));

    // 电机复位
    motor_fast_rst(M01, M01_DIR, M01_GO_GPIO_PORT, M01_GO_GPIO_PIN);
    motor_fast_rst(M02, M02_DIR, M02_GO_GPIO_PORT, M02_GO_GPIO_PIN);

    // 电机运行
    uint16_t fre = Motor_Fre(Globle.POCT_Par.M02_P1);
    MotorFre_Abs_Run(Globle.POCT_Par.M02_P1, M02, M02_DIR, fre);
    MotorFre_Abs_Run(Globle.POCT_Par.M01_P2, M01, M01_DIR, 3500);

    // 启动定时器
    if (TimerHandle != NULL) {
        xTimerStart(TimerHandle, 0);
    }

    // 主循环
    while (1) {
        if (myTimer_flag) {
            handle_temperature_data(pMsg, SendT2T3buf, NULL, 2);
        }

        vTaskDelay(10);

        // 温度达到设定值
        if (SendT2T3buf[TEMP2_SEND] >= Globle.TempControl.Temp_Ch2_Set - 2) {
            if (TimerHandle != NULL) {
                xTimerStop(TimerHandle, 0);
                xTimerStart(TimerHandle, 0);
            }

            // 倒计时阶段
            TimeNowCnt = HAL_GetTick();
            while (TimeNowCnt + Globle.TempControl.Temp_T2 * 1000 > HAL_GetTick()) {
                if (myTimer_flag) {
                    handle_temperature_data(pMsg, SendT2T3buf, &countdown, 2);
                }
                vTaskDelay(1);
            }

            // 最终发送
            handle_temperature_data(pMsg, SendT2T3buf, &countdown, 2);
            break;
        }
    }

    // 清理阶段
    motor_fast_rst(M01, M01_DIR, M01_GO_GPIO_PORT, M01_GO_GPIO_PIN);
    FAN_RUN(0);
    if (TimerHandle != NULL) {
        xTimerStop(TimerHandle, 0);
    }

    return true;
	
/*	
    uint16_t SendT2T3buf[4] = {0};
    uint16_t countdown = 0;
    uint32_t TimeNowCnt = 0;
    
    // 初始化阶段
//    FAN_RUN(1);
    LED_EventGroupSetbit(PCR_LED_FLAG);
    memset(SendT2T3buf, 0, sizeof(SendT2T3buf));

//    // 电机复位
//    motor_fast_rst(M01, M01_DIR, M01_GO_GPIO_PORT, M01_GO_GPIO_PIN);
//    motor_fast_rst(M02, M02_DIR, M02_GO_GPIO_PORT, M02_GO_GPIO_PIN);

    // 电机运行
    uint16_t fre = Motor_Fre(Globle.POCT_Par.M02_P2);
    MotorFre_Abs_Run(Globle.POCT_Par.M02_P2,M02,M02_DIR,fre);	
	
	MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,3500);
	
	Globle.TempControl.PcrTempSet = Globle.TempControl.Temp_Ch2_Set;
	
	// 启动 TIM8 和 TIM9 的 PWM 输出
    HAL_TIM_PWM_Start(&TIM8_Handler, TIM_CHANNEL_4);

    // 启用温度控制通道
    Globle.TempControl.Temp_Ch2_ENABLE = SET;

    // 启动定时器
    if (TimerHandle != NULL) {
        xTimerStart(TimerHandle, 0);
    }

    // 主循环
    while (1) {
        if (myTimer_flag) {
            handle_temperature_data(pMsg, SendT2T3buf, NULL, 2);
        }

        vTaskDelay(10);

        // 温度达到设定值
        if (SendT2T3buf[TEMP2_SEND] >= Globle.TempControl.PcrTempSet - 2) {
            if (TimerHandle != NULL) {
                xTimerStop(TimerHandle, 0);
                xTimerStart(TimerHandle, 0);
            }

            // 倒计时阶段
            TimeNowCnt = HAL_GetTick();
            while (TimeNowCnt + Globle.TempControl.Temp_T2 * 1000 > HAL_GetTick()) {
                if (myTimer_flag) {
                    handle_temperature_data(pMsg, SendT2T3buf, &countdown, 2);
                }
                vTaskDelay(1);
            }

            // 最终发送
            handle_temperature_data(pMsg, SendT2T3buf, &countdown, 2);
            break;
        }
    }

    // 清理阶段
//    motor_fast_rst(M01, M01_DIR, M01_GO_GPIO_PORT, M01_GO_GPIO_PIN);
//    FAN_RUN(0);
    if (TimerHandle != NULL) {
        xTimerStop(TimerHandle, 0);
    }
*/

    return true;
}

// 辅助函数：处理温度阶段 
static void handle_temp_phase(SMsg *pMsg, uint16_t *SendT2T3buf, uint16_t *countdown, 
                            uint8_t temp_channel, uint32_t duration_ms)
{
    uint32_t TimeNowCnt = HAL_GetTick();
    while (TimeNowCnt + duration_ms > HAL_GetTick()) {
        if (myTimer_flag) {
            temp_filtering(pMsg, Globle.TempControl.Temp_Ch2_Set, 
                         Globle.TempControl.Temp_Ch3_Set, SendT2T3buf);
            if (countdown) {
                (*countdown)++;
                SendT2T3buf[temp_channel == 2 ? TIME2_SEND : TIME3_SEND] = *countdown;
            }
            SendT2T3Values(pMsg, SendT2T3buf);
            myTimer_flag = 0;
        }
        vTaskDelay(1);
    }
}

void App_PCR(SMsg *pMsg)
{
	uint16_t fre = 0;
    uint16_t SendT2T3buf[4] = {0};
    uint16_t countdown = 0;
    
    // 初始化阶段
    FAN_RUN(1);
    LED_EventGroupSetbit(PCR_LED_FLAG);
    memset(SendT2T3buf, 0, sizeof(SendT2T3buf));
	
    motor_fast_rst(M01, M01_DIR, M01_GO_GPIO_PORT, M01_GO_GPIO_PIN);

    // 启动定时器
    if (TimerHandle != NULL) {
        xTimerStart(TimerHandle, 0);
    }

    // 第一阶段：等待温度2达到设定值
    while (1) {
        if (myTimer_flag) {
            temp_filtering(pMsg, Globle.TempControl.Temp_Ch2_Set, 
                         Globle.TempControl.Temp_Ch3_Set, SendT2T3buf);
            SendT2T3Values(pMsg, SendT2T3buf);
            myTimer_flag = 0;
        }

        vTaskDelay(1);

        if (SendT2T3buf[TEMP2_SEND] >= Globle.TempControl.Temp_Ch2_Set - 20) {
            // 控制电机
            fre = (Stepper[M02].position < 3000) ? Motor_Fre(Globle.POCT_Par.M02_P1) : 3500;
            MotorFre_Abs_Run(Globle.POCT_Par.M02_P1, M02, M02_DIR, fre);
            MotorFre_Abs_Run(Globle.POCT_Par.M01_P2, M01, M01_DIR, 3500);

            // 重启定时器
            if (TimerHandle != NULL) {
                xTimerStop(TimerHandle, 0);
                xTimerStart(TimerHandle, 0);
            }

            // 处理温度保持阶段
            handle_temp_phase(pMsg, SendT2T3buf, &countdown, 2, 
                            Globle.TempControl.Temp_T2 * 1000);
            
            // 最终发送
            countdown++;
            SendT2T3buf[TIME2_SEND] = countdown;
            SendT2T3Values(pMsg, SendT2T3buf);
            break;
        }
    }

    // 第二阶段：等待温度3达到设定值
    motor_fast_rst(M01, M01_DIR, M01_GO_GPIO_PORT, M01_GO_GPIO_PIN);
	countdown = 0;
    if (TimerHandle != NULL) {
        xTimerStart(TimerHandle, 0);
    }

    while (1) {
        if (myTimer_flag) {
            temp_filtering(pMsg, Globle.TempControl.Temp_Ch2_Set, 
                          Globle.TempControl.Temp_Ch3_Set, SendT2T3buf);
            SendT2T3Values(pMsg, SendT2T3buf);
            myTimer_flag = 0;
        }

        vTaskDelay(1);

        if (SendT2T3buf[TEMP3_SEND] >= Globle.TempControl.Temp_Ch3_Set - 20) {
            MotorFre_Abs_Run(Globle.POCT_Par.M02_P2, M02, M02_DIR, 3500);
            MotorFre_Abs_Run(Globle.POCT_Par.M01_P1, M01, M01_DIR, 3500);

            if (TimerHandle != NULL) {
                xTimerStop(TimerHandle, 0);
                xTimerStart(TimerHandle, 0);
            }

            // 处理温度保持阶段
            handle_temp_phase(pMsg, SendT2T3buf, &countdown, 3, 
                            Globle.TempControl.Temp_T3 * 1000);
            break;
        }
    }
    
    if (TimerHandle != NULL) {
        xTimerStop(TimerHandle, 0);
    }
		
/************************************************	
    uint16_t SendT2T3buf[4] = {0};
    uint16_t countdown = 0;
    
    // 初始化阶段
    LED_EventGroupSetbit(PCR_LED_FLAG);
    memset(SendT2T3buf, 0, sizeof(SendT2T3buf));
	
	// 电机运行
    uint16_t fre = Motor_Fre(Globle.POCT_Par.M02_P2);
    MotorFre_Abs_Run(Globle.POCT_Par.M02_P2,M02,M02_DIR,fre);	
	
	MotorFre_Abs_Run(Globle.POCT_Par.M01_P1,M01,M01_DIR,3500);
	

    // 启动定时器
    if (TimerHandle != NULL) {
        xTimerStart(TimerHandle, 0);
    }
	
	Globle.TempControl.PcrTempSet = Globle.TempControl.Temp_Ch2_Set;
	
	// 启动 TIM8 和 TIM9 的 PWM 输出
    HAL_TIM_PWM_Start(&TIM8_Handler, TIM_CHANNEL_4);

    // 启用温度控制通道
    Globle.TempControl.Temp_Ch2_ENABLE = SET;

    // 第一阶段：等待温度2达到设定值
    while (1) {
        if (myTimer_flag) {
            temp_filtering(pMsg, Globle.TempControl.Temp_Ch2_Set, 
                         Globle.TempControl.Temp_Ch3_Set, SendT2T3buf);
            SendT2T3Values(pMsg, SendT2T3buf);
            myTimer_flag = 0;
        }

        vTaskDelay(1);

        if ((SendT2T3buf[TEMP2_SEND] >= Globle.TempControl.Temp_Ch2_Set - 20)&&(SendT2T3buf[TEMP2_SEND]<=Globle.TempControl.Temp_Ch2_Set + 10)) {

            // 重启定时器
            if (TimerHandle != NULL) {
                xTimerStop(TimerHandle, 0);
                xTimerStart(TimerHandle, 0);
            }

            // 处理温度保持阶段
            handle_temp_phase(pMsg, SendT2T3buf, &countdown, 2, 
                            Globle.TempControl.Temp_T2 * 1000);
            
            // 最终发送
            countdown++;
            SendT2T3buf[TIME2_SEND] = countdown;
            SendT2T3Values(pMsg, SendT2T3buf);
            break;
        }
    }

    // 第二阶段：等待温度3达到设定值   
	countdown = 0;
    if (TimerHandle != NULL) {
        xTimerStart(TimerHandle, 0);
    }
	
	Globle.TempControl.PcrTempSet = Globle.TempControl.Temp_Ch3_Set;

    while (1) {
        if (myTimer_flag) {
            temp_filtering(pMsg, Globle.TempControl.Temp_Ch2_Set, 
                          Globle.TempControl.Temp_Ch3_Set, SendT2T3buf);
            SendT2T3Values(pMsg, SendT2T3buf);
            myTimer_flag = 0;
        }

        vTaskDelay(1);
		if ((SendT2T3buf[TEMP2_SEND] >= Globle.TempControl.PcrTempSet - 20)&&(SendT2T3buf[TEMP2_SEND]<=Globle.TempControl.PcrTempSet + 10)) {
			if (TimerHandle != NULL) {
				xTimerStop(TimerHandle, 0);
				xTimerStart(TimerHandle, 0);
			}

            // 处理温度保持阶段
            handle_temp_phase(pMsg, SendT2T3buf, &countdown, 3, 
                            Globle.TempControl.Temp_T3 * 1000);
            break;
        }
    }
    
    if (TimerHandle != NULL) {
        xTimerStop(TimerHandle, 0);
    }
********************************************/
}


/**
 * @磁吸函数.
 *
 * This function is used to apply a magnetic field to the sample for a specified number of times,
 * with a wait time in between each application. The magnetic field is applied using a pump,
 * which is controlled by the PUMP_Func function.
 *
 * @param times      泵来回吹打的次数.
 * @param waitTime   等待时间.
 * @param Volume     吸体积.
 */
void Magnet(uint8_t times,uint8_t waitTime,uint32_t Volume)
{
	for(int i = 0;i<times;i++)
	{
		PUMP_Func(Volume,PUMP_OUT,500);

		Wait_Time(waitTime);

		PUMP_Func(Volume,PUMP_IN,700);
	}

}

void FaStartValveActuatorStart(void){
	
	uint16_t offset1 = (Globle.POCT_Par.M10_Fa_P[2] - Globle.POCT_Par.M10_Fa_P[1]); 
		
	uint16_t offset0 = (Globle.POCT_Par.M10_Fa_P[1] - Globle.POCT_Par.M10_Fa_P[0]) - offset1;
		
	MotorFre_Abs_Run(Globle.POCT_Par.M10_Fa_P[0],M10,M10_DIR,1500);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M11_P1,M11,M11_DIR,1500);
		
	MotorFre_Abs_Run(Globle.POCT_Par.M10_Fa_P[0] + 2*offset1 + offset0,M10,M10_DIR,500);

	Globle.CurrentValvePos = 9;
	
	Globle.LastDirection = CCW;
	
	Globle.LastValvePos = 7;	
}


/*****************************裂解细胞*****************************
*函数名称：Extraction_Tiqu
*pMsg:消息队列
*
*说明：裂解细胞。
*返回：无
******************************************************************/
void Extraction_Tiqu(SMsg * pMsg)
{
		
#define LYSE_MAGNET_MIX_TIMES        2      //磁吸混匀次数 5
	
#define LYSE_MAGENT_MIX_TIME         0      //磁吸单次等待时间。 原来4秒
	
#define LYSE_MAGNET_MIX_VOLUME       35000  //磁吸吸液的体积。
	
	BaseType_t err = pdFALSE;
	
	runStep = 1;
	
	LED_EventGroupSetbit(EXTRACTION_LED_FLAG);
	
	Globle.TempControl.Temp_Ch1_ENABLE  = SET;
	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_3);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M06_P1,M06,M06_DIR,1500); //先加热
	
	MotorFre_Abs_Run(Globle.POCT_Par.M07_P1,M07,M07_DIR,2600);
	
	PUMP_Func(25000,PUMP_IN,2500); 
	
	FaStartValveActuatorStart();     //CCW
	
	PUMP_Func(25000,PUMP_OUT,2500);

//	PumpMixControl(2,35000,0,2000);

	PUMP_Func(35000,PUMP_IN,1500);
	
//	Valve_Rotation(EFFLUENT_1, CCW);
	Valve_Rotation(PUBLIC_HOLE, CW);

	PUMP_Func(35000,PUMP_OUT,2000); 
	
//	Valve_Rotation(EFFLUENT_1, CCW);
/**********************抽到裂解腔******************************/
//	PUMP_Func(35000,PUMP_IN,1500);
//	
//	Valve_Rotation(PUBLIC_HOLE, CW);
//	
//	PUMP_Func(35000,PUMP_OUT,2000);
//	
//	Valve_Rotation(EFFLUENT_1, CCW);
//	
//	PUMP_Func(35000,PUMP_IN,1600);
//	
//	Valve_Rotation(PUBLIC_HOLE, CW);
//	
//	PUMP_Func(35000,PUMP_OUT,2000);	
/***************************************************/ 	

///*******************************************************************///
	Valve_Rotation(SAMPLE, CCW);
	
	PUMP_Func(26400,PUMP_IN,1200);        //吸样本   26400 = 600ul    
	
	Wait_Time(1);												    
	Valve_Rotation(EFFLUENT_1, CCW);      //EFFLUENT_1
	
	PUMP_Func(26400,PUMP_OUT,2000);
	
	PUMP_Func(30000,PUMP_IN,1500);        //吸样本   26400 = 600ul
	
	PUMP_Func(30000,PUMP_OUT,2000);
	
	PUMP_Func(30000,PUMP_IN,1500);        //吸样本   26400 = 600ul
	
	Wait_Time(1);
	
	Valve_Rotation(PUBLIC_HOLE, CW);

	PUMP_Func(30000,PUMP_OUT,2000); 
/******************************************/
//	Valve_Rotation(EFFLUENT_1, CCW);//EFFLUENT_1
//	
//	PUMP_Func(38000,PUMP_IN,1500);        //吸样本   26400 = 600ul         
//	
//	Wait_Time(1);
//	
//	Valve_Rotation(PUBLIC_HOLE, CW);
//	
//	PUMP_Func(38000,PUMP_OUT,1800); 
///*******************************************************************///
	Globle.washTime = 3;
   	
	xSemaphoreGive(Vibrate_SemaphoreHandle);                                  

	T1_Heating(pMsg);                                                     

	xSemaphoreGive(Vibrate_SemaphoreHandle); 

	while(1)
	{		
		if(VibrateStop_SemaphoreHandle!=NULL)
		{
			err = xSemaphoreTake(VibrateStop_SemaphoreHandle,portMAX_DELAY);

			if(err==pdTRUE)
			{
				break;
			}			
		}
		vTaskDelay(1);
	} 
	                                                        
	Motor_Rst(M06_FRE,M06,M06_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN,0);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,800);
/************************************************************/	
//	PUMP_Func(35000,PUMP_IN,1500);        
//	
//	Wait_Time(1);
//	
//	Valve_Rotation(PUBLIC_HOLE, CW);

//	PUMP_Func(35000,PUMP_OUT,700);
//	
//	Valve_Rotation(EFFLUENT_1, CCW);
//	
//	PUMP_Func(35000,PUMP_IN,1500);        
//	
//	Wait_Time(1);
//	
//	Valve_Rotation(PUBLIC_HOLE, CW);

//	PUMP_Func(35000,PUMP_OUT,700);
		
/**********************************************************/	
	PUMP_Func(LYSE_MAGNET_MIX_VOLUME,PUMP_IN,2000);	
		
//	Wait_Time(1);

	Magnet(LYSE_MAGNET_MIX_TIMES,LYSE_MAGENT_MIX_TIME,LYSE_MAGNET_MIX_VOLUME); 
	
	PUMP_Func(LYSE_MAGNET_MIX_VOLUME,PUMP_OUT,1800);
	
	PUMP_Func(35000,PUMP_IN,1200);
	
	Valve_Rotation(EFFLUENT_1, CCW);

	PUMP_Func(35000,PUMP_OUT,2000);	

	Valve_Rotation(PUBLIC_HOLE, CW);
	
	PUMP_Func(30000,PUMP_IN,2000);
	
//	Wait_Time(1);
	
	PUMP_Func(5000,PUMP_IN,1000);
	
	Wait_Time(1);
	
	Valve_Rotation(EFFLUENT_1, CCW);
	
	PUMP_Func(35000,PUMP_OUT,2000);

/***********************************************************************/
	// Fa_Function(WASHING_3); 
	
	// PUMP_Func(25000,PUMP_IN,1500);
	
	// Fa_Function(PUBLIC_HOLE); 
	
	// PUMP_Func(25000,PUMP_OUT,1500);
	
	// PUMP_Func(25000,PUMP_IN,1500); 
	
	// PUMP_Func(25000,PUMP_OUT,1500);
	
	// PUMP_Func(28000,PUMP_IN,1500);

	// Fa_Function(EFFLUENT_1);
	
	// PUMP_Func(28000,PUMP_OUT,1500); 
/**********************************************************************/
 	Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0);   
}

/*****************************第一次清洗*****************************
*函数名称：Extraction_Clean1
*
*说明：首次清洗。
*返回：无
******************************************************************/
void Extraction_Clean1(void)   
{
	
#define CLEANONE_MAGNET_MIX_TIMES  4      //磁吸混匀次数
	
#define CLEANONE_MAGENT_MIX_TIME   0      //磁吸单次等待时间。 原来4秒
	
#define CLEANONE_MAGNET_MIX_VOLUME 34000  //磁吸吸液的体积。
	
	runStep = 2;

	LED_EventGroupSetbit(EXTRACTION_LED_FLAG);
	
	Valve_Rotation(WASHING_1, CCW);

	PUMP_Func(CLEANONE_MAGNET_MIX_VOLUME,PUMP_IN,1500);
	
	Wait_Time(1);
	
	Valve_Rotation(PUBLIC_HOLE, CW);

	PUMP_Func(CLEANONE_MAGNET_MIX_VOLUME,PUMP_OUT,1500);                                         

 	PumpMixControl(5,CLEANONE_MAGNET_MIX_VOLUME,0,MIX_FRE); 
	
	PUMP_Func(CLEANONE_MAGNET_MIX_VOLUME,PUMP_IN,1800);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,800);               
	
	Magnet(CLEANONE_MAGNET_MIX_TIMES,CLEANONE_MAGENT_MIX_TIME,CLEANONE_MAGNET_MIX_VOLUME);

	PUMP_Func(CLEANONE_MAGNET_MIX_VOLUME,PUMP_OUT,700);
	
	PUMP_Func(35000,PUMP_IN,1500);
	
	Valve_Rotation(WASHING_1, CCW);

 	PUMP_Func(35000,PUMP_OUT,1500); 

	Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0); 
}

/*****************************************************************
*函数名称：Extraction_Clean2
*说明：第二次清洗
*返回：无
******************************************************************/
void Extraction_Clean2(SMsg * pMsg) 
{	
//	BaseType_t err = pdFALSE;
	
#define CLEANTWO_MAGNET_MIX_TIMES    2      //磁吸混匀次数
	
#define CLEANTWO_MAGENT_MIX_TIME     0      //磁吸单次等待时间。  原来是3秒
	
#define CLEANTWO_MAGNET_MIX_VOLUME   34000  //磁吸吸液的体积。

	runStep = 3;
	
	LED_EventGroupSetbit(EXTRACTION_LED_FLAG);
	
	Valve_Rotation(WASHING_2, CCW);

	PUMP_Func(35000,PUMP_IN,1500);	

	Wait_Time(1);
	
	Valve_Rotation(PUBLIC_HOLE, CW);

	PUMP_Func(35000,PUMP_OUT,1800);  

 	PumpMixControl(5,CLEANTWO_MAGNET_MIX_VOLUME,0,MIX_FRE); 

	PUMP_Func(CLEANTWO_MAGNET_MIX_VOLUME,PUMP_IN,1600);

	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,800);

    Magnet(CLEANTWO_MAGNET_MIX_TIMES,CLEANTWO_MAGENT_MIX_TIME,CLEANTWO_MAGNET_MIX_VOLUME);//磁吸 次数 时间 吸液量
	
	PUMP_Func(CLEANTWO_MAGNET_MIX_VOLUME,PUMP_OUT,700);
	
	PUMP_Func(35000,PUMP_IN,1200);
	
	Valve_Rotation(WASHING_2, CCW);

 	PUMP_Func(35000,PUMP_OUT,2000); 
/***************************洗管道****************************/
	// Fa_Function(WASHING_3); //2
	
	// PUMP_Func(3300,PUMP_IN,900);
	
	// Fa_Function(PUBLIC_HOLE); 
	
	// PUMP_Func(2000,PUMP_OUT,900);
	
	// PUMP_Func(5300,PUMP_IN,900); 
	
	// PUMP_Func(2100,PUMP_OUT,900);
	
	// PUMP_Func(5500,PUMP_IN,900);
	
	// Fa_Function(WASHING_2);
	
	// PUMP_Func(10000,PUMP_OUT,1000); //9000
/***************************没过磁珠*******************************/
	Valve_Rotation(WASHING_3, CW);

	PUMP_Func(35000,PUMP_IN,1500);
	
	Valve_Rotation(PUBLIC_HOLE, CCW);

	PUMP_Func(35000,PUMP_OUT,1800);

//	PUMP_Func(35000,PUMP_IN,2000);

//	PUMP_Func(35000,PUMP_OUT,2000);

//	PUMP_Func(35000,PUMP_IN,2000);

//	PUMP_Func(35000,PUMP_OUT,2000);

	PUMP_Func(35000,PUMP_IN,1200);
	
	Valve_Rotation(WASHING_2, CCW);

	PUMP_Func(35000,PUMP_OUT,1800); 

	Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0);    
		
	MotorFre_Abs_Run(Globle.POCT_Par.M06_P1,M06,M06_DIR,800);  

	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,800);  

	// Globle.washTime = 3;
	
	// xSemaphoreGive(Vibrate_SemaphoreHandle);                               
	
	T1_Heating(pMsg);                                                       
	
	// xSemaphoreGive(Vibrate_SemaphoreHandle);                                 
	
	// while(1)
	// {		
	// 	if(VibrateStop_SemaphoreHandle!=NULL)
	// 	{
	// 		err = xSemaphoreTake(VibrateStop_SemaphoreHandle,portMAX_DELAY);
	// 		if(err==pdTRUE)
	// 		{
	// 			break;
	// 		}
	// 	}
	// }
               
/*****************************************************************/
	Valve_Rotation(WASHING_3, CW);
	
	PUMP_Func(3000,PUMP_IN,1000);

	Valve_Rotation(PUBLIC_HOLE, CCW);
	
	PUMP_Func(1800,PUMP_OUT,1000);
	
	PUMP_Func(5000,PUMP_IN,1000); 
	
	PUMP_Func(2100,PUMP_OUT,1000);
	
	PUMP_Func(5000,PUMP_IN,1000);

	PUMP_Func(2200,PUMP_OUT,1000);
	
	PUMP_Func(6900,PUMP_IN,1000);

	Valve_Rotation(EFFLUENT_1, CCW);
	
	PUMP_Func(13800,PUMP_OUT,2000); 
/*************************清洗泵腔室***************************** */
	Valve_Rotation(WASHING_3, CW);
	PUMP_Func(35000,PUMP_IN,2000);

	PUMP_Func(35000,PUMP_OUT,2000);

	PUMP_Func(35000,PUMP_IN,2000);

	PUMP_Func(35000,PUMP_OUT,2000);
	
	PUMP_Func(35000,PUMP_IN,2000);

	PUMP_Func(35000,PUMP_OUT,2000);
	
	PUMP_Func(35000,PUMP_IN,2000);

	PUMP_Func(35000,PUMP_OUT,2000);
/*************************************************************** */

	Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0);                                   

//	Motor_Rst(M06_FRE,M06,M06_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN,0);     
}

/*****************************核酸洗脱*****************************
*函数名称：Extraction_Elution
*说明：核酸洗脱
*返回：无
******************************************************************/
void Extraction_Elution(SMsg * pMsg)          
{
	BaseType_t err = pdFALSE;
		
	runStep = 4;
	
	LED_EventGroupSetbit(EXTRACTION_LED_FLAG);

	Valve_Rotation(ELUTION, CW);
	
	PUMP_Func(14000,PUMP_IN,900);
	
	Wait_Time(1);
	
	// Motor_Rst(M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN,0);      

	Valve_Rotation(PUBLIC_HOLE, CCW);
	
	PUMP_Func(14000,PUMP_OUT,900);
	
	MotorFre_Abs_Run(Globle.POCT_Par.M06_P1,M06,M06_DIR,1500);                
	
	Globle.washTime = 3;
	
	xSemaphoreGive(Vibrate_SemaphoreHandle);                               
	
	T1_Heating(pMsg);                                                       
	
	xSemaphoreGive(Vibrate_SemaphoreHandle);                                 
	
	while(1)
	{		
		if(VibrateStop_SemaphoreHandle!=NULL)
		{
			err = xSemaphoreTake(VibrateStop_SemaphoreHandle,portMAX_DELAY);
			if(err==pdTRUE)
			{
				break;
			}
		}
	}

	Motor_Rst(M06_FRE,M06,M06_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN,0);  

	PUMP_Func(14000,PUMP_IN,600);		
	
	MotorFre_Abs_Run(Globle.POCT_Par.M12_P1,M12,M12_DIR,800);             
	
	PID_Stop(0);

	//预升温  95摄氏度
    Globle.TempControl.Temp_Ch3_Set = 500u;
	
	Globle.TempControl.Temp_Ch2_Set = 950u;
	
	HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_1);
	
	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_4);
	
	Globle.TempControl.Temp_Ch3_ENABLE = SET;
	
	Globle.TempControl.Temp_Ch2_ENABLE = SET;
	
	Magnet(5,0,14000);  //原来是4次2秒
}

/*****************************溶液分配*****************************
*函数名称：Extraction_Solution_Apportion
*说明：溶液分配
*返回：无
******************************************************************/
void Extraction_Solution_Apportion(SMsg * pMsg)
{	
//	uint32_t temp_V = 0;

	uint8_t err = 0;

	runStep = 5;

	LED_EventGroupSetbit(EXTRACTION_LED_FLAG);

	Valve_Rotation(DRYING, CW);

	PUMP_Func(14000,PUMP_OUT,1000); 
	
	Wait_Time(15);
	
	PumpMixControl(5,12000,0,1300);  
	
	PUMP_Func(12000,PUMP_IN,500);

//	Wait_Time(5);
	
//	PUMP_Func(1100,PUMP_OUT,500);

	Wait_Time(1);

	Valve_Rotation(LQD_HOLE, CW);

//  PUMP_Func(temp_V,PUMP_OUT,350);
	// JG_LED = 1;
	LASER_LED_ON();  //开激光
	
	Wait_Time(1);
	
	// 在这里检测一次，如果没有检测到电平。
	if(LASER_PHOTODIDE_Scan() == 1)   //if(GetMedianNum(7,10) < 30000)
    {
        SaveFaultCode("F^FY_No_optical_path_detected");
		
		LASER_LED_OFF();// JG_LED = 0;
		
        return;
    }
	
	err = Liquid_seperation(12000,PUMP_OUT,250); //体积，泵方向，速度.

	if (err == 1)
	{
		// JG_LED = 0;
		LASER_LED_OFF();
		
		SaveFaultCode("F^FY_TIMEOUT");
		
		return;
	}
	
	// JG_LED = 0;
	LASER_LED_OFF();
	
//	Wait_Time(1);
	
	Valve_Rotation(ELUTION, CCW);
	
	Motor_Run(1000,M10,CCW,500);
//  Fa_NegativeDirection(10);
	 
//	Globle.FaNum = 0;
//	
//	Globle.LastFaNum = 0;	
}

/**
 * @brief 重置所有电机状态（包括步进电机结构体、全局计数器及硬件复位）
 * @details 1. 初始化步进电机结构体数组
 *          2. 清零全局故障计数器
 *          3. 依次执行各电机硬件复位（含超时检测）
 *          4. 任意电机复位失败时保存故障码并退出
 */
void ALL_Motor_RESET(void)
{
    // 初始化所有步进电机结构体（共12个）
    for(int i = 0; i < 12; i++) {
        memset(&Stepper[i], 0, sizeof(Stepper_Typedef));
    }
    
    // 重置全局故障计数器
    Globle.CurrentValvePos = 0;
    Globle.LastDirection = 0;
	Globle.LastValvePos = 0;
    runStep = 0;
    
    // 关闭风扇
    FAN_RUN(0);
    
    // 定义电机复位参数数组（频率、控制引脚、方向引脚、使能端口、使能引脚、模式）
    const struct {
        uint32_t fre;
		u8 motorNumber;
		u8 motorDir;
        GPIO_TypeDef* port;
        uint16_t pin;
        uint8_t mode;
        char* fault_code;
    } motor_list[] = {
        {M01_FRE, M01, M01_DIR, M01_GO_GPIO_PORT, M01_GO_GPIO_PIN, 0, "F^M01_TIMEOUT"},
        {M02_FRE, M02, M02_DIR, M02_GO_GPIO_PORT, M02_GO_GPIO_PIN, 0, "F^M02_TIMEOUT"},
        {M08_FRE, M08, M08_DIR, M08_GO_GPIO_PORT, M08_GO_GPIO_PIN, 1, "F^M08_TIMEOUT"},
//        {M04_FRE, M04, M04_DIR, M04_GO_GPIO_PORT, M04_GO_GPIO_PIN, 1, "F^M04_TIMEOUT"}, // 原注释的M04
        {M05_FRE, M05, M05_DIR, M05_GO_GPIO_PORT, M05_GO_GPIO_PIN, 0, "F^M05_TIMEOUT"},
        {M06_FRE, M06, M06_DIR, M06_GO_GPIO_PORT, M06_GO_GPIO_PIN, 0, "F^M06_TIMEOUT"},
        {3800, M07, M07_DIR, M07_GO_GPIO_PORT, M07_GO_GPIO_PIN, 0, NULL}, // 无故障码的M07第一次调用
        {M07_FRE, M07, M07_DIR, M07_GO_GPIO_PORT, M07_GO_GPIO_PIN, 1, "F^M07_TIMEOUT"},
        {M11_FRE, M11, M11_DIR, M11_GO_GPIO_PORT, M11_GO_GPIO_PIN, 1, "F^M11_TIMEOUT"},
        {M10_FRE, M10, M10_DIR, M10_GO_GPIO_PORT, M10_GO_GPIO_PIN, 1, "F^M10_TIMEOUT"},
        {M12_FRE, M12, M12_DIR, M12_GO_GPIO_PORT, M12_GO_GPIO_PIN, 0, "F^M12_TIMEOUT"}
    };
    
    // 遍历电机列表执行复位
    for(int i = 0; i < sizeof(motor_list)/sizeof(motor_list[0]); i++) {
       // const auto* motor = &motor_list[i];
        if(motor_list[i].fault_code) {
            uint8_t Err = Motor_Rst(motor_list[i].fre, motor_list[i].motorNumber, motor_list[i].motorDir, motor_list[i].port, motor_list[i].pin, motor_list[i].mode);
            if(Err != 0) {
                SaveFaultCode(motor_list[i].fault_code);
                return;
            }
        } else {
            // 处理无故障码的特殊调用（如M07第一次调用）
            Motor_Rst(motor_list[i].fre, motor_list[i].motorNumber, motor_list[i].motorDir, motor_list[i].port, motor_list[i].pin, motor_list[i].mode);
        }
    }
}

/*************************
 * @brief  电机移动
 * @param  str:主参数
 * @retval 无
  *************************/
void Motor_Move(SMsg * pMsg,uint16_t Rstfre,uint8_t motor_num,uint8_t dir,GPIO_TypeDef *Port,uint16_t Pin)
{
	SpeedCalc_TypeDef * pSpeed = NULL;
	
	pSpeed = (SpeedCalc_TypeDef *)pvPortMalloc(sizeof(SpeedCalc_TypeDef) + 1);
	
	uint32_t step = ExtractNumberAfterEqualSign(pMsg->m_pAppMsg->m_pAppLayerSubPara);
		
	uint16_t fre = Motor_Fre(step);
	
	if((motor_num == M12)||(motor_num == M11)|(motor_num == M05)||(motor_num == M06)||(motor_num == M04)||(motor_num == M10))
	{
		if(fre > 1000)
			fre = 1000;
	}

//	if(motor_num == M10)
//	{
//		if(fre > 500)
//		{
//			fre = 500;
//		}
//	}
		
	if(pMsg->m_pAppMsg->m_pAppLayerSubPara[1] == '+') //CW
	{		
		if(dir == CCW)		
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CW,pSpeed);
		
		else			
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CCW,pSpeed);
		
		while(Stepper[motor_num].status != STOP){vTaskDelay(1);}
		
		Stepper[motor_num].position = Stepper[motor_num].position + step;
	}
	else if(pMsg->m_pAppMsg->m_pAppLayerSubPara[1] == '-')
	{	
		if(dir == CCW)
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CCW,pSpeed);
		else
			Stepper_Move_S(MIN_FRE,fre,0.01f, step,motor_num,CW,pSpeed);
		
		while(Stepper[motor_num].status != STOP){vTaskDelay(1);}
		
		Stepper[motor_num].position = Stepper[motor_num].position - step;
	}
	else
	{
		if((motor_num==M01)||(motor_num==M02)||(motor_num==M03)||(motor_num==M08)||(motor_num==M07)){
			motor_fast_rst(motor_num,dir,Port,Pin);
		}
		else{
			Motor_Rst(Rstfre,motor_num,dir,Port,Pin,1);
		}
	}
	vPortFree(pSpeed);
}

uint8_t Door_Control(uint8_t sta)
{	
	return 0;
}

void push_motor(void)
{
	MotorFre_Abs_Run(Globle.POCT_Par.M05_P1,M05,M05_DIR,M05_FRE);
}

#include "StartTask.h"
TaskHandle_t VibrateTask_Handler;

void VibrateTask(void *pData)
{
	BaseType_t err = pdFALSE;
	
	uint32_t mix_v = MIX_V;

	uint16_t mix_fre = MIX_FRE;

	while(1)
	{		
		if(Vibrate_SemaphoreHandle!=NULL)
		{
			err = xSemaphoreTake(Vibrate_SemaphoreHandle,portMAX_DELAY);
			
			if(err==pdTRUE)										              
			{
				MotorFre_Abs_Run(Globle.POCT_Par.M07_P1,M07,M07_DIR,mix_fre);	
				
				vTaskDelay(1000);

				while(1)
				{
					if(runStep == 4){
						mix_v = 14000;
						mix_fre = 1600;
					}
					else{
                        mix_v = MIX_V;
                        mix_fre = MIX_FRE;
                    }
					
					MotorFre_Abs_Run(Globle.POCT_Par.M07_P1 - mix_v,M07,M07_DIR,mix_fre);
				
					MotorFre_Abs_Run(Globle.POCT_Par.M07_P1,M07,M07_DIR,mix_fre);
											
					Wait_Time(Globle.washTime);
					
					vTaskDelay(5);
					
					if(xSemaphoreTake(Vibrate_SemaphoreHandle,0) == pdTRUE)									
					{							
						xSemaphoreGive(VibrateStop_SemaphoreHandle);
						
						Stepper[M07].status = STOP;
						
						break;
					}
				}				
			}                
		}
		
		else if(err==pdFALSE)
		{
			vTaskDelay(10);           
		}	
		
		vTaskDelay(5);	
	}
}

#endif

	
	
