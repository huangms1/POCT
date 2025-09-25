#ifndef __ZMEM__H
#define __ZMEM__H

#include <stdlib.h>
#include "FreeRTOS.h"

void * NewMemWithMark(size_t xWantedSize, char * pMark);
void * NewMemWithMarkFunLine(size_t xWantedSize, char * pMark, char * pFun, char * pLine);
void DeleteMemWithMark(void *pMem, char * pMark);
void DeleteMemWithMarkFunLine(void *pMem, char * pMark, char * pFun, char * pLine);

#ifdef LOG_ENABLE

#define NEW_MEM_WITH_MARK(a,b) NewMemWithMark(a,b)
#define NEW_MEM_WITH_MARK_FUN_LINE(a,b) NewMemWithMarkFunLine(a,b,__FUNCTION__,__LINE__)
#define DELETE_MEM_WITH_MARK(a,b) DeleteMemWithMark(a,b)
#define DELETE_MEM_WITH_MARK_FUN_LINE(a,b) DeleteMemWithMarkFunLine(a,b,__FUNCTION__,__LINE__)
	
#else

#define NEW_MEM_WITH_MARK(a,b) pvPortMalloc(a)
#define NEW_MEM_WITH_MARK_FUN_LINE(a,b) pvPortMalloc(a)
#define DELETE_MEM_WITH_MARK(a,b) vPortFree(a)
#define DELETE_MEM_WITH_MARK_FUN_LINE(a,b) vPortFree(a)
#endif
#endif //__ZMEM__H
