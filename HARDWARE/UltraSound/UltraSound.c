#include "UltraSound.h"
#include "delay.h"

/**************************************************************************
 * 函数名称：UltraSound_Init
 * 描述  ：初始化超声模块
 * 输入  ：无
 * 输出  ：无
 * 返回  ：无
 * 注意  ：
 * 作者  ：hms
 * 日期  ：2025-05-15
 * 版本  ：V1.0
 */

void UltraSound_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    // 启用GPIOB外设时钟（超声模块控制信号）
    ULTRA_SOUND_CLK_ENABLE();

    // 配置PB0引脚为推挽输出模式
    GPIO_Initure.Pin = ULTRA_SOUND_PIN;             // 超声控制信号引脚
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;        // 推挽输出驱动能力强
    GPIO_Initure.Pull = GPIO_PULLUP;                // 上拉电阻防止信号漂移
    HAL_GPIO_Init(ULTRA_SOUND_PORT, &GPIO_Initure);

    // 初始化默认为高电平（关闭超声发射）
    HAL_GPIO_WritePin(ULTRA_SOUND_PORT, GPIO_PIN_0, GPIO_PIN_SET);
}

/**
  * @brief  超声周期输出控制（使用HAL系统延迟）
  * @param  duration_ms: 总持续时间（单位：毫秒）
  * @param  pulse_width: 脉冲有效宽度（单位：毫秒）
  * @retval 无
  * @note   使用HAL_Delay实现阻塞式延时
  *         每个周期包含100ms间隔
  */
void UltraSound_PeriodicOutput(uint32_t duration_ms, uint16_t pulse_width)
{
	uint32_t i = 0;
	
    uint32_t cycles = duration_ms / 100;
    
    for(i = 0; i < cycles; i++) {
        // 生成脉冲
        HAL_GPIO_WritePin(ULTRA_SOUND_PORT, ULTRA_SOUND_PIN, GPIO_PIN_RESET);
        delay_ms(pulse_width);
        HAL_GPIO_WritePin(ULTRA_SOUND_PORT, ULTRA_SOUND_PIN, GPIO_PIN_SET);
        
        // 维持间隔
        if(pulse_width < 200) {
            delay_ms(200 - pulse_width);
        }
    }
}



