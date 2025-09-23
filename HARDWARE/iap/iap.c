
#include "iap.h"
#include "IAPTask.h"


iapfun jump2app;

u32 iapbuf[512]; //2K字节缓存 

//读取指定地址的字(32位数据) 
//faddr:读地址 
//返回值:对应数据. 
u32 STMFLASH_ReadWord(u32 faddr) 
{  
	return *(vu32*)faddr; 
} 


//获取某个地址所在的flash扇区 
//addr:flash地址 
//返回值:0~11,即addr所在的扇区 
u8 STMFLASH_GetFlashSector(u32 Address) 
{ 
	uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_7;  
  }
  return sector;
}


//从指定地址开始写入指定长度的数据 
//特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数 
// 写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以 
// 写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里 
// 没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
//该函数对OTP区域也有效!可以用来写OTP区! 
//OTP区域地址范围:0X1FFF7800~0X1FFF7A0F(最后16字节用于OTP数据块锁定) 
//WriteAddr:起始地址(此地址必须为4的倍数!!) 
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.) 

int STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)
{
	FLASH_EraseInitTypeDef FlashEraseInit;
	
	HAL_StatusTypeDef FlashStatus=HAL_OK;
	
	uint32_t SectorError=0;
	
	uint32_t addrx = 0;
	
	uint32_t endaddr = 0;
	
//	int result = 0;
	
//	if(WriteAddr < STM32_FLASH_BASE||WriteAddr%4)return; //非法地址
	
	HAL_FLASH_Unlock();//解锁 
	
	addrx = WriteAddr; //写入的起始地址 
	
	endaddr=WriteAddr+NumToWrite*4; //写入的结束地址
	
	if(addrx < 0X1FFF0000) 
	{
		while(addrx < endaddr)//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if(STMFLASH_ReadWord(addrx) != 0XFFFFFFFF) //有非0XFFFFFFFF的地方,要擦除这个扇区 
			{  
				FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS; //类型扇区擦除
				
				FlashEraseInit.Sector = STMFLASH_GetFlashSector(addrx); //要擦除的扇区 
				
				FlashEraseInit.NbSectors = 1; //一次只擦除一个扇区  
				
				FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;//VCC=2.7~3.6V  
				
				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK)  
				{ 
					break;//发生错误了 
				}
			}
			else 
				addrx+=4; 
			
			FLASH_WaitForLastOperation(100); //等待上次操作完成 
		}
	}
	FlashStatus = FLASH_WaitForLastOperation(100); //等待上次操作完成 
	if(FlashStatus==HAL_OK)  
	{  
		while(WriteAddr<endaddr)//写数据  
		{  
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, WriteAddr,*pBuffer)!=HAL_OK)//写入数据  
			{  
				break; //写入异常  
			}  
			WriteAddr += 4;  
			
			pBuffer++;  
		}  
	}  
	HAL_FLASH_Lock(); //上锁 
	
	return 0;
}

/**
 * @brief Write application binary to flash memory.
 *
 * This function writes the application binary to the flash memory. It reads the binary data in 4-byte chunks,
 * converts it to a 32-bit integer, and stores it in a buffer. Once the buffer is full, it writes the buffer
 * to the flash memory and clears it for the next set of data.
 *
 * @param appxaddr 应用程序的起始地址。
 * @param appbuf 应用程序CODE. 
 * @param appsize 应用程序大小(字节). 
 */
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize) 
{  
	u32 t;
	
	u16 i=0;  
	
	u32 temp; 
	
	u32 fwaddr = appxaddr;//当前写入的地址 
	
	u8 *dfu = appbuf;  
	
	for(t=0;t<appsize;t+=4)  
	{  
		temp=(u32)dfu[3]<<24; 
		temp|=(u32)dfu[2]<<16;  
		temp|=(u32)dfu[1]<<8; 
		temp|=(u32)dfu[0];  
		dfu+=4;
		iapbuf[i++] = temp;  
		if(i == 512)  
		{  
			i=0;  
			STMFLASH_Write(fwaddr,iapbuf,512);  
			fwaddr += 2048;//偏移2048 512*4=2048
			flash_app_addr += 2048;
		}
	}  
//	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去. 
} 

/**
 * @brief Loads and executes the application from the specified address.
 *
 * This function checks if the stack top address is valid. If it is, it sets the program start address,
 * initializes the APP stack pointer, and jumps to the APP.
 *
 * @param appxaddr 用户代码起始地址. 
 */
void iap_load_app(u32 appxaddr)
{
    if(((*(vu32*)appxaddr)&0x2FF00000)==0x20000000) // Check if the stack top address is valid.
    {
        jump2app=(iapfun)*(vu32*)(appxaddr+4);      // The second word in the user code area is the program start address (reset address).
        MSR_MSP(*(vu32*)appxaddr);                  // Initialize the APP stack pointer (the first word in the user code area is used to store the stack top address).
        jump2app();                                 // Jump to the APP.
    }
}



