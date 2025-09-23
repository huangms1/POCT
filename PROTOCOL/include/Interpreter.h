#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include <stdio.h>
#include <stdbool.h>
#include "ZQueue.h"
#include "ZList.h"
#include "Msg.h"

bool Interprete(SMsg * pMsg);

bool InterpreteHeader(SMsg * pMsg);

bool InterpreteAppMsg(SMsg * pMsg);

#endif //__INTERPRETER_H__

