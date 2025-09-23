#include "pac9535.h"
#include "stm32f4xx.h"
#include "bsp_motor.h"
#include "FreeRTOS.h"
#include "DIV_Control.h"
#include "delay.h"


//保存电机细分
uint16_t IO1_port;

uint16_t IO2_port;

void pac9535_output(uint8_t addr ,uint16_t data,uint8_t port)
{
	PAC9535_IIC_Start();  
	
	PAC9535_IIC_Send_Byte(0x40|(addr<<1)); 
	
	PAC9535_IIC_Wait_Ack();
	
	if (port == 0)
	{
		PAC9535_IIC_Send_Byte(0x02);
	}
	else
	{
		PAC9535_IIC_Send_Byte(0x03);
	}
	
	PAC9535_IIC_Wait_Ack();
	
	PAC9535_IIC_Send_Byte((uint8_t)(data>>8));
	
	PAC9535_IIC_Wait_Ack();
	
	PAC9535_IIC_Send_Byte((uint8_t)data);
	
	PAC9535_IIC_Wait_Ack();
	
	PAC9535_IIC_Stop();
}


void pac9535_set_configuration_registers(uint8_t addr,uint16_t data,uint8_t portNum)
{
	PAC9535_IIC_Start();  
	
	PAC9535_IIC_Send_Byte(0x40|(addr<<1));
	
	PAC9535_IIC_Wait_Ack();
	
	if(portNum == 0)
	{
		PAC9535_IIC_Send_Byte(0x06);
	}
	else
	{
		PAC9535_IIC_Send_Byte(0x07);
	}
	PAC9535_IIC_Wait_Ack();
	
	PAC9535_IIC_Send_Byte((uint8_t)(data>>8)); 
	
	PAC9535_IIC_Wait_Ack();
	
	PAC9535_IIC_Send_Byte((uint8_t)data);
	
	PAC9535_IIC_Wait_Ack();
	
	PAC9535_IIC_Stop();	
}

//#include "InstrumentInclude.h"


#ifdef POCT_HMN_H5L5_A 

//初始化电机细分。
void motor_div_init(void)
{
	uint16_t tem = 0;
	
	pac9535_set_configuration_registers(0,0,0);
	pac9535_set_configuration_registers(0,0,1);
	
	pac9535_set_configuration_registers(4,0,0);
	pac9535_set_configuration_registers(4,0,1);
	
	tem = tem|(M08_DIV<<11)|(M03_DIV<<8)|(M01_DIV<<5)|(M02_DIV<<2);

	IO1_port = tem;
	pac9535_output(0,tem,0);
	delay_ms(50);
	pac9535_output(0,tem,1);
	
	tem = 0;
	tem = tem|(M09_DIV<<8)|(M04_DIV<<5)|(M07_DIV<<2);

	IO2_port = tem;
	pac9535_output(4,tem,0);
	delay_ms(50);
	pac9535_output(4,tem,1);	
}

#elif defined POCT_ANM_H3L5_A

//初始化电机细分。
void motor_div_init(void)
{
	uint16_t tem = 0;
	
	pac9535_set_configuration_registers(0,0,0);
	pac9535_set_configuration_registers(0,0,1);
	
	pac9535_set_configuration_registers(4,0,0);
	pac9535_set_configuration_registers(4,0,1);
	
	tem = tem|(M07_DIV<<12)|(M12_DIV<<9)|(M09_DIV<<6)|(M05_DIV<<3)|(M04_DIV<<0);

	IO1_port = tem;
	pac9535_output(0,tem,0);
	delay_ms(50);
	pac9535_output(0,tem,1);
	
	tem = 0;
	tem = tem|(M06_DIV<<12)|(M08_DIV<<9)|(M03_DIV<<6)|(M01_DIV<<3)|(M02_DIV<<0);

	IO2_port = tem;
	pac9535_output(4,tem,0);
	delay_ms(50);
	pac9535_output(4,tem,1);	
}

#elif defined POCT_ANM_H1L4_A
void motor_div_init(void)
{
	uint16_t config_data = 0;
	
	pac9535_set_configuration_registers(0,0,0);
	
	pac9535_set_configuration_registers(0,0,1);

	config_data = config_data|(M08_DIV<<9)|(M07_DIV<<6)|(M01_DIV<<3)|(M02_DIV<<0);

	IO1_port = config_data;
	
	pac9535_output(0,config_data,0);
	
	delay_ms(50);
	
	pac9535_output(0,config_data,1);
}
#elif defined POCT_HMN_H2L8_A
void motor_div_init(void)
{
	uint16_t tem = 0;
	
	pac9535_set_configuration_registers(0,0,0);
	pac9535_set_configuration_registers(0,0,1);

	tem = tem|(M03_DIV<<12)|(M08_DIV<<9)|(M07_DIV<<6)|(M01_DIV<<3)|(M02_DIV<<0);

	IO1_port = tem;
	pac9535_output(0,tem,0);
	delay_ms(50);
	pac9535_output(0,tem,1);
	
}

#endif


/*设置电机单个细分：电机编号，细分：DIV_8，DIV_16，DIV_32*/
void set_motor_div(uint8_t pac9535_addr,uint16_t IOport, uint8_t div,uint8_t shift,uint8_t port)
{
	pac9535_output(pac9535_addr,IOport|(div<<shift),port);
}

/*IO为输入模式：*/
void pac9535_input(uint8_t addr,uint8_t data)
{
	
}

