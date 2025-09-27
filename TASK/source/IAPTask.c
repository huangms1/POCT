#include "IAPTask.h"
#include "delay.h"
#include "iap.h"
#include "utility.h"
#include "Reply.h"
#include "AppMsgDefine.h"
#include "bsp_i2c_ee.h"
#include "InstrumentInclude.h"

#define FLASH_APP1_ADDR 0x08010000
#define APP_BUF_SIZE    2*1024

uint32_t flash_app_addr = FLASH_APP1_ADDR;

TaskHandle_t IAPTask_Handler;

uint8_t App_Buf[APP_BUF_SIZE];  //2K写一次

uint32_t Rx_cnt = 0;            //接收计数 

uint32_t iBytesInAppBuf = 0;

/*事件组*/
//static EventGroupHandle_t Event_Handle = NULL; 
uint8_t  AppUpgrade(SMsg * pMsg)
{
	uint32_t len = 0;//512
	uint32_t totle_len = 0;
	uint8_t temp = 0;
	
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_QUY))//读取版本号
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_VER))
		{
			SendFrameD(pMsg,"BOOTLOADER");
			SendFrameR(pMsg,SUB_PARA_T);
		}
	}
	
	//等待需要升级的数据 10秒内如果没有数据上传，则跳转到APP
	if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerMainCmd,APP_MSG_MAIN_CMD_EXE))
	{
		if(!strcmp(pMsg->m_pAppMsg->m_pAppLayerSubCmd,APP_MSG_SUB_CMD_UPG))
		{
			
			char * pSegment = NULL;
			int iSubCmdLength = strlen(pMsg->m_pAppMsg->m_pAppLayerMainPara);
			char *pAppLayerMainPara = pvPortMalloc(iSubCmdLength + 1);
			uint32_t uiBytesWritten = Rx_cnt;
			memcpy(pAppLayerMainPara,pMsg->m_pAppMsg->m_pAppLayerMainPara,iSubCmdLength + 1);
			pSegment = strtok(pAppLayerMainPara,"^");
			len = atoi32(pSegment,10); //接收数据长度 512			
			pSegment = strtok(NULL,"^");
			Rx_cnt = atoi32(pSegment,10); //512
			pSegment = strtok(NULL,"^");
			totle_len = atoi32(pSegment,10);
			vPortFree(pAppLayerMainPara);
			
			if(uiBytesWritten + len != Rx_cnt)
			{
				Rx_cnt = uiBytesWritten;
				DeleteMsg(pMsg);
			}
			
			if(totle_len == 0)
			{
				Rx_cnt = 0;
				iBytesInAppBuf = 0;
				SendFrameR(pMsg,SUB_PARA_T);				
				delay_ms(3000);
				__set_FAULTMASK(1);
				NVIC_SystemReset();
			}
			
			if(len==0)
			{
				
			}
			else
			{
				if(APP_BUF_SIZE - iBytesInAppBuf >= len)
				{
					memcpy(&App_Buf[iBytesInAppBuf],pMsg->m_pAppMsg->m_pAppLayerSubPara,len);
					iBytesInAppBuf += len;
				}
				else
				{
					int iCopiedBtyes = APP_BUF_SIZE - iBytesInAppBuf;
					memcpy(&App_Buf[iBytesInAppBuf],pMsg->m_pAppMsg->m_pAppLayerSubPara,iCopiedBtyes);
					iap_write_appbin(flash_app_addr,App_Buf,APP_BUF_SIZE); //更新FLASH代码
					iBytesInAppBuf = 0;
					memcpy(&App_Buf[iBytesInAppBuf],pMsg->m_pAppMsg->m_pAppLayerSubPara + iCopiedBtyes, len -iCopiedBtyes);
					iBytesInAppBuf = iBytesInAppBuf + len -iCopiedBtyes;
					temp = 1;
				}
				if(totle_len == Rx_cnt)
				{
					iap_write_appbin(flash_app_addr,App_Buf,APP_BUF_SIZE); 
					iBytesInAppBuf = 0;						
					Globle.BootLoader.bootLoaderFlag = 0;
					ee_WriteBytes(&Globle.BootLoader.bootLoaderFlag,EEPROM_BOOTLOAD_FLAG_ADDR,sizeof(BootLoaderTypeDef));
					SendFrameR(pMsg,SUB_PARA_T);
					delay_ms(3000);
					__set_FAULTMASK(1);
					NVIC_SystemReset();
				}
			}
			SendFrameR(pMsg,SUB_PARA_T);
		}	
	}
	
	return temp;
}

void IAPTask(void *pData)
{
	uint16_t TimeOut = 0;

	while(1)
	{
		SMsg * pMsg = PopFromQueue(Me.m_pNormalExeQueue);
		
		if(NULL != pMsg)
		{
			AppUpgrade(pMsg); 
			TimeOut = 0;
		}
		
		TimeOut ++;
		if(TimeOut > 5000)
		{
			TimeOut = 0;
			Globle.BootLoader.bootLoaderFlag = 0;
			ee_WriteBytes(&Globle.BootLoader.bootLoaderFlag,EEPROM_BOOTLOAD_FLAG_ADDR,sizeof(BootLoaderTypeDef));
			delay_ms(3000);
			__set_FAULTMASK(1);
			NVIC_SystemReset();
		}
		vTaskDelay(5);
	}
	
} 




