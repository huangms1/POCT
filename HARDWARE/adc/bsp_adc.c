#include "bsp_adc.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "poct_process.h"
#include "ADS8688_conf.h"

//__IO uint16_t ADC_ConvertedValue[PT100_NOFCHANEL]={0};

//DMA_HandleTypeDef DMA_Init_Handle;

//ADC_HandleTypeDef ADC_Handle;

//ADC_ChannelConfTypeDef ADC_Config;


//static void PT100_ADC_GPIO_Config(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//   
//    PT100_ADC_GPIO_CLK1_ENABLE();    
//   
//    GPIO_InitStructure.Pin = PT100_ADC_GPIO_PIN1;
//    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
//    GPIO_InitStructure.Pull = GPIO_NOPULL ; 
//    HAL_GPIO_Init(PT100_ADC_GPIO_PORT1, &GPIO_InitStructure);
//    
//    PT100_ADC_GPIO_CLK2_ENABLE();    

//    GPIO_InitStructure.Pin = PT100_ADC_GPIO_PIN2;
//    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
//    GPIO_InitStructure.Pull = GPIO_NOPULL ; 
//    HAL_GPIO_Init(PT100_ADC_GPIO_PORT2, &GPIO_InitStructure);
//    
//    PT100_ADC_GPIO_CLK3_ENABLE();    
//  
//    GPIO_InitStructure.Pin = PT100_ADC_GPIO_PIN3;
//    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
//    GPIO_InitStructure.Pull = GPIO_NOPULL ; 
//    HAL_GPIO_Init(PT100_ADC_GPIO_PORT3, &GPIO_InitStructure);    
//	

//    PT100_ADC_GPIO_CLK4_ENABLE();    

//    GPIO_InitStructure.Pin = PT100_ADC_GPIO_PIN4;
//    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
//    GPIO_InitStructure.Pull = GPIO_NOPULL ; 
//    HAL_GPIO_Init(PT100_ADC_GPIO_PORT4, &GPIO_InitStructure); 
//	
//    PT100_ADC_GPIO_CLK5_ENABLE();    

//    GPIO_InitStructure.Pin = PT100_ADC_GPIO_PIN5;
//    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
//    GPIO_InitStructure.Pull = GPIO_NOPULL ; 
//    HAL_GPIO_Init(PT100_ADC_GPIO_PORT5, &GPIO_InitStructure); 
//}

//static void PT100_ADC_Mode_Config(void)
//{   
//    PT100_ADC_DMA_CLK_ENABLE();
//   
//    DMA_Init_Handle.Instance = PT100_ADC_DMA_STREAM;
//    
//    DMA_Init_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;	
//  
//    DMA_Init_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
//   
//    DMA_Init_Handle.Init.MemInc = DMA_MINC_ENABLE; 
//   
//    DMA_Init_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; 
//  
//    DMA_Init_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	
//   
//    DMA_Init_Handle.Init.Mode = DMA_CIRCULAR;
//   
//    DMA_Init_Handle.Init.Priority = DMA_PRIORITY_HIGH;
// 
//    DMA_Init_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;  
//    
//    DMA_Init_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
//	
//    DMA_Init_Handle.Init.MemBurst = DMA_MBURST_SINGLE;
//	
//    DMA_Init_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;  
//   
//    DMA_Init_Handle.Init.Channel = PT100_ADC_DMA_CHANNEL; 
//   
//    HAL_DMA_Init(&DMA_Init_Handle); 

//    HAL_DMA_Start (&DMA_Init_Handle,PT100_ADC_DR_ADDR,(uint32_t)&ADC_ConvertedValue,PT100_NOFCHANEL);

//    PT100_ADC_CLK_ENABLE();
//    
//    ADC_Handle.Instance = PT100_ADC;
//    
//    ADC_Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
//    
//    ADC_Handle.Init.Resolution = ADC_RESOLUTION_12B;
//   
//    ADC_Handle.Init.ScanConvMode = ENABLE; 
//   
//    ADC_Handle.Init.ContinuousConvMode = ENABLE;
//   
//    ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
//   
//    ADC_Handle.Init.NbrOfDiscConversion   = 0;
//   
//    ADC_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
//  
//    ADC_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//   
//    ADC_Handle.Init.NbrOfConversion = PT100_NOFCHANEL;
//   
//    ADC_Handle.Init.DMAContinuousRequests = ENABLE;
//   
//    ADC_Handle.Init.EOCSelection          = DISABLE;    
//            
//    HAL_ADC_Init(&ADC_Handle);
//   
//    ADC_Config.Channel      = PT100_ADC_CHANNEL1;
//    ADC_Config.Rank         = 1;    
//    ADC_Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;
//    ADC_Config.Offset       = 0;
//    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
//    
//    ADC_Config.Channel      = PT100_ADC_CHANNEL2;
//    ADC_Config.Rank         = 2;
//    ADC_Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;
//    ADC_Config.Offset       = 0;
//    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
//    
//    ADC_Config.Channel      = PT100_ADC_CHANNEL3;
//    ADC_Config.Rank         = 3;    	
//    ADC_Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;
//    ADC_Config.Offset       = 0;
//    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
//	
//    ADC_Config.Channel      = PT100_ADC_CHANNEL4;
//    ADC_Config.Rank         = 4;    	
//    ADC_Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;
//    ADC_Config.Offset       = 0;
//    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
//	
//    ADC_Config.Channel      = PT100_ADC_CHANNEL5;
//    ADC_Config.Rank         = 5;    	
//    ADC_Config.SamplingTime = ADC_SAMPLETIME_3CYCLES;
//    ADC_Config.Offset       = 0;
//    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);

//    HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&ADC_ConvertedValue,5);
//}

/**************************************
*PT100初始化
*
*
*************************************/
void PT100_Init(void)
{
//	PT100_ADC_GPIO_Config();
//	
//	PT100_ADC_Mode_Config();
}

//交换两个元素
#define SWAP(a, b) do { int temp = (a); (a) = (b); (b) = temp; } while (0)

/**
 * @brief 交换两个整数的值
 * @param a 第一个整数的指针
 * @param b 第二个整数的指针
 */
// 原 swap 函数可以注释掉，用宏替换
// void swap(int *a, int *b)
// {
//     int temp = *a;
//     *a = *b;
//     *b = temp;
// }

/**
 * @brief 对数组进行分区操作，用于快速排序
 * @param arr 待分区的数组
 * @param low 分区的起始索引
 * @param high 分区的结束索引
 * @return 分区点的索引
 */
int partition(int arr[], int low, int high) {
    // 选择最后一个元素作为基准
    int pivot = arr[high];
    // i 用于记录小于基准的元素的位置
    int i = low - 1,j = 0;

    // 遍历数组，将小于基准的元素交换到左边
    for (j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            SWAP(arr[i], arr[j]);
        }
    }
    // 将基准元素放到正确的位置
    SWAP(arr[i + 1], arr[high]);
    return i + 1;
}

/**
 * @brief 对数组进行快速排序
 * @param arr 待排序的数组
 * @param low 排序的起始索引
 * @param high 排序的结束索引
 */
void quickSort(int arr[], int low, int high) {
	int pi = 0;
    // 检查索引的合法性
    if (low < 0 || high < 0 || low >= high) {
        return;
    }
    // 进行分区操作
    pi = partition(arr, low, high);
    // 检查索引的合法性
    if (low < 0 || high < 0 || low >= high) {
        return;
    }
    // 进行分区操作
    pi = partition(arr, low, high);
    // 递归排序基准左边的元素
    quickSort(arr, low, pi - 1);
    // 递归排序基准右边的元素
    quickSort(arr, pi + 1, high);
}

/**
 * @brief 获取指定通道的ADC平均值
 * @param ch 通道号 (0, 1, 2, 7)
 * @param len 采样次数
 * @return ADC平均值
 */
uint16_t GetAverageADCValue(uint8_t ch, uint8_t len) {
    // 参数验证
    if (len == 0) {
        return 0;  // 避免除以零
    }

    // 检查通道是否有效
    if (ch != 0 && ch != 1 && ch != 2 && ch != 7) {
        return 0;  // 无效通道返回0
    }

    uint32_t sum = 0;  // 采样总和
    uint16_t averageValue = 0;  // 平均值

    // 采样len次
    for (int i = 0; i < len; i++) {
        // 根据通道选择对应的模式
        switch (ch) {
            case 0:
                MAN_CH_Mode(MAN_CH_0);
                break;
            case 1:
                MAN_CH_Mode(MAN_CH_1);
                break;
            case 2:
                MAN_CH_Mode(MAN_CH_2);
                break;
            case 7:
                MAN_CH_Mode(MAN_CH_7);
                break;
            default:
                // 已经在参数验证中处理过无效通道
                break;
        }

        // 获取数据并累加到总和
        sum += Get_MAN_CH_Mode_Data();
    }

    // 计算平均值
    averageValue = sum / len;

    return averageValue;
}

///**
// * @brief 获取指定通道的中位数数值
// * @param ch 通道号
// * @param len 采样次数
// * @return 计算得到的中位数数值
// */
//uint16_t GetMedianNum(uint8_t ch, uint8_t len) {
//    int i;
//	
//	uint32_t  tmp = 0;

//	uint16_t  bTemp = 0;
//	
////	int arr[10] = {0};

//    for(i = 0;i<len;i++)
//	{	
//		if(ch == 0)
//		{
//			MAN_CH_Mode(MAN_CH_0);
//			tmp += Get_MAN_CH_Mode_Data();
////			arr[i] = Get_MAN_CH_Mode_Data();
//		}
//		else if(ch == 1)
//		{
//			MAN_CH_Mode(MAN_CH_1);
//			tmp += Get_MAN_CH_Mode_Data();
////			arr[i] = Get_MAN_CH_Mode_Data();
//		}
//		else if(ch == 2)
//		{
//			MAN_CH_Mode(MAN_CH_2);
//			tmp += Get_MAN_CH_Mode_Data();
////			arr[i] = Get_MAN_CH_Mode_Data();
//		}
//		else if (ch == 7)
//		{
//			MAN_CH_Mode(MAN_CH_7);
//			tmp += Get_MAN_CH_Mode_Data();
//		}
//	}
//	bTemp = tmp/len;
//	
////	quickSort(arr, 0, len-1);
////	
////	for(i = 1;i<len-1;i++)
////	{
////		sum += arr[i];
////	}	
////	bTemp = sum/8;
//	
//	return bTemp;
//}

int applyTemperatureCompensation(int temp, uint16_t positive, uint16_t negative, uint16_t limit) {
    if (positive > negative) {
        if (positive <= limit) {
            temp -= (int)(positive - negative);
        }
    } else {
        if (negative <= limit) {
            temp += (int)(negative - positive);
        }
    }
    return temp;
}

/**
 * @brief 获取指定通道的温度值
 * @param ch 通道号
 * @return 温度值
 **/
int GetTemp(uint8_t ch) {
	
	uint16_t adcValue = GetAverageADCValue(ch,10);	

	float voltage = (((float)adcValue / 65535.0f) * 3750.0f) / 11.0f + 526.316f;   
	
	float rpt100 = voltage * 470.0f * 1.0f / (3000.0f * 1.0f - voltage);       
	
	int temp = (int)(((rpt100 - 100) / 0.3844495f)*100);
	
	switch(ch)
	{
		case 0:
        {
			temp = applyTemperatureCompensation(temp, Globle.POCT_Par.T1_Compensate_Pesitive, Globle.POCT_Par.T1_Compensate_Negative, 5000);
            break;
        }
		case 1:
        {
			temp = applyTemperatureCompensation(temp, Globle.POCT_Par.T2_Compensate_Pesitive, Globle.POCT_Par.T2_Compensate_Negative, 2000);
            break;
        }
		case 2:
        {
			temp = applyTemperatureCompensation(temp, Globle.POCT_Par.T3_Compensate_Pesitive, Globle.POCT_Par.T3_Compensate_Negative, 2000);
            break;
        }
			
		default :break;
			
	}
	return temp;
}



