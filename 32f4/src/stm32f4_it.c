/*
 * stm32f4_it.c
 *
 *  Created on: 26 Dec 2013
 *      Author: phalt
 */

//#include "main.h"
#include "stm32f4xx.h"
#include "hwdefs.h"

__IO uint8_t PauseResumeStatus = 2, Count = 0, LED_Toggle = 0;
extern __IO uint16_t CCR_Val;
uint16_t capture = 0;

/**
  * @brief  This function handles TIM4 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{

  /* Checks whether the TIM interrupt has occurred */
  if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
    capture = TIM_GetCapture1(TIM4);
    TIM_SetCompare1(TIM4, capture + CCR_Val);
  }
}




