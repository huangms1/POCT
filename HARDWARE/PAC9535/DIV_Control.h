#ifndef __DIV_CONTROL_H
#define __DIV_CONTROL_H

//#include "delay.h"
#include "sys.h"
//#include "stm32f4xx.h"
#include "InstrumentInclude.h"

#ifdef POCT_HMN_H5L5_A
#define DIV_8       0x06
#define DIV_16      0x01
#define DIV_32      0x05

#define DIV1_8      0x03
#define DIV1_16     0x04
#define DIV1_32     0x05

#define M01_DIV     DIV_16
#define M02_DIV     DIV_16
#define M03_DIV     DIV1_16
#define M04_DIV     DIV_16
#define M05_DIV     DIV_16
#define M06_DIV     DIV_16
#define M07_DIV     DIV_16
#define M08_DIV     DIV1_16
#define M09_DIV     DIV1_16
#define M12_DIV     DIV_16

#elif defined POCT_ANM_H3L5_A
#define DIV_8      0x03
#define DIV_16     0x04   //
#define DIV_32     0x05

#define M01_DIV    DIV_16
#define M02_DIV    DIV_16
#define M03_DIV    DIV_16
#define M04_DIV    DIV_16
#define M05_DIV    DIV_16
#define M06_DIV    DIV_16
#define M07_DIV    DIV_16
#define M08_DIV    DIV_8
#define M09_DIV    DIV_16
#define M12_DIV    DIV_16

#elif defined POCT_ANM_H1L4_A
#define DIV_8      0x03
#define DIV_16     0x04   //
#define DIV_32     0x05

#define M01_DIV    DIV_16
#define M02_DIV    DIV_16
#define M07_DIV    DIV_16
#define M08_DIV    DIV_16

#elif defined POCT_HMN_H2L8_A
#define DIV_8      0x03
#define DIV_16     0x04   //
#define DIV_32     0x05

#define M01_DIV    DIV_16
#define M02_DIV    DIV_16
#define M07_DIV    DIV_16
#define M08_DIV    DIV_16
#define M03_DIV    DIV_16

#endif

extern uint16_t IO1_port;

extern uint16_t IO2_port;

void pac9535_output(uint8_t addr ,uint16_t data,uint8_t port);

void pac9535_set_configuration_registers(uint8_t addr,uint16_t data,uint8_t portNum);

void motor_div_init(void);

void set_motor_div(uint8_t motor_num, uint8_t div);

#endif


