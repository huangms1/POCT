#ifndef __INSTRUMENT_INCLUDE_H 
#define __INSTRUMENT_INCLUDE_H

//仪器相关型号。
#define POCT_ANM_H1L4_A 

//bootloader 引导程序此处设置为      1 
#define IS_BOOTLOADER_PROGRAM        0

#if defined POCT_ANM_H1L4_A
	#include "POCT_ANM_H1L4_A.h"
	#define  INSTRUMENTS_VERSION   "@S_POCT_ANM_H1L4_A_V1.0.0.VT03.Beta.20230310_E@"
	
#elif defined POCT_ANM_H3L5_A
	#include "POCT_ANM_H3L5_A.h"
	#define  INSTRUMENTS_VERSION   "@S_POCT_ANM_H3L5_A_V1.0.0.VT03.Beta.20230320_E@"
#elif defined POCT_HMN_H5L5_A
	#include "POCT_HMN_H5L5_A.h"
	#define  INSTRUMENTS_VERSION   "@S_POCT_HMN_H5L5_A_V1.0.0.VT02.Beta.20221229_E@"
#endif

#endif//__INSTRUMENT_INCLUDE_H

