/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief  		配置MCU，移植W5500程序需要修改的文件，配置W5500的MAC和IP地址
**************************************************************************************************
*/
#include <stdio.h> 
#include <string.h>

#include "w5500_conf.h"
#include "bsp_i2c_ee.h"
#include "utility.h"
#include "w5500.h"
#include "dhcp.h"
#include "bsp_TiMbase.h"
#include "delay.h"

CONFIG_MSG  ConfigMsg;																	/*配置结构体*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM存储信息结构体*/

/*定义MAC地址,如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};

/*定义默认IP信息*/
uint8 local_ip[4]  ={192,168,1,199};										/*定义W5500默认IP地址*/

uint8 subnet[4]    ={255,255,255,0};										/*定义W5500默认子网掩码*/

uint8 gateway[4]   ={192,168,1,1};											/*定义W5500默认网关*/

uint8 dns_server[4]={114,114,114,114};									    /*定义W5500默认DNS*/

uint16 local_port=5000;	                       					            /*定义本地端口*/

/*定义远端IP信息*/
uint8  remote_ip[4]={192,168,1,100};										/*远端IP地址*/

uint16 remote_port = 6000;													/*远端端口号*/

uint8	ip_from = IP_FROM_EEPROM;	//IP_FROM_EEPROM	IP_FROM_DEFINE		

uint8     dhcp_ok   = 0;													   	/*dhcp成功获取IP*/
uint32	  ms        = 0;														/*毫秒计数*/
uint32	  dhcp_time = 0;														/*DHCP运行计数*/
uint8_t	  ntptimer  = 0;													/*NPT秒计数*/

/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
void set_w5500_ip(void)
{		
		
   /*复制定义的配置信息到配置结构体*/
	memcpy(ConfigMsg.mac, mac, 6);
	
	memcpy(ConfigMsg.lip,local_ip,4);
	
	memcpy(ConfigMsg.sub,subnet,4);
	
	memcpy(ConfigMsg.gw,gateway,4);
	
	memcpy(ConfigMsg.dns,dns_server,4);
	//if(ip_from==IP_FROM_DEFINE)	
	//	printf(" 使用定义的IP信息配置W5500\r\n");
	
	/*使用EEPROM存储的IP参数*/	
	if(ip_from==IP_FROM_EEPROM)
	{
		/*从EEPROM中读取IP配置信息*/
		read_config_from_eeprom();		
		
		/*如果读取EEPROM中MAC信息,如果已配置，则可使用*/		
		if( *(EEPROM_MSG.mac)==0x00&& *(EEPROM_MSG.mac+1)==0x08&&*(EEPROM_MSG.mac+2)==0xdc)		
		{
			//printf(" IP from EEPROM\r\n");
			/*复制EEPROM配置信息到配置的结构体变量*/		
			memcpy(ConfigMsg.lip,EEPROM_MSG.lip, 4);				
			memcpy(ConfigMsg.sub,EEPROM_MSG.sub, 4);
			memcpy(ConfigMsg.gw, EEPROM_MSG.gw, 4);
		}
		else
		{
			memcpy(EEPROM_MSG.mac,ConfigMsg.mac, 6);
			memcpy(EEPROM_MSG.lip,ConfigMsg.lip, 4);				
			memcpy(EEPROM_MSG.sub,ConfigMsg.sub, 4);
			memcpy(EEPROM_MSG.gw, ConfigMsg.gw, 4);
			write_config_to_eeprom();	/*使用默认的IP信息，并初始化EEPROM中数据*/
		}			
	}

	/*使用DHCP获取IP参数，需调用DHCP子函数*/		
	if(ip_from==IP_FROM_DHCP)								
	{
		/*复制DHCP获取的配置信息到配置结构体*/
		if(dhcp_ok==1)
		{
			//printf(" IP from DHCP\r\n");		 
			memcpy(ConfigMsg.lip,DHCP_GET.lip, 4);
			memcpy(ConfigMsg.sub,DHCP_GET.sub, 4);
			memcpy(ConfigMsg.gw,DHCP_GET.gw, 4);
			memcpy(ConfigMsg.dns,DHCP_GET.dns,4);
		}
		else
		{
			memcpy(EEPROM_MSG.mac,ConfigMsg.mac, 6);
			memcpy(EEPROM_MSG.lip,ConfigMsg.lip, 4);				
			memcpy(EEPROM_MSG.sub,ConfigMsg.sub, 4);
			memcpy(EEPROM_MSG.gw, ConfigMsg.gw, 4);
			write_config_to_eeprom();	/*使用默认的IP信息，并初始化EEPROM中数据*/
		}
	}
		
	/*以下配置信息，根据需要选用*/	
	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*将IP配置信息写入W5500相应寄存器*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	getSIPR (local_ip);			
	//printf(" W5500 IP地址   : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	//printf(" W5500 子网掩码 : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	//printf(" W5500 网关     : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
}

/**
*@brief		配置W5500的MAC地址
*@param		无
*@return	无
*/
void set_w5500_mac(void)
{	
	/*从EEPROM中读取IP配置信息*/
	if(ip_from==IP_FROM_EEPROM)
	{		
		read_config_from_eeprom();	
		memcpy(mac,EEPROM_MSG.mac, 6);
		setSHAR(EEPROM_MSG.mac);	      /**/
	}
	else
	{
		memcpy(ConfigMsg.mac, mac, 6);
		setSHAR(ConfigMsg.mac);	      /**/
	}
	memcpy(DHCP_GET.mac, mac, 6);
}

SPI_HandleTypeDef SpiHandle; //SPI 句柄

/**
  * @brief SPI MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  SPIx_SCK_GPIO_CLK_ENABLE();
  SPIx_MISO_GPIO_CLK_ENABLE();
  SPIx_MOSI_GPIO_CLK_ENABLE();
  SPIx_CS_GPIO_CLK_ENABLE();
  /* Enable SPI clock */
  SPIx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* SPI SCK GPIO pin configuration  */
  GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = SPIx_SCK_AF;
  
  HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);
    
  /* SPI MISO GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MISO_PIN;
  GPIO_InitStruct.Alternate = SPIx_MISO_AF;
  
  HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);
  
  /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPIx_MOSI_AF;  
    HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);   

    GPIO_InitStruct.Pin = WIZ_SPIx_SCS ;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(WIZ_SPIx_SCS_PORT, &GPIO_InitStruct); 
	
//	GPIO_InitStruct.Pin = WIZ_SPIx_SCS ;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  HAL_GPIO_Init(WIZ_SPIx_SCS_PORT, &GPIO_InitStruct); 
	
	//中断
	GPIO_InitStruct.Pin = WIZ_INT ;
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_InitStruct.Pull=GPIO_PULLUP;          //上拉
    HAL_GPIO_Init(WIZ_SPIx_INT_PORT, &GPIO_InitStruct); 
	
	//复位
	GPIO_InitStruct.Pin = WIZ_RESET ;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(WIZ_SPIx_RESET_PORT, &GPIO_InitStruct); 
		
}


void SPI_FLASH_Init(void)
{
  /*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  SpiHandle.Instance               = SPIx;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	
  SpiHandle.Init.Mode = SPI_MODE_MASTER;

  HAL_SPI_Init(&SpiHandle); 
  
  __HAL_SPI_ENABLE(&SpiHandle); 

  /* 使 SPI_FLASH 进入 4 字节地址模式 */
  //	SPI_FLASH_Mode_Init();  
}

/**
*@brief		配置W5500的GPIO接口
*@param		无
*@return	无
*/

/**
*@brief		W5500片选信号设置函数
*@param		val: 为“0”表示片选端口为低，为“1”表示片选端口为高
*@return	无
*/
void wiz_cs(uint8_t val)
{
	if (val == LOW) 
	{
	  HAL_GPIO_WritePin(WIZ_SPIx_SCS_PORT, WIZ_SPIx_SCS,GPIO_PIN_RESET); 
	}
	else if (val == HIGH)
	{
	  HAL_GPIO_WritePin(WIZ_SPIx_SCS_PORT, WIZ_SPIx_SCS,GPIO_PIN_SET); 
	}
}

/**
*@brief		设置W5500的片选端口SCSn为低
*@param		无
*@return	无
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		设置W5500的片选端口SCSn为高
*@param		无
*@return	无
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}

/**
*@brief		W5500复位设置函数
*@param		无
*@return	无
*/
void reset_w5500(void)
{
	HAL_GPIO_WritePin(WIZ_SPIx_RESET_PORT, WIZ_RESET,GPIO_PIN_RESET);
	delay_us(2);  
	HAL_GPIO_WritePin(WIZ_SPIx_RESET_PORT, WIZ_RESET,GPIO_PIN_SET);
	delay_ms(1600);
}

uint8_t SPI_SendByte(uint8_t byte)
{
	uint8_t Rxdata;	
	HAL_SPI_TransmitReceive(&SpiHandle,&byte,&Rxdata,1, 1000);
	return Rxdata; //返回收到的数据
}

/**
*@brief		STM32 SPI1读写8位数据
*@param		dat：写入的8位数据
*@return	无
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI_SendByte(dat));
}

/**
*@brief		写入一个8位数据到W5500
*@param		addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return	无
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
   iinchip_csoff();                              		
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
   IINCHIP_SpiSendData(data);                   
   iinchip_cson();                            
}

/**
*@brief		从W5500读出一个8位数据
*@param		addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return	无
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
   uint8 data = 0;
   iinchip_csoff();                            
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
   data = IINCHIP_SpiSendData(0x00);            
   iinchip_cson();                               
   return data;    
}

/**
*@brief		向W5500写入len字节数据
*@param		addrbsb: 写入数据的地址
*@param   buf：写入字符串
*@param   len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
   uint16 idx = 0;
   //if(len == 0) printf("Unexpected2 length 0\r\n");
   iinchip_csoff();                               
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
   for(idx = 0; idx < len; idx++)
   {
     IINCHIP_SpiSendData(buf[idx]);
   }
   iinchip_cson();                           
   return len;  
}

/**
*@brief		从W5500读出len字节数据
*@param		addrbsb: 读取数据的地址
*@param 	buf：存放读取数据
*@param		len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint16 idx = 0;
  //if(len == 0)
  //{
  //  printf("Unexpected2 length 0\r\n");
  //}
  iinchip_csoff();                                
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
  for(idx = 0; idx < len; idx++)                   
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();                                  
  return len;
}

/**
*@brief		写配置信息到EEPROM
*@param		无
*@return	无
*/
void write_config_to_eeprom(void)
{
	uint16 dAddr = EEPROM_IP_ADDR;
	ee_WriteBytes(EEPROM_MSG.mac,dAddr,EEPROM_MSG_LEN); //ConfigMsg.mac
	delay_ms(10);																							
}

/**
*@brief		从EEPROM读配置信息
*@param		无
*@return	无
*/
void read_config_from_eeprom(void)
{
	ee_ReadBytes(EEPROM_MSG.mac,EEPROM_IP_ADDR,EEPROM_MSG_LEN);
	delay_us(10);
}

/**
*@brief		STM32定时器2初始化
*@param		无
*@return	无
*/
void timer5_init(void)
{
//	TIM5_Configuration();																		/* TIM2 定时配置 */
	TIM5_Init(50-1,90-1);
//	TIM2_NVIC_Configuration();															/* 定时器的中断优先级 */
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);		/* TIM2 重新开时钟，开始计时 */
}

/**
*@brief		dhcp用到的定时器初始化
*@param		无
*@return	无
*/
void dhcp_timer_init(void)
{
  timer5_init();																	
}

/**
*@brief		ntp用到的定时器初始化
*@param		无
*@return	无
*/
void ntp_timer_init(void)
{
  timer5_init();																	
}

/**
*@brief		定时器5中断函数
*@param		无
*@return	无
*/
void timer5_isr(void)
{
  ms++;	
  if(ms>=1000)
  {  
    ms=0;
    dhcp_time++;																					/*DHCP定时加1S*/
	  #ifndef	__NTP_H__
	  ntptimer++;																						/*NTP重试时间加1S*/
	  #endif
  }
}


/**
*@brief		STM32系统软复位函数
*@param		无
*@return	无
*/
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32_t JumpAddress;
  //printf(" 系统重启中……\r\n");
  JumpAddress = *(uint32_t*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}


/**
*@brief		初始化哇500
*@param		无
*@return	无
*/
void W5500_SPI_Init(void)
{
	HAL_SPI_MspInit(&SpiHandle);
	SPI_FLASH_Init();
}

