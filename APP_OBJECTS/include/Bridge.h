#ifndef __BRIDGE_H__
#define __BRIDGE_H__
#include <stdio.h>
#include <stdbool.h>
#include "Msg.h"

void ProcessMsg(SMsg * pMsg);


void CommBoardMsgRouter(SMsg * pMsg);

void MainBoardMsgRouter(SMsg * pMsg);

void TempBoardMsgRouter(SMsg * pMsg);

#endif //__BRIDGE_H__
