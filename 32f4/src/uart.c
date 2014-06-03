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

/* Private function prototypes -----------------------------------------------*/
void UART_Configuration(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  UART_Configuration program
  * @param  None
  * @retval None
  */
void UART_Configuration(void)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

  	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);

  	USART_Cmd(USART3, DISABLE); // disable USART3

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // enble TX and RX
  	USART_Init(USART3, &USART_InitStructure);

//	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART3, ENABLE); // enable USART3

	//__Enable the UART3 Receive Interrupt__
//	USART_ClearITPendingBit(USART3, USART_IT_TC);
//	USART3->SR &= ~USART_FLAG_RXNE;
}

int
myputchar (int c) {

  while (!(USART3->SR & USART_FLAG_TXE));
  USART3->DR = ((uint8_t) c);
  while((USART3->SR & USART_FLAG_TXE) == 0);
  return (c);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
int
mygetchar (void) {

  while (!(USART3->SR & USART_FLAG_RXNE));
  return (USART3->DR & 0xFF);
}
///**
//  * @brief  Retargets the C library printf function to the USART3.
//  * @param  None
//  * @retval None
//  */
//int fputc(int ch, FILE *f)
//{
//  /* Place your implementation of fputc here */
//  /* e.g. write a character to the USART3 */
//
//  /* Write a character to the USART */
//  USART3->DR = ((uint8_t) ch);
//  /* Loop until the end of transmission */
//  while((USART3->SR & USART_FLAG_TXE) == 0);
//
//  return ch;
//}
///**
//  * @brief  Get a key from the USART3
//  * @param  None
//  * @retval None
//  */
//int fgetc(FILE *f)
//{
//  /* Place your implementation of fgetc here */
//  /* Waiting for user input */
//  while((USART3->SR & USART_FLAG_RXNE) == 0);
//  return (uint8_t)USART3->DR;
//}
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
