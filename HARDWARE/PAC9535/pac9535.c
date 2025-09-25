#include "pac9535.h"
#include "delay.h"

//IIC��ʼ��
void PAC9535_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOD_CLK_ENABLE();             //ʹ��GPIODʱ��
	__HAL_RCC_GPIOG_CLK_ENABLE();             //ʹ��GPIOGʱ��
    
    GPIO_Initure.Pin  = IO_SDA_PIN|IO_SCL_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull = GPIO_PULLUP;          //����
    GPIO_Initure.Speed= GPIO_SPEED_FAST;      //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
	
	GPIO_Initure.Pin=IO_INT2_PIN|IO_INT1_PIN;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;    //�������
    GPIO_Initure.Pull=GPIO_PULLUP;            //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;       //����
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
	
    PAC9535_IIC_SDA = 1;	
    PAC9535_IIC_SCL = 1;  
}

//����IIC��ʼ�ź�
void PAC9535_IIC_Start(void)
{
	PAC9535_SDA_OUT();     //sda�����
	
	PAC9535_IIC_SDA = 1;	
	
	PAC9535_IIC_SCL = 1;
	
	delay_us(4);
 	PAC9535_IIC_SDA = 0;    //START:when CLK is high,DATA change form high to low 
	
	delay_us(4);
	PAC9535_IIC_SCL = 0; 
}	  

//����IICֹͣ�ź�
void PAC9535_IIC_Stop(void)
{
	PAC9535_SDA_OUT();//sda�����
	
	PAC9535_IIC_SCL = 0;
	
	PAC9535_IIC_SDA = 0;//STOP:when CLK is high DATA change form low to high
	
 	delay_us(4);
	PAC9535_IIC_SCL = 1; 
	
	PAC9535_IIC_SDA = 1;//����I2C���߽����ź�
	delay_us(4);							   	
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t PAC9535_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	
	PAC9535_SDA_IN();      //SDA����Ϊ����  
	
	PAC9535_IIC_SDA = 1;delay_us(1);
	
	PAC9535_IIC_SCL = 1;delay_us(1);	 
	while(PAC9535_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			PAC9535_IIC_Stop();
			return 1;
		}
	}
	PAC9535_IIC_SCL = 0;//ʱ�����0 	   
	return 0;  
} 

//����ACKӦ��
void PAC9535_IIC_Ack(void)
{
	PAC9535_IIC_SCL = 0;
	
	PAC9535_SDA_OUT();
	
	PAC9535_IIC_SDA = 0;
	
	delay_us(2);
	PAC9535_IIC_SCL = 1;
	
	delay_us(2);
	PAC9535_IIC_SCL = 0;
}

//������ACKӦ��		    
void PAC9535_IIC_NAck(void)
{
	PAC9535_IIC_SCL = 0;
	
	PAC9535_SDA_OUT();
	
	PAC9535_IIC_SDA = 1;
	
	delay_us(2);
	PAC9535_IIC_SCL = 1;
	
	delay_us(2);
	PAC9535_IIC_SCL = 0;
}		

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void PAC9535_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;  
	
	PAC9535_SDA_OUT();
	
    PAC9535_IIC_SCL=0;  //����ʱ�ӿ�ʼ���ݴ���
	
    for(t = 0; t < 8; t++)
    {              
        PAC9535_IIC_SDA=(txd&0x80) >> 7;
		
        txd <<= 1; 	
		
		delay_us(2);    //��TEA5767��������ʱ���Ǳ����
		PAC9535_IIC_SCL = 1;	
		delay_us(2); 		
		PAC9535_IIC_SCL = 0;	
		delay_us(2);
    }	
} 	    

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t PAC9535_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	
	PAC9535_SDA_IN();       //SDA����Ϊ����
	
    for(i = 0; i < 8; i++)
	{
        PAC9535_IIC_SCL = 0; 
		
        delay_us(2);
		
		PAC9535_IIC_SCL = 1;
		
        receive <<= 1;
		
        if(PAC9535_READ_SDA)receive++;   
		
		delay_us(1); 
    }					 
    if (!ack)
        PAC9535_IIC_NAck(); //����nACK
    else
        PAC9535_IIC_Ack();  //����ACK   
    return receive;
}


