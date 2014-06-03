/**
  ******************************************************************************
  * @file GPIO/IOToggle/main.c 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  Main program body.
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
#include "hwdefs.h"

/** @addtogroup StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void GPIO_Test(void);
extern void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */
void
GPIO_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure pin in output push/pull mode */
	GPIO_InitStructure.GPIO_Pin |= (0xF << LED1);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(BLINK_PORT, &GPIO_InitStructure);

	GPIO_SetBits(BLINK_PORT, (1 << LED1));
	GPIO_SetBits(BLINK_PORT, (1 << LED2));
	GPIO_SetBits(BLINK_PORT, (1 << LED3));
	GPIO_SetBits(BLINK_PORT, (1 << LED4));
}
/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */
void
GPIO_Configuration(void)
{
	GPIO_Initialize();
}

void
GPIO_Test(void)
{
    GPIO_ResetBits(BLINK_RCC_BIT, LED4);
/* Insert delay */
    Delay(500);
    GPIO_SetBits(BLINK_RCC_BIT, LED4);
/* Insert delay */
    Delay(500);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
