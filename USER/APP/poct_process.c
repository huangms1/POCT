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

/*****************************全局变量**********************************/
GlobleTypedef Globle;

POCT_EEPROMTypedef POCT_Para;


/*****************************函数***************************************/
/****************************
	* @brief  淇濆瓨閿欒浠ｇ爜銆
	* @param  FaultStr;
	* @retval 鏃
 **************************/
void SaveFaultCode(char *FaultStr)
{
	Globle.Mechine.State = FAULT;
	
	xEventGroupSetBits(ledEventGroupHandle,ERROR_LED_FLAG);
	
	memset(Globle.Mechine.faultStr,0,sizeof(Globle.Mechine.faultStr));
	
	strcat(Globle.Mechine.faultStr,FaultStr);
	
	SendFrameI(APP_MSG_UNIT_MAIN,APP_MSG_UNIT_PC,\
			   APP_MSG_MAIN_CMD_RPT,APP_MSG_SUB_CMD_ERR,"",Globle.Mechine.faultStr,&SocketEndpoint);
}


/***************************
  * @brief  鑾峰彇鍙傛暟鏁板瓧
  * @param  
  * @retval 32浣嶆暣褰
  *************************/
uint32_t PARA_Get_num(char *token)
{
	int i = 0,Flag = 0,len;
	
	char num[5] = {0};
	
	memset(num,0,5);
	
	while(token[i]!=NULL)
	{
		if(token[i]=='=')
		{
			i++;
			len = i;
			Flag = 1;
		}
		if(Flag == 1)
		{
			num[i-len] = token[i];			
		}			
		i++;
	}
	return atoi32(num,10);
}

/**************************
  * @brief  鑾峰彇鍙傛暟鏁板瓧
  * @param  
  * @retval 
  ************************/
void Get_Para_Num(char *str,uint8_t len)
{
	char *pSegment = NULL;
	
	uint8_t i = 0;
	
	char *pTempStr = NULL;
	
	pTempStr = pvPortMalloc(strlen(str) + 1);
	
	memcpy(pTempStr,str, strlen(str) + 1);
	
	pSegment = strtok(pTempStr, "^");
	
	Globle.SavaParaNum.Para[0] = (uint16_t)PARA_Get_num(pSegment);
	
	for(i=1;i<len;i++)
	{
		pSegment = strtok(NULL, "^");
		
		Globle.SavaParaNum.Para[i] = (uint16_t)PARA_Get_num(pSegment);
	}
	vPortFree(pTempStr);
}


/****************************
  * @brief  鍐欏崟涓弬鏁板埌鍐呭瓨
  * @param  
  * @retval 
  **************************/
void Write_One_Para(char *str)
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

/****************************
  * @brief  鍐欏涓弬鏁
  * @param  
  * @retval
  **************************/
void Write_Para(SMsg * pMsg)
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
	
	*P++ = (uint32_t)PARA_Get_num(token);
	
	for(i=0;i<Num;i++)
	{
		token = strtok(NULL, "^"); 	   
		
		*P++ = (uint16_t)PARA_Get_num(token);
	}
	
	ee_WriteBytes(&Globle.POCT_Par.rs1,EEPROM_PARA_ADDR,sizeof(POCT_EEPROMTypedef));
	
	vPortFree(pAppLayerSubPara);
}

/*************************
* @brief  鍙戦€乀1娓╁害
* @param  
* @retval 
***********************/
void Send_T1(SMsg * pMsg,uint16_t *Tbuf)
{
	char *newstr;
	
	char strbuf[10] = {0};	
	
	newstr = pvPortMalloc(50);
	
	memset(newstr,0,50);
	
	strcat(newstr,"T1=");
	
	memset(strbuf,0,sizeof(strbuf));
	
	Int2Str((uint32_t)Tbuf[0],strbuf);
	
	strcat(newstr,strbuf);
	
	strcat(newstr,"^t1=");
	
	memset(strbuf,0,sizeof(strbuf));
	
	Int2Str((uint32_t)Tbuf[1],strbuf);
	
	strcat(newstr,strbuf);
	
	SendFrameD(pMsg,newstr);
	
	vPortFree(newstr);
}

/**************************
* @brief  
* @param  
* @retval 
**************************/
void Send_T2T3(SMsg * pMsg,uint16_t *Tbuf)
{
	char *newstr;
	
	char strbuf[10] = {0};
	
	memset(strbuf,0,sizeof(strbuf));
	
	newstr = pvPortMalloc(50);
	
	memset(newstr,0,50);
	
	strcat(newstr,"T2=");
	
	memset(strbuf,0,sizeof(strbuf));
	
	Int2Str((uint32_t)Tbuf[0],strbuf);
	
	strcat(newstr,strbuf);
	
	strcat(newstr,"^t2=");
	
	memset(strbuf,0,sizeof(strbuf));
	
	Int2Str((uint32_t)Tbuf[1],strbuf);
	
	strcat(newstr,strbuf);
	
	strcat(newstr,"^T3=");
	
	memset(strbuf,0,sizeof(strbuf));
	
	Int2Str((uint32_t)Tbuf[2],strbuf);
	
	strcat(newstr,strbuf);
	
	strcat(newstr,"^t3=");
	
	memset(strbuf,0,sizeof(strbuf));
	
	Int2Str((uint32_t)Tbuf[3],strbuf);
	
	strcat(newstr,strbuf);

	SendFrameD(pMsg,newstr);
	
	vPortFree(newstr);
}

/***************************
  * @brief  
  * @param  
  * @retval 
  ***********************/
void Send_Para(SMsg * pMsg)
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

/**************************
* @brief 
  * @param  
  * @retval 
  **************************/
void Send_IP(SMsg * pMsg)
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

/**************************
* @brief  
* @param 
* @retval 
************************/
void Send_MAC(SMsg * pMsg)
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


/****************************
  * @brief  
  * @param  SMsg * pMsg
  * @retval 
  **************************/
void TimingDisplay(SMsg * pMsg)
{
	char *newstr;
	
	uint32_t timeNow = 0;
	
	uint16_t timeSend = 0;
	
	char strbuf[10] = {0};
	
	ZStr2StrMap *pMap = ParseStr2Map(pMsg->m_pAppMsg->m_pAppLayerSubPara, TOP_SEPARATE_STRING, EQUAL_SEPARAT_STRING);
	char * pTime = GetValueFromStr2StrMap(pMap,"t1");
	uint16_t uTime = atoi(pTime);
		
	memset(strbuf,0,sizeof(strbuf));
	
	newstr = pvPortMalloc(50);
	
	timeNow = HAL_GetTick();
	
	while(timeNow + uTime*1000 > HAL_GetTick())
	{
		timeSend ++;
		delay_ms(1000);
		memset(newstr,0,50);
		Int2Str(timeSend,strbuf);
		strcat(newstr,"t1=");
		strcat(newstr,strbuf);
		SendFrameD(pMsg,newstr);
	}
	vPortFree(newstr);
}


/****************************
  * @brief  
  * @param  str:
  * @retval 
  **************************/
void User_Motor_Control(SMsg * pMsg)
{
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M01))//
	{
		Motor_Move(pMsg,M01_FRE,M01,M01_DIR,M01_GO_GPIO_PORT,M01_GO_GPIO_PIN);	
	}
	
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M02))//
	{
		Motor_Move(pMsg,M02_FRE,M02,M02_DIR,M02_GO_GPIO_PORT,M02_GO_GPIO_PIN);	
	}
	
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M03))
	{
		Motor_Move(pMsg,M03_FRE,M03,M03_DIR,M03_GO_GPIO_PORT,M03_GO_GPIO_PIN);
	}
	
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M04))
	{ 
		Motor_Move(pMsg,M04_FRE,M04,M04_DIR,M04_GO_GPIO_PORT,M04_GO_GPIO_PIN);
	}

	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M05))
	{
		Motor_Move(pMsg,M05_FRE,M05,M05_DIR,M05_GO_GPIO_PORT,M05_GO_GPIO_PIN);
	}
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M06))
	{		
		Motor_Move(pMsg,M06_FRE,M06,M06_DIR,M06_GO_GPIO_PORT,M06_GO_GPIO_PIN);
	}
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M07))
	{		
		Motor_Move(pMsg,M07_FRE,M07,M07_DIR,M07_GO_GPIO_PORT,M07_GO_GPIO_PIN);
	}
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M08))
	{		
		Motor_Move(pMsg,M08_FRE,M08,M08_DIR,M08_GO_GPIO_PORT,M08_GO_GPIO_PIN);
	}
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M09))
	{		
		Motor_Move(pMsg,M09_FRE,M09,M09_DIR,M09_GO_GPIO_PORT,M09_GO_GPIO_PIN);
	}
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M10))
	{
		Motor_Move(pMsg,M10_FRE,M10,M10_DIR,M10_GO_GPIO_PORT,M10_GO_GPIO_PIN);
	}
	
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M11))
	{		
		Motor_Move(pMsg,M11_FRE,M11,M11_DIR,M11_GO_GPIO_PORT,M11_GO_GPIO_PIN);
	}
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M12))
	{	
		Motor_Move(pMsg,M12_FRE,M12,M12_DIR,M12_GO_GPIO_PORT,M12_GO_GPIO_PIN);
	}
	
//	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M13))
//	{
//		Motor_Move(pMsg,M13_FRE,M13,M13_DIR,M13_GO_GPIO_PORT,M13_GO_GPIO_PIN);
//	}
//	
//	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M14))
//	{
//		Motor_Move(pMsg,M14_FRE,M14,M14_DIR,M14_GO_GPIO_PORT,M14_GO_GPIO_PIN);
//	}
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_M15))
	{
		Motor_Move(pMsg,M15_FRE,M15,M15_DIR,M15_GO_GPIO_PORT,M15_GO_GPIO_PIN);
	}
}

/*****************************
  * @brief  锟斤拷锟斤拷时锟斤拷锟斤拷LED
	* @param  
  * @retval 锟斤拷
  ****************************/
void UserDebug_LED_ON(char *App_Main_Para,char *App_Slave_Para)
{
	uint8_t LED_b = PARA_Get_num(App_Slave_Para);
	
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_ATTO))
	{
		LED_SPI_SendData(LED_ATTO ,LED_b);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_FAM))
	{
		LED_SPI_SendData(LED_FAM ,LED_b);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_ROX))
	{
		LED_SPI_SendData(LED_ROX ,LED_b);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_CY5))
	{
		LED_SPI_SendData(LED_CY5 ,LED_b);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_CY5DOT5))
	{
		LED_SPI_SendData(LED_CY5DOT5 ,LED_b);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_HEX))
	{
		LED_SPI_SendData(LED_HEX ,LED_b);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_CY7))
	{
		LED_SPI_SendData(LED_CY7 ,LED_b);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_AMCA))
	{
		LED_SPI_SendData(LED_AMCA ,LED_b);
	}
}

/*********************************
  * @brief 
  * @param  *App_Main_Para 
  * @retval 
  *******************************/
void UserDebug_LED_OFF(char *App_Main_Para)
{
//	uint8_t LED_b = PARA_Get_num(Globle.soketStr.App_Slave_Para);
	
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_ATTO))
	{
		LED_SPI_SendData(LED_ATTO ,0);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_FAM))
	{
		LED_SPI_SendData(LED_FAM ,0);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_ROX))
	{
		LED_SPI_SendData(LED_ROX ,0);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_CY5))
	{
		LED_SPI_SendData(LED_CY5 ,0);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_CY5DOT5))
	{
		LED_SPI_SendData(LED_CY5DOT5 ,0);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_HEX))
	{
		LED_SPI_SendData(LED_HEX ,0);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_CY7))
	{
		LED_SPI_SendData(LED_CY7 ,0);
	}
	if(!strcmp(App_Main_Para,MAIN_PARA_LED_AMCA))
	{
		LED_SPI_SendData(LED_AMCA ,0);
	}
}

/****************************
  * @brief  
  * @param  
  * @retval 
  **************************/
int LedNickName2Index(char *pLedNickName)
{
	if(!strcmp(pLedNickName,"ATTO"))
		 return 1;
	else if(!strcmp(pLedNickName,"FAM"))
		 return 2;
	 else if(!strcmp(pLedNickName,"HEX"))
		 return 3;
	 else if(!strcmp(pLedNickName,"ROX"))
		 return 4;
	 else if(!strcmp(pLedNickName,"CY5"))
		 return 5;
	 else if(!strcmp(pLedNickName,"CY7"))
		 return 6;
	 else if(!strcmp(pLedNickName,"CY5.5"))
		 return 7;
	  else if(!strcmp(pLedNickName,"AMCA"))
		 return 8;
	 else
		 return 0;
}



/****************************
  * @brief 
  * @param  
  * @retval 
  **************************/
bool Poct_init(void)
{
//	uint8_t i = 0;
	
	ALL_LED_OFF();	
	
	ALL_Motor_RESET();
	
	PID_Stop(0);
	
	PID_Stop(1);
	
	PID_Stop(2);
	
	PID_Stop(3);
	
	PID_Stop(4);
	
	if(Globle.Mechine.State==FAULT)
	{
		return false;
	}
	
//	Door_Control(DoorState.Door_sta);
	
	xEventGroupClearBits(ledEventGroupHandle,PCR_LED_FLAG);
	
	xEventGroupClearBits(ledEventGroupHandle,EXTRACTION_LED_FLAG);
	
	xEventGroupSetBits(ledEventGroupHandle,END_LED_FLAG); //指示灯
		
	return true;
}

/****************************
  * @brief  
  * @param  
  * @retval 
  **************************/
bool AppMsgCheck(SMsg * pMsg)
{
	return true;
}


void set_temp(SMsg * pMsg,uint8_t mode)
{
	Get_Para_Num(pMsg->m_pAppMsg->m_pAppLayerSubPara,4);
	
	if(mode == 0)
	{		
		Globle.TempControl.Temp_Ch2_Set = Globle.SavaParaNum.Para[0];
		
		Globle.TempControl.Temp_T2 = Globle.SavaParaNum.Para[1];
		
		Globle.TempControl.Temp_Ch3_Set = Globle.SavaParaNum.Para[2];
		
		Globle.TempControl.Temp_T3 = Globle.SavaParaNum.Para[3];
	}
	else
	{
		Globle.TempControl.Temp_Ch3_Set = Globle.SavaParaNum.Para[0];
		
		Globle.TempControl.Temp_T3 = Globle.SavaParaNum.Para[1];
		
		Globle.TempControl.Temp_Ch2_Set = Globle.SavaParaNum.Para[2];
		
		Globle.TempControl.Temp_T2 = Globle.SavaParaNum.Para[3];
	}

	HAL_TIM_PWM_Start(&TIM8_Handler,TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_1);
	
	Globle.TempControl.Temp_Ch2_ENABLE = SET;
	
	Globle.TempControl.Temp_Ch3_ENABLE = SET;
}



/*************************************
* @brief  
* @param  
* @retval 
*************************************/
bool User_App(SMsg * pMsg,int * pFaultCode)
{
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_EXE))
	{	
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_UPG))
		{			
			Globle.BootLoader.bootLoaderFlag = 1;
			ee_WriteBytes(&Globle.BootLoader.bootLoaderFlag,EEPROM_BOOTLOAD_FLAG_ADDR,sizeof(BootLoaderTypeDef));
			SendFrameR(pMsg,SUB_PARA_T);
			delay_ms(3000);
			__set_FAULTMASK(1);
			NVIC_SystemReset();
		}
		
		/* n,C,PC,MCU,EXE,INI,,,*/
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_INI))
		{
			if(Poct_init() == true)
				return true;
			else
				return false;
			
		}
				
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_OPEN))
		{	
			if(DoorState.Door_sta == CLOSE)
				Door_Control(DoorState.Door_sta);
		}
		
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_CLOS))
		{
			if(DoorState.Door_sta == OPPEN)
				Door_Control(DoorState.Door_sta);
		} 
		
		//4,C,PC,MCU,EXE,DOWN,M05,,,  
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_DOWN))
		{
			push_motor();			
		}
		
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_LJXB))
		{
			if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_T_t))
			{
				Get_Para_Num(pMsg->m_pAppMsg->m_pAppLayerSubPara,2);
				
				Globle.TempControl.Temp_Ch1_Set = Globle.SavaParaNum.Para[0];
				
				Globle.TempControl.Temp_T1 = Globle.SavaParaNum.Para[1];
							
				Extraction_Tiqu(pMsg);				
			}
		}
		
		//n,C,PC,MCU,EXE,QXZZ,,,
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_QXZZ))
		{
			Extraction_Clean1();
		}
		//n,C,PC,MCU,EXE,QXDB,T_t,T1=550^t1=300,
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_QXDB))
		{
			
			Get_Para_Num(pMsg->m_pAppMsg->m_pAppLayerSubPara,2);
		
			Globle.TempControl.Temp_Ch1_Set = Globle.SavaParaNum.Para[0];
			
			Globle.TempControl.Temp_T1 = Globle.SavaParaNum.Para[1];
		
			Extraction_Clean2(pMsg);
		}
		//n,C,PC,MCU,EXE,XTHS,T_t,T1=550^t1=360,
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_XTHS))
		{
		
			Get_Para_Num(pMsg->m_pAppMsg->m_pAppLayerSubPara,2);
			
			Globle.TempControl.Temp_Ch1_Set = Globle.SavaParaNum.Para[0];
			
			Globle.TempControl.Temp_T1 = Globle.SavaParaNum.Para[1];
		
			Extraction_Elution(pMsg);
			
						
		}
		//n,C,PC,MCU,EXE,RYFP,,,
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_RYFP))
		{
			Extraction_Solution_Apportion(pMsg);			
		}
		
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_ZD))
		{
			ZD_motor_run(pMsg);
		}
		
		//n,C,PC,MCU,EXE,NZL,T_t,T2=970^t2=0^T3=550^t3=900,		
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_NZL))
		{
			if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_T_t))
			{
				set_temp(pMsg,0);
				
				App_NZL(pMsg,pFaultCode);								
			}
		}
		
		//n,C,PC,MCU,EXE,YBX,T_t,T2=970^t2=60^T3=600^t3=0,
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_YBX))
		{
			if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_T_t))
			{

				set_temp(pMsg,0);
								
				App_YBX(pMsg,pFaultCode);
								
			}
		}
				
		//13,C,PC,MCU,EXE,PCR,1,,,
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_PCR))
		{
//			atoi32(pMsg->m_pAppMsg->m_pAppLayerSubPara,10);
			Globle.pcr_times = atoi32(pMsg->m_pAppMsg->m_pAppLayerMainPara,10);
			App_PCR(pMsg);
		}
		
		//14,C,PC,MCU,EXE,MOV,MCL,H=1^L=1,
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_MOV))
		{
			if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_MCL))
			{
				
				if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubPara,"L=0"))
				{
//					*pFaultCode = Motor_Reset(5000,M08);
				}
				else
				{
					ZStr2StrMap *pMap = ParseStr2Map(pMsg->m_pAppMsg->m_pAppLayerSubPara, TOP_SEPARATE_STRING, EQUAL_SEPARAT_STRING);
	
					char * pHole = GetValueFromStr2StrMap(pMap,"H");
					
					char * pLed = GetValueFromStr2StrMap(pMap,"L");
					
					uint8_t uiHoleIndex = atoi(pHole);
					
					uint8_t uiLedIndex = LedNickName2Index(pLed);
					
					DeleteStr2StrMap(pMap);
					//Get_Para_Num(pMsg->m_pAppMsg->m_pAppLayerSubPara,2);
					//*pFaultCode = LED_CAM_Scan(Globle.SavaParaNum.Para[0],Globle.SavaParaNum.Para[1]);  			
					*pFaultCode = LED_CAM_Scan(uiHoleIndex,uiLedIndex); 
				
				}				
				if(*pFaultCode != 0)
				{
					return false;
				}
			}
		}	
	}
	
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_DEB))
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_MOV))
		{
			User_Motor_Control(pMsg);
		}
	}
	/*LED锟斤拷锟斤拷*/
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_LED))
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,MAIN_PARA_ON))
		{						
			UserDebug_LED_ON(pMsg->m_pAppMsg->m_pAppLayerMainPara,
								pMsg->m_pAppMsg->m_pAppLayerSubPara);
		}
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,MAIN_PARA_OFF))
		{						
			UserDebug_LED_OFF(pMsg->m_pAppMsg->m_pAppLayerMainPara);
		}	
	}
	
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_WR))
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_MAIN_CMD_PARA))
		{
			if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,"1"))
			{
				Write_One_Para(pMsg->m_pAppMsg->m_pAppLayerSubPara);
			}
			else
			{				
				Write_Para(pMsg);
			}
		}
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_IP))
		{
		
			Get_Para_Num(pMsg->m_pAppMsg->m_pAppLayerSubPara,4);
			
			EEPROM_MSG.lip[0] = Globle.SavaParaNum.Para[0];
			
			EEPROM_MSG.lip[1] = Globle.SavaParaNum.Para[1];
			
			EEPROM_MSG.lip[2] = Globle.SavaParaNum.Para[2];
			
			EEPROM_MSG.lip[3] = Globle.SavaParaNum.Para[3];
			
			write_config_to_eeprom();	
			
		}
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_MAC))
		{
			
			Get_Para_Num(pMsg->m_pAppMsg->m_pAppLayerSubPara,6);
			
			EEPROM_MSG.mac[0] = Globle.SavaParaNum.Para[0];
			
			EEPROM_MSG.mac[1] = Globle.SavaParaNum.Para[1];
			
			EEPROM_MSG.mac[2] = Globle.SavaParaNum.Para[2];
			
			EEPROM_MSG.mac[3] = Globle.SavaParaNum.Para[3];
			
			EEPROM_MSG.mac[4] = Globle.SavaParaNum.Para[4];
			
			EEPROM_MSG.mac[5] = Globle.SavaParaNum.Para[5];
			
			write_config_to_eeprom(); 
		}		
	}
	
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_RD)) 
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_MAIN_CMD_PARA))
		{
			Send_Para(pMsg);			
		}
		
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_IP))
		{
			Send_IP(pMsg);
		}
		
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_MAC))
		{
			Send_MAC(pMsg);
		}		
	}
	
	//4,C,PC,MCU,SET,PCR,T_t,T2=970^t2=10^T3=600^t3=30,
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_SET))
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_PCR))
		{
			if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainPara,MAIN_PARA_T_t))	
			{	
				set_temp(pMsg,0);
			}
		}			
	}
	
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_QUY))
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_VER))
		{
			SendFrameD(pMsg,INSTRUMENTS_VERSION);
		}
	}
	
	if(Globle.Mechine.State == FAULT)
	{
		return false;
	}
	
	return true;	
}



