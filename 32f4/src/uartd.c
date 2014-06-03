/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "stdarg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

#ifdef TRACEUART

/* Private function prototypes -----------------------------------------------*/
void TRACEUART_Configuration(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  UART_Configuration program
  * @param  None
  * @retval None
  */
void TRACEUART_Configuration(void)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

//  	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  	NVIC_Init(&NVIC_InitStructure);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

  	USART_Cmd(USART2, DISABLE); // disable USART2

	USART_InitStructure.USART_BaudRate = 460800;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;// | USART_Mode_Rx; // enble TX and RX
  	USART_Init(USART2, &USART_InitStructure);

  	//	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART2, ENABLE); // enable USART2

	//__Enable the UART3 Receive Interrupt__
//	USART_ClearITPendingBit(USART2, USART_IT_TC);
//	USART2->SR &= ~USART_FLAG_RXNE;
}

int
traceputchar (int c) {

  while (!(USART2->SR & USART_FLAG_TXE));
  USART2->DR = ((uint8_t) c);
  while((USART2->SR & USART_FLAG_TXE) == 0);
  return (c);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
int
tracegetchar (void) {

  while (!(USART2->SR & USART_FLAG_RXNE));
  return (USART2->DR & 0xFF);
}
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
#endif// TRACEUART
