#ifndef __TCP_DEMO_H
#define __TCP_DEMO_H
#include <stdbool.h>
#include "types.h"
#include "stm32F4xx.h"
#include "PublicDefine.h"

extern uint16 W5500_tcp_server_port;

//void do_tcp_server(void);//TCP Server回环演示函数

//void do_tcp_client(void);//TCP Clinet回环演示函数

void ActIfNetStateChanged(void);

bool IsNetConnected(void);

void NetRecv(uchar * pData, ushort * usSize);

void NetSend(uchar * pData, ushort usSize);

#endif 

