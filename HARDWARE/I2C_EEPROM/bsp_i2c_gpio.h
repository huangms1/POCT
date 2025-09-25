#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

#include <inttypes.h>


//IO��������
#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PH5����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PH5���ģʽ

/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define GPIO_PORT_I2C	    GPIOB			               /* GPIO�˿� */
//#define RCC_I2C_PORT 	    RCC_AHB1Periph_GPIOB		 /* GPIO�˿�ʱ�� */
#define   RCC_I2C_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ�� 
#define I2C_SCL_PIN		    GPIO_PIN_6			         /* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_PIN		    GPIO_PIN_7			         /* ���ӵ�SDA�����ߵ�GPIO */


/* �����дSCL��SDA�ĺ꣬�����Ӵ���Ŀ���ֲ�ԺͿ��Ķ��� */
#if 1	/* �������룺 1 ѡ��GPIO�Ŀ⺯��ʵ��IO��д */
	#define I2C_SCL_1()     HAL_GPIO_WritePin(GPIO_PORT_I2C,I2C_SCL_PIN,GPIO_PIN_SET)	
	#define I2C_SCL_0()     HAL_GPIO_WritePin(GPIO_PORT_I2C,I2C_SCL_PIN,GPIO_PIN_RESET)
	
	#define I2C_SDA_1()     HAL_GPIO_WritePin(GPIO_PORT_I2C,I2C_SDA_PIN,GPIO_PIN_SET)			/* SDA = 1 */
	#define I2C_SDA_0()     HAL_GPIO_WritePin(GPIO_PORT_I2C,I2C_SDA_PIN,GPIO_PIN_RESET)		    /* SDA = 0 */
	/* ��SDA����״̬ */
	#define I2C_SDA_READ()  HAL_GPIO_ReadPin(GPIO_PORT_I2C,I2C_SDA_PIN)
	//GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SDA_PIN)	
	
#else	/* �����֧ѡ��ֱ�ӼĴ�������ʵ��IO��д */

    /*��ע�⣺����д������IAR��߼����Ż�ʱ���ᱻ�����������Ż� */
	#define I2C_SCL_1()     GPIO_PORT_I2C->BSRR = I2C_SCL_PIN				          /* SCL = 1 */
	#define I2C_SCL_0()     GPIO_PORT_I2C->BRR = I2C_SCL_PIN				          /* SCL = 0 */
	
	#define I2C_SDA_1()     GPIO_PORT_I2C->BSRR = I2C_SDA_PIN				          /* SDA = 1 */
	#define I2C_SDA_0()     GPIO_PORT_I2C->BRR = I2C_SDA_PIN				          /* SDA = 0 */
	
	#define I2C_SDA_READ()  ((GPIO_PORT_I2C->IDR & I2C_SDA_PIN) != 0)	        /* ��SDA����״̬ */
  
#endif

#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */

void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);
void i2c_CfgGpio(void);
#endif
