/*
 * main.h
 *
 *  Created on: 3 Jan 2014
 *      Author: phalt
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "waveplayer.h"
#include "stm32f4_discovery_audio_codec.h"


static __IO uint32_t TimingDelay;

void Delay(__IO uint32_t nTime);

void SysTick_Handler(void);

/* ----- SysTick definitions ----------------------------------------------- */

#define SYSTICK_FREQUENCY_HZ       1000

#endif /* MAIN_H_ */
