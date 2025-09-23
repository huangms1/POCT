#include "bsp_adc.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "poct_process.h"
#include "ADS8688_conf.h"

__IO uint16_t ADC_ConvertedValue[PT100_NOFCHANEL]={0};

DMA_HandleTypeDef DMA_Init_Handle;

ADC_HandleTypeDef ADC_Handle;

ADC_ChannelConfTypeDef ADC_Config;

uint16_t GetMedianNum(uint8_t ch,uint8_t len);

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
void swap(int *a,int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

int partition(int arr[],int low,int high)
{
	int pivot = arr[high];
	
	int i = (low - 1),j = 0;
	
	for(j = low;j<=high-1;j++){
		if(arr[j]<pivot){
			i++;
			swap(&arr[i],&arr[j]);
		}
	}
	swap(&arr[i+1],&arr[high]);
	return(i+1);
}

// 快速排序函数
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

/**
*@brief		
*@param		len: 
*@return	
*/
uint16_t GetMedianNum(uint8_t ch,uint8_t len)
{
	int i;
	
	uint32_t  tmp = 0;

	uint16_t  bTemp = 0;
	
//	int arr[10] = {0};

    for(i = 0;i<len;i++)
	{	
		if(ch == 0)
		{
			MAN_CH_Mode(MAN_CH_0);
			tmp += Get_MAN_CH_Mode_Data();
//			arr[i] = Get_MAN_CH_Mode_Data();
		}
		else if(ch == 1)
		{
			MAN_CH_Mode(MAN_CH_1);
			tmp += Get_MAN_CH_Mode_Data();
//			arr[i] = Get_MAN_CH_Mode_Data();
		}
		else if(ch == 2)
		{
			MAN_CH_Mode(MAN_CH_2);
			tmp += Get_MAN_CH_Mode_Data();
//			arr[i] = Get_MAN_CH_Mode_Data();
		}
	}
	bTemp = tmp/len;
	
//	quickSort(arr, 0, len-1);
//	
//	for(i = 1;i<len-1;i++)
//	{
//		sum += arr[i];
//	}	
//	bTemp = sum/8;
	
	return bTemp;
}

/***********************************************
*@brief		获取温度
*@param		ch : ͨ通道号。
*@return	温度值
************************************************/
int GetTemp(uint8_t ch)
{	
	float upt100 = 0 ,rpt100 = 0;
	
	uint16_t bTemp = 0;

	int Temp = 0;
	
	bTemp = GetMedianNum(ch,10);	
	
	//温度计算  
	upt100 = (((float)bTemp / 65535) * 3750) / 11 + 526.316f;   
	
	rpt100 = upt100 * 470 * 1.0f / (3000 * 1.0f - upt100);       
	
	Temp = (int)(((rpt100 - 100) / 0.3844495f)*100);
	
	if(ch == 4)
		Temp = Temp;
	
	switch(ch)
	{
		case 0:
			
			if(Globle.POCT_Par.T1_Compensate_Pesitive > Globle.POCT_Par.T1_Compensate_Negative)
			{
				if(Globle.POCT_Par.T1_Compensate_Pesitive <= 2000)
					Temp = Temp - (int)(Globle.POCT_Par.T1_Compensate_Pesitive-Globle.POCT_Par.T1_Compensate_Negative);
			}
			else
			{
				if(Globle.POCT_Par.T1_Compensate_Negative <= 2000)
					Temp = Temp + (int)(Globle.POCT_Par.T1_Compensate_Negative - Globle.POCT_Par.T1_Compensate_Pesitive);	
			}
		break;
			
		case 1:

			if(Globle.POCT_Par.T2_Compensate_Pesitive > Globle.POCT_Par.T2_Compensate_Negative)
			{
				if(Globle.POCT_Par.T2_Compensate_Pesitive <= 2000)
					Temp = Temp - (int)(Globle.POCT_Par.T2_Compensate_Pesitive-Globle.POCT_Par.T2_Compensate_Negative);
			}
			else
			{
				if(Globle.POCT_Par.T2_Compensate_Negative <= 2000)
					Temp = Temp + (int)(Globle.POCT_Par.T2_Compensate_Negative + Globle.POCT_Par.T2_Compensate_Pesitive);		
			}
			
		break;
		
		case 2:

			if(Globle.POCT_Par.T3_Compensate_Pesitive > Globle.POCT_Par.T3_Compensate_Negative)
			{
				if(Globle.POCT_Par.T3_Compensate_Pesitive <= 2000)
					Temp = Temp - (int)(Globle.POCT_Par.T3_Compensate_Pesitive - Globle.POCT_Par.T3_Compensate_Negative);
			}
			else
			{
				if(Globle.POCT_Par.T3_Compensate_Negative <= 2000)
					Temp = Temp + (int)(Globle.POCT_Par.T3_Compensate_Negative + Globle.POCT_Par.T3_Compensate_Pesitive);		
			}
			
		break;
		
		default :break;
			
	}
	return Temp;
}



