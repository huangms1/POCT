#ifndef _IAP_H
#define _IAP_H
#include "sys.h"
#include "stm32f4xx.h"



#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes   */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes  */


typedef void (*iapfun)(void); //定义一个函数类型的参数. 
#define FLASH_APP1_ADDR 0x08010000 
//第一个应用程序起始地址(存放在FLASH) 
//保留0X08000000~0X0800FFFF的空间为Bootloader使用(共64KB) 
int STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);
void iap_load_app(u32 appxaddr); //跳转到APP程序执行 
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);//在指定地址开始,写入bin


#endif
