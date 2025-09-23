/*****************************头文件*******************************/
#include "poct_process.h"
#include "FreeRTOS.h"
#include "usart.h"
#include "bsp_motor.h"
#include "utility.h"
#include "w5500_conf.h"
#include "bsp_i2c_ee.h"
#include "bsp_motor_conf.h"
#include "led.h"
#include "bsp_pwm_conf.h"
#include "LogTask.h"
#include "Reply.h"
#include "TempControlTask.h"
#include "AppMsgDefine.h"
#include "Endpoint.h"
#include "ZStr2StrMap.h"
#include "laser.h"
#include "UltraSound.h"

/*****************************全局变量**********************************/
GlobleTypedef Globle;

POCT_EEPROMTypedef POCT_Para;

// 定义电机信息数组
const MotorInfo motorInfos[] = {
    {MAIN_PARA_M01, M01_FRE, M01, M01_DIR, M01_GO_GPIO_PORT, M01_GO_GPIO_PIN},
    {MAIN_PARA_M02, M02_FRE, M02, M02_DIR, M02_GO_GPIO_PORT, M02_GO_GPIO_PIN},
    {MAIN_PARA_M03, M03_FRE, M03, M03_DIR, M03_GO_GPIO_PORT, M03_GO_GPIO_PIN},
    {MAIN_PARA_M04, M04_FRE, M04, M04_DIR, M04_GO_GPIO_PORT, M04_GO_GPIO_PIN},
    {MAIN_PARA_M05, M05_FRE, M05, M05_DIR, M05_GO_GPIO_PORT, M05_GO_GPIO_PIN},
    {MAIN_PARA_M06, M06_FRE, M06, M06_DIR, M06_GO_GPIO_PORT, M06_GO_GPIO_PIN},
    {MAIN_PARA_M07, M07_FRE, M07, M07_DIR, M07_GO_GPIO_PORT, M07_GO_GPIO_PIN},
    {MAIN_PARA_M08, M08_FRE, M08, M08_DIR, M08_GO_GPIO_PORT, M08_GO_GPIO_PIN},
    {MAIN_PARA_M09, M09_FRE, M09, M09_DIR, M09_GO_GPIO_PORT, M09_GO_GPIO_PIN},
    {MAIN_PARA_M10, M10_FRE, M10, M10_DIR, M10_GO_GPIO_PORT, M10_GO_GPIO_PIN},
    {MAIN_PARA_M11, M11_FRE, M11, M11_DIR, M11_GO_GPIO_PORT, M11_GO_GPIO_PIN},
    {MAIN_PARA_M12, M12_FRE, M12, M12_DIR, M12_GO_GPIO_PORT, M12_GO_GPIO_PIN},
};

// 定义 LED 映射表
const LEDMapping ledMappings[] = {
    {MAIN_PARA_LED_ATTO, LED_ATTO},
    {MAIN_PARA_LED_FAM, LED_FAM},
    {MAIN_PARA_LED_ROX, LED_ROX},
    {MAIN_PARA_LED_CY5, LED_CY5},
    {MAIN_PARA_LED_QUAS705, LED_QUAS705},
    {MAIN_PARA_LED_HEX, LED_HEX},
    {MAIN_PARA_LED_CY7, LED_CY7},
    {MAIN_PARA_LED_AF405, LED_AF405}
};
/*****************************函数***************************************/

// void SaveFaultCode(char *FaultStr)
// {
// 	Globle.Mechine.State = FAULT;

// 	xEventGroupSetBits(ledEventGroupHandle,ERROR_LED_FLAG);

// 	memset(Globle.Mechine.faultStr,0,sizeof(Globle.Mechine.faultStr));

// 	strcat(Globle.Mechine.faultStr,FaultStr);

// 	SendFrameI(APP_MSG_UNIT_MAIN,APP_MSG_UNIT_PC,\
// 			   APP_MSG_MAIN_CMD_RPT,APP_MSG_SUB_CMD_ERR,"",Globle.Mechine.faultStr,&SocketEndpoint);
// }

// ... existing code ...

/**
 * @brief 保存故障代码到全局变量，并设置故障状态和点亮错误指示灯，同时发送故障信息给PC
 *
 * 此函数将设备状态设置为故障状态，点亮错误指示灯，清空之前的故障信息，
 * 然后将新的故障信息添加到全局变量中，并将故障信息发送给PC。
 *
 * @param FaultStr 指向包含故障信息的字符串的指针
 */
void SaveFaultCode(char *FaultStr)
{
    // 设置设备状态为故障状态
    Globle.Mechine.State = FAULT;

    // 点亮错误指示灯
    xEventGroupSetBits(ledEventGroupHandle, ERROR_LED_FLAG);

    // 清空之前的故障信息
    Globle.Mechine.faultStr[0] = '\0';

    // 将新的故障信息添加到全局变量中，避免缓冲区溢出
    snprintf(Globle.Mechine.faultStr, sizeof(Globle.Mechine.faultStr), "%s", FaultStr);

    // 发送故障信息给PC
    SendFrameI(APP_MSG_UNIT_MAIN, APP_MSG_UNIT_PC,
               APP_MSG_MAIN_CMD_RPT, APP_MSG_SUB_CMD_ERR, "",
               Globle.Mechine.faultStr, &SocketEndpoint);
}

// ... existing code ...

// ... existing code ...

/**
 * @brief 从字符串中提取等号后面的数字
 *
 * 该函数从输入字符串中查找等号 '=', 并提取等号后面的数字部分，
 * 然后将其转换为 32 位整数返回。如果输入字符串中不包含等号，
 * 或者等号后面没有有效的数字，则返回 0。
 *
 * @param token 包含等号的字符串
 * @return 等号后面的数字转换为 32 位整数的结果
 */
uint32_t ExtractNumberAfterEqualSign(char *token)
{
    int i = 0, Flag = 0, len;

    char num[5] = {0};

    memset(num, 0, 5);

    while (token[i] != '\0') {
        if (token[i] == '=') {
            i++;
            len = i;
            Flag = 1;
        }
        if (Flag == 1) {
            num[i - len] = token[i];
        }
        i++;
    }
    return atoi32(num, 10);
}

// ... existing code ...

// uint32_t PARA_Get_num(char *token)
// {
//     int i = 0, Flag = 0, len;

//     char num[5] = {0};

//     memset(num, 0, 5);

//     while (token[i] != '\0') {
//         if (token[i] == '=') {
//             i++;
//             len = i;
//             Flag = 1;
//         }
//         if (Flag == 1) {
//             num[i - len] = token[i];
//         }
//         i++;
//     }
//     return atoi32(num, 10);
// }

/**
 * @brief 从字符串中提取参数并转换为整数，存储到全局变量中
 *
 * 该函数将输入字符串按 "^" 分割，提取每个参数并使用 PARA_Get_num 函数将其转换为整数，
 * 然后存储到全局变量 Globle.SavaParaNum.Para 中。
 *
 * @param str 包含参数的字符串，参数之间用 "^" 分隔
 * @param len 需要提取的参数数量
 */
void ExtractParametersToGlobal(char *str, uint8_t len)
{
    char *pSegment = NULL;

    uint8_t i = 0;

    char *pTempStr = NULL;

    /** Allocate memory for a temporary string to avoid modifying the original string */
    pTempStr = pvPortMalloc(strlen(str) + 1);

    /** Copy the original string to the temporary string */
    memcpy(pTempStr,str, strlen(str) + 1);

    /** Extract the first parameter */
    pSegment = strtok(pTempStr, "^");

    /** Convert the parameter to an integer and store it in the global variable */
    Globle.SavaParaNum.Para[0] = (uint16_t)ExtractNumberAfterEqualSign(pSegment);

    /** Loop to extract the remaining parameters */
    for(i=1;i<len;i++)
    {
        /** Extract the next parameter */
        pSegment = strtok(NULL, "^");

        /** Convert the parameter to an integer and store it in the global variable */
        Globle.SavaParaNum.Para[i] = (uint16_t)ExtractNumberAfterEqualSign(pSegment);
    }

    /** Free the allocated memory */
    vPortFree(pTempStr);
}


/**
 * @brief 向 POCT 设备的 EEPROM 中写入一个参数
 *
 * 该函数从输入字符串中解析出参数索引和参数值，并将参数值写入到 POCT 设备的 EEPROM 中。
 * 输入字符串格式应为 "索引=值"，例如 "0=123"。
 *
 * @param str 包含参数索引和参数值的字符串
 */
void WriteSingleParameterToEEPROM(char *str)
{
    int tmp = 0;

    char *pSegment;

    POCT_EEPROMTypedef* P_Member;

    uint32_t* P;

    char *pTempStr = NULL;

    pTempStr = pvPortMalloc(strlen(str) + 1);

    P_Member = &Globle.POCT_Par;

    P = (uint32_t*)P_Member;

    memcpy(pTempStr,str,strlen(str) + 1);

    pSegment = strtok(pTempStr,"=");

    tmp = atoi32(pSegment,10);

    pSegment = strtok(NULL,"=");

    P[tmp + 1] = atoi32(pSegment,10);

    ee_WriteBytes(&Globle.POCT_Par.rs1,EEPROM_PARA_ADDR,sizeof(POCT_EEPROMTypedef));

    vPortFree(pTempStr);
}


/**
 * @brief 向 POCT 设备的 EEPROM 中写入多个参数
 *
 * 该函数从 SMsg 结构体中解析出参数数量和参数值，并将参数值写入到 POCT 设备的 EEPROM 中。
 * 输入的主参数表示参数数量，子参数包含多个以 "^" 分隔的参数值。
 *
 * @param pMsg 指向 SMsg 结构体的指针，包含参数信息
 */
void WriteParametersToPOCTEEPROMFromMsg(SMsg * pMsg)
{
    char *token = NULL;

    char *pAppLayerSubPara = NULL;

    uint8_t i = 0,Num = 0;

    POCT_EEPROMTypedef* P_Member;

    uint32_t* P;

    P_Member = &Globle.POCT_Par;

    P = (uint32_t*)P_Member;

    pAppLayerSubPara = pvPortMalloc(strlen(pMsg->m_pAppMsg->m_pAppLayerSubPara) + 1);

    Num = atoi32(pMsg->m_pAppMsg->m_pAppLayerMainPara,10);

    P++;

    memcpy(pAppLayerSubPara,pMsg->m_pAppMsg->m_pAppLayerSubPara,strlen(pMsg->m_pAppMsg->m_pAppLayerSubPara) + 1);

    token = strtok(pAppLayerSubPara, "^");

    *P++ = (uint32_t)ExtractNumberAfterEqualSign(token);

    for(i=0;i<Num;i++)
    {
        token = strtok(NULL, "^");

        *P++ = (uint16_t)ExtractNumberAfterEqualSign(token);
    }

    ee_WriteBytes(&Globle.POCT_Par.rs1,EEPROM_PARA_ADDR,sizeof(POCT_EEPROMTypedef));

    vPortFree(pAppLayerSubPara);
}

/**
 * @brief 发送 T1 和 t1 的值到指定目标
 *
 * 该函数将 T1 和 t1 的值格式化为字符串 "T1=value1^t1=value2"，
 * 并通过 SendFrameD 函数发送到 SMsg 结构体指定的目标。
 * 若内存分配失败，会保存错误代码并直接返回。
 *
 * @param pMsg 指向 SMsg 结构体的指针，包含目标信息
 * @param Tbuf 指向包含 T1 和 t1 值的数组的指针
 */
void SendT1Values(SMsg * pMsg, uint16_t *Tbuf)
{
    char *newstr = pvPortMalloc(50);
    if (newstr == NULL) {
        // 内存分配失败，保存错误代码并返回
        SaveFaultCode("F^SEND_T1_MEM_ALLOC_FAILED");
        return;
    }

    // 使用 snprintf 一次性格式化字符串
    snprintf(newstr, 50, "T1=%u^t1=%u", Tbuf[0], Tbuf[1]);

    // 发送格式化后的字符串
    SendFrameD(pMsg, newstr);

    // 释放分配的内存
    vPortFree(newstr);
}

/**
 * @brief 发送 T2、t2、T3 和 t3 的值到指定目标
 *
 * 该函数将 T2、t2、T3 和 t3 的值格式化为字符串 "T2=value1^t2=value2^T3=value3^t3=value4"，
 * 并通过 SendFrameD 函数发送到 SMsg 结构体指定的目标。
 *
 * @param pMsg 指向 SMsg 结构体的指针，包含目标信息
 * @param Tbuf 指向包含 T2、t2、T3 和 t3 值的数组的指针
 */
void SendT2T3Values(SMsg * pMsg, uint16_t *Tbuf)
{
    char *newstr = pvPortMalloc(50);
    if (newstr == NULL) {
        // 内存分配失败，保存错误代码
        SaveFaultCode("F^SEND_T2T3_MEM_ALLOC_FAILED");
        return;
    }

    // 使用 snprintf 一次性格式化字符串
    snprintf(newstr, 50, "T2=%u^t2=%u^T3=%u^t3=%u", Tbuf[0], Tbuf[1], Tbuf[2], Tbuf[3]);

    // 发送格式化后的字符串
    SendFrameD(pMsg, newstr);

    // 释放分配的内存
    vPortFree(newstr);
}

/**
 * @brief 发送 POCT 设备的参数信息
 *
 * 该函数将 POCT 设备的参数信息从全局结构体中提取出来，
 * 按照特定格式拼接成字符串，然后通过 SendFrameD 函数发送出去。
 * 最后释放分配的内存。
 *
 * @param pMsg 指向 SMsg 结构体的指针，用于传递消息信息
 */
void SendPOCTParameters(SMsg * pMsg)
{
	uint8_t i = 0;
	
	char *pSubPara = NULL;
	
	char strbuf[15] = {0};
	
	POCT_EEPROMTypedef* P_Member; 
	
	uint32_t* P;
	
	P_Member = &Globle.POCT_Par;
	
	P = (uint32_t*)P_Member;
	
	pSubPara = pvPortMalloc(1024);	
	
	memset(pSubPara,0,1024);
	
	P++;
	
	for(i=0;i<sizeof(POCT_EEPROMTypedef)/4 + 1;i++)
	{
		memset(strbuf,0,sizeof(strbuf));
		
		if(i!=0)
			strcat(pSubPara,"^");
		
		Int2Str(i,strbuf);
		
		strcat(pSubPara,strbuf);
		
		strcat(pSubPara,"=");
		
		memset(strbuf,0,sizeof(strbuf));
		
		Int2Str(*P++,strbuf);
		
		strcat(pSubPara,strbuf);	
	}
	SendFrameD(pMsg,pSubPara);
	
	vPortFree(pSubPara);
}
// ... existing code ...

/**
 * @brief 发送 IP 地址信息
 *
 * 该函数将 EEPROM 中存储的 IP 地址信息按照特定格式拼接成字符串，
 * 并通过 SendFrameD 函数发送出去，最后释放分配的内存。
 *
 * @param pMsg 指向 SMsg 结构体的指针，用于传递消息信息
 */
void SendIPAddress(SMsg * pMsg)
{
	uint8_t i = 0;
	
	char *newstr;
	
	char strbuf[15] = {0};
	
	newstr = pvPortMalloc(50);	
	
	memset(newstr,0,50);
	
	for(i=0;i<4;i++)
	{
		memset(strbuf,0,sizeof(strbuf));
		
		if(i!=0)
			strcat(newstr,"^");
		
		Int2Str(i,strbuf);
		
		strcat(newstr,strbuf);
		
		strcat(newstr,"=");
		
		memset(strbuf,0,sizeof(strbuf));
		
		Int2Str(EEPROM_MSG.lip[i],strbuf);
		
		strcat(newstr,strbuf);	
	}
	
	SendFrameD(pMsg,newstr);	
	vPortFree(newstr);
}


/**
 * @brief 发送 MAC 地址信息
 *
 * 该函数将 EEPROM 中存储的 MAC 地址信息按照特定格式拼接成字符串，
 * 并通过 SendFrameD 函数发送出去，最后释放分配的内存。
 *
 * @param pMsg 指向 SMsg 结构体的指针，用于传递消息信息
 */
void SendMACAddress(SMsg * pMsg)
{
	uint8_t i = 0;
	
	char *newstr;
	
	char strbuf[15] = {0};
	
	newstr = pvPortMalloc(50);	
	
	memset(newstr,0,50);
	
	for(i=0;i<6;i++)
	{
		memset(strbuf,0,sizeof(strbuf));
		
		if(i!=0)
			strcat(newstr,"^");
		
		Int2Str(i,strbuf);
		
		strcat(newstr,strbuf);
		
		strcat(newstr,"=");
		
		memset(strbuf,0,sizeof(strbuf));
		
		Int2Str(EEPROM_MSG.mac[i],strbuf);
		
		strcat(newstr,strbuf);	
	}	
	SendFrameD(pMsg,newstr);
	vPortFree(newstr);
}


/**
 * @brief 定时显示时间信息
 *
 * 该函数根据传入的消息中的时间参数，定时发送时间信息。
 * 它会从消息的子参数中提取时间值，然后在指定的时间内，每秒发送一次递增的时间信息。
 *
 * @param pMsg 指向包含时间参数的 SMsg 结构体指针
 */
void TimingDisplay(SMsg * pMsg)
{
    char *newstr = NULL;
    uint32_t startTime = 0;
    uint16_t timeElapsed = 0,uTime = 0;
//    char strbuf[10] = {0};
	char * pTime = NULL;

    // 解析消息的子参数，获取时间值
    ZStr2StrMap *pMap = ParseStr2Map(pMsg->m_pAppMsg->m_pAppLayerSubPara, TOP_SEPARATE_STRING, EQUAL_SEPARAT_STRING);
    if (pMap == NULL) {
        // 解析失败，保存错误代码并返回
        SaveFaultCode("F^TIMING_DISPLAY_PARSE_MAP_FAILED");
        return;
    }

    pTime = GetValueFromStr2StrMap(pMap, "t1");
    if (pTime == NULL) {
        // 未找到时间参数，释放内存并保存错误代码
        DeleteStr2StrMap(pMap);
        SaveFaultCode("F^TIMING_DISPLAY_TIME_PARAM_NOT_FOUND");
        return;
    }

    uTime = atoi(pTime);
    // 释放解析结果的内存
    DeleteStr2StrMap(pMap);

    // 分配内存用于存储发送的消息
    newstr = pvPortMalloc(50);
    if (newstr == NULL) {
        // 内存分配失败，保存错误代码并返回
        SaveFaultCode("F^TIMING_DISPLAY_MEM_ALLOC_FAILED");
        return;
    }

    // 获取当前时间
    startTime = HAL_GetTick();

    // 在指定的时间内，每秒发送一次递增的时间信息
    while (HAL_GetTick() - startTime < uTime * 1000) {
        // 格式化时间信息
        snprintf(newstr, 50, "t1=%u", timeElapsed);

        // 发送时间信息
        SendFrameD(pMsg, newstr);

        // 延时 1 秒
        vTaskDelay(pdMS_TO_TICKS(1000));
        timeElapsed++;
    }

    // 释放分配的内存
    vPortFree(newstr);
}

/**
 * @brief 根据消息控制电机移动
 *
 * 该函数根据传入的消息中的主参数，查找对应的电机信息，并调用 Motor_Move 函数控制电机移动。
 *
 * @param pMsg 指向包含电机控制信息的 SMsg 结构体指针
 */
void ControlUserMotor(SMsg * pMsg)
{
	size_t i = 0;
    // 计算电机信息数组的元素数量
    const size_t motorCount = sizeof(motorInfos) / sizeof(motorInfos[0]);

    // 遍历电机信息数组
    for (i = 0; i < motorCount; i++) {
        // 比较主参数是否匹配
        if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara, motorInfos[i].mainPara)) {
            // 调用 Motor_Move 函数控制电机移动
            Motor_Move(pMsg, motorInfos[i].fre, motorInfos[i].motor, motorInfos[i].dir,
                       motorInfos[i].goGpioPort, motorInfos[i].goGpioPin);
            break; // 找到匹配的电机信息后，跳出循环
        }
    }
}


/**
 * @brief 打开指定的 LED 灯
 *
 * 该函数根据传入的主参数和从参数，打开对应的 LED 灯，并设置亮度。
 * 主参数指定 LED 的名称，从参数指定 LED 的亮度。
 *
 * @param App_Main_Para 指向包含 LED 名称的字符串的指针
 * @param App_Slave_Para 指向包含 LED 亮度值的字符串的指针
 */
void TurnOnUserLED(char *App_Main_Para, char *App_Slave_Para)
{
	size_t i = 0;
    // 从从参数中提取 LED 亮度值
    uint8_t LED_b = ExtractNumberAfterEqualSign(App_Slave_Para);

    // 遍历 LED 映射表，查找匹配的 LED 名称
    for (i = 0; i < sizeof(ledMappings) / sizeof(ledMappings[0]); i++) {
        if (!strcmp(App_Main_Para, ledMappings[i].name)) {
            // 调用 LED_SPI_SendData 函数打开 LED 灯并设置亮度
            LED_SPI_SendData(ledMappings[i].index, LED_b);

            // 对于特殊的 LED（AF405），打开激光 LED
            if (ledMappings[i].index == LED_AF405) {
                LASER_LED_ON();
            }
			if (ledMappings[i].index == LED_QUAS705)
				UltraSound_PeriodicOutput(10000,100);
//				ULTRA_SOUND_ON();
            return;
        }
    }
    // 如果没有找到匹配的 LED 名称，记录错误信息
    SaveFaultCode("F^LED_NAME_NOT_FOUND");
}


/**
 * @brief 关闭指定名称的 LED 灯
 *
 * 该函数根据传入的 LED 名称，在 ledMappings 数组中查找对应的 LED 索引，
 * 并调用 LED_SPI_SendData 函数将其亮度设置为 0 以关闭 LED 灯。
 * 对于特殊的 LED（AF405），还会调用 LASER_LED_OFF 函数关闭激光 LED。
 *
 * @param App_Main_Para 指向包含 LED 名称的字符串的指针
 */
void TurnOffUserLED(char *App_Main_Para)
{
    size_t i;
    // 遍历 LED 映射表，查找匹配的 LED 名称
    for (i = 0; i < sizeof(ledMappings) / sizeof(ledMappings[0]); i++) {
        if (!strcmp(App_Main_Para, ledMappings[i].name)) {
            // 调用 LED_SPI_SendData 函数关闭 LED 灯
            LED_SPI_SendData(ledMappings[i].index, 0);

            // 对于特殊的 LED（AF405），关闭激光 LED
            if (ledMappings[i].index == LED_AF405) {
                LASER_LED_OFF();
			if (ledMappings[i].index == LED_QUAS705)
				ULTRA_SOUND_OFF();
            }
            return;
        }
    }
    // 如果没有找到匹配的 LED 名称，记录错误信息
    SaveFaultCode("F^LED_NAME_NOT_FOUND");
}


/**
 * @brief 根据 LED 昵称获取对应的索引
 *
 * 该函数接受一个指向 LED 昵称的字符串指针，通过比较字符串来确定对应的 LED 索引。
 * 如果找到匹配的昵称，则返回对应的索引；如果没有找到匹配的昵称，则返回 0。
 *
 * @param pLedNickName 指向 LED 昵称的字符串指针
 * @return 对应的 LED 索引，如果未找到匹配则返回 0
 */
int LedNickName2Index(char *pLedNickName)
{
    // 遍历所有可能的 LED 昵称
    if (!strcmp(pLedNickName, "ATTO")) return 1;
    if (!strcmp(pLedNickName, "FAM")) return 2;
    if (!strcmp(pLedNickName, "HEX")) return 3;
    if (!strcmp(pLedNickName, "ROX")) return 4;
    if (!strcmp(pLedNickName, "CY5")) return 5;
    if (!strcmp(pLedNickName, "CY7")) return 6;
    if (!strcmp(pLedNickName, "QUAS705")) return 7;
    if (!strcmp(pLedNickName, "AF405")) return 8;

    // 如果没有找到匹配的昵称，返回 0
    return 0;
}

/**
 * @brief Initializes the PCR machine.
 *
 * This function performs the following tasks:
 * - Turns off all LEDs.
 * - Resets all motors.
 * - Stops all PID controllers.
 * - Checks the machine's state.
 * - Controls the door.
 * - Clears the LED event group flags.
 *
 * @return true if the initialization is successful, false otherwise.
 */
bool Poct_init(void)
{
    // uint8_t i = 0;

    ALL_LED_OFF();

    ALL_Motor_RESET();

    PID_Stop(0);

    PID_Stop(1);

    PID_Stop(2);

    PID_Stop(3);

    PID_Stop(4);

    if(Globle.Mechine.State == FAULT)
    {
        return false;
    }

    // Door_Control(DoorState.Door_sta);

    xEventGroupClearBits(ledEventGroupHandle,PCR_LED_FLAG);

    xEventGroupClearBits(ledEventGroupHandle,EXTRACTION_LED_FLAG);

    xEventGroupSetBits(ledEventGroupHandle,END_LED_FLAG); // 指示灯

    return true;
}

bool AppMsgCheck(SMsg * pMsg)
{
    return true;
}

/*******************************************************************
 * @brief 设置温度控制参数并启动相关 PWM 定时器
 *
 * 该函数从传入的消息中提取温度控制参数，并将其存储到全局变量中。
 * 然后启动 TIM8 和 TIM9 的 PWM 输出，并设置相应的温度控制通道为启用状态。
 *
 * @param pMsg 指向包含温度控制参数的 SMsg 结构体指针
 * @param mode 模式参数，当前未使用
 ******************************************************************/
void SetTemperatureControlParameters(SMsg * pMsg)
{
    // 从消息中提取温度控制参数
    ExtractParametersToGlobal(pMsg->m_pAppMsg->m_pAppLayerSubPara, 4);

    // 设置温度控制参数
    Globle.TempControl.Temp_Ch2_Set = Globle.SavaParaNum.Para[0];
    Globle.TempControl.Temp_T2 = Globle.SavaParaNum.Para[1];
    Globle.TempControl.Temp_Ch3_Set = Globle.SavaParaNum.Para[2];
    Globle.TempControl.Temp_T3 = Globle.SavaParaNum.Para[3];

    // 启动 TIM8 和 TIM9 的 PWM 输出
    HAL_TIM_PWM_Start(&TIM8_Handler, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&TIM9_Handler, TIM_CHANNEL_1);

    // 启用温度控制通道
    Globle.TempControl.Temp_Ch2_ENABLE = SET;
    Globle.TempControl.Temp_Ch3_ENABLE = SET;
}


// ... existing code ...

// 封装处理 EXE 命令的函数
void handle_exe_command(SMsg * pMsg, int * pFaultCode) {
    if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_UPG)) {
        Globle.BootLoader.bootLoaderFlag = 1;
        ee_WriteBytes(&Globle.BootLoader.bootLoaderFlag, EEPROM_BOOTLOAD_FLAG_ADDR, sizeof(BootLoaderTypeDef));
        SendFrameR(pMsg, SUB_PARA_T);
        delay_ms(3000);
        __set_FAULTMASK(1);
        NVIC_SystemReset();
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_INI)) {
        if (Poct_init() == true) {
            return;
        }
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_OPEN)) {
        if (DoorState.Door_sta == CLOSE) {
            Door_Control(DoorState.Door_sta);
        }
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_CLOS)) {
        if (DoorState.Door_sta == OPPEN) {
            Door_Control(DoorState.Door_sta);
        }
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_DOWN)) {
        push_motor();
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_LJXB)) {
        if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara, MAIN_PARA_T_t)) {
            ExtractParametersToGlobal(pMsg->m_pAppMsg->m_pAppLayerSubPara, 2);
            Globle.TempControl.Temp_Ch1_Set = Globle.SavaParaNum.Para[0];
            Globle.TempControl.Temp_T1 = Globle.SavaParaNum.Para[1];
            Extraction_Tiqu(pMsg);
        }
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_QXZZ)) {
        Extraction_Clean1();
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_QXDB)) {
        ExtractParametersToGlobal(pMsg->m_pAppMsg->m_pAppLayerSubPara, 2);
        Globle.TempControl.Temp_Ch1_Set = Globle.SavaParaNum.Para[0];
        Globle.TempControl.Temp_T1 = Globle.SavaParaNum.Para[1];
        Extraction_Clean2(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_XTHS)) {
        ExtractParametersToGlobal(pMsg->m_pAppMsg->m_pAppLayerSubPara, 2);
        Globle.TempControl.Temp_Ch1_Set = Globle.SavaParaNum.Para[0];
        Globle.TempControl.Temp_T1 = Globle.SavaParaNum.Para[1];
        Extraction_Elution(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_RYFP)) {
        Extraction_Solution_Apportion(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_ZD)) {
//        ZD_motor_run(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_NZL)) {
        if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara, MAIN_PARA_T_t)) {
            SetTemperatureControlParameters(pMsg);
            App_NZL(pMsg, pFaultCode);
        }
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_YBX)) {
        if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara, MAIN_PARA_T_t)) {
            SetTemperatureControlParameters(pMsg);
            App_YBX(pMsg, pFaultCode);
        }
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_PCR)) {
        Globle.pcr_times = atoi32(pMsg->m_pAppMsg->m_pAppLayerMainPara, 10);
        App_PCR(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_MOV)) {
        if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara, MAIN_PARA_MCL)) {
            if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubPara, "L=0")) {
                // *pFaultCode = Motor_Reset(5000, M08);
            } else {
                ZStr2StrMap *pMap = ParseStr2Map(pMsg->m_pAppMsg->m_pAppLayerSubPara, TOP_SEPARATE_STRING, EQUAL_SEPARAT_STRING);
                char * pHole = GetValueFromStr2StrMap(pMap, "H");
                char * pLed = GetValueFromStr2StrMap(pMap, "L");
                uint8_t uiHoleIndex = atoi(pHole);
                uint8_t uiLedIndex = LedNickName2Index(pLed);
                DeleteStr2StrMap(pMap);
                *pFaultCode = LED_CAM_Scan(uiHoleIndex, uiLedIndex);
            }
            if (*pFaultCode != 0) {
                return;
            }
        }
    }
}

// 封装处理 DEB 命令的函数
void handle_deb_command(SMsg * pMsg) {
    if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_MOV)) {
        ControlUserMotor(pMsg);
    }
}

// 封装处理 LED 命令的函数
void handle_led_command(SMsg * pMsg) {
    if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, MAIN_PARA_ON)) {
        TurnOnUserLED(pMsg->m_pAppMsg->m_pAppLayerMainPara, pMsg->m_pAppMsg->m_pAppLayerSubPara);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, MAIN_PARA_OFF)) {
        TurnOffUserLED(pMsg->m_pAppMsg->m_pAppLayerMainPara);
    }
}

// 封装处理 WR 命令的函数
void handle_wr_command(SMsg * pMsg) {
    if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_MAIN_CMD_PARA)) {
        if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara, "1")) {
            WriteSingleParameterToEEPROM(pMsg->m_pAppMsg->m_pAppLayerSubPara);
        } else {
            WriteParametersToPOCTEEPROMFromMsg(pMsg);
        }
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_IP)) {
        ExtractParametersToGlobal(pMsg->m_pAppMsg->m_pAppLayerSubPara, 4);
        EEPROM_MSG.lip[0] = Globle.SavaParaNum.Para[0];
        EEPROM_MSG.lip[1] = Globle.SavaParaNum.Para[1];
        EEPROM_MSG.lip[2] = Globle.SavaParaNum.Para[2];
        EEPROM_MSG.lip[3] = Globle.SavaParaNum.Para[3];
        write_config_to_eeprom();
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_MAC)) {
        ExtractParametersToGlobal(pMsg->m_pAppMsg->m_pAppLayerSubPara, 6);
        EEPROM_MSG.mac[0] = Globle.SavaParaNum.Para[0];
        EEPROM_MSG.mac[1] = Globle.SavaParaNum.Para[1];
        EEPROM_MSG.mac[2] = Globle.SavaParaNum.Para[2];
        EEPROM_MSG.mac[3] = Globle.SavaParaNum.Para[3];
        EEPROM_MSG.mac[4] = Globle.SavaParaNum.Para[4];
        EEPROM_MSG.mac[5] = Globle.SavaParaNum.Para[5];
        write_config_to_eeprom();
    }
}

// 封装处理 RD 命令的函数
void handle_rd_command(SMsg * pMsg) {
    if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_MAIN_CMD_PARA)) {
        SendPOCTParameters(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_IP)) {
        SendIPAddress(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_MAC)) {
        SendMACAddress(pMsg);
    }
}

// 封装处理 SET 命令的函数
void handle_set_command(SMsg * pMsg) {
    if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_PCR)) {
        if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara, MAIN_PARA_T_t)) {
            SetTemperatureControlParameters(pMsg);
        }
    }
}

// 封装处理 QUY 命令的函数
void handle_quy_command(SMsg * pMsg) {
    if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd, APP_MSG_SUB_CMD_VER)) {
        SendFrameD(pMsg, INSTRUMENTS_VERSION);
    }
}

bool User_App(SMsg * pMsg, int * pFaultCode) {
    if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd, APP_MSG_MAIN_CMD_EXE)) {
        handle_exe_command(pMsg, pFaultCode);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd, APP_MSG_MAIN_CMD_DEB)) {
        handle_deb_command(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd, APP_MSG_MAIN_CMD_LED)) {
        handle_led_command(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd, APP_MSG_MAIN_CMD_WR)) {
        handle_wr_command(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd, APP_MSG_MAIN_CMD_RD)) {
        handle_rd_command(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd, APP_MSG_MAIN_CMD_SET)) {
        handle_set_command(pMsg);
    } else if (!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd, APP_MSG_MAIN_CMD_QUY)) {
        handle_quy_command(pMsg);
    }

    if (Globle.Mechine.State == FAULT) {
        return false;
    }

    return true;
}

// ... existing code ...



