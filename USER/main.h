/**
  ******************************************************************************
  * @file    Templates/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.2.2
  * @date    25-May-2015
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H

#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
// ... existing code ...

/* Includes ------------------------------------------------------------------*/
// 标准库头文件
#include <string.h>
#include <malloc.h>

// 第三方库头文件
#include "FreeRTOS.h"

// STM32 HAL库头文件
#include "stm32f4xx_hal.h"

// 项目自定义头文件
#include "timer.h"
#include "led.h"
#include "usart.h"
#include "bsp_TiMbase.h"
#include "w5500_conf.h"
#include "bsp_motor_conf.h"
#include "bsp_motor.h"
#include "InstrumentInclude.h"
#include "ADS8688_conf.h"
#include "timers.h"
#include "sys.h"
#include "delay.h"
#include "laser.h"
// #include "key.h"
#include "bsp_pwm_conf.h"
#include "bsp_adc.h"
#include "bsp_pid.h"
#include "utility.h"
#include "poct_process.h"
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "StartTask.h"
#include "pac9535.h"
#include "DIV_Control.h"
#include "mytimer.h"
#include "fuzzypid.h"
#include "UltraSound.h"

// ... existing code ...

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
