/**
  ******************************************************************************
  * @file FLASH/Program/main.c 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stdio.h"
#include "stm32f4xx_flash.h"

/** @addtogroup StdPeriph_Examples
  * @{
  */

/** @addtogroup FLASH_Program
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define FlashBase  ((uint32_t)0x08000000)
//#define StartAddr  ((uint32_t)0x08008000)
//#define EndAddr    ((uint32_t)0x0800ffff)

#define FLASH_PAGE_SIZE         ((uint16_t)0x4000)   // 1KB


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/  

void FLASH_Configuration(void)
{
  	/* Unlock the Flash Program Erase controller */
  	FLASH_Unlock();
  	
	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP);
}  
//void FLASH_Lock(void)
  /* Erase the FLASH pages */
FLASH_Status FLASH_Erase_Sector(unsigned int StartAddr,unsigned int Length)
{
    __IO uint32_t NbrOfPage;
    uint32_t EraseCounter ;
    volatile FLASH_Status FLASHStatus;
    
    NbrOfPage = Length / FLASH_PAGE_SIZE;
    FLASHStatus = FLASH_COMPLETE;
    
//    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
//		FLASHStatus = FLASH_EraseSector(StartAddr + (FLASH_PAGE_SIZE * EraseCounter), VoltageRange_3);
//    	FLASHStatus = FLASH_EraseSector((StartAddr - 0x8000000)/FLASH_PAGE_SIZE + EraseCounter, VoltageRange_3);
    	FLASHStatus = FLASH_EraseAllSectors(VoltageRange_3);
    }
    
    return FLASHStatus;
}

FLASH_Status FLASH_Write_Word(unsigned int Address,unsigned int Data)
{
    volatile FLASH_Status FLASHStatus;
    
    FLASHStatus = FLASH_ProgramWord(Address, Data);
    
    return FLASHStatus;
}
	
unsigned int FLASH_Read_Word(unsigned int Address)
{
	unsigned int Data;
    
    Data = (*(__IO uint32_t*) Address);
	
	return Data;
}
/*
void FLASH_Read_String(unsigned int Address, unsigned int length , unsigned char *str)
{
	unsigned int i;
	unsigned int tmpdata;
	
    for(i=0;i<length;)
	{
        tmpdata = (*(__IO uint32_t*) Address+i);
		str[i+0] = (tmpdata >>  0)&0xff;
        str[i+1] = (tmpdata >>  8)&0xff;
        str[i+2] = (tmpdata >> 16)&0xff;
        str[i+3] = (tmpdata >> 24)&0xff;
        i+=4;
	}
}
FLASH_Status FLASH_Write_String(unsigned int Address, unsigned int length , unsigned char *str)
{
	unsigned int i;
	unsigned int tmpdata;
    volatile FLASH_Status FLASHStatus;
	
	FLASHStatus=FLASH_ErasePage(Address);
	
	for(i=0;i<length;)
	{
		tmpdata = (str[i+3] << 24 ) + (str[i+2] << 16) + (str[i+1] << 8) + str[i];
        FLASHStatus=FLASH_ProgramWord(Address+i, tmpdata);
        i+=4;
	}
    return FLASHStatus;
}
*/
