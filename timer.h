/*
 *******************************************************************************
 * @file           : timer.h
 * @brief          : X
 * project         : EE 329 S'26 AX
 * authors         : joeym
 * version         : 0.1
 * date            : Apr 27, 2026
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Description: X
 *
 *******************************************************************************
 * GPIO Wiring
 * |   Component    | GPIO Identifier | Connector Location | Config
 *-----------------------------------------------------------------------------
 * | LCD - DB4 - 11 | PC0             | CN9-3              | OUT
 *******************************************************************************
 * Version History
 *  Ver.|   Date   |  Description
 *  ---------------------------------------------------------------------------
 *      |          | 
 *******************************************************************************
 *
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */

/*
 * timer.h
 *
 *  Created on: Apr 24, 2026
 *      Author: joeym
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"


void setup_TIM2(void);
void TIM2_IRQHandler(void);
void setup_MCO_CLK(void);
void start_TIM2(void);
void stop_TIM2(void);
uint32_t readCount_TIM2(void);


#endif /* INC_TIMER_H_ */

