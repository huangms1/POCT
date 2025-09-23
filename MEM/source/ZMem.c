#include "ZMem.h"
#include "LogTask.h"

void * NewMemWithMark(size_t xWantedSize, char * pMark)
{
	void * pMem = pvPortMalloc(xWantedSize);
	
	if(NULL != pMem)
	{
		WRITE_FREE_LOG("%s-%s@%d\n",pMark,"New Mem Success",pMem);
		
		return pMem;
	}
	else
	{
		WRITE_FREE_LOG("%s-%s\n",pMark,"New Mem Failed");
		
		return NULL;
	}
}

void * NewMemWithMarkFunLine(size_t xWantedSize, char * pMark, char * pFun, char * pLine)
{
	void * pMem = pvPortMalloc(xWantedSize);
	
	if(NULL != pMem)
	{
		WRITE_FREE_LOG("%s-%s@%d %s:%s\n",pMark,"New Mem Success",pMem, pFun, pLine);
		
		return pMem;
	}
	else
	{
		WRITE_FREE_LOG("%s-%s %s:%s\n",pMark,"New Mem Failed", pFun, pLine);
		
		return NULL;
	}
}

void DeleteMemWithMark(void *pMem, char * pMark)
{
	WRITE_FREE_LOG("%s-%s@%d\n",pMark,"Delete Mem Success",pMem);
	
	vPortFree(pMem);
}

void DeleteMemWithMarkFunLine(void *pMem, char * pMark, char * pFun, char * pLine)
{
	WRITE_FREE_LOG("%s-%s@%d %s:%s\n",pMark,"Delete Mem Success",pMem, pFun, pLine);
	
	vPortFree(pMem);
}
