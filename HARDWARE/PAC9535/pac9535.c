#include "pac9535.h"
#include "delay.h"

//IIC初始化
void PAC9535_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOD_CLK_ENABLE();             //使能GPIOD时钟
	__HAL_RCC_GPIOG_CLK_ENABLE();             //使能GPIOG时钟
    
    GPIO_Initure.Pin  = IO_SDA_PIN|IO_SCL_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed= GPIO_SPEED_FAST;      //快速
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
	
	GPIO_Initure.Pin=IO_INT2_PIN|IO_INT1_PIN;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;    //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;       //快速
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
	
    PAC9535_IIC_SDA = 1;	
    PAC9535_IIC_SCL = 1;  
}

//产生IIC起始信号
void PAC9535_IIC_Start(void)
{
	PAC9535_SDA_OUT();     //sda线输出
	
	PAC9535_IIC_SDA = 1;	
	
	PAC9535_IIC_SCL = 1;
	
	delay_us(4);
 	PAC9535_IIC_SDA = 0;    //START:when CLK is high,DATA change form high to low 
	
	delay_us(4);
	PAC9535_IIC_SCL = 0; 
}	  

//产生IIC停止信号
void PAC9535_IIC_Stop(void)
{
	PAC9535_SDA_OUT();//sda线输出
	
	PAC9535_IIC_SCL = 0;
	
	PAC9535_IIC_SDA = 0;//STOP:when CLK is high DATA change form low to high
	
 	delay_us(4);
	PAC9535_IIC_SCL = 1; 
	
	PAC9535_IIC_SDA = 1;//发送I2C总线结束信号
	delay_us(4);							   	
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t PAC9535_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	
	PAC9535_SDA_IN();      //SDA设置为输入  
	
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
	PAC9535_IIC_SCL = 0;//时钟输出0 	   
	return 0;  
} 

//产生ACK应答
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

//不产生ACK应答		    
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

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void PAC9535_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;  
	
	PAC9535_SDA_OUT();
	
    PAC9535_IIC_SCL=0;  //拉低时钟开始数据传输
	
    for(t = 0; t < 8; t++)
    {              
        PAC9535_IIC_SDA=(txd&0x80) >> 7;
		
        txd <<= 1; 	
		
		delay_us(2);    //对TEA5767这三个延时都是必须的
		PAC9535_IIC_SCL = 1;	
		delay_us(2); 		
		PAC9535_IIC_SCL = 0;	
		delay_us(2);
    }	
} 	    

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t PAC9535_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	
	PAC9535_SDA_IN();       //SDA设置为输入
	
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
        PAC9535_IIC_NAck(); //发送nACK
    else
        PAC9535_IIC_Ack();  //发送ACK   
    return receive;
}


