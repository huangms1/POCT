#ifndef __INSTRUMENT_INCLUDE_H 
#define __INSTRUMENT_INCLUDE_H

//仪器相关型号。
#define POCT_ANM_H1L4_A 

//bootloader 引导程序此处设置为        1 
#define IS_BOOTLOADER_PROGRAM          0

#if defined POCT_ANM_H1L4_A
	#include "POCT_ANM_H1L4_A.h"
	#define  INSTRUMENTS_VERSION   "@S_POCT_ANM_H1L8_A_V1.0.0.VT10.Beta.20250923_E@"
	
#elif defined POCT_HMN_H2L8_A
	#include "POCT_HMN_H2L8_A.h"
	#define  INSTRUMENTS_VERSION   "@S_POCT_HMN_H2L8_A_V1.0.0.VT01.Beta.20231009_E@"

#elif defined POCT_ANM_H3L5_A
	#include "POCT_ANM_H3L5_A.h"
	#define  INSTRUMENTS_VERSION   "@S_POCT_ANM_H3L5_A_V1.0.0.VT03.Beta.202300412_E@"
	
#elif defined POCT_HMN_H5L5_A
	#include "POCT_HMN_H5L5_A.h"
	#define  INSTRUMENTS_VERSION   "@S_POCT_HMN_H5L5_A_V1.0.0.VT03.Beta.20230417_E@"
	
#endif

#endif//__INSTRUMENT_INCLUDE_H

