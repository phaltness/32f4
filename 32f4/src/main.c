/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "core_cm4.h"
#include "main.h"
#include "hwdefs.h"
#include "spi_loader.h"
#include "waveplayer.h"
#include "stm32f4_discovery_audio_codec.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define DEFAULT_BOOT_DELAY     		6

#define SYSTICK_FREQUENCY_HZ       	1000

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern void UART_Configuration(void);
extern void TRACEUART_Configuration(void);
typedef void (*pFunction)(void);
void run_app(uint32_t ApplicationAddress);
void LED_Init(void);
extern unsigned char awaitkey(unsigned char timeout);
extern void serial_term(void);
extern int init_builtin_cmds(void);
static void TimingDelay_Decrement(void);

#ifdef TRACEUART
extern int traceprintf(const char *format, ...);
#endif //TRACEUART

RCC_ClocksTypeDef RCC_Clocks;
__IO uint8_t RepeatState = 0;
__IO uint16_t CCR_Val = 16826;
extern __IO uint8_t LED_Toggle;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
//	int c;
   /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
    SystemInit();
	RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(SystemCoreClock / SYSTICK_FREQUENCY_HZ);

#ifdef TRACEUART
    TRACEUART_Configuration();
#endif //TRACEUART
	UART_Configuration();

	LED_Init();

	/* Output a message on Hyperterminal using printf function */
	printf("\n\r\n\r");
	printf("*** LATTICE FW LOADER V1.00 Build by phalt on ("__DATE__ "-" __TIME__ ")\n\r");
	printf("*** LATTICE FW LOADER V1.00 Rebooting ...\n\r");

	/*
     * wait for a keystroke (or a button press if you want.)
	*/
//    printf("\n\rPress Return to start, any other key To Enter the Console ...\n\r");
//	c = awaitkey(DEFAULT_BOOT_DELAY);
	if(1) 			//(((c != '\r') && (c != '\n') && (c != '\0')))
    {
		Delay(500);
		GPIO_ResetBits(BLINK_PORT, (0xF << LED1));
//		traceprintf("\rtrace start OK\n\r");
//		Codec_GPIO_Init();
//		Delay(1);
//		/* Reset the Codec Registers */
//		Codec_Reset();
//		Delay(1);
//		/* Initialize the Control interface of the Audio Codec */
//		Codec_CtrlInterface_Init();
//		Delay(1);
		if(1)
		{
	        GPIO_SetBits(BLINK_PORT, (0xF << LED1));
//	        WavePlayBack(AUDIOFREQ, 1);
	        WavePlayerInit(AUDIOFREQ);
	        GPIO_ResetBits(BLINK_PORT, (0xF << LED1));
		}
		while(1)						//eugene loop
		{
//			Codec_CtrlInterface_Init();
			GPIO_SetBits(BLINK_PORT, (0x1 << LED1));
//			Codec_Write();
			Codec_ReadRegister(0x00); //test read of codec
			GPIO_ResetBits(BLINK_PORT, (0x1 << LED1));
			Delay(1000);
		}

		if(1)									//test
		{
			init_builtin_cmds();
			serial_term();
		}


    }
	printf("*** JUMP to Application now ...\n\r");
    run_app(0x08000000);



    /* Infinite loop */
	while (1);
}

void
LED_Init(void)
{
	RCC_AHB1PeriphClockCmd(BLINK_RCC_BIT, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure pin in output push/pull mode */
	GPIO_InitStructure.GPIO_Pin |= (0xF << LED1);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(BLINK_PORT, &GPIO_InitStructure);

	GPIO_SetBits(BLINK_PORT, (0xF << LED1));	//blink all of 4 LEDs
}
void run_app(uint32_t ApplicationAddress)
{
	uint32_t JumpAddress;
	pFunction Jump_To_Application;
	
	printf("AppAddress : %h\n\r", ApplicationAddress);
	/* Test if user code is programmed starting from address "ApplicationAddress" */
    if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFC0000 ) == 0x20000000)
	{ 
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) ApplicationAddress);
		Jump_To_Application();
    }
    else
    {
    	printf("*** Application isn't Exist ...\n\r");
    	  /* Infinite loop */
		while (1) {}
    }
} 

void
error (uint8_t numerror)
{
	GPIO_SetBits(BLINK_PORT, (numerror << LED1));
}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0)
  {};
}


/**
 * @brief  Decrements the TimingDelay variable.
 * @param  None
 * @retval None
 */
void
TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
    {
      TimingDelay--;
    }
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval : None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

long
atox(const char *_S)
{
	char *p = _S;

	int i = 0;
	int base = 10;

		while(*p != '\0')
		{
			if(*p - '0' >= 0 && *p - '9' <= 9)
			{
				i = i * base + (*p - '0');
				p++;
			}
			else
			if(*p - 'a' >= 0 && *p <= 'f')
			{
				i = i * base + (*p - 'a' + 10);
				p++;
			}
			else
			if(*p - 'A' >= 0 && *p <= 'F')
			{
				i = i * base + (*p - 'A' + 10);
				p++;
			}
			else
			if(*p == 'x' || *p == 'X')
			{
				base = 16;
				p++;
			}
			else
			return -1;
		}
		return i;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
