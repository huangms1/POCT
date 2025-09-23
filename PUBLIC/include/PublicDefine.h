#ifndef __PUBLIC_DEFINE_H__
#define __PUBLIC_DEFINE_H__

#include "includes.h"


typedef 	uint8_t 		uchar;

typedef 	uint16_t 		ushort;

typedef 	uint32_t 		uint;


typedef void * (*NodeDataMemGetFunPtr)(void);

typedef void (*NodeDataMemPutFunPtr)(void *);

typedef void (*SendFunPtr)(uchar * pData, ushort usSize);

typedef void (*RecvFunPtr)(uchar * pData, ushort * usSize);

struct SMsg;

typedef bool (*IsMsg4MeFunPtr)(struct SMsg * pMsg);

typedef bool (*IsMsgUrgent2MeFunPtr)(struct SMsg * pMsg);

typedef void (*UrgentExeFunPtr)(struct SMsg * pMsg);

typedef void (*NormalExeFunPtr)(struct SMsg * pMsg);

typedef void (*MsgExeFunPtr)(struct SMsg * pMsg);

typedef void (*InitUsartsFunPtr)(void);

typedef void (*CreateTasksFunPtr)(void);

typedef void (*MsgRouterFunPtr)(struct SMsg * pMsg);

typedef void (*RoutineFunPtr)(void);

typedef void (*RingDataDeleteFunPtr)(void * pData);

typedef bool (*RingIsSameDataFunPtr)(void * pData, void * pData1);

typedef void (*DeleteDataFunPtr)(void * pData);

#define ENDPOINT_NAME_LEN 								32

#define COMM_SERIAL_1_TASK_PRIO    						31 

#define COMM_SERIAL_2_TASK_PRIO    						32

#define COMM_SERIAL_3_TASK_PRIO    						33

#define COMM_SERIAL_4_TASK_PRIO    						34


#define MAIN_SERIAL_UP_TASK_PRIO    					31

#define MAIN_SERIAL_DOWN_TASK_PRIO    				    32


#define TEMP_SERIAL_TASK_PRIO    					    32

//#define URGENT_EXE_TASK_PRIO       				    40 

#define COMMUNICATION_BOARD_ADDR				        0

#define MAIN_1_BOARD_ADDR							    1

#define MAIN_BOARD_TRANS_FRAME_NO_BELONG 			    2

#define MAX_RECEIVE_SIZE 	                           1024*2

#define MAX_MSG_SIZE	 		                       1024

#define FRAME_MAX_RECV_MS                              10000




#endif //__PUBLIC_DEFINE_H__
