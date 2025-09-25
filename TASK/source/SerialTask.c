//#include "SerialTask.h"
//#include "delay.h"
//#include "Endpoint.h"

//OS_STK CommSerial1TaskStack[SERIAL_STK_SIZE];

//OS_STK CommSerial2TaskStack[SERIAL_STK_SIZE];

//OS_STK CommSerial3TaskStack[SERIAL_STK_SIZE];

//OS_STK CommSerial4TaskStack[SERIAL_STK_SIZE];

//OS_STK MainSerialUpTaskStack[SERIAL_STK_SIZE];

//OS_STK MainSerialDownTaskStack[SERIAL_STK_SIZE];

//OS_STK TempSerialTaskStack[SERIAL_STK_SIZE];


//void SerialTask(void *pData)
//{
//	SEndpoint * pEndpoint = pData;
//	
//	while(1)
//	{
//		EndpointSpell(pEndpoint);
//		
//		EndpointSendOne(pEndpoint);
//		
//		delay_ms(5);
//	};
//}
