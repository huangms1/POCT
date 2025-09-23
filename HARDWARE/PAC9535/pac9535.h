#ifndef __PAC9535_H
#define __PAC9535_H
#include "sys.h"

#include "stm32f4xx.h"
#include "DIV_Control.h"

#define  IO_SDA_PORT   GPIOD
#define  IO_SDA_PIN    GPIO_PIN_2

#define  IO_SCL_PORT   GPIOD
#define  IO_SCL_PIN    GPIO_PIN_3

#define  IO_INT1_PORT   GPIOG
#define  IO_INT1_PIN    GPIO_PIN_11

#define  IO_INT2_PORT   GPIOG
#define  IO_INT2_PIN    GPIO_PIN_10
	
//IO方向设置
#define PAC9535_SDA_IN()  {GPIOD->MODER&=~(3<<(2*2));GPIOD->MODER|=0<<(2*2);}	
#define PAC9535_SDA_OUT() {GPIOD->MODER&=~(3<<(2*2));GPIOD->MODER|=1<<(2*2);}

//IO操作
#define PAC9535_IIC_SCL   PDout(3)                  //SCL
#define PAC9535_IIC_SDA   PDout(2)                  //SDA
#define PAC9535_READ_SDA  PDin(2)                   //输入SDA

//IIC所有操作函数
void PAC9535_IIC_Init(void);                        //初始化IIC的IO口				 
void PAC9535_IIC_Start(void);				        //发送IIC开始信号
void PAC9535_IIC_Stop(void);	  			        //发送IIC停止信号

uint8_t PAC9535_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void PAC9535_IIC_Ack(void);					        //IIC发送ACK信号
void PAC9535_IIC_NAck(void);				        //IIC不发送ACK信号

void PAC9535_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t PAC9535_IIC_Read_Byte(unsigned char ack);   //IIC读取一个字节


//void PAC9535_IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
//uint8_t PAC9535_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 
#endif

