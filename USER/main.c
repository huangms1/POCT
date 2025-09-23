#include "main.h"

TimerHandle_t TimerHandle = NULL;

#if IS_BOOTLOADER_PROGRAM

#include "iap.h"
int main(void)
{		
	HAL_Init();                     //初始化HAL库
	
	HAL_NVIC_SetPriorityGrouping(0);

	Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	
    delay_init(180);                //初始化延时函数	
	
	LED_Init();                     //初始化LED 
	
	PAC9535_IIC_Init();             //初始化PAC9535
	
	motor_div_init();

	i2c_CheckDevice(0xA0);
	
	ee_ReadBytes(&Globle.BootLoader.bootLoaderFlag,EEPROM_BOOTLOAD_FLAG_ADDR,sizeof(BootLoaderTypeDef)); //读取EEPROM 参数
	
	if(Globle.BootLoader.bootLoaderFlag == 0)
	{
		iap_load_app(0x08010000);
	}
	
	Motor_Init();                   //电机初始化
		
	ALL_LED_OFF();                  //关闭LED
			
	//创建开始任务
    xTaskCreate((TaskFunction_t )bootloader_start_task,            //任务函数
                (const char*    )"bootloader_start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,                   //任务堆栈大小
                (void*          )NULL,                             //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,                  //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);              //任务句柄       
				
    vTaskStartScheduler();          //开启任务调度
}

#else 

/**
 * @brief 初始化HAL库和时钟
 */
static void init_core_system(void)
{
    HAL_Init();
    HAL_NVIC_SetPriorityGrouping(0);
    Stm32_Clock_Init(360, 25, 2, 8);
}

/**
 * @brief 初始化基础外设
 */
static void init_basic_peripherals(void)
{
    TEMP_TIM_PWM_Init();
    delay_init(180);
    PAC9535_IIC_Init();
    motor_div_init();
    
    // USART_Config();
}

/**
 * @brief 初始化ADC通道
 */
static void init_adc_channels(void)
{
//	int i=0;
//	const uint8_t channels[] = {CHIR_0, CHIR_1, CHIR_2};	
    ADS8688_GPIO_Init();
	MX_SPI4_Init();
    ADS8688_Init(CH0_EN | CH1_EN | CH2_EN);	
	Set_CH_Range(CHIR_0, ADS8688_IR_1_25V);	
	Set_CH_Range(CHIR_1, ADS8688_IR_1_25V);
	Set_CH_Range(CHIR_2, ADS8688_IR_1_25V);
}

void hardware_init(void)
{
    init_core_system();        // 核心系统初始化
    init_basic_peripherals();  // 基础外设初始化
    
    LED_Init();
    LASER_Init();
    PID_Init();
    Motor_Init();
    UltraSound_Init();         //超声初始化
    
    init_adc_channels();       // ADC通道配置
    
    ALL_LED_OFF();
	
    delay_ms(50);              // 确保外设稳定
}

/**
 * @brief 初始化 EEPROM 数据
 */
void eeprom_init(void) {
    ee_ReadBytes(&Globle.POCT_Par.rs1, EEPROM_PARA_ADDR, sizeof(POCT_EEPROMTypedef));
    Globle.BootLoader.bootLoaderFlag = 0;
    ee_WriteBytes(&Globle.BootLoader.bootLoaderFlag, EEPROM_BOOTLOAD_FLAG_ADDR, sizeof(BootLoaderTypeDef));
}

/**
 * @brief 创建任务和定时器
 */
void task_and_timer_init(void) {
    // 创建开始任务
    xTaskCreate((TaskFunction_t)start_task,
                (const char*)"start_task",
                (uint16_t)START_STK_SIZE,
                (void*)NULL,
                (UBaseType_t)START_TASK_PRIO,
                (TaskHandle_t*)&StartTask_Handler);

    // 创建软件定时器 周期为1秒
    TimerHandle = xTimerCreate("Timer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, vTimerCallback);
	
	// 开启任务调度
    vTaskStartScheduler();
}

int main(void) {

	SCB->VTOR = FLASH_BASE | 0x10000;
    // 初始化硬件设备
    hardware_init();
    // 初始化 EEPROM 数据
	
    eeprom_init();
    // 创建任务和定时器
    task_and_timer_init();
    // 若调度器启动失败，进入无限循环
    while (1) {
        // 可以添加错误处理代码
    }
}

#endif






