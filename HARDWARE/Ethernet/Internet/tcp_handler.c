/**
************************************************************************************************
* @file   		tcp_demo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		TCP ��ʾ����
* @attention  
************************************************************************************************
**/

#include <stdio.h>
#include <string.h>
#include "tcp_handler.h"
#include "W5500_conf.h"
#include "w5500.h"
#include "socket.h"
#include "bsp_adc.h"
#include "utility.h"
#include "poct_process.h"
#include "usart.h"
//			                              	         /*����һ��2KB�Ļ���*/

///**
//*@brief		TCP Server�ػ���ʾ������
//*@param		��
//*@return	��
//*/
//void do_tcp_server(void)
//{	
//	char sta;
//	uint16 len=0;  
//	sta = (char)getSn_SR(SOCK_TCPS);
//	switch(getSn_SR(SOCK_TCPS))											            	  /*��ȡsocket��״̬*/
//	{
//		case SOCK_CLOSED:													              /*socket���ڹر�״̬*/
//			socket(SOCK_TCPS ,Sn_MR_TCP,local_port,Sn_MR_ND);	                         /*��socket*/
//		break;     
//    
//		case SOCK_INIT:														             /*socket�ѳ�ʼ��״̬*/
//			listen(SOCK_TCPS);												             /*socket��������*/
//		break;
//		
//		case SOCK_ESTABLISHED:												            /*socket�������ӽ���״̬*/
//		
//			if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
//			{
//				setSn_IR(SOCK_TCPS, Sn_IR_CON);								            /*��������жϱ�־λ*/
//			}
//			len = getSn_RX_RSR(SOCK_TCPS);									            /*����lenΪ�ѽ������ݵĳ���*/
//			if(len>0)
//			{
//				recv(SOCK_TCPS,Recv_Buff,len);								              	/*��������Client������*/
////				Recv_Buff[len] = 0x00; 											            /*����ַ���������*/
////				printf("%s\r\n",Recv_Buff);
//				/* USART1 �� DMA����TX���� */
////				HAL_UART_Transmit_DMA(&Uart3Handle, (uint8_t *)Recv_Buff ,len);
//				send(SOCK_TCPS,Recv_Buff,len);									            /*��Client��������*/
//		  }
//		break;
//		
//		case SOCK_CLOSE_WAIT:												             /*socket���ڵȴ��ر�״̬*/
//			close(SOCK_TCPS);
//		 break;
//		
//	}
//}

///**
//*@brief		TCP Client�ػ���ʾ������
//*@param		��
//*@return	��
//*/
//void do_tcp_client(void)
//{	
//	uint16 len=0;
//	static uint16_t time;
//	int i = 0;
//	uint8_t Rx_DMA[512] = {0}; 

////	char temp;
////	u8 *Tempbuf1;
////	
////	Rx_DMA = pvPortMalloc(1024);
//	switch(getSn_SR(SOCK_TCPC))								  				         /*��ȡsocket��״̬*/
//	{
//		case SOCK_CLOSED:											        		         /*socket���ڹر�״̬*/
//			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
//		  break;
//		
//		case SOCK_INIT:													        	         /*socket���ڳ�ʼ��״̬*/
//			connect(SOCK_TCPC,remote_ip,remote_port);                                       /*socket���ӷ�����*/ 
//		  break;
//		
//		case SOCK_ESTABLISHED: 												             /*socket�������ӽ���״̬*/
//			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
//			{
//				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*��������жϱ�־λ*/
//			}
//			len=getSn_RX_RSR(SOCK_TCPC); 								  	         /*����lenΪ�ѽ������ݵĳ���*/
//			if(len>0)
//			{
//				recv(SOCK_TCPC,Recv_Buff,len); 							   		     /*��������Server������*/
//				memset(Rx_DMA,0,len);/* ���� */
//				memcpy(Rx_DMA,Recv_Buff,len);				
//				protocol_analysis(len);	
//				HAL_UART_Transmit_DMA(&Uart1_Handle, (uint8_t *)Rx_DMA ,len);				
//				Globle.PC_State.IDLE_Flag = 1;
//			}		  
//		  break;
//			
//		case SOCK_CLOSE_WAIT: 											    	         /*socket���ڵȴ��ر�״̬*/
//			close(SOCK_TCPC);
//		 break;
//						
//	}
//}
void ActIfNetStateChanged(void)
{
	switch(getSn_SR(SOCK_TCPC))								  				              /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:											        		      /*socket���ڹر�״̬*/
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  break;
		
		case SOCK_INIT:													        	      /*socket���ڳ�ʼ��״̬*/
			connect(SOCK_TCPC,remote_ip,remote_port);                 				      /*socket���ӷ�����*/ 
		  break;
		
		case SOCK_ESTABLISHED: 												              /*socket�������ӽ���״̬*/
		  break;
			
		case SOCK_CLOSE_WAIT: 											    	         /*socket���ڵȴ��ر�״̬*/
			close(SOCK_TCPC);
		 break;						
	}
}


bool IsNetConnected(void)
{
		return SOCK_ESTABLISHED == getSn_SR(SOCK_TCPC);
}

void NetRecv(uchar * pData, ushort * usSize)
{
	* usSize = 0;
	
	if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
	{
		setSn_IR(SOCK_TCPC, Sn_IR_CON); 						/*��������жϱ�־λ*/
	}
	* usSize = getSn_RX_RSR(SOCK_TCPC);							/*����lenΪ�ѽ������ݵĳ���*/
	
	if(* usSize>0)
	{
		recv(SOCK_TCPC,pData,* usSize); 						/*��������Server������*/
	}	
}

void NetSend(uchar * pData, ushort usSize)
{
	send(SOCK_TCPC, pData, usSize); 
}
