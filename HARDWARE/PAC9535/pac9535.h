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
	
//IO��������
#define PAC9535_SDA_IN()  {GPIOD->MODER&=~(3<<(2*2));GPIOD->MODER|=0<<(2*2);}	
#define PAC9535_SDA_OUT() {GPIOD->MODER&=~(3<<(2*2));GPIOD->MODER|=1<<(2*2);}

//IO����
#define PAC9535_IIC_SCL   PDout(3)                  //SCL
#define PAC9535_IIC_SDA   PDout(2)                  //SDA
#define PAC9535_READ_SDA  PDin(2)                   //����SDA

//IIC���в�������
void PAC9535_IIC_Init(void);                        //��ʼ��IIC��IO��				 
void PAC9535_IIC_Start(void);				        //����IIC��ʼ�ź�
void PAC9535_IIC_Stop(void);	  			        //����IICֹͣ�ź�

uint8_t PAC9535_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void PAC9535_IIC_Ack(void);					        //IIC����ACK�ź�
void PAC9535_IIC_NAck(void);				        //IIC������ACK�ź�

void PAC9535_IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t PAC9535_IIC_Read_Byte(unsigned char ack);   //IIC��ȡһ���ֽ�


//void PAC9535_IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
//uint8_t PAC9535_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 
#endif

