#include "SocketTask.h"
#include "delay.h"
#include "W5500.h"
#include "Endpoint.h"

#include "w5500_conf.h"
#include "w5500.h"
#include "socket.h"

#include "led.h"

#include "StartTask.h"

TaskHandle_t SocketTask_Handler;

static void SocketInit(void)
{
	W5500_SPI_Init();
	
	reset_w5500();									/*硬复位W5500*/
	
	set_w5500_mac();								/*配置MAC地址*/

	set_w5500_ip();									/*配置IP地址*/
	
	socket_buf_init(txsize, rxsize);				/*初始化8个Socket的发送接收缓存大小*/
}

void SocketTask(void *pData)                       //W5500 网络芯片任务函数
{
#define LED_SWITCH_THRESHOLD 100
	
	ushort usLoopCycle = 0;
	
	SEndpoint * pEndpoint = pData;
	
	SocketInit();
		
	while(1)
	{
		++usLoopCycle;
		
		if(usLoopCycle>LED_SWITCH_THRESHOLD)
		{
			SYS_LED =! SYS_LED;
			
			LED_StateControl();
			
			usLoopCycle = 0;
		}
		ActIfNetStateChanged();
		
		if(IsNetConnected())
		{			
			EndpointRecvAndSpell(pEndpoint);
			
			EndpointSendOne(pEndpoint);					
				
			xSemaphoreGive(Socket_InitSeceass_SemaphoreHandle);//释放信号量			
		}
		else
		{
			EndpointRemoveFirstMsgIfMemAlmostUsedUp(pEndpoint);						
		}
		vTaskDelay(5);      			//延时10ms，也就是10个时钟节拍	

	};
}

