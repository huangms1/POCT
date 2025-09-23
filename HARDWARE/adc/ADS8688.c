#include "ADS8688_conf.h"

SPI_HandleTypeDef ADS8688_SPI_Handle;

uint8_t tx_buffer[] = {0x00, 0x00}; // Dummy data to transmit to ADS8688
uint8_t rx_buffer[2]; // Buffer to store received data from ADS8688

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

void ADS8688_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	ADS8688_SPIx_CLK_ENABLE();
	ADS8688_SPIx_MISO_GPIO_CLK_ENABLE();
	ADS8688_SPIx_MOSI_GPIO_CLK_ENABLE();
	ADS8688_SPIx_CS_GPIO_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/  
	/* SPI SCK GPIO pin configuration  */
	GPIO_InitStruct.Pin       = ADS8688_SPIx_SCK_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = ADS8688_SPIx_SCK_AF;

	HAL_GPIO_Init(ADS8688_SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MISO GPIO pin configuration  */
	GPIO_InitStruct.Pin = ADS8688_SPIx_MISO_PIN;
	GPIO_InitStruct.Alternate = ADS8688_SPIx_MISO_AF;

	HAL_GPIO_Init(ADS8688_SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MOSI GPIO pin configuration  */
	GPIO_InitStruct.Pin = ADS8688_SPIx_MOSI_PIN;
	GPIO_InitStruct.Alternate = ADS8688_SPIx_MISO_AF;  
	HAL_GPIO_Init(ADS8688_SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);   

	GPIO_InitStruct.Pin = ADS8688_SPIx_SCS ;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(ADS8688_SPIx_SCS_PORT, &GPIO_InitStruct); 

	GPIO_InitStruct.Pin = ADS8688_RESET_PIN ;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(ADS8688_SPIx_RESET_PORT, &GPIO_InitStruct); 
}

/*===============================================================
函数名称：SPI_Init
函数功能：SPI初始化
输入参数：无
返 回 值：无
================================================================*/

void MX_SPI4_Init(void)
{
	ADS8688_SPIx_CLK_ENABLE();
	ADS8688_SPIx_MISO_GPIO_CLK_ENABLE();
	ADS8688_SPIx_MOSI_GPIO_CLK_ENABLE();
		
	/*##-1- Configure the SPI peripheral #######################################*/
	/* Set the SPI parameters */
	ADS8688_SPI_Handle.Instance = SPI4;
	ADS8688_SPI_Handle.Init.Mode = SPI_MODE_MASTER;
	ADS8688_SPI_Handle.Init.Direction = SPI_DIRECTION_2LINES;
	ADS8688_SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
	ADS8688_SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
	ADS8688_SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
	ADS8688_SPI_Handle.Init.NSS = SPI_NSS_SOFT;
	ADS8688_SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	ADS8688_SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	ADS8688_SPI_Handle.Init.TIMode = SPI_TIMODE_DISABLE;
	ADS8688_SPI_Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	ADS8688_SPI_Handle.Init.CRCPolynomial = 10;

	if (HAL_SPI_Init(&ADS8688_SPI_Handle) != HAL_OK)
	{
	    Error_Handler();
	}
	__HAL_SPI_ENABLE(&ADS8688_SPI_Handle);
}

/**********************************************************************************************************
*	函 数 名: ADS8688_IO_Init
*	功能说明: ADS8688 IO口初始化
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_IO_Init(void) //ADS8688 IO口初始化
{
	HAL_Delay(20);
	CS_H;
	RST_H;
	HAL_Delay(5);
}
/**********************************************************************************************************
*	函 数 名: ADS8688_Init
*	功能说明: ADS8688初始化
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_Init(uint8_t ch_en) //ADS8688初始化
{
	ADS8688_IO_Init();
	SOFT_RESET();                                 //软件复位
	HAL_Delay(5);                                 //手册上没找到具体的，基本上是ns级，这里设置1ms
	Set_Auto_Scan_Sequence(ch_en);                //使能通道
	ADS8688_WriteReg(CH_PD, ~ch_en);              //通道退出低功耗状态 通道上电
	HAL_Delay(20);                                //手册上说退出低功耗至少15ms
	while (ADS8688_ReadReg(AUTO_SEQ_EN) != ch_en) //是否使能成功
	{
		Set_Auto_Scan_Sequence(ch_en);   //使能通道
		ADS8688_WriteReg(CH_PD, ~ch_en); //通道退出低功耗状态 通道上电
		HAL_Delay(20);
	}
}

/**********************************************************************************************************
*	函 数 名: ADS8688_SPI_Read8Bit
*	功能说明: 从SPI总线读8个bit数据。 不带CS控制。
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
uint8_t ADS8688_SPI_Read8Bit(void)
{
	uint8_t read = 0;
	HAL_SPI_Receive(&ADS8688_SPI_Handle, &read, 1,1000);
	return read;
}
/**********************************************************************************************************
*	函 数 名: ADS8688_SPI_Write8Bit
*	功能说明: 向SPI总线写8个bit数据。 不带CS控制。
*	形    参: data : 数据
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_SPI_Write8Bit(uint8_t data)
{
	CS_L;
	HAL_SPI_Transmit(&ADS8688_SPI_Handle, &data, 1, 1000);
}

/**********************************************************************************************************
*	函 数 名: ADS8688_WriteCmd
*	功能说明: 写命令寄存器
*	形    参:  cmd : 命令
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_WriteCmd(uint16_t cmd) //写ADS8688命令寄存器
{
	CS_L;
	ADS8688_SPI_Write8Bit(cmd >> 8 & 0XFF);
	ADS8688_SPI_Write8Bit(cmd & 0XFF);
	ADS8688_SPI_Write8Bit(0X00);
	ADS8688_SPI_Write8Bit(0X00);
	CS_H;
}

/*
*********************************************************************************************************
*	函 数 名: SOFT_RESET
*	功能说明: 软件复位
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SOFT_RESET(void) //软件复位
{
	ADS8688_WriteCmd(RST);
}

/**********************************************************************************************************
*	函 数 名: AUTO_RST_Mode
*	功能说明: 自动模式
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void AUTO_RST_Mode(void) //自动扫描模式
{
	ADS8688_WriteCmd(AUTO_RST);
}
/**********************************************************************************************************
*	函 数 名: MAN_CH_Mode
*	功能说明: 手动模式通道选择
*	形    参:  ch : 要选择的通道
*	返 回 值: 无
**********************************************************************************************************/
void MAN_CH_Mode(uint16_t ch) //手动模式
{
	ADS8688_WriteCmd(ch);
}

/**********************************************************************************************************
*	函 数 名: ADS8688_ReadReg
*	功能说明: 读指定的寄存器
*	形    参:  addr : 寄存器地址
*	返 回 值: data : 读到的寄存器值。
**********************************************************************************************************/
uint8_t ADS8688_ReadReg(uint8_t addr)
{
	uint8_t data = 0;
	CS_L;
	ADS8688_SPI_Write8Bit(addr << 1);
	data = ADS8688_SPI_Read8Bit();
	data = ADS8688_SPI_Read8Bit();
	CS_H;
	return data;
}
/**********************************************************************************************************
*	函 数 名: ADS8688_WriteReg
*	功能说明: 写指定的寄存器
*	形    参:  addr : 寄存器地址
*			  		data : 寄存器值
*	返 回 值: 无
**********************************************************************************************************/
void ADS8688_WriteReg(uint8_t addr, uint8_t data)
{
	CS_L;
	ADS8688_SPI_Write8Bit(addr << 1 | 0X01);
	ADS8688_SPI_Write8Bit(data);
	CS_H;
}
/**********************************************************************************************************
*	函 数 名: Set_Auto_Scan_Sequence
*	功能说明: 设置自动扫描序列通道
*	形    参:  seq：需要设置自动扫描序列使能寄存器的值
*	返 回 值: 无
**********************************************************************************************************/
void Set_Auto_Scan_Sequence(uint8_t seq)
{
	ADS8688_WriteReg(AUTO_SEQ_EN, seq);
}
/**********************************************************************************************************
*	函 数 名: Set_CH_Range
*	功能说明: 设置指定通道测量范围
*	形    参:  ch : 通道
*			  		range : 测量范围
*	返 回 值: 无
**********************************************************************************************************/
void Set_CH_Range(uint8_t ch, uint8_t range)
{
	ADS8688_WriteReg(ch, range);
}

/**********************************************************************************************************
*	函 数 名: Get_AUTO_RST_Mode_Data
*	功能说明: 读取自动扫描模式AD值
*	形    参:data : 数据指针
*			  	chnum : 通道个数
*	返 回 值: 无
**********************************************************************************************************/
void Get_AUTO_RST_Mode_Data(uint16_t *data, uint8_t chnum)
{
	uint8_t i = 0, datal = 0, datah = 0;
	for (i = 0; i < chnum; i++)
	{
		CS_L;
		ADS8688_SPI_Write8Bit(0X00);
		ADS8688_SPI_Write8Bit(0X00);
		datah = ADS8688_SPI_Read8Bit();
		datal = ADS8688_SPI_Read8Bit();
		CS_H;
		*(data + i) = datah << 8 | datal;
	}
}
/**********************************************************************************************************
*	函 数 名: Get_MAN_CH_Mode_Data
*	功能说明: 读取手动模式AD值
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
uint16_t Get_MAN_CH_Mode_Data(void)
{
	uint8_t datah = 0, datal = 0;
	CS_L;
	ADS8688_SPI_Write8Bit(0X00);
	ADS8688_SPI_Write8Bit(0X00);
	datah = ADS8688_SPI_Read8Bit();
	datal = ADS8688_SPI_Read8Bit();
	CS_H;
	return (datah << 8 | datal);
}

