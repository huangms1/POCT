#ifndef __REPLY_H__
#define __REPLY_H__
#include <stdio.h>
#include <stdbool.h>
#include "Msg.h"

struct SEndpoint;

void AssembleAndSend(SMsg * pMsg);

SMsg * CreateTransAck(SMsg * pJustRecvMsg);

SMsg * CreateFrameD(SMsg * pJustRecvMsg, char * pSubPara);

void CreateFrameR(SMsg * pMsg,char * pSubPara);

void CreateFrameMainParaR(SMsg * pMsg,char * pMainPara,char * pSubPara);

SMsg *  CreateFrameI(char * pAppSender,	char * pAppReceiver,\
										 char * pAppMainCmd,	char * pAppSubCmd,\
										 char * pAppMainPara,	char * pAppSubPara,\
										 struct SEndpoint * pEndpoint);

SMsg *  CreateFrameC(uint uiReceiverAddr, char * pAppSender,	char * pAppReceiver,\
										 char * pAppMainCmd,	char * pAppSubCmd,\
										 char * pAppMainPara,	char * pAppSubPara,\
										 struct SEndpoint * pEndpoint);

void SendTransAck(SMsg * pJustRecvMsg);

void SendFrameR(SMsg * pJustRecvMsg,char * pSubPara);
										 
void SendFrameMainParaR(SMsg * pJustRecvMsg,char * pMainPara,char * pSubPara);

void SendFrameD(SMsg * pJustRecvMsg,char * pSubPara);

void SendFrameI(char * pAppSender,	char * pAppReceiver,\
								char * pAppMainCmd,	char * pAppSubCmd,\
								char * pAppMainPara,	char * pAppSubPara,\
								struct SEndpoint * pEndpoint);
								
void SendFrameC(uint uiReceiverAddr, char * pAppSender,	char * pAppReceiver,\
								char * pAppMainCmd,	char * pAppSubCmd,\
								char * pAppMainPara,	char * pAppSubPara,\
								struct SEndpoint * pEndpoint);
								
void Int2Str(uint uiNumber,char * str);

#endif //__REPLY_H__
