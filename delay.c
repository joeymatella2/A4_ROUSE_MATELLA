/*
 *******************************************************************************
 * @file           : delay.c
 * @brief          : keypad configuration functions and keypress detection functions
 * project         : EE 329 S'26 A3
 * authors         : Joseph Matella and Gabriel Rouse
 * version         : 0.1
 * date            : 20260421
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Description: Handles input of timer set point along with timer count down.
 *
 *******************************************************************************
 * Wiring
 * TIMER LED - PA4 / CN9-1 - OUT
 *******************************************************************************
* Header format adapted from [Code Appendix by Kevin Vo] pg 5
*/

#include "delay.h"


/*
*-----------------------------------------------------------------------------
* Citation: SysTick_Init and delay_us copied from Lab Manual, pg 15
*-----------------------------------------------------------------------------
*/
/* -----------------------------------------------------------------------------
 * function : SysTick_Init()
 * INs      : none
 * OUTs     : none
 * action   : configures SysTick for polling-based microsecond delay timing
 * -------------------------------------------------------------------------- */
void SysTick_Init(void) {
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |     	// enable SysTick Timer
                      SysTick_CTRL_CLKSOURCE_Msk); 	// select CPU clock
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);  	  // disable interrupt
}

/* -----------------------------------------------------------------------------
 * function : delay_us()
 * INs      : time_us = requested delay in microseconds
 * OUTs     : none
 * action   : generates a blocking delay in microseconds using SysTick counts
 * -------------------------------------------------------------------------- */
void delay_us(const uint32_t time_us) {
	// set the counts for the specified delay
	SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
	SysTick->VAL = 0;                                  	 // clear timer count
	SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);    	 // clear count flag
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for flag
}

